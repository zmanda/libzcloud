/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/*  ***** BEGIN LICENSE BLOCK *****
 * Copyright (C) 2009 Zmanda Incorporated. All Rights Reserved.
 *
 * This file is part of libzcloud.
 *
 * libzcloud is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License (the LGPL)
 * as published by the Free Software Foundation, either version 2.1 of
 * the LGPL, or (at your option) any later version.
 *
 * libzcloud is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *  ***** END LICENSE BLOCK ***** */

#include <errno.h>
#include <string.h>
#include "internal.h"

gboolean
write_full(
    int fd,
    gconstpointer buf,
    gsize buf_len,
    GError **error)
{
    size_t written;

    g_assert(buf);

    for (written = 0; written < buf_len; /*nothing*/) {
        ssize_t w_ret = write(fd, buf+written, buf_len-written);
        if (w_ret < 0) {
            if (EINTR != errno) {
                g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN,
                    "an error ocurred while writing to fd %d: %s",
                    fd, strerror(errno));
                return FALSE;
            }
        } else {
            written += w_ret;
        }
    }

    return TRUE;
}
