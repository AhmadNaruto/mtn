/*  mtn - movie thumbnailer
    Error handling utilities

    Copyright (C) 2007-2017 tuit <tuitfun@yahoo.co.th>
    Copyright (C) 2017-2024 wahibre <wahibre@gmx.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
*/

#ifndef MTN_ERROR_H
#define MTN_ERROR_H

#include "libavutil/log.h"

/**
 * Error codes used throughout mtn
 */
typedef enum MtnError {
    MTN_SUCCESS = 0,
    MTN_ERROR_GENERIC = -1,
    MTN_ERROR_INVALID_ARG = -2,
    MTN_ERROR_OUT_OF_MEMORY = -3,
    MTN_ERROR_FILE_NOT_FOUND = -4,
    MTN_ERROR_FILE_CREATE_FAILED = -5,
    MTN_ERROR_CODEC_NOT_FOUND = -6,
    MTN_ERROR_CODEC_OPEN_FAILED = -7,
    MTN_ERROR_STREAM_NOT_FOUND = -8,
    MTN_ERROR_DECODE_FAILED = -9,
    MTN_ERROR_FILTER_INIT_FAILED = -10,
    MTN_ERROR_IMAGE_SAVE_FAILED = -11,
    MTN_ERROR_BUFFER_TOO_SMALL = -12,
} MtnError;

/**
 * Error handling macros for consistent error reporting
 */

/** Log error and return error code */
#define MTN_RETURN_ERROR(msg, errcode) do { \
    av_log(NULL, AV_LOG_ERROR, "%s: " msg "\n", __func__); \
    return (errcode); \
} while(0)

/** Log error with format string and return */
#define MTN_RETURN_ERROR_FMT(msg, errcode, ...) do { \
    av_log(NULL, AV_LOG_ERROR, "%s: " msg "\n", __func__, ##__VA_ARGS__); \
    return (errcode); \
} while(0)

/** Log error and goto cleanup label */
#define MTN_GOTO_ERROR(msg, label) do { \
    av_log(NULL, AV_LOG_ERROR, "%s: " msg "\n", __func__); \
    goto label; \
} while(0)

/** Log error with format and goto cleanup */
#define MTN_GOTO_ERROR_FMT(msg, label, ...) do { \
    av_log(NULL, AV_LOG_ERROR, "%s: " msg "\n", __func__, ##__VA_ARGS__); \
    goto label; \
} while(0)

/** Check condition and return error if true */
#define MTN_CHECK_ERROR(cond, msg, errcode) do { \
    if (cond) { \
        MTN_RETURN_ERROR(msg, errcode); \
    } \
} while(0)

/** Check condition and goto error label if true */
#define MTN_CHECK_ERROR_GOTO(cond, msg, label) do { \
    if (cond) { \
        MTN_GOTO_ERROR(msg, label); \
    } \
} while(0)

/** Check FFmpeg return code and handle error */
#define MTN_CHECK_FFMPEG(ret, msg) do { \
    if ((ret) < 0) { \
        char errbuf[AV_ERROR_MAX_STRING_SIZE]; \
        av_strerror(ret, errbuf, sizeof(errbuf)); \
        MTN_RETURN_ERROR_FMT(msg ": %s", MTN_ERROR_GENERIC, errbuf); \
    } \
} while(0)

/** Check pointer and return error if NULL */
#define MTN_CHECK_NULL(ptr, msg) do { \
    if ((ptr) == NULL) { \
        MTN_RETURN_ERROR(msg, MTN_ERROR_OUT_OF_MEMORY); \
    } \
} while(0)

/** Check pointer and goto error if NULL */
#define MTN_CHECK_NULL_GOTO(ptr, msg, label) do { \
    if ((ptr) == NULL) { \
        MTN_GOTO_ERROR(msg, label); \
    } \
} while(0)

/**
 * Convert MtnError to human-readable string
 */
const char* mtn_error_string(MtnError err);

/**
 * Get current error count (for debugging)
 */
int mtn_get_error_count(void);

/**
 * Reset error count (for debugging)
 */
void mtn_reset_error_count(void);

#endif /* MTN_ERROR_H */
