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

#define mkstub(methname, ...) \
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->methname != NULL); \
    return (c->methname)(self, __VA_ARGS__);
#define mkstub0(methname, ...) \
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self); \
    g_assert(c->methname != NULL); \
    return (c->methname)(self);

gboolean
zcloud_store_create(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudProgressListener *progress,
    GError **error)
{
    mkstub(create,
        address, progress, error);
}

gboolean
zcloud_store_upload(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudUploadProducer *upload,
    ZCloudProgressListener *progress,
    GError **error)
{
    mkstub(upload,
        address, upload, progress, error);
}

gboolean
zcloud_store_download(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudDownloadConsumer *download,
    ZCloudProgressListener *progress,
    GError **error)
{
    mkstub(download,
        address, download, progress, error);
}

gboolean
zcloud_store_delete(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudProgressListener *progress,
    GError **error)
{
    mkstub(delete,
        address, progress, error);
}

gboolean
zcloud_store_exists(
    ZCloudStore *self,
    ZCloudAddress *address,
    ZCloudProgressListener *progress,
    GError **error)
{
    mkstub(exists,
        address, progress, error);
}

gboolean
zcloud_store_list(
    ZCloudStore *self,
    ZCloudAddressTemplate *template,
    ZCloudListConsumer *list,
    ZCloudProgressListener *progress,
    GError **error)
{
    mkstub(list,
        template, list, progress, error);
}

ZCloudAddress *
zcloud_store_parse_address(
    ZCloudStore *self,
    gchar *address_str,
    GError **error)
{
    mkstub(parse_address,
        address_str, error);
}

ZCloudAddressTemplate *
zcloud_store_parse_address_template(
    ZCloudStore *self,
    gchar *address_template_str,
    GError **error)
{
    mkstub(parse_address_template,
        address_template_str, error);
}
