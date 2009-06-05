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

ZCloudStore *
zcloud_new(const gchar *storespec, GError **error)
{
    gchar *prefix, *colon;
    ZCloudStorePlugin *plugin;
    ZCloudStore *store;

    if ((colon = strchr(storespec, ':')) == NULL)
        goto unknown_spec;
    prefix = g_strndup(storespec, colon-storespec);

    plugin = zcloud_get_store_plugin_by_prefix(prefix);
    g_free(prefix);
    if (!plugin)
        goto unknown_spec;

    if (!zcloud_load_store_plugin(plugin, error))
        return NULL;

    g_assert(plugin->constructor != NULL);
    store = plugin->constructor(colon+1, error);

    if (!store) {
        g_assert(*error != NULL);
        return NULL;
    }
    g_assert(!*error);
    return store;

unknown_spec:
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_MODULE,
                    "unknown store specification '%s'",
                    prefix);
        return NULL;
}

gboolean
zcloud_init(GError **error)
{
    static gboolean initialized = FALSE;

    if (initialized)
        return TRUE;

    if (!g_thread_supported ()) g_thread_init (NULL);

    /* TODO: can this be called multiple times? */
    g_type_init();

    if (!zc_plugins_init(error))
        return FALSE;

    initialized = TRUE;

    return TRUE;
}
