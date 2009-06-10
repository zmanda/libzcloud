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


#ifndef ZCLOUD_STORE_H
#define ZCLOUD_STORE_H

#include "download_consumer.h"
#include "list_consumer.h"
#include "progress_listener.h"
#include "upload_producer.h"

G_BEGIN_DECLS

GType zcloud_store_get_type(void);
#define ZCLOUD_TYPE_STORE (zcloud_store_get_type())
#define ZCLOUD_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_STORE, ZCloudStore))
#define ZCLOUD_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_STORE, ZCloudStoreClass))
#define ZCLOUD_IS_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_STORE))
#define ZCLOUD_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_STORE))
#define ZCLOUD_STORE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_STORE, ZCloudStoreClass))

typedef struct ZCloudStore_s {
    GObject parent;
} ZCloudStore;

typedef struct ZCloudStoreClass_s {
    GObjectClass parent_class;

    gboolean (*setup)(
        ZCloudStore *self,
        const gchar *suffix,
        gint n_parameters,
        GParameter *parameters,
        GError **error);

    /* general methods */
    gboolean (*create)(
        ZCloudStore *self,
        gchar *address,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*upload)(
        ZCloudStore *self,
        gchar *address,
        ZCloudUploadProducer *upload,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*download)(
        ZCloudStore *self,
        gchar *address,
        ZCloudDownloadConsumer *download,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*delete)(
        ZCloudStore *self,
        gchar *address,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*exists)(
        ZCloudStore *self,
        gchar *address,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*list)(
        ZCloudStore *self,
        gchar *template,
        ZCloudListConsumer *list,
        ZCloudProgressListener *progress,
        GError **error);
} ZCloudStoreClass;

/* Create a new ZCloudStore object with the given prefix.
 *
 * @param storespec: store specifier
 * @returns: NULL on error, with ERROR set properly
 */
ZCloudStore *zcloud_store_new(const gchar *storespec, GError **error);

/*
 * Method stubs
 */

gboolean zcloud_store_setup(
    ZCloudStore *self,
    const gchar *suffix,
    gint n_parameters,
    GParameter *parameters,
    GError **error);

gboolean zcloud_store_create(
    ZCloudStore *self,
    gchar *address,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_upload(
    ZCloudStore *self,
    gchar *address,
    ZCloudUploadProducer *upload,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_download(
    ZCloudStore *self,
    gchar *address,
    ZCloudDownloadConsumer *download,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_delete(
    ZCloudStore *self,
    gchar *address,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_exists(
    ZCloudStore *self,
    gchar *address,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_list(
    ZCloudStore *self,
    gchar *template,
    ZCloudListConsumer *list,
    ZCloudProgressListener *progress,
    GError **error);

G_END_DECLS

#endif
