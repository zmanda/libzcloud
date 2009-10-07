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

#ifndef ZC_FD_UTIL_H
#define ZC_FD_UTIL_H

G_BEGIN_DECLS

/*
 * writes buf_len bytes from buf to fd. Return TRUE if successful. Otherwise,
 * returns FALSE and sets error.
 */
G_GNUC_INTERNAL
gboolean
write_full(
    int fd,
    gconstpointer buf,
    gsize buf_len,
    GError **error);

/*
 * reads buf_len bytes from fd int buf. Return TRUE if successful. Otherwise,
 * returns FALSE and sets error. bytes_read, if non-NULL, will be set to the
 * number of bytes read.
 */
G_GNUC_INTERNAL
gboolean
read_full(
    int fd,
    gconstpointer buf,
    gsize buf_len,
    gsize *bytes_read,
    GError **error);

G_END_DECLS

#endif
