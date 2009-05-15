/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: LGPL 2.1/GPL 2.0
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
 *
 * The Original Code is Zmanda Incorporated code.
 *
 * The Initial Developer of the Original Code is
 *  Zmanda Incorporated
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Nikolas Coukouma <atrus@zmanda.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License Version 2 or later (the "GPL"),
 * in which case the provisions of the GPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL and not to allow others to
 * use your version of this file under the terms of the LGPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of either the the GPL or the LGPL.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * and GNU General Public License along with libzcloud. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * ***** END LICENSE BLOCK ***** */

#include <glib.h>
#include <glib-object.h>
#include <string.h>
#include "zcloud/upload_producer.h"
#include "zcloud/memory_upload_producer.h"

/* class mechanics */
static void
zc_memory_producer_class_init(ZCloudMemoryUploadProducerClass *klass);

static void
zc_memory_producer_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);

static void
zc_memory_producer_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);

/* prototypes for method implementations */
static gsize
zc_memory_producer_read(
    ZCloudUploadProducer *self,
    gpointer buffer,
    gsize bytes,
    GError **error);

static gsize
zc_memory_producer_get_size(ZCloudUploadProducer *self, GError **error);

static GByteArray*
zc_memory_producer_calculate_md5(
    ZCloudUploadProducer *self,
    GError **error);

static void
zc_memory_producer_reset(ZCloudUploadProducer *self, GError **error);

GType
zcloud_memory_upload_producer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudMemoryUploadProducerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) zc_memory_producer_class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudMemoryUploadProducer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(G_TYPE_OBJECT, "ZCloudMemoryUploadProducer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

static void
zc_memory_producer_class_init(ZCloudMemoryUploadProducerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    ZCloudUploadProducerClass *up_class = ZCLOUD_UPLOAD_PRODUCER_CLASS(klass);
    GParamSpec *pspec;

    gobject_class->get_property = zc_memory_producer_get_property;
    gobject_class->set_property = zc_memory_producer_set_property;

    up_class->read = zc_memory_producer_read;
    up_class->get_size = zc_memory_producer_get_size;
    up_class->calculate_md5 = zc_memory_producer_calculate_md5;
    up_class->reset = zc_memory_producer_reset;

    pspec = g_param_spec_pointer("buffer",
        "ZCloudMemoryUploadProducer's buffer",
        "Buffer to read data from (for uploading)",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class,
        ZCLOUD_PROP_MEMORY_UPLOAD_PRODUCER_BUFFER, pspec);

    pspec = g_param_spec_uint("buffer-length",
        "ZCloudMemoryUploadProducer's buffer length",
        "Length of the buffer to read data from (for uploading)",
        0,
        G_MAXUINT,
        0,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property(gobject_class,
        ZCLOUD_PROP_MEMORY_UPLOAD_PRODUCER_BUFFER_LENGTH, pspec); 

    pspec = g_param_spec_uint("buffer-position",
        "ZCloudMemoryUploadProducer's buffer position",
        "Current position within the buffer",
        0,
        G_MAXUINT,
        0,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE | G_PARAM_READABLE);
    g_object_class_install_property(gobject_class,
        ZCLOUD_PROP_MEMORY_UPLOAD_PRODUCER_BUFFER_POSITION, pspec); 
}

static void
zc_memory_producer_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(object);

    switch (property_id) {
    case ZCLOUD_PROP_MEMORY_UPLOAD_PRODUCER_BUFFER_POSITION:
        g_value_set_uint(value, self->buffer_position);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
zc_memory_producer_set_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(object);

    switch (property_id) {
    case ZCLOUD_PROP_MEMORY_UPLOAD_PRODUCER_BUFFER:
        self->buffer = g_value_get_pointer(value);
        break;
    case ZCLOUD_PROP_MEMORY_UPLOAD_PRODUCER_BUFFER_LENGTH:
        self->buffer_length = g_value_get_uint(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

gsize zcloud_upload_producer_read(
    ZCloudUploadProducer *o,
    gpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(o);

    /* check the number of bytes remaining, just to be safe */
    if (bytes > self->buffer_length - self->buffer_position)
        bytes = self->buffer_length - self->buffer_position;

    memcpy((char *) buffer, self->buffer + self->buffer_position, bytes);
    self->buffer_position += bytes;

    return bytes;
}

static gsize
zc_memory_producer_get_size(ZCloudUploadProducer *o, GError **error)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(o);
    return self->buffer_length;
}

static GByteArray*
zc_memory_producer_calculate_md5(
    ZCloudUploadProducer *o,
    GError **error)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(o);
    MD5_CTX md5_ctx;
    GByteArray *ret;

    ret = g_byte_array_sized_new(ZCLOUD_MD5_HASH_BYTE_LEN);
    g_byte_array_set_size(ret, ZCLOUD_MD5_HASH_BYTE_LEN);

    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, self->buffer, self->buffer_length);
    MD5_Final(ret->data, &md5_ctx);

    return ret;
}

static void
zc_memory_producer_reset(ZCloudUploadProducer *o, GError **error)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(o);

    self->buffer_position = 0;
}
