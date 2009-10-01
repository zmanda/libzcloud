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


#include <stdio.h>
#include "zcloud.h"

int main(int argc, char **argv)
{
    gchar *plugin_dir = NULL;

    const GOptionEntry entries[] = {
        {"plugin-dir", 0, 0, G_OPTION_ARG_STRING, &plugin_dir, "set the plugin directory", "DIR"},
        {NULL},
    };

    gchar *operation = NULL, *store_spec = NULL, *key = NULL;

    GOptionContext *context;
    GError *error = NULL;
    ZCloudStore *store;

    context = g_option_context_new("bar");
    g_option_context_add_main_entries(context, entries, "OPERATION STORE [KEY]");
    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        fprintf(stderr, "option parsing failed: %s\n", error->message);
        return 1;
    }

    if (argc != 3) {
        fprintf(stderr, "%s", g_option_context_get_help(context, TRUE, NULL));
        return 1;
    }

    operation = argv[1];
    store_spec = argv[2];
    if (argc > 3)
        key = argv[3];

    if (!zcloud_init(&error)) {
        fprintf(stderr, "Could not initialize libzcloud: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    if (plugin_dir && setenv(ZCLOUD_PLUGIN_PATH_ENV, plugin_dir, 1) < 0) {
        fprintf(stderr, "setting " ZCLOUD_PLUGIN_PATH_ENV " failed\n");
        return 1;
    }

    store = zcloud_store_new(store_spec, &error, NULL);
    if (!store) {
        fprintf(stderr, "Could not load store '%s': %s\n", store_spec,
            error->message);
        g_error_free(error);
        return 1;
    }

    if (!strcmp("create", operation)) {
        if (!zcloud_store_create(store, key, NULL, &error)) {
            fprintf(stderr, "Failed to create '%s' in '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
            return 1;
        }
    } else if (!strcmp("exists", operation)) {
        if (!zcloud_store_exists(store, key, NULL, &error)) {
            int ret = 1;
            if (ZCLOUD_ERROR == error->domain && ZCERR_MISSING == error->code) {
                fprintf(stderr, "Could not test for the existence of '%s' in '%s': %s\n",
                    key, store_spec, error->message);
                ret = 2;
            }
            g_error_free(error);
            return ret;
        }
    } else if (!strcmp("delete", operation)) {
        if (!zcloud_store_delete(store, key, NULL, &error)) {
            fprintf(stderr, "Failed to delete '%s' from '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
            return 1;
        }
    } else {
        fprintf(stderr, "unknown operation %s\n", operation);
        return 1;
    }

    return 0;
}
