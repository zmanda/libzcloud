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
    const gchar *prefix G_GNUC_UNUSED,
    const gchar *storespec_suffix,
    GError **error)
{
    DiskStore *store;

    if(!g_file_test(storespec_suffix, G_FILE_TEST_IS_DIR)) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PLUGIN,
            "'%s' is not a directory", storespec_suffix);
        return NULL;
    }

    store = g_object_new(DISK_TYPE_STORE, NULL);
    g_assert(store != NULL);
    store->basedir = g_strdup(storespec_suffix);

    return ZCLOUD_STORE(store);
}
