/*  mtn - movie thumbnailer
    Thumbnail processing helper functions implementation

    Copyright (C) 2007-2017 tuit <tuitfun@yahoo.co.th>
    Copyright (C) 2017-2024 wahibre@gmx.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
*/

#include "mtn_thumbnail.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libavutil/display.h"
#include "libswscale/swscale.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void thumbnail_context_init(ThumbnailContext *ctx)
{
    if (!ctx) return;
    
    memset(ctx, 0, sizeof(ThumbnailContext));
}

void thumbnail_context_cleanup(ThumbnailContext *ctx)
{
    if (!ctx) return;
    
    /* Free filter graph */
    if (ctx->filter_graph) {
        avfilter_graph_free(&ctx->filter_graph);
        ctx->filter_graph = NULL;
        ctx->buffersink_ctx = NULL;
        ctx->buffersrc_ctx = NULL;
    }
    
    /* Free codec context */
    if (ctx->codec_ctx) {
        if (ctx->codec_ctx_opened) {
            avcodec_free_context(&ctx->codec_ctx);
        } else {
            avcodec_free_context(&ctx->codec_ctx);
        }
        ctx->codec_ctx = NULL;
    }
    
    /* Free format context */
    if (ctx->format_ctx && ctx->format_ctx_opened) {
        avformat_close_input(&ctx->format_ctx);
        ctx->format_ctx = NULL;
    }
    
    /* Free frames */
    if (ctx->frame) {
        av_frame_free(&ctx->frame);
        ctx->frame = NULL;
    }
    if (ctx->frame_rgb) {
        av_frame_free(&ctx->frame_rgb);
        ctx->frame_rgb = NULL;
    }
    
    /* Free RGB buffer */
    if (ctx->rgb_buffer) {
        av_free(ctx->rgb_buffer);
        ctx->rgb_buffer = NULL;
    }
    
    /* Free SwsContext */
    if (ctx->sws_ctx) {
        sws_freeContext(ctx->sws_ctx);
        ctx->sws_ctx = NULL;
    }
    
    /* Free GD images */
    if (ctx->out_image) {
        gdImageDestroy(ctx->out_image);
        ctx->out_image = NULL;
    }
    if (ctx->shadow_image) {
        gdImageDestroy(ctx->shadow_image);
        ctx->shadow_image = NULL;
    }
    
    /* Close info file */
    if (ctx->info_fp) {
        fclose(ctx->info_fp);
        ctx->info_fp = NULL;
    }
}

int thumbnail_open_file(ThumbnailContext *ctx, const char *filename, int select_stream)
{
    int ret;
    
    if (!ctx || !filename) return -1;
    
    /* Open video file */
    ret = avformat_open_input(&ctx->format_ctx, filename, NULL, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_open_input failed: %d\n", ret);
        return -1;
    }
    ctx->format_ctx_opened = 1;
    
    /* Generate PTS */
    ctx->format_ctx->flags |= AVFMT_FLAG_GENPTS;
    
    /* Retrieve stream information */
    ret = avformat_find_stream_info(ctx->format_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_find_stream_info failed: %d\n", ret);
        return -1;
    }
    
    /* Find video stream */
    ctx->video_index = select_stream >= 0 ? select_stream : 0;
    
    /* TODO: Implement proper stream selection */
    
    if (ctx->video_index < 0 || (unsigned int)ctx->video_index >= ctx->format_ctx->nb_streams) {
        av_log(NULL, AV_LOG_ERROR, "Invalid video stream index: %d\n", ctx->video_index);
        return -1;
    }
    
    AVStream *stream = ctx->format_ctx->streams[ctx->video_index];
    if (stream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
        av_log(NULL, AV_LOG_ERROR, "Stream %d is not a video stream\n", ctx->video_index);
        return -1;
    }
    
    ctx->time_base = av_q2d(stream->time_base);
    
    return 0;
}

int thumbnail_init_decoder(ThumbnailContext *ctx)
{
    const AVCodec *codec;
    int ret;
    
    if (!ctx || !ctx->format_ctx) return -1;
    
    AVStream *stream = ctx->format_ctx->streams[ctx->video_index];
    
    /* Get codec context from codec parameters */
    ctx->codec_ctx = avcodec_alloc_context3(NULL);
    if (!ctx->codec_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate codec context\n");
        return -1;
    }
    
    ret = avcodec_parameters_to_context(ctx->codec_ctx, stream->codecpar);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to copy codec parameters: %d\n", ret);
        avcodec_free_context(&ctx->codec_ctx);
        return -1;
    }
    
    /* Find decoder */
    codec = avcodec_find_decoder(ctx->codec_ctx->codec_id);
    if (!codec) {
        av_log(NULL, AV_LOG_ERROR, "Decoder not found for codec id: %d\n", ctx->codec_ctx->codec_id);
        return -1;
    }
    
    /* Open codec */
    ret = avcodec_open2(ctx->codec_ctx, codec, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to open codec: %d\n", ret);
        avcodec_free_context(&ctx->codec_ctx);
        return -1;
    }
    ctx->codec_ctx_opened = 1;
    
    /* Get rotation if any */
    ctx->rotation = 0;
    AVDictionaryEntry *rotate_tag = av_dict_get(stream->metadata, "rotate", NULL, 0);
    if (rotate_tag) {
        char *tail;
        ctx->rotation = strtol(rotate_tag->value, &tail, 10);
        if (*tail == '\0') {
            av_log(NULL, AV_LOG_INFO, "Rotation: %d degrees\n", ctx->rotation);
        }
    }
    
    return 0;
}

