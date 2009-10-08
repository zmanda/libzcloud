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


#include "internal.h"
#include <errno.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

ZCloudFDUploadProducer *
zcloud_fd_upload_producer(int fd)
{
    ZCloudFDUploadProducer *ret;

    ret = g_object_new(ZCLOUD_TYPE_FD_UPLOAD_PRODUCER, NULL);
    ret->fd = fd;
    ret->bytes_read = 0;

    return ret;
}

static gsize
read_impl(
    ZCloudUploadProducer *o,
    gpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudFDUploadProducer *self = ZCLOUD_FD_UPLOAD_PRODUCER(o);
    gsize bytes_read;

    read_full(self->fd, buffer, bytes, &bytes_read, error);
    self->bytes_read += bytes_read;

    return bytes_read;
}

static gsize
get_size_impl(ZCloudUploadProducer *o, GError **error)
{
    ZCloudFDUploadProducer *self = ZCLOUD_FD_UPLOAD_PRODUCER(o);
    struct stat st;

    bzero(&st, sizeof(st));
    if (fstat(self->fd, &st) < 0) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN,
            "Unable to stat fd %d: %s", self->fd, strerror(errno));
        return 0;
    }
    if (!S_ISREG(st.st_mode)) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN,
            "fd %d is not a regular file", self->fd);
        return 0;
    }
    if (st.st_size < 0) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN,
            "fd %d claims its size less than zero", self->fd);
        return 0;
    }

    return (gsize) st.st_size;
}

static GByteArray*
calculate_md5_impl(
    ZCloudUploadProducer *o,
    GError **error)
{
    static const gsize BUF_SIZE = 8*1024;
    ZCloudFDUploadProducer *self = ZCLOUD_FD_UPLOAD_PRODUCER(o);
    off_t prev_pos;
    GByteArray *ret;
    MD5_CTX md5_ctx;
    gchar *buf;
    gsize read_bytes;
    GError *tmp_err = NULL;

    prev_pos = lseek(self->fd, 0, SEEK_CUR);
    if (prev_pos < 0) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN,
            "failed to get current position in fd %d: %s", self->fd, strerror(errno));
        return NULL;
    }

    ret = g_byte_array_sized_new(ZCLOUD_MD5_HASH_BYTE_LEN);
    g_byte_array_set_size(ret, ZCLOUD_MD5_HASH_BYTE_LEN);

    MD5_Init(&md5_ctx);

    buf = g_malloc(BUF_SIZE);
    while (read_full(self->fd, buf, BUF_SIZE, &read_bytes, &tmp_err)) {
        if (!read_bytes) break; /* eof */
        MD5_Update(&md5_ctx, buf, read_bytes);
    }
    g_free(buf);
    MD5_Final(ret->data, &md5_ctx);

    if (!tmp_err && lseek(self->fd, prev_pos, SEEK_SET) < 0) {
        g_set_error(&tmp_err, ZCLOUD_ERROR, ZCERR_UNKNOWN,
            "failed to set position in fd %d to what it was before: %s",
            self->fd, strerror(errno));
    }

    if (tmp_err) {
        g_byte_array_free(ret, TRUE);
        ret = NULL;
        g_propagate_error(error, tmp_err);
    }
    return ret;
}

static gboolean
reset_impl(ZCloudUploadProducer *o, GError **error)
{
    ZCloudFDUploadProducer *self = ZCLOUD_FD_UPLOAD_PRODUCER(o);

    if (lseek(self->fd, -(self->bytes_read), SEEK_CUR) < 0) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN,
            "failed to position of fd %d: %s", self->fd, strerror(errno));
        return FALSE;
    }

    self->bytes_read = 0;
    return TRUE;
}

static void
class_init(ZCloudFDUploadProducerClass *klass)
{
    ZCloudUploadProducerClass *up_class = ZCLOUD_UPLOAD_PRODUCER_CLASS(klass);

    up_class->read = read_impl;
    up_class->get_size = get_size_impl;
    up_class->calculate_md5 = calculate_md5_impl;
    up_class->reset = reset_impl;
}

GType
zcloud_fd_upload_producer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudFDUploadProducerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudFDUploadProducer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_UPLOAD_PRODUCER,
                                      "ZCloudFDUploadProducer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}
