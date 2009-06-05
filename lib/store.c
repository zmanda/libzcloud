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

GType
zcloud_store_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (ZCloudStoreClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) NULL,
            (GClassFinalizeFunc) NULL,
            NULL /* class_data */,
            sizeof (ZCloudStore),
            0 /* n_preallocs */,
            (GInstanceInitFunc) NULL,
            NULL
        };

        type = g_type_register_static (G_TYPE_OBJECT, "ZCloudStore", &info,
                                       (GTypeFlags)G_TYPE_FLAG_ABSTRACT);
    }

    return type;
}

/*
 * method stubs
 */

gboolean
zcloud_store_create(
    ZCloudStore *self,
    gchar *address,
    ZCloudProgressListener *progress,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self);
    g_assert(c->create != NULL);
    return (c->create)(self, address, progress, error);
}

gboolean
zcloud_store_upload(
    ZCloudStore *self,
    gchar *address,
    ZCloudUploadProducer *upload,
    ZCloudProgressListener *progress,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->upload != NULL); \
    return (c->upload)(self, address, upload, progress, error);
}

gboolean
zcloud_store_download(
    ZCloudStore *self,
    gchar *address,
    ZCloudDownloadConsumer *download,
    ZCloudProgressListener *progress,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->download != NULL); \
    return (c->download)(self, address, download, progress, error);
}

gboolean
zcloud_store_delete(
    ZCloudStore *self,
    gchar *address,
    ZCloudProgressListener *progress,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->delete != NULL); \
    return (c->delete)(self, address, progress, error);
}

gboolean
zcloud_store_exists(
    ZCloudStore *self,
    gchar *address,
    ZCloudProgressListener *progress,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->exists != NULL); \
    return (c->exists)(self, address, progress, error);
}

gboolean
zcloud_store_list(
    ZCloudStore *self,
    gchar *template,
    ZCloudListConsumer *list,
    ZCloudProgressListener *progress,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->list != NULL); \
    return (c->list)(self, template, list, progress, error);
}
