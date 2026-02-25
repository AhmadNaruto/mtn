/*  mtn - movie thumbnailer
    Context header for managing global state

    Copyright (C) 2007-2017 tuit <tuitfun@yahoo.co.th>
    Copyright (C) 2017-2024 wahibre <wahibre@gmx.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
*/

#ifndef MTN_CONTEXT_H
#define MTN_CONTEXT_H

#include <stdint.h>
#include <time.h>
#include "libavutil/rational.h"

/**
 * RGB color structure
 */
typedef struct rgb_color {
    int r, g, b;
} rgb_color;

#define COLOR_BLACK  (rgb_color){0, 0, 0}
#define COLOR_GREY   (rgb_color){128, 128, 128}
#define COLOR_WHITE  (rgb_color){255, 255, 255}
#define COLOR_INFO   (rgb_color){85, 85, 85}

/**
 * Default values for command line options
 */
#define GB_A_RATIO           (AVRational){0, 1}
#define GB_B_BLANK           0.8
#define GB_B_BEGIN           0.0
#define GB_C_COLUMN          3
#define GB_C_CUT             -1
#define GB_D_DEPTH           -1
#define GB_D_EDGE            12
#define GB_E_EXT             "3gp,3g2,asf,avi,avs,dat,divx,dsm,evo,flv,m1v,m2ts,m2v,m4v,mj2,mjpg,mjpeg,mkv,mov,moov,mp4,mpg,mpeg,mpv,nut,ogg,ogm,qt,rm,rmvb,swf,ts,vob,webm,wmv,xvid"
#define GB_G_GAP             0
#define GB_H_HEIGHT          150
#define GB_I_INFO            1
#define GB_I_INDIVIDUAL      0
#define GB_J_QUALITY         90
#define GB_K_BCOLOR          COLOR_WHITE
#define GB_L_INFO_LOCATION   4
#define GB_L_TIME_LOCATION   1
#define GB_N_NORMAL          0
#define GB_O_SUFFIX          "_s.jpg"
#define GB_P_PAUSE           0
#define GB_P_DONTPAUSE       0
#define GB_Q_QUIET           0
#define GB_R_ROW             0
#define GB_S_STEP            120
#define GB_S_SELECT_VIDEO_STREAM 0
#define GB_T_TIME            1
#define GB_V_VERBOSE         0
#define GB_W_WIDTH           1024
#define GB_W_OVERWRITE       1
#define GB_X_FILENAME_USE_FULL 0
#define GB_Z_SEEK            0
#define GB_Z_NONSEEK         0

/**
 * MTN Context - contains all configuration and state
 * This replaces the global variables for better testability and maintainability
 */
typedef struct MtnContext {
    /* Command line options */
    AVRational a_ratio;              /* -a: aspect ratio override */
    double b_blank;                  /* -b: blank threshold */
    double B_begin;                  /* -B: skip seconds from beginning */
    int c_column;                    /* -c: number of columns */
    double C_cut;                    /* -C: cut movie duration */
    int d_depth;                     /* -d: directory recursion depth */
    int D_edge;                      /* -D: edge detection */
    char *e_ext;                     /* -e: file extensions */
    double E_end;                    /* -E: skip seconds at end */
    char *f_fontname;                /* -f: font file */
    rgb_color F_info_color;          /* -F: info font color */
    double F_info_font_size;         /* -F: info font size */
    char *F_ts_fontname;             /* -F: timestamp font */
    rgb_color F_ts_color;            /* -F: timestamp color */
    rgb_color F_ts_shadow;           /* -F: timestamp shadow color */
    double F_ts_font_size;           /* -F: timestamp font size */
    int g_gap;                       /* -g: gap between shots */
    int h_height;                    /* -h: minimum height */
    int H_human_filesize;            /* -H: human readable filesize */
    int i_info;                      /* -i: info text on/off */
    int I_individual;                /* -I: save individual shots */
    int I_individual_thumbnail;      /* -I t: thumbnail size */
    int I_individual_original;       /* -I o: original size */
    int I_individual_ignore_grid;    /* -I i: ignore grid */
    int j_quality;                   /* -j: jpeg quality */
    rgb_color k_bcolor;              /* -k: background color */
    int L_info_location;             /* -L: info text location */
    int L_time_location;             /* -L: time location */
    int n_normal;                    /* -n: normal priority */
    char *N_suffix;                  /* -N: info file suffix */
    char *o_suffix;                  /* -o: output suffix */
    char *O_outdir;                  /* -O: output directory */
    int p_pause;                     /* -p: pause before exit */
    int P_dontpause;                 /* -P: don't pause */
    int q_quiet;                     /* -q: quiet mode */
    int r_row;                       /* -r: number of rows */
    int s_step;                      /* -s: time step */
    int S_select_video_stream;       /* -S: video stream selection */
    int t_timestamp;                 /* -t: timestamp on/off */
    char *T_text;                    /* -T: additional text */
    int v_verbose;                   /* -v: verbose mode */
    int V;                           /* -V: debug mode */
    int w_width;                     /* -w: output width */
    int W_overwrite;                 /* -W: overwrite files */
    char *x_basename_custom;         /* -x: custom filename */
    int X_filename_use_full;         /* -X: use full filename */
    int z_seek;                      /* -z: always seek mode */
    int Z_nonseek;                   /* -Z: always non-seek mode */

    /* Long options */
    int _shadow;                     /* --shadow */
    int _transparent_bg;             /* --transparent */
    int _cover;                      /* --cover */
    int _webvtt;                     /* --vtt */
    const char *_cover_suffix;       /* --cover suffix */
    const char *_webvtt_prefix;      /* --vtt prefix */
    void *_options;                  /* --options: AVDictionary* */
    char *_filters;                  /* --filters */
    char *_filter_color_primaries;   /* --filter-color-primaries */
    int _tonemap;                    /* --tonemap */

    /* Runtime state */
    char *argv0;                     /* Program name */
    char *version;                   /* Version string */
    time_t st_start;                 /* Start time */
    char **movie_ext;                /* Movie extensions array */

    /* Font config */
    void *fcStrFlagsInfotext;        /* gdFTStringExtra for info text */
    void *fcStrFlagsTimestamp;       /* gdFTStringExtra for timestamp */
} MtnContext;

/**
 * Initialize MtnContext with default values
 */
void mtn_context_init(MtnContext *ctx);

/**
 * Free dynamically allocated members of MtnContext
 */
void mtn_context_cleanup(MtnContext *ctx);

#endif /* MTN_CONTEXT_H */
