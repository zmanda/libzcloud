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

gpointer
zcloud_growing_memory_download_consumer_get_contents(
    ZCloudGrowingMemoryDownloadConsumer *self,
    gsize *length)
{
    ZCloudGrowingMemoryDownloadConsumerClass *c = ZCLOUD_GROWING_MEMORY_DOWNLOAD_CONSUMER_GET_CLASS(self);
    g_assert(c->get_contents != NULL);
    return (c->get_contents)(self, length);
}

gchar *
zcloud_growing_memory_download_consumer_get_contents_as_string(
    ZCloudGrowingMemoryDownloadConsumer *self)
{
    ZCloudGrowingMemoryDownloadConsumerClass *c = ZCLOUD_GROWING_MEMORY_DOWNLOAD_CONSUMER_GET_CLASS(self);
    g_assert(c->get_contents_as_string != NULL);
    return (c->get_contents_as_string)(self);
}

static gsize
write_impl(
    ZCloudDownloadConsumer *o,
    gconstpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudGrowingMemoryDownloadConsumer *self = ZCLOUD_GROWING_MEMORY_DOWNLOAD_CONSUMER(o);
    gsize length_wanted = self->buffer_position + bytes;

    /* reallocate if necessary. We use exponential sizing to make this
     * happen less often. */
    if (length_wanted > self->buffer_length) {
        gsize new_length = MAX(length_wanted, self->buffer_length * 2);
        if (self->max_buffer_length) {
            new_length = MIN(new_length, self->max_buffer_length);
        }
        self->buffer = g_realloc(self->buffer, new_length);
        self->buffer_length = new_length;
    }
    if (self->max_buffer_length && length_wanted > self->max_buffer_length)
        bytes -= length_wanted - self->max_buffer_length;

    /* actually copy the data to the buffer */
    memcpy(self->buffer + self->buffer_position, buffer, bytes);
    self->buffer_position += bytes;

    return bytes;
}

static gboolean reset_impl(
    ZCloudDownloadConsumer *o,
    GError **error)
{
    ZCloudGrowingMemoryDownloadConsumer *self = ZCLOUD_GROWING_MEMORY_DOWNLOAD_CONSUMER(o);

    self->buffer_position = 0;

    return TRUE;
}

static gpointer
get_contents_impl(
    ZCloudGrowingMemoryDownloadConsumer *self,
    gsize *length)
{
    gpointer ret;

    if (length)
        *length = self->buffer_position;

    ret = g_malloc(self->buffer_position);
    memcpy(ret, self->buffer, self->buffer_position);

    return ret;
}

static gchar *
get_contents_as_string_impl(
    ZCloudGrowingMemoryDownloadConsumer *self)
{
    return g_strndup((gchar *) self->buffer, self->buffer_position);
}

static void
finalize_impl(GObject *o)
{
    ZCloudGrowingMemoryDownloadConsumer *self = ZCLOUD_GROWING_MEMORY_DOWNLOAD_CONSUMER(o);

    g_free(self->buffer);
}

static void
class_init(ZCloudGrowingMemoryDownloadConsumerClass *klass)
{
    GObjectClass *go_class = G_OBJECT_CLASS(klass);
    ZCloudDownloadConsumerClass *up_class = ZCLOUD_DOWNLOAD_CONSUMER_CLASS(klass);

    go_class->finalize = finalize_impl;
    up_class->write = write_impl;
    up_class->reset = reset_impl;
    klass->get_contents = get_contents_impl;
    klass->get_contents_as_string = get_contents_as_string_impl;
}

GType
zcloud_growing_memory_download_consumer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudGrowingMemoryDownloadConsumerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudGrowingMemoryDownloadConsumer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_DOWNLOAD_CONSUMER,
                                      "ZCloudGrowingMemoryDownloadConsumer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

ZCloudGrowingMemoryDownloadConsumer *
zcloud_growing_memory_download_consumer(
    gsize max_buffer_length)
{
    ZCloudGrowingMemoryDownloadConsumer *ret;

    ret = g_object_new(ZCLOUD_TYPE_GROWING_MEMORY_DOWNLOAD_CONSUMER, NULL);
    ret->buffer = NULL;
    ret->buffer_length = 0;
    ret->max_buffer_length = max_buffer_length;
    ret->buffer_position = 0;

    return ret;
}
