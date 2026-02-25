/*  mtn - movie thumbnailer
    Context implementation for managing global state

    Copyright (C) 2007-2017 tuit <tuitfun@yahoo.co.th>
    Copyright (C) 2017-2024 wahibre <wahibre@gmx.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
*/

#include "mtn_context.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

/* Import defaults from mtn.c - these should match */
#define GB_E_END             0.0
#define GB_H_human_filesize  0
#define GB_N_SUFFIX          NULL
#define GB_O_OUTDIR          NULL
#define GB_T_TEXT            NULL

#ifndef GB_F_FONTNAME
#ifdef __APPLE__
#   define GB_F_FONTNAME "Tahoma Bold.ttf"
#else
#ifdef _WIN32
#   define GB_F_FONTNAME "tahomabd.ttf"
#else
#   define GB_F_FONTNAME "DejaVuSans.ttf"
#endif
#endif
#endif

void mtn_context_init(MtnContext *ctx)
{
    if (!ctx) return;

    memset(ctx, 0, sizeof(MtnContext));

    /* Command line options with defaults */
    ctx->a_ratio = GB_A_RATIO;
    ctx->b_blank = GB_B_BLANK;
    ctx->B_begin = GB_B_BEGIN;
    ctx->c_column = GB_C_COLUMN;
    ctx->C_cut = GB_C_CUT;
    ctx->d_depth = GB_D_DEPTH;
    ctx->D_edge = GB_D_EDGE;
    ctx->e_ext = GB_E_EXT;
    ctx->E_end = GB_E_END;
    ctx->f_fontname = GB_F_FONTNAME;
    ctx->F_info_color = COLOR_INFO;
    ctx->F_info_font_size = 9.0;
    ctx->F_ts_fontname = GB_F_FONTNAME;
    ctx->F_ts_color = COLOR_WHITE;
    ctx->F_ts_shadow = COLOR_BLACK;
    ctx->F_ts_font_size = 8.0;
    ctx->g_gap = GB_G_GAP;
    ctx->h_height = GB_H_HEIGHT;
    ctx->H_human_filesize = GB_H_human_filesize;
    ctx->i_info = GB_I_INFO;
    ctx->I_individual = GB_I_INDIVIDUAL;
    ctx->I_individual_thumbnail = 0;
    ctx->I_individual_original = 0;
    ctx->I_individual_ignore_grid = 0;
    ctx->j_quality = GB_J_QUALITY;
    ctx->k_bcolor = GB_K_BCOLOR;
    ctx->L_info_location = GB_L_INFO_LOCATION;
    ctx->L_time_location = GB_L_TIME_LOCATION;
    ctx->n_normal = GB_N_NORMAL;
    ctx->N_suffix = GB_N_SUFFIX;
    ctx->o_suffix = GB_O_SUFFIX;
    ctx->O_outdir = GB_O_OUTDIR;
#ifdef _WIN32
    ctx->p_pause = 1;  /* Windows default */
#else
    ctx->p_pause = 0;  /* Unix default */
#endif
    ctx->P_dontpause = GB_P_DONTPAUSE;
    ctx->q_quiet = GB_Q_QUIET;
    ctx->r_row = GB_R_ROW;
    ctx->s_step = GB_S_STEP;
    ctx->S_select_video_stream = GB_S_SELECT_VIDEO_STREAM;
    ctx->t_timestamp = GB_T_TIME;
    ctx->T_text = GB_T_TEXT;
    ctx->v_verbose = GB_V_VERBOSE;
    ctx->V = GB_V_VERBOSE;
    ctx->w_width = GB_W_WIDTH;
    ctx->W_overwrite = GB_W_OVERWRITE;
    ctx->x_basename_custom = NULL;
    ctx->X_filename_use_full = GB_X_FILENAME_USE_FULL;
    ctx->z_seek = GB_Z_SEEK;
    ctx->Z_nonseek = GB_Z_NONSEEK;

    /* Long options */
    ctx->_shadow = -1;
    ctx->_transparent_bg = 0;
    ctx->_cover = 0;
    ctx->_webvtt = 0;
    ctx->_cover_suffix = "_cover.jpg";
    ctx->_webvtt_prefix = "";
    ctx->_options = NULL;
    ctx->_filters = NULL;
    ctx->_filter_color_primaries = NULL;
    ctx->_tonemap = 0;

    /* Runtime state */
    ctx->argv0 = NULL;
    ctx->version = "3.5.0";
    ctx->st_start = 0;
    ctx->movie_ext = NULL;
    ctx->fcStrFlagsInfotext = NULL;
    ctx->fcStrFlagsTimestamp = NULL;
}

void mtn_context_cleanup(MtnContext *ctx)
{
    if (!ctx) return;

    /* Note: String literals (GB_*) should not be freed */
    /* Only free dynamically allocated strings */
    
    if (ctx->argv0) {
        free(ctx->argv0);
        ctx->argv0 = NULL;
    }
    if (ctx->x_basename_custom) {
        free(ctx->x_basename_custom);
        ctx->x_basename_custom = NULL;
    }
    if (ctx->_filters) {
        free(ctx->_filters);
        ctx->_filters = NULL;
    }
    if (ctx->_filter_color_primaries) {
        free(ctx->_filter_color_primaries);
        ctx->_filter_color_primaries = NULL;
    }

    /* Free movie extensions array */
    if (ctx->movie_ext) {
        char **ext = ctx->movie_ext;
        while (*ext) {
            free(*ext);
            ext++;
        }
        free(ctx->movie_ext);
        ctx->movie_ext = NULL;
    }

    /* Note: _options (AVDictionary*) should be freed by caller using av_dict_free */
}
