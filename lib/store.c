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
#include <gobject/gvaluecollector.h>

/*
 * Constructor
 */

static ZCloudStorePlugin *
store_new_begin(
    const gchar *storespec,
    const gchar **suffix,
    GError **error)
{
    gchar *prefix, *colon;
    ZCloudStorePlugin *plugin;

    if ((colon = strchr(storespec, ':')) == NULL)
        goto unknown_spec;
    *suffix = colon+1;
    prefix = g_strndup(storespec, colon-storespec);

    plugin = zcloud_get_store_plugin_by_prefix(prefix);
    g_free(prefix);
    if (!plugin)
        goto unknown_spec;

    return plugin;

unknown_spec:
    g_set_error(error,
                ZCLOUD_ERROR,
                ZCERR_MODULE,
                "unknown store specification '%s'",
                storespec);
    return NULL;
}

static ZCloudStore *
store_new_finish(
    ZCloudStorePlugin *plugin,
    const gchar *suffix,
    gint n_parameters,
    GParameter *parameters,
    GError **error)
{
    ZCloudStore *store;

    if (!zcloud_load_store_plugin(plugin, error))
        return NULL;

    /* create the object */
    g_assert(plugin->type != 0);
    store = (ZCloudStore *)g_object_new(plugin->type, NULL);
    g_assert(ZCLOUD_IS_STORE(store));

    /* and call setup */
    if (!zcloud_store_setup(store, suffix, n_parameters, parameters, error)) {
        g_object_unref(store);
        g_assert(!error || *error != NULL);
        return NULL;
    }

    return store;
}

ZCloudStore *
zcloud_store_newv(
    const gchar *storespec,
    gint n_parameters,
    GParameter *parameters,
    GError **error)
{
    const gchar *suffix = NULL;
    ZCloudStorePlugin *plugin;

    plugin = store_new_begin(storespec, &suffix, error);
    if (!plugin)
        return NULL;

    return store_new_finish(plugin, suffix, n_parameters, parameters, error);
}

ZCloudStore *
zcloud_store_new(
        const gchar *storespec,
        GError **error,
        const gchar *first_param_name,
        ...)
{
    va_list var_args;
    ZCloudStore *store = NULL;
    ZCloudStorePlugin *plugin = NULL;
    const gchar *suffix = NULL;
    GArray *params = NULL;
    const gchar *name;

    /* take the short way out, if possible */
    if (!first_param_name)
        return zcloud_store_newv(storespec, 0, NULL, error);

    plugin = store_new_begin(storespec, &suffix, error);
    if (!plugin)
        return NULL;

    params = g_array_new(FALSE, TRUE, sizeof(GParameter));
    va_start(var_args, first_param_name);
    for (name = first_param_name; name; name = va_arg(var_args, const gchar*)) {
        guint i;
        GParamSpec *spec;
        GParameter *param;
        gchar *errmsg = NULL;

        /* look up this parameter in the plugin */
        for (i = 0; i < plugin->paramspecs->len; i++) {
            spec = (GParamSpec *)g_ptr_array_index(plugin->paramspecs, i);
            if (0 == g_ascii_strcasecmp(name, spec->name))
                break;
            spec = NULL;
        }
        if (!spec) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "unknown parameter '%s'", name);
            goto error;
        }

        /* expand the array and get a new parameter */
        g_array_set_size(params, params->len + 1);
        param = &g_array_index(params, GParameter, params->len - 1);

        /* fill in the name and value of the parameter */
        param->name = name; /* name will stay allocated through the store_new_finish call */
        g_value_init(&param->value, G_PARAM_SPEC_VALUE_TYPE(spec));
        G_VALUE_COLLECT(&param->value, var_args, 0, &errmsg);
        if (errmsg) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "%s", errmsg);
            g_free(errmsg);
            goto error;
        }
    }
    va_end(var_args);

    /* finally, call store_new_finish with the collection of parameters */
    store = store_new_finish(plugin, suffix,
        params->len, (GParameter *)params->data, error);
    
error:
    if (params) {
        guint i;
        for (i = 0; i < params->len; i++) {
            GParameter *param = &g_array_index(params, GParameter, i);
            g_value_unset(&param->value);
        }
        g_array_free(params, 1);
    }

    return store;
}

/*
 * Method implementations
 */

static gboolean
setup_impl(
    ZCloudStore *self,
    const gchar *suffix,
    gint n_parameters,
    GParameter *parameters,
    GError **error)
{
    if (n_parameters > 0) {
        /* arbitrarily pick the first parameter to complain about */
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
            "Unknown parameter '%s'", parameters[0].name);
        return FALSE;
    }

    return TRUE;
}

/*
 * Class mechanics
 */

static void
class_init(
    ZCloudStoreClass *zc_class)
{
    zc_class->setup = setup_impl;
}

GType
zcloud_store_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (ZCloudStoreClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
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
zcloud_store_setup(
    ZCloudStore *self,
    const gchar *suffix,
    gint n_parameters,
    GParameter *parameters,
    GError **error)
{
    ZCloudStoreClass *c = ZCLOUD_STORE_GET_CLASS(self);
    g_assert(c->setup != NULL);
    return (c->setup)(self, suffix, n_parameters, parameters, error);
}

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
