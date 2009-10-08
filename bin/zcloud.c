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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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
    gchar *data = NULL;
    gchar *filename = NULL;
    gboolean append = FALSE;

    const GOptionEntry entries[] = {
        {"plugin-path", 0, 0, G_OPTION_ARG_STRING, &plugin_path, "set the plugin path", "PATH"},
        {"param", 0, 0, G_OPTION_ARG_CALLBACK, parse_param_arg, "set store property NAME to VALUE", "NAME=VALUE"},
        {"append", 0, 0, G_OPTION_ARG_NONE, &nul_suf, "(download only) append to FILENAME (instead of clobbering it outright)", NULL},
        {"data", 0, 0, G_OPTION_ARG_STRING, &data, "(upload only) upload STRING to the key", "STRING"},
        {"filename", 0, 0, G_OPTION_ARG_STRING, &filename, "(download and upload only) download to, or upload from, FILENAME", "FILENAME"},
        {"null", 0, 0, G_OPTION_ARG_NONE, &nul_suf, "(list only) print NUL after each key instead of newline", NULL},
        {NULL},
    };

    gchar *operation = NULL, *store_spec = NULL, *key = NULL;

    GOptionGroup *main_group;
    GOptionContext *context;
    GError *error = NULL;
    ZCloudStore *store;
    int ret = 1;

    ZCloudUploadProducer *up_prod = NULL;
    ZCloudDownloadConsumer *down_con = NULL;
    ZCloudListConsumer *list_con = NULL;
    int file_fd = -1;

    /* TODO: handle locale environment variables, calling setlocale() */

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

    /* validate options based on the operation */
    if (!key && strcmp("list", operation)) {
        fprintf(stderr, "You must supply a key for the operation '%s'\n", operation);
        goto cleanup;
    }
    if (nul_suf && strcmp("list", operation)) {
        fprintf(stderr, "Operation '%s' does not accept --null option\n", operation);
        goto cleanup;
    }
    if (data && strcmp("upload", operation)) {
        fprintf(stderr, "Operation '%s' does not accept --data option\n", operation);
        goto cleanup;
    }
    if (append && strcmp("download", operation)) {
        fprintf(stderr, "Operation '%s' does not accept --append option\n", operation);
        goto cleanup;
    }
    if (filename && strcmp("upload", operation) && strcmp("download", operation)) {
        fprintf(stderr, "Operation '%s' does not accept --filename option\n", operation);
        goto cleanup;
    }
    if (data && filename) {
        fprintf(stderr, "You must specify only one of the --filename and --data options\n");
        goto cleanup;
    }
    if (append && !filename) {
        fprintf(stderr, "--append can only be specified with --filename\n");
        goto cleanup;
    }

    /* try to apply --plugin-path */
    if (plugin_path && setenv(ZCLOUD_PLUGIN_PATH_ENV, plugin_path, 1) < 0) {
        fprintf(stderr, "setting " ZCLOUD_PLUGIN_PATH_ENV " failed\n");
        goto cleanup;
    }

    /* now try to create the store, including any --param stuff we got */
    store = zcloud_store_newv(store_spec, param_data.n_params, param_data.params, &error);
    if (!store) {
        fprintf(stderr, "Could not load store '%s': %s\n", store_spec,
            error->message);
        g_error_free(error);
        goto cleanup;
    }

    /* onto the actual operations! */
    if (!strcmp("create", operation)) {
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
        if (zcloud_store_delete(store, key, NULL, &error)) {
            fprintf(stderr, "Successfully deleted '%s' in '%s'\n", key,
                store_spec);
            ret = 0;
        } else {
            fprintf(stderr, "Failed to delete '%s' from '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
        }

    } else if (!strcmp("upload", operation)) {
        /* create appropriate upload producer */
        if (data) {
            up_prod = ZCLOUD_UPLOAD_PRODUCER(
                zcloud_memory_upload_producer(data, strlen(data)));
        } else if (filename) {
            file_fd = open(filename, O_RDONLY);
            if (file_fd < 0) {
                fprintf(stderr, "could not open filename '%s' for reading: %s\n",
                    filename, strerror(errno));
                goto cleanup;
            }
            up_prod = ZCLOUD_UPLOAD_PRODUCER(
                zcloud_fd_upload_producer(file_fd));
        } else {
            up_prod = ZCLOUD_UPLOAD_PRODUCER(
                zcloud_fd_upload_producer(STDIN_FILENO));
        }
        /* now do it! */
        if (zcloud_store_upload(store, key, up_prod, NULL, &error)) {
            fprintf(stderr, "Successfully uploaded data to '%s' in '%s'\n", key,
                store_spec);
            ret = 0;
        } else {
            fprintf(stderr, "Failed to upload data to '%s' in '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
        }

    } else if (!strcmp("download", operation)) {
        if (filename) {
            if (append) {
                file_fd = open(filename, O_WRONLY|O_APPEND);
            } else {
                /* create with mode 666, modified by umask */
                file_fd = creat(filename, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            }
            if (file_fd < 0) {
                fprintf(stderr, "could not open filename '%s' for writing: %s\n",
                    filename, strerror(errno));
                goto cleanup;
            }
            down_con = ZCLOUD_DOWNLOAD_CONSUMER(
                zcloud_fd_download_consumer(file_fd));
        } else {
            down_con = ZCLOUD_DOWNLOAD_CONSUMER(
                zcloud_fd_download_consumer(STDOUT_FILENO));
        }
        /* now do it! */
        if (zcloud_store_download(store, key, down_con, NULL, &error)) {
            fprintf(stderr, "Successfully downloaded data from '%s' in '%s'\n", key,
                store_spec);
            ret = 0;
        } else {
            fprintf(stderr, "Failed to download data from '%s' in '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
            if (filename && !append) {
                unlink(filename);
            }
        }

    } else if (!strcmp("list", operation)) {
        list_con = ZCLOUD_LIST_CONSUMER(
            zcloud_fd_list_consumer(STDOUT_FILENO, nul_suf? '\0' : '\n'));
        if (!zcloud_store_list(store, key, list_con, NULL, &error)) {
            fprintf(stderr, "Failed to delete '%s' from '%s': %s\n", key,
                store_spec, error->message);
            g_error_free(error);
        }

    } else {
        fprintf(stderr, "unknown operation %s\n", operation);
    }

cleanup:    
    g_option_context_free(context); /* this frees main_group */
    if (store) g_object_unref(store);
    if (up_prod) g_object_unref(up_prod);
    if (down_con) g_object_unref(down_con);
    if (list_con) g_object_unref(list_con);
    if (file_fd >= 0) close(file_fd);

    return ret;
}
