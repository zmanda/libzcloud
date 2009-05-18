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
#include <openssl/md5.h>
#include "zcloud.h"

/* class mechanics */
static void
class_init(ZCloudMemoryUploadProducerClass *klass);

/* prototypes for method implementations */
static gsize
read(
    ZCloudUploadProducer *self,
    gpointer buffer,
    gsize bytes,
    GError **error);

static gsize
get_size(ZCloudUploadProducer *self, GError **error);

static GByteArray*
calculate_md5(
    ZCloudUploadProducer *self,
    GError **error);

static gboolean
reset(ZCloudUploadProducer *self, GError **error);

GType
zcloud_memory_upload_producer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudMemoryUploadProducerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudMemoryUploadProducer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_UPLOAD_PRODUCER,
                                      "ZCloudMemoryUploadProducer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

static void
class_init(ZCloudMemoryUploadProducerClass *klass)
{
    ZCloudUploadProducerClass *up_class = ZCLOUD_UPLOAD_PRODUCER_CLASS(klass);

    up_class->read = read;
    up_class->get_size = get_size;
    up_class->calculate_md5 = calculate_md5;
    up_class->reset = reset;
}


ZCloudMemoryUploadProducer *
zcloud_memory_upload_producer(gconstpointer buffer, guint buffer_length)
{
    ZCloudMemoryUploadProducer *ret;

    g_assert(buffer);

    ret = g_object_new(ZCLOUD_TYPE_MEMORY_UPLOAD_PRODUCER, NULL);
    ret->buffer = buffer;
    ret->buffer_length = buffer_length;
    ret->buffer_position = 0;

    return ret;
}

static gsize
read(
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
get_size(ZCloudUploadProducer *o, GError **error)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(o);
    return self->buffer_length;
}

static GByteArray*
calculate_md5(
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

static gboolean
reset(ZCloudUploadProducer *o, GError **error)
{
    ZCloudMemoryUploadProducer *self = ZCLOUD_MEMORY_UPLOAD_PRODUCER(o);

    self->buffer_position = 0;
    return TRUE;
}
