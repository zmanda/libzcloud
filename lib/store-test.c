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

#define FAKE_TYPE_STORE (fake_store_get_type())
#define FAKE_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), FAKE_TYPE_STORE, FakeStore))
#define FAKE_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), FAKE_TYPE_STORE, FakeStoreClass))
#define FAKE_IS_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FAKE_TYPE_STORE))
#define FAKE_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FAKE_TYPE_STORE))
#define FAKE_STORE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), FAKE_TYPE_STORE, FakeStoreClass))

typedef struct FakeStore_s {
    ZCloudStore parent;

    gchar *suffix;
    gchar *param_str;
} FakeStore;

typedef struct FakeStoreClass_s {
    ZCloudStoreClass parent_class;
} FakeStoreClass;

static GType fake_store_get_type(void);

/*
 * Method implementations
 */

/*
 * Class mechanics
 */

static void
finalize_impl(
    GObject *obj)
{
    FakeStore *self = FAKE_STORE(obj);
    if (self->suffix)
        g_free(self->suffix);
    if (self->param_str)
        g_free(self->param_str);
}

static gboolean
setup_impl(
    ZCloudStore *pself,
    const gchar *suffix,
    gint n_parameters,
    GParameter *parameters,
    GError **error)
{
    FakeStore *self = FAKE_STORE(pself);

    self->suffix = g_strdup(suffix);

    while (n_parameters--) {
        if (0 == g_ascii_strcasecmp(parameters->name, "param-str")) {
            self->param_str = g_value_dup_string(&parameters->value);
        } else {
            g_assert_not_reached();
        }
        parameters++;
    }

    return TRUE;
}

static void
class_init(
    FakeStoreClass *cls)
{
    GObjectClass *goc = G_OBJECT_CLASS(cls);
    ZCloudStoreClass *store_class = ZCLOUD_STORE_CLASS(cls);

    store_class->setup = setup_impl;
    goc->finalize = finalize_impl;
}

static GType
fake_store_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (FakeStoreClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            (GClassFinalizeFunc) NULL,
            NULL /* class_data */,
            sizeof (FakeStore),
            0 /* n_preallocs */,
            (GInstanceInitFunc) NULL,
            NULL
        };

        type = g_type_register_static (ZCLOUD_TYPE_STORE, "FakeStore", &info,
                                       (GTypeFlags)0);
    }

    return type;
}

static void
fake_setup(void)
{
    GError *error = NULL;

    zc_plugins_clear();
    zc_load_module_xml(
"<zcloud-module basename=\"fake\">\n"
" <store-plugin prefix=\"fake\">\n"
"  <parameter name=\"param-str\" type=\"string\" blurb=\"string param\" />\n"
" </store-plugin>\n"
"</zcloud-module>", &error);
    if (error)
        g_error("could not load fake plugin: %s", error->message);

    zcloud_register_store_plugin("fake", "fake", FAKE_TYPE_STORE);
}

void
test_store(void)
{
    ZCloudStore *store;
    FakeStore *fstore;
    GError *error = NULL;

    fake_setup();

    /* no properties (test defaults) */
    {
        store = zcloud_store_new("fake:foo", &error, NULL);
        fstore = FAKE_STORE(store);
        gerror_is_clear(&error, "create fake store with no properties:");
        is_string(fstore->param_str, "", " param-str is set to its default");
        g_object_unref(store);
    }

    /* varargs */
    {
        store = zcloud_store_new("fake:foo", &error,
                "param-str", "mystr",
                NULL);
        fstore = FAKE_STORE(store);
        gerror_is_clear(&error, "create fake store with varargs properties:");
        is_string(fstore->param_str, "mystr", " param_str is set correctly");
        g_object_unref(store);
    }

    /* GParameter array */
    {
        guint i;
        GParameter params[1];
        bzero(params, sizeof(params));

        i = 0;

        params[i].name = "param-str";
        g_value_init(&params[i].value, G_TYPE_STRING);
        g_value_set_string(&params[i].value, "mystr");
        i++;

        store = zcloud_store_newv("fake:foo", i, params, &error);
        fstore = FAKE_STORE(store);
        gerror_is_clear(&error, "create fake store with an array of properties:");
        is_string(fstore->param_str, "mystr", " param_str is set correctly");
        g_object_unref(store);

        for (i = 0; i < G_N_ELEMENTS(params); i++) {
            g_value_unset(&params[i].value);
        }
    }
}
