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
#include <string.h>
#include <unistd.h>
#include "zcloud.h"

typedef struct ParamData_s {
    guint n_params;
    GParameter *params;
} ParamData;

#define PARAM_DATA_INIT {0, NULL};

static gboolean
parse_param_arg(
    const gchar *option_name,
    const gchar *value,
    gpointer data,
    GError **error)
{
    ParamData *pd = (ParamData *) data;
    gchar *equal_sign = strchr(value, '=');
    gchar *p_val;

    if (!equal_sign) {
        g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
            "value for option '%s' does not contain '='", value, option_name);
        return FALSE;
    }
    if (value == equal_sign) {
        g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
            "value '%s' for option '%s' does not have a parameter name", value,
            option_name);
        return FALSE;
    }

    pd->params = g_renew(GParameter, pd->params, pd->n_params+1);
    bzero(&pd->params[pd->n_params], sizeof(GParameter));
    pd->params[pd->n_params].name = g_strndup(value, equal_sign-value);
    g_value_init(&pd->params[pd->n_params].value, G_TYPE_STRING);
    p_val = g_strdup(equal_sign+1);
    g_value_set_string(&pd->params[pd->n_params].value, p_val);
    g_free(p_val);

    pd->n_params++;

    return TRUE;
}

static void
parse_param_arg_destroy(gpointer data)
{
    ParamData *pd = (ParamData *) data;
    guint i;

    for(i = 0; i < pd->n_params; i++) {
        g_free((gchar *) pd->params[i].name);
        g_value_unset(&pd->params[i].value);
    }
    g_free(pd->params);
}

int main(int argc, char **argv)
{
    gchar *plugin_path = NULL;
    ParamData param_data = PARAM_DATA_INIT;
    gboolean nul_suf = FALSE;

    const GOptionEntry entries[] = {
        {"plugin-path", 0, 0, G_OPTION_ARG_STRING, &plugin_path, "set the plugin path", "PATH"},
        {"param", 0, 0, G_OPTION_ARG_CALLBACK, parse_param_arg, "set store property NAME to VALUE", "NAME=VALUE"},
        {"null", 0, 0, G_OPTION_ARG_CALLBACK, &nul_suf, "(list only) print NUL after each key instead of newline", NULL},
        {NULL},
    };

    gchar *operation = NULL, *store_spec = NULL, *key = NULL;

    GOptionGroup *main_group;
    GOptionContext *context;
    GError *error = NULL;
    ZCloudStore *store;
    int ret = 1;

    /* die on anything worse than a message */
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

    if (!zcloud_init(&error)) {
        fprintf(stderr, "Could not initialize libzcloud: %s\n", error->message);
        g_error_free(error);
        goto cleanup;
    }

    /* setup parsing */
    context = g_option_context_new("OPERATION STORE [KEY]");
    main_group = g_option_group_new(NULL, NULL, NULL, &param_data, parse_param_arg_destroy);
    g_option_group_add_entries(main_group, entries);
    g_option_context_set_main_group(context, main_group);

    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        fprintf(stderr, "option parsing failed: %s\n", error->message);
        goto cleanup;
    }

    if (argc < 3) {
        gchar *help_str = g_option_context_get_help(context, TRUE, NULL);
        fprintf(stderr, "%s", help_str);
        g_free(help_str);
        goto cleanup;
    }

    operation = argv[1];
    store_spec = argv[2];
    if (argc > 3) key = argv[3];

    if (plugin_path && setenv(ZCLOUD_PLUGIN_PATH_ENV, plugin_path, 1) < 0) {
        fprintf(stderr, "setting " ZCLOUD_PLUGIN_PATH_ENV " failed\n");
        goto cleanup;
    }

    store = zcloud_store_newv(store_spec, param_data.n_params, param_data.params, &error);
    if (!store) {
        fprintf(stderr, "Could not load store '%s': %s\n", store_spec,
            error->message);
        g_error_free(error);
        goto cleanup;
    }

    if (!strcmp("create", operation)) {
        if (!key) {
            fprintf(stderr, "You must supply a key for the operation '%s'\n", operation);
            goto cleanup;
        }
        if (zcloud_store_create(store, key, NULL, &error)) {
            fprintf(stderr, "Successfully created '%s' in '%s'\n", key,
                store_spec);
            ret = 0;
        } else {
            fprintf(stderr, "Failed to create '%s' in '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
        }
    } else if (!strcmp("exists", operation)) {
        if (!key) {
            fprintf(stderr, "You must supply a key for the operation '%s'\n", operation);
            goto cleanup;
        }
        if (zcloud_store_exists(store, key, NULL, &error)) {
            fprintf(stderr, "'%s' exists in '%s'\n", key, store_spec);
            ret = 0;
        } else {
            if (ZCLOUD_ERROR == error->domain && ZCERR_MISSING == error->code) {
                fprintf(stderr, "'%s' does not exist in '%s'\n", key, store_spec);
                ret = 2;
            } else {
                fprintf(stderr, "Could not test for the existence of '%s' in '%s': %s\n",
                    key, store_spec, error->message);
            }
            g_error_free(error);
        }
    } else if (!strcmp("delete", operation)) {
        if (!key) {
            fprintf(stderr, "You must supply a key for the operation '%s'\n", operation);
            goto cleanup;
        }
        if (zcloud_store_delete(store, key, NULL, &error)) {
            fprintf(stderr, "Successfully created '%s' in '%s'\n", key,
                store_spec);
            ret = 0;
        } else {
            fprintf(stderr, "Failed to delete '%s' from '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
        }
    } else if (!strcmp("list", operation)) {
        ZCloudListConsumer *lc = ZCLOUD_LIST_CONSUMER(
            zcloud_fd_list_consumer(STDOUT_FILENO, nul_suf? '\0' : '\n'));
        if (!zcloud_store_list(store, key, lc, NULL, &error)) {
            fprintf(stderr, "Failed to delete '%s' from '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
        }
    } else {
        fprintf(stderr, "unknown operation %s\n", operation);
    }

cleanup:
    /* this frees main_group */
    g_option_context_free(context);
    return ret;
}
