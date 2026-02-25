/*  mtn - movie thumbnailer
    Error handling utilities implementation

    Copyright (C) 2007-2017 tuit <tuitfun@yahoo.co.th>
    Copyright (C) 2017-2024 wahibre <wahibre@gmx.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
*/

#include "mtn_error.h"
#include <stdio.h>

static int error_count = 0;

const char* mtn_error_string(MtnError err)
{
    switch (err) {
        case MTN_SUCCESS:
            return "Success";
        case MTN_ERROR_GENERIC:
            return "Generic error";
        case MTN_ERROR_INVALID_ARG:
            return "Invalid argument";
        case MTN_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case MTN_ERROR_FILE_NOT_FOUND:
            return "File not found";
        case MTN_ERROR_FILE_CREATE_FAILED:
            return "File creation failed";
        case MTN_ERROR_CODEC_NOT_FOUND:
            return "Codec not found";
        case MTN_ERROR_CODEC_OPEN_FAILED:
            return "Codec open failed";
        case MTN_ERROR_STREAM_NOT_FOUND:
            return "Stream not found";
        case MTN_ERROR_DECODE_FAILED:
            return "Decode failed";
        case MTN_ERROR_FILTER_INIT_FAILED:
            return "Filter initialization failed";
        case MTN_ERROR_IMAGE_SAVE_FAILED:
            return "Image save failed";
        case MTN_ERROR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        default:
            return "Unknown error";
    }
}

int mtn_get_error_count(void)
{
    return error_count;
}

void mtn_reset_error_count(void)
{
    error_count = 0;
}
