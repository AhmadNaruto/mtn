/*  mtn - movie thumbnailer
    Thumbnail processing helper functions

    Copyright (C) 2007-2017 tuit <tuitfun@yahoo.co.th>
    Copyright (C) 2017-2024 wahibre <wahibre@gmx.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
*/

#ifndef MTN_THUMBNAIL_H
#define MTN_THUMBNAIL_H

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "gd.h"

/**
 * ThumbnailContext - manages resources for thumbnail generation
 */
typedef struct ThumbnailContext {
    /* FFmpeg resources */
    AVFormatContext *format_ctx;
    AVCodecContext *codec_ctx;
    AVFrame *frame;
    AVFrame *frame_rgb;
    uint8_t *rgb_buffer;
    struct SwsContext *sws_ctx;
    
    /* Filter resources */
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    
    /* Output resources */
    gdImagePtr out_image;
    gdImagePtr shadow_image;
    FILE *info_fp;
    
    /* State */
    int video_index;
    int rotation;
    double time_base;
    int timestamp_enabled;
    
    /* Cleanup flags */
    int format_ctx_opened;
    int codec_ctx_opened;
    int filter_graph_initialized;
} ThumbnailContext;

/**
 * Initialize thumbnail context with NULL/default values
 */
void thumbnail_context_init(ThumbnailContext *ctx);

/**
 * Free all resources in thumbnail context
 */
void thumbnail_context_cleanup(ThumbnailContext *ctx);

/**
 * Open video file and find video stream
 * Returns 0 on success, -1 on error
 */
int thumbnail_open_file(ThumbnailContext *ctx, const char *filename, int select_stream);

/**
 * Initialize video decoder
 * Returns 0 on success, -1 on error
 */
int thumbnail_init_decoder(ThumbnailContext *ctx);

/**
 * Initialize video filters if specified
 * Returns 0 on success, -1 on error
 */
int thumbnail_init_filters(ThumbnailContext *ctx, const char *filter_graph_str);

/**
 * Allocate and prepare frame buffers
 * Returns 0 on success, -1 on error
 */
int thumbnail_alloc_frames(ThumbnailContext *ctx, int width, int height, enum AVPixelFormat pix_fmt);

/**
 * Calculate thumbnail dimensions and layout
 */
void thumbnail_calc_dimensions(ThumbnailContext *ctx, int *width, int *height, int *columns, int *rows);

/**
 * Main decoding and thumbnail assembly loop
 * Returns 0 on success, -1 on error
 */
int thumbnail_decode_and_assemble(ThumbnailContext *ctx, int columns, int rows, int step);

/**
 * Save thumbnail to file
 * Returns 0 on success, -1 on error
 */
int thumbnail_save_image(ThumbnailContext *ctx, const char *filename, int quality);

/**
 * Save info text to file
 * Returns 0 on success, -1 on error
 */
int thumbnail_save_info(ThumbnailContext *ctx, const char *filename);

#endif /* MTN_THUMBNAIL_H */