int thumbnail_init_filters(ThumbnailContext *ctx, const char *filter_str)
{
    int ret;
    char args[512];
    AVFilterInOut *inputs, *outputs;
    
    if (!ctx || !ctx->codec_ctx || !filter_str) return 0; /* No filters */
    
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    
    ctx->filter_graph = avfilter_graph_alloc();
    if (!ctx->filter_graph) return -1;
    
    /* Create buffer source */
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            ctx->codec_ctx->width, ctx->codec_ctx->height, ctx->codec_ctx->pix_fmt,
            ctx->format_ctx->streams[ctx->video_index]->time_base.num,
            ctx->format_ctx->streams[ctx->video_index]->time_base.den,
            ctx->codec_ctx->sample_aspect_ratio.num,
            ctx->codec_ctx->sample_aspect_ratio.den);
    
    ret = avfilter_graph_create_filter(&ctx->buffersrc_ctx, buffersrc, "in",
                                       args, NULL, ctx->filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        return -1;
    }
    
    /* Create buffer sink */
    ret = avfilter_graph_create_filter(&ctx->buffersink_ctx, buffersink, "out",
                                       NULL, NULL, ctx->filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        return -1;
    }
    
    /* Set up filter graph */
    outputs = avfilter_inout_alloc();
    inputs = avfilter_inout_alloc();
    
    if (!outputs || !inputs) {
        avfilter_inout_free(&outputs);
        avfilter_inout_free(&inputs);
        return -1;
    }
    
    outputs->name = av_strdup("in");
    outputs->filter_ctx = ctx->buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;
    
    inputs->name = av_strdup("out");
    inputs->filter_ctx = ctx->buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;
    
    ret = avfilter_graph_parse_ptr(ctx->filter_graph, filter_str, &inputs, &outputs, NULL);
    if (ret < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }
    
    ret = avfilter_graph_config(ctx->filter_graph, NULL);
    if (ret < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }
    
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    
    ctx->filter_graph_initialized = 1;
    
    return 0;
}

int thumbnail_alloc_frames(ThumbnailContext *ctx, int width, int height, enum AVPixelFormat pix_fmt)
{
    int ret;
    
    if (!ctx) return -1;
    
    /* Allocate frame */
    ctx->frame = av_frame_alloc();
    if (!ctx->frame) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate video frame\n");
        return -1;
    }
    
    /* Allocate RGB frame */
    ctx->frame_rgb = av_frame_alloc();
    if (!ctx->frame_rgb) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate RGB frame\n");
        return -1;
    }
    
    /* Allocate RGB buffer */
    int bufsize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    if (bufsize < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to calculate buffer size\n");
        return -1;
    }
    
    ctx->rgb_buffer = av_malloc(bufsize);
    if (!ctx->rgb_buffer) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate RGB buffer\n");
        return -1;
    }
    
    /* Fill RGB frame with buffer */
    ret = av_image_fill_arrays(ctx->frame_rgb->data, ctx->frame_rgb->linesize,
                               ctx->rgb_buffer, AV_PIX_FMT_RGB24, width, height, 1);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to fill RGB frame arrays: %d\n", ret);
        return -1;
    }
    
    /* Create SwsContext for conversion */
    ctx->sws_ctx = sws_getContext(width, height, pix_fmt,
                                   width, height, AV_PIX_FMT_RGB24,
                                   SWS_BILINEAR, NULL, NULL, NULL);
    if (!ctx->sws_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Failed to create SwsContext\n");
        return -1;
    }
    
    return 0;
}

void thumbnail_calc_dimensions(ThumbnailContext *ctx, int *width, int *height, 
                                int *columns, int *rows)
{
    /* Default implementation - can be customized */
    if (!ctx || !width || !height || !columns || !rows) return;
    
    /* Use codec dimensions as base */
    *width = ctx->codec_ctx->width;
    *height = ctx->codec_ctx->height;
    
    /* Default grid */
    if (*columns <= 0) *columns = 3;
    if (*rows <= 0) *rows = 0; /* Auto-calculate */
}

int thumbnail_decode_and_assemble(ThumbnailContext *ctx, int columns, int rows, int step)
{
    /* Suppress unused parameter warnings - placeholder for future implementation */
    (void)columns;
    (void)rows;
    (void)step;
    
    /* Placeholder - actual implementation requires more context from original code */
    /* This would contain the main decoding loop from make_thumbnail() */
    
    if (!ctx || !ctx->codec_ctx) return -1;
    
    /* TODO: Implement decoding loop */
    
    return 0;
}

int thumbnail_save_image(ThumbnailContext *ctx, const char *filename, int quality)
{
    FILE *fp;
    
    if (!ctx || !ctx->out_image || !filename) return -1;
    
    fp = fopen(filename, "wb");
    if (!fp) {
        av_log(NULL, AV_LOG_ERROR, "Failed to open output file: %s\n", filename);
        return -1;
    }
    
    /* Save based on extension */
    const char *ext = strrchr(filename, '.');
    if (ext && strcasecmp(ext, ".png") == 0) {
        gdImagePng(ctx->out_image, fp);
    } else {
        gdImageJpeg(ctx->out_image, fp, quality);
    }
    
    fclose(fp);
    return 0;
}

int thumbnail_save_info(ThumbnailContext *ctx, const char *filename)
{
    if (!ctx || !filename) return -1;
    
    ctx->info_fp = fopen(filename, "w");
    if (!ctx->info_fp) {
        av_log(NULL, AV_LOG_ERROR, "Failed to create info file: %s\n", filename);
        return -1;
    }
    
    /* TODO: Write stream info to file */
    
    return 0;
}
