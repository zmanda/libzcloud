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
#include <sys/types.h>
#include <unistd.h>
#include "internal.h"

/* class mechanics */
static void
class_init(ZCloudFDDownloadConsumerClass *klass);

/* prototypes for method implementations */
static gsize
write_impl(
    ZCloudDownloadConsumer *o,
    gconstpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudFDDownloadConsumer *self = ZCLOUD_FD_DOWNLOAD_CONSUMER(o);

    if (write_full(self->fd, buffer, bytes, error)) {
        self->bytes_written += bytes;
        return bytes;
    }

    return 0;
}

static gboolean reset_impl(
    ZCloudDownloadConsumer *o,
    GError **error)
{
    ZCloudFDDownloadConsumer *self = ZCLOUD_FD_DOWNLOAD_CONSUMER(o);
    off_t off;

    off = lseek(self->fd, -(self->bytes_written), SEEK_CUR);
    if (off < 0) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN, "could not seek back: %s",
            strerror(errno));
        return FALSE;
    }
    if (ftruncate(self->fd, off) < 0) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN, "could not truncate: %s",
            strerror(errno));
        return FALSE;
    }
    self->bytes_written = 0;

    return TRUE;
}

GType
zcloud_fd_download_consumer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudFDDownloadConsumerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudFDDownloadConsumer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_DOWNLOAD_CONSUMER,
                                      "ZCloudFDDownloadConsumer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

static void
class_init(ZCloudFDDownloadConsumerClass *klass)
{
    ZCloudDownloadConsumerClass *up_class = ZCLOUD_DOWNLOAD_CONSUMER_CLASS(klass);

    up_class->write = write_impl;
    up_class->reset = reset_impl;
}


ZCloudFDDownloadConsumer *
zcloud_fd_download_consumer(
    int fd)
{
    ZCloudFDDownloadConsumer *ret;

    ret = g_object_new(ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER, NULL);
    ret->fd = fd;
    ret->bytes_written = 0;

    return ret;
}
