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


#include "test.h"

#define MOCK_TYPE_STORE (mock_store_get_type())
#define MOCK_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOCK_TYPE_STORE, MockStore))
#define MOCK_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MOCK_TYPE_STORE, MockStoreClass))
#define MOCK_IS_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOCK_TYPE_STORE))
#define MOCK_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOCK_TYPE_STORE))
#define MOCK_STORE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MOCK_TYPE_STORE, MockStoreClass))

typedef struct MockStore_s {
    ZCloudStore parent;
} MockStore;

typedef struct MockStoreClass_s {
    ZCloudStoreClass parent_class;
} MockStoreClass;

/*
 * Method implementations
 */

/*
 * Class mechanics
 */

static void
instance_init(
    MockStore *self)
{
}

static void
finalize_impl(
    GObject *obj)
{
    //MockStore *self = MOCK_STORE(obj);
}

static void
class_init(
    MockStoreClass *cls)
{
    GObjectClass *goc = G_OBJECT_CLASS(cls);
    //ZCloudStoreClass *store_class = ZCLOUD_STORE_CLASS(cls);

    goc->finalize = finalize_impl;
}

static GType
mock_store_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (MockStoreClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            (GClassFinalizeFunc) NULL,
            NULL /* class_data */,
            sizeof (MockStore),
            0 /* n_preallocs */,
            (GInstanceInitFunc) instance_init,
            NULL
        };

        type = g_type_register_static (ZCLOUD_TYPE_STORE, "MockStore", &info,
                                       (GTypeFlags)0);
    }

    return type;
}

void
mock_setup(void)
{
    GError *error = NULL;

    zc_plugins_clear();
    zc_load_module_xml(
"<zcloud-module basename=\"mock\">\n"
" <store-plugin prefix=\"mock\">\n"
"  <parameter name=\"bool\" type=\"string\" blurb=\"simple param\" />\n"
" </store-plugin>\n"
"</zcloud-module>", &error);
    if (error)
        g_error("could not load mock plugin: %s", error->message);

    zcloud_register_store_plugin("mock", "mock", MOCK_TYPE_STORE);
}
