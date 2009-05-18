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

#include "disk.h"

GType disk_store_get_type(void);
#define DISK_TYPE_STORE (disk_store_get_type())
#define DISK_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), DISK_TYPE_STORE, DiskStore))
#define DISK_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), DISK_TYPE_STORE, DiskStoreClass))
#define DISK_IS_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DISK_TYPE_STORE))
#define DISK_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DISK_TYPE_STORE))
#define DISK_STORE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), DISK_TYPE_STORE, DiskStoreClass))

typedef struct DiskStore_s {
    ZCloudStore parent;

    gchar *basedir;
} DiskStore;

typedef struct DiskStoreClass_s {
    ZCloudStoreClass parent_class;
} DiskStoreClass;

/*
 * Method implementations
 */

/*
 * Class mechanics
 */

void
instance_init(
    DiskStore *self)
{
}

void
finalize_impl(
    GObject *obj)
{
    DiskStore *self = DISK_STORE(obj);

    if (self->basedir) {
        g_free(self->basedir);
    }
}

void
class_init(
    DiskStoreClass *cls)
{
    GObjectClass *goc = G_OBJECT_CLASS(cls);
    ZCloudStoreClass *store_class = ZCLOUD_STORE_CLASS(cls);

    goc->finalize = finalize_impl;
}

GType
disk_store_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (DiskStoreClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) NULL,
            (GClassFinalizeFunc) NULL,
            NULL /* class_data */,
            sizeof (DiskStore),
            0 /* n_preallocs */,
            (GInstanceInitFunc) instance_init,
            NULL
        };

        type = g_type_register_static (ZCLOUD_TYPE_STORE, "DiskStore", &info,
                                       (GTypeFlags)0);
    }

    return type;
}

ZCloudStore *
disk_constructor(
    const gchar *storenode,
    GError **error)
{
    DiskStore *store;

    if(!g_file_test(storenode, G_FILE_TEST_IS_DIR)) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PLUGIN,
            "'%s' is not a directory", storenode);
        return NULL;
    }

    store = g_object_new(DISK_TYPE_STORE, NULL);
    g_assert(store != NULL);
    store->basedir = g_strdup(storenode);

    return ZCLOUD_STORE(store);
}
