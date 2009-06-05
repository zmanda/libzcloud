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
#include <openssl/md5.h>

/* class mechanics */
static void
class_init(ZCloudFixedMemoryDownloadConsumerClass *klass);

/* prototypes for method implementations */
static gsize
write_impl(
    ZCloudDownloadConsumer *self,
    gconstpointer buffer,
    gsize bytes,
    GError **error);

static gboolean
reset_impl(
    ZCloudDownloadConsumer *self,
    GError **error);

static guint8 *
get_contents_impl(
    ZCloudFixedMemoryDownloadConsumer *self,
    gsize *length);

GType
zcloud_fixed_memory_download_consumer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudFixedMemoryDownloadConsumerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudFixedMemoryDownloadConsumer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_DOWNLOAD_CONSUMER,
                                      "ZCloudFixedMemoryDownloadConsumer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

static void
class_init(ZCloudFixedMemoryDownloadConsumerClass *klass)
{
    ZCloudDownloadConsumerClass *up_class = ZCLOUD_DOWNLOAD_CONSUMER_CLASS(klass);

    up_class->write = write_impl;
    up_class->reset = reset_impl;
    klass->get_contents = get_contents_impl;
}


ZCloudFixedMemoryDownloadConsumer *
zcloud_fixed_memory_download_consumer(
    guint8 *buffer,
    guint buffer_length)
{
    ZCloudFixedMemoryDownloadConsumer *ret;

    ret = g_object_new(ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER, NULL);
    ret->buffer = buffer;
    ret->buffer_length = buffer_length;
    ret->buffer_position = 0;

    return ret;
}

guint8 *
zcloud_fixed_memory_download_consumer_get_contents(
    ZCloudFixedMemoryDownloadConsumer *self,
    gsize *length)
{
    ZCloudFixedMemoryDownloadConsumerClass *c = ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER_GET_CLASS(self);
    g_assert(c->get_contents != NULL);
    return (c->get_contents)(self, length);
}

static gsize
write_impl(
    ZCloudDownloadConsumer *o,
    gconstpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudFixedMemoryDownloadConsumer *self = ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER(o);
    guint length_wanted = self->buffer_position + bytes;

    if (length_wanted > self->buffer_length)
        bytes -= length_wanted - self->buffer_length;

    /* actually copy the data to the buffer */
    memcpy(self->buffer + self->buffer_position, buffer, bytes);
    self->buffer_position += bytes;

    return bytes;
}

static gboolean reset_impl(
    ZCloudDownloadConsumer *o,
    GError **error)
{
    ZCloudFixedMemoryDownloadConsumer *self = ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER(o);

    self->buffer_position = 0;

    return TRUE;
}

static guint8 *
get_contents_impl(
    ZCloudFixedMemoryDownloadConsumer *self,
    gsize *length)
{
    if (length)
        *length = self->buffer_position;

    return self->buffer;
}
