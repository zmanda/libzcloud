/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/* ***** BEGIN LICENSE BLOCK ***** Version: LGPL 2.1/GPL 2.0 This file is part
 * of libzcloud.
 *
 * libzcloud is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License (the LGPL) as published
 * by the Free Software Foundation, either version 2.1 of the LGPL, or (at your
 * option) any later version.
 *
 * libzcloud is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * The Original Code is Zmanda Incorporated code.
 *
 * The Initial Developer of the Original Code is Zmanda Incorporated Portions
 * created by the Initial Developer are Copyright (C) 2009 the Initial
 * Developer. All Rights Reserved.
 *
 * Contributor(s): Nikolas Coukouma <atrus@zmanda.com>
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU General Public License Version 2 or later (the "GPL"), in which case the
 * provisions of the GPL are applicable instead of those above. If you wish to
 * allow use of your version of this file only under the terms of either the
 * GPL and not to allow others to use your version of this file under the terms
 * of the LGPL, indicate your decision by deleting the provisions above and
 * replace them with the notice and other provisions required by the GPL. If
 * you do not delete the provisions above, a recipient may use your version of
 * this file under the terms of either the the GPL or the LGPL.
 *
 * You should have received a copy of the GNU Lesser General Public License and
 * GNU General Public License along with libzcloud. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * ***** END LICENSE BLOCK ***** */

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

#define mkstub(methname, ...) \
    ZCloudUploadProducerClass *c = ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(self); \
    g_assert(c->methname != NULL); \
    return (c->methname)(self, __VA_ARGS__);
#define mkstub0(methname) \
    ZCloudUploadProducerClass *c = ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(self); \
    g_assert(c->methname != NULL); \
    return (c->methname)(self);

gsize
zcloud_upload_producer_read(
    ZCloudUploadProducer *self,
    gpointer buffer,
    gsize bytes,
    GError **error)
{
    mkstub(read,
        buffer, bytes, error);
}

gsize
zcloud_upload_producer_get_size(
    ZCloudUploadProducer *self,
    GError **error)
{
    mkstub(get_size,
        error);
}

GByteArray *
zcloud_upload_producer_calculate_md5(
    ZCloudUploadProducer *self,
    GError **error)
{
    mkstub(calculate_md5,
        error);
}

void
zcloud_upload_producer_reset(
    ZCloudUploadProducer *self,
    GError **error)
{
    mkstub(reset,
        error);
}
