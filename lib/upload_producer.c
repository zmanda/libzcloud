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


#include "zcloud.h"

GType
zcloud_upload_producer_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (ZCloudUploadProducerClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) NULL,
            (GClassFinalizeFunc) NULL,
            NULL /* class_data */,
            sizeof (ZCloudUploadProducer),
            0 /* n_preallocs */,
            (GInstanceInitFunc) NULL,
            NULL
        };

        type = g_type_register_static (G_TYPE_OBJECT, "ZCloudUploadProducer", &info,
                                       (GTypeFlags)G_TYPE_FLAG_ABSTRACT);
    }

    return type;
}

/*
 * method stubs
 */

gsize
zcloud_upload_producer_read(
    ZCloudUploadProducer *self,
    gpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudUploadProducerClass *c = ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(self);
    g_assert(c->read != NULL);
    return (c->read)(self, buffer, bytes, error);
}

gsize
zcloud_upload_producer_get_size(
    ZCloudUploadProducer *self,
    GError **error)
{
    ZCloudUploadProducerClass *c = ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(self);
    g_assert(c->get_size != NULL);
    return (c->get_size)(self, error);
}

GByteArray *
zcloud_upload_producer_calculate_md5(
    ZCloudUploadProducer *self,
    GError **error)
{
    ZCloudUploadProducerClass *c = ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(self);
    g_assert(c->calculate_md5 != NULL);
    return (c->calculate_md5)(self, error);
}

gboolean
zcloud_upload_producer_reset(
    ZCloudUploadProducer *self,
    GError **error)
{
    ZCloudUploadProducerClass *c = ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(self);
    g_assert(c->reset != NULL);
    return (c->reset)(self, error);
}
