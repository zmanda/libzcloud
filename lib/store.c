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

    if (!zc_load_store_plugin(plugin, error))
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

/* canonicalize the parameter name to glib's standards */
static gchar *
canonicalize_param_name(
    const gchar *name)
{
    gchar *p;
    gchar *rv = g_strdup(name);
    
    for (p = rv; *p; p++) {
        if (g_ascii_isupper(*p))
            *p = g_ascii_tolower(*p);
        else if (!g_ascii_isalnum(*p))
            *p = '-';
    }

    return rv;
}

ZCloudStore *
zcloud_store_newv(
    const gchar *storespec,
    gint n_input_params,
    GParameter *input_params,
    GError **error)
{
    const gchar *suffix = NULL;
    ZCloudStorePlugin *plugin;
    ZCloudStore *store = NULL;
    GParameter *my_params = NULL;
    guint i;

    plugin = store_new_begin(storespec, &suffix, error);
    if (!plugin)
        return NULL;

    /* sadly, we need to re-do the nicely packaged parameters array,
     * to canonicalize the names, add default values, and transform any
     * values that require it. */
    my_params = g_new(GParameter, plugin->paramspecs->len);
    for (i = 0; i < plugin->paramspecs->len; i++) {
        /* "borrow" a reference to the name from the paramspec */
        my_params[i].name = ((GParamSpec *)g_ptr_array_index(plugin->paramspecs, i))->name;
    }

    for (i = 0; i < (guint)n_input_params; i++) {
        gchar *canon_name;
        GParamSpec *spec = NULL;
        guint j;

        /* search for this parameter in my_params */
        canon_name = canonicalize_param_name(input_params[i].name);
        for (j = 0; j < plugin->paramspecs->len; j++) {
            if (0 == strcmp(canon_name, my_params[j].name)) {
                spec = (GParamSpec *)g_ptr_array_index(plugin->paramspecs, j);
                break;
            }
        }
        g_free(canon_name);
        if (!spec) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "unknown parameter '%s'", input_params[i].name);
            goto error;
        }

        /* and try transforming the value */
        g_value_init(&my_params[j].value, spec->value_type);
        if (!g_param_value_convert(spec, &input_params[i].value,
                                   &my_params[j].value, TRUE)) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "cannot convert parameter value for '%s'", input_params[i].name);
            goto error;
        }
    }

    /* set the default value for any parameters not supplied */
    for (i = 0; i < plugin->paramspecs->len; i++) {
        if (G_VALUE_TYPE(&my_params[i].value) == G_TYPE_INVALID) {
            GParamSpec *spec = (GParamSpec *)g_ptr_array_index(plugin->paramspecs, i);
            g_value_init(&my_params[i].value, spec->value_type);
            g_param_value_set_default(spec, &my_params[i].value);
        }
    }

    store = store_new_finish(plugin, suffix, plugin->paramspecs->len, my_params, error);

error:
    for (i = 0; i < plugin->paramspecs->len; i++) {
        g_value_unset(&my_params[i].value);
        /* my_params[i].name is a borrowed reference */
    }
    g_free(my_params);

    return store;
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
    GParameter *params;
    const gchar *name;
    guint i;

    plugin = store_new_begin(storespec, &suffix, error);
    if (!plugin)
        return NULL;

    /* set up a new params array */
    params = g_new0(GParameter, plugin->paramspecs->len);
    for (i = 0; i < plugin->paramspecs->len; i++) {
        /* "borrow" a reference to the name from the paramspec */
        params[i].name = ((GParamSpec *)g_ptr_array_index(plugin->paramspecs, i))->name;
    }

    va_start(var_args, first_param_name);
    for (name = first_param_name; name; name = va_arg(var_args, const gchar*)) {
        GParamSpec *spec;
        gchar *errmsg = NULL;
        gchar *canon_name = NULL;

        /* look up this parameter in the plugin's paramspecs */
        canon_name = canonicalize_param_name(name);
        for (i = 0; i < plugin->paramspecs->len; i++) {
            spec = (GParamSpec *)g_ptr_array_index(plugin->paramspecs, i);
            if (0 == g_ascii_strcasecmp(canon_name, spec->name))
                break;
            spec = NULL;
        }
        g_free(canon_name);
        if (!spec) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "unknown parameter '%s'", name);
            goto error;
        }

        /* fill in the name and value of the parameter */
        g_value_init(&params[i].value, G_PARAM_SPEC_VALUE_TYPE(spec));
        G_VALUE_COLLECT(&params[i].value, var_args, 0, &errmsg);
        if (errmsg) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "%s", errmsg);
            g_free(errmsg);
            /* glib seems to think it's a bad idea to leave this GValue
             * partially initialized */
            bzero(&params[i].value, sizeof(GValue));
            goto error;
        }
    }
    va_end(var_args);

    /* set the default value for any parameters not supplied */
    for (i = 0; i < plugin->paramspecs->len; i++) {
        if (G_VALUE_TYPE(&params[i].value) == G_TYPE_INVALID) {
            GParamSpec *spec = (GParamSpec *)g_ptr_array_index(plugin->paramspecs, i);
            g_value_init(&params[i].value, spec->value_type);
            g_param_value_set_default(spec, &params[i].value);
        }
    }

    /* finally, call store_new_finish with the collection of parameters */
    store = store_new_finish(plugin, suffix,
        plugin->paramspecs->len, params, error);
    
error:
    if (params) {
        guint i;
        for (i = 0; i < plugin->paramspecs->len; i++) {
            g_value_unset(&params[i].value);
            /* params[i].name is a borrowed reference */
        }
        g_free(params);
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
