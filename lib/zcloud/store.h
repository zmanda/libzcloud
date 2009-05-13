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

#ifndef ZCLOUD_STORE_H
#define ZCLOUD_STORE_H

#include <glib.h>
#include <glib-object.h>
#include "address.h"
#include "address_template.h"
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

    GHashTable *simple_properties;
} ZCloudStore;

typedef struct ZCloudStoreClass_s {
    GObjectClass parent_class;

    gboolean (*create)(
        ZCloudStore *self,
        ZCloudAddress *address,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*upload)(
        ZCloudStore *self,
        ZCloudAddress *address,
        ZCloudUploadProducer *upload,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*download)(
        ZCloudStore *self,
        ZCloudAddress *address,
        ZCloudDownloadConsumer *download,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*delete)(
        ZCloudStore *self,
        ZCloudAddress *address,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*exists)(
        ZCloudStore *self,
        ZCloudAddress *address,
        ZCloudProgressListener *progress,
        GError **error);
    gboolean (*list)(
        ZCloudStore *self,
        ZCloudAddressTemplate *template,
        ZCloudListConsumer *list,
        ZCloudProgressListener *progress,
        GError **error);
    ZCloudAddress *(*parse_address)(
        ZCloudStore *self,
        gchar *address_str,
        GError **error);
    ZCloudAddressTemplate *(*parse_address_template)(
        ZCloudStore *self,
        gchar *address_template_str,
        GError **error);
} ZCloudStoreClass;

gboolean zcloud_store_create(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_upload(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudUploadProducer *upload,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_download(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudDownloadConsumer *download,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_delete(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_exists(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudProgressListener *progress,
    GError **error);

gboolean zcloud_store_list(
    ZCloudStore *self,
    ZCloudAddressTemplate *template,
    ZCloudListConsumer *list,
    ZCloudProgressListener *progress,
    GError **error);

ZCloudAddress *zcloud_store_parse_address(
    ZCloudStore *self,
    gchar *address_str,
    GError **error);

ZCloudAddressTemplate *zcloud_store_parse_address_template(
    ZCloudStore *self,
    gchar *address_template_str,
    GError **error);

G_END_DECLS

#endif
