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

/* List of all ZCloudStorePlugin objects */
static GSList *all_store_plugins;

/* Hash of all ZCloudModule objects, by basename */
static GSList *all_modules;

/*
 * XML parser
 */

/* user_data for the markup parser */
struct markup_parser_state {
    /* parser context */
    GMarkupParseContext *ctxt;

    /* directory in which the parse is taking place */
    const gchar *dir_name;

    /* filename of the XML file */
    gchar *filename;

    /* name of a element which must end immediately */
    gchar *open_empty_element;

    ZCloudModule *current_module;
    ZCloudStorePlugin *current_plugin;
};

static void markup_error(
    struct markup_parser_state *state,
    GMarkupParseContext *context,
    GError **error,
    gint code,
    const gchar *format,
    ...) G_GNUC_PRINTF(5, 6);

static void
markup_error(
    struct markup_parser_state *state,
    GMarkupParseContext *context,
    GError **error,
    gint code,
    const gchar *format,
    ...)
{
    gint line, col;
    va_list ap;
    gchar *msg;

    va_start(ap, format);
    msg = g_strdup_vprintf(format, ap);
    va_end(ap);

    g_markup_parse_context_get_position(context, &line, &col);

    g_set_error(error, G_MARKUP_ERROR, code,
                "%s:%d: %s", state->filename, line, msg);

    g_free(msg);
}

static GParamSpec *
markup_make_param_spec(
    struct markup_parser_state *state,
    GMarkupParseContext *context,
    const gchar **attribute_names,
    const gchar **attribute_values,
    GError **error)
{
    const gchar **i1, **i2;
    const gchar *name, *type, *blurb, *nick;
    gboolean name_ok;

    /* process attributes */
    name = type = blurb = nick = NULL;
    for (i1 = attribute_names, i2 = attribute_values;
                                *i1 && *i2; i1++, i2++) {
        if (0 == g_ascii_strcasecmp("name", *i1)) {
            name = *i2;
        } else if (0 == g_ascii_strcasecmp("type", *i1)) {
            type = *i2;
        } else if (0 == g_ascii_strcasecmp("nick", *i1)) {
            nick = *i2;
        } else if (0 == g_ascii_strcasecmp("blurb", *i1)) {
            blurb = *i2;
        } else {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
                        "'parameter' attribute '%s' not recognized", *i1);
            return NULL;
        }
    }
    if (!name) {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_INVALID_CONTENT,
                    "'parameter' attribute 'name' is required");
        return NULL;
    }
    if (!type) {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_INVALID_CONTENT,
                    "'parameter' attribute 'type' is required");
        return NULL;
    }
    if (!nick)
        nick = name;
    if (!blurb) {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_INVALID_CONTENT,
                    "'parameter' attribute 'blurb' is required");
        return NULL;
    }

    /* check that 'name' is canonical, as per glib's description */
    name_ok = TRUE;
    if (!g_ascii_islower(name[0])) {
        name_ok = FALSE;
    } else {
        const gchar *p;
        for (p = name+1; *p; p++) {
            if (!g_ascii_islower(*p) && !g_ascii_isdigit(*p) && *p != '-') {
                name_ok = FALSE;
                break;
            }
        }
    }
    if (!name_ok) {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_INVALID_CONTENT,
                    "invalid parameter name '%s'", name);
        return NULL;
    }

    if (0 == g_ascii_strcasecmp(type, "string")) {
        return g_param_spec_string(name, nick, blurb, "",
            G_PARAM_READABLE|G_PARAM_CONSTRUCT_ONLY);
    } else {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_INVALID_CONTENT,
                    "invalid parameter type '%s'", type);
        return NULL;
    }
}

static void
markup_start_element(GMarkupParseContext *context,
                   const gchar *element_name,
                   const gchar **attribute_names,
                   const gchar **attribute_values,
                   gpointer user_data,
                   GError **error)
{
    struct markup_parser_state *state = (struct markup_parser_state *)user_data;

    if (state->open_empty_element) {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_INVALID_CONTENT,
                    "'%s' element must be empty", state->open_empty_element);
        return;
    }

    /* <zcloud-module> */
    if (g_ascii_strcasecmp(element_name, "zcloud-module") == 0) {
        GSList *iter;
        ZCloudModule *module;
        gchar *basename;
        const gchar **i1, **i2;

        if (state->current_module) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "element 'zcloud-module' cannot be nested");
            return;
        }

        basename = NULL;
        for (i1 = attribute_names, i2 = attribute_values;
                                    *i1 && *i2; i1++, i2++) {
            if (0 == g_ascii_strcasecmp("basename", *i1)) {
                basename = g_strdup(*i2);
            } else {
                markup_error(state, context, error,
                            G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
                            "'zcloud-module' attribute '%s' not recognized", *i1);
                return;
            }
        }
        if (!basename) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "'zcloud-module' attribute 'basename' is required");
            return;
        }

        /* check for duplicate module */
        for (iter = all_modules; iter; iter = iter->next) {
            module = (ZCloudModule *)iter->data;
            if (0 == strcmp(module->basename, basename)) {
                markup_error(state, context, error,
                            G_MARKUP_ERROR_INVALID_CONTENT,
                            "zcloud-module with basename '%s' is already defined", basename);
                return;
            }
        }

        module = state->current_module = g_new0(ZCloudModule, 1);
        module->basename = basename;
        module->module_path = g_module_build_path(state->dir_name, basename);
        module->xml_path = g_strdup(state->filename);
        module->loaded = FALSE;

        all_modules = g_slist_append(all_modules, module);

    /* <store-plugin> */
    } else if (g_ascii_strcasecmp(element_name, "store-plugin") == 0) {
        ZCloudStorePlugin *plugin;
        gchar *prefix;
        const gchar **i1, **i2;

        if (!state->current_module) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "element 'store-plugin' must appear in a 'zcloud-module' element");
            return;
        }
        if (state->current_plugin) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "element 'store-plugin' cannot be nested");
            return;
        }

        prefix = NULL;
        for (i1 = attribute_names, i2 = attribute_values;
                                    *i1 && *i2; i1++, i2++) {
            if (0 == g_ascii_strcasecmp("prefix", *i1)) {
                prefix = g_strdup(*i2);
            } else {
                markup_error(state, context, error,
                            G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
                            "'store-plugin' attribute '%s' not recognized", *i1);
                return;
            }
        }
        if (!prefix) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "'store-plugin' attribute 'prefix' is required");
            return;
        }

        /* check for duplicate prefix */
        if (zcloud_get_store_plugin_by_prefix(prefix)) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "store plugin prefix '%s' is already defined", prefix);
            return;
        }

        plugin = state->current_plugin = g_new0(ZCloudStorePlugin, 1);
        plugin->module = state->current_module;
        plugin->type = G_TYPE_INVALID;
        plugin->prefix = prefix;
        plugin->paramspecs = g_ptr_array_new();

        /* add this plugin to the list of all store plugins */
        all_store_plugins = g_slist_append(all_store_plugins, plugin);

    /* <parameter> */
    } else if (g_ascii_strcasecmp(element_name, "parameter") == 0) {
        GParamSpec *spec;
        GPtrArray *speclist;
        guint i;

        if (!state->current_plugin) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "element 'parameter' must appear in a 'store-plugin' element");
            return;
        }

        spec = markup_make_param_spec(state, context, attribute_names,
                                      attribute_values, error);
        if (!spec)
            return;

        /* capture the floating reference to this param spec */
        g_param_spec_ref(spec);
        g_param_spec_sink(spec);

        /* search for duplicates */
        speclist = state->current_plugin->paramspecs;
        for (i = 0; i < speclist->len; i++) {
            GParamSpec *inlist = (GParamSpec *)g_ptr_array_index(speclist, i);
            if (0 == g_ascii_strcasecmp(spec->name, inlist->name)) {
                markup_error(state, context, error,
                            G_MARKUP_ERROR_INVALID_CONTENT,
                            "duplicate parameter name '%s'", spec->name);
                g_param_spec_unref(spec);
                return;
            }
        }

        /* add to the list */
        g_ptr_array_add(state->current_plugin->paramspecs, spec);

        /* this element must be closed immediately */
        state->open_empty_element = g_strdup(element_name);
    } else {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_UNKNOWN_ELEMENT,
                    "element '%s' not recognized", element_name);
        return;
    }
}

static void
markup_end_element(GMarkupParseContext *context,
                 const gchar *element_name,
                 gpointer user_data,
                 GError **error)
{
    struct markup_parser_state *state = (struct markup_parser_state *)user_data;

    if (state->open_empty_element) {
        g_assert(0 == g_ascii_strcasecmp(element_name, state->open_empty_element));
        g_free(state->open_empty_element);
        state->open_empty_element = NULL;
    }

    /* </zcloud-module> */
    if (g_ascii_strcasecmp(element_name, "zcloud-module") == 0) {
        if (!state->current_module || state->current_plugin) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "unexpected '</zcloud-module>'");
            return;
        }

        state->current_module = NULL;

    /* </store-plugin> */
    } else if (g_ascii_strcasecmp(element_name, "store-plugin") == 0) {
        if (!state->current_plugin) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "unexpected '</store-plugin>'");
            return;
        }
        state->current_plugin = NULL;

    /* </parameter> */
    } else if (g_ascii_strcasecmp(element_name, "parameter") == 0) {
        return; /* parameter is an empty element, so the end tag is boring */
    } else {
        markup_error(state, context, error,
                    G_MARKUP_ERROR_UNKNOWN_ELEMENT,
                    "end element '%s' not recognized", element_name);
        return;
    }
}

static void
markup_text(GMarkupParseContext *context,
          const gchar *text,
          gsize text_len,
          gpointer user_data,
          GError **error)
{
    struct markup_parser_state *state = (struct markup_parser_state *)user_data;
    gsize i;

    /* if there's any non whitespace, then this is unexpected.  If this changes
     * to allow text in some places, then it should check open_empty_element */
    for (i = 0; i < text_len; i++) {
        if (!g_ascii_isspace(text[i])) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "unexpected text");
            return;
        }
    }

}

static void
markup_parser_init(
    struct markup_parser_state *state,
    const gchar *dir_name,
    gchar *filename)
{
    static GMarkupParser parser = {
        markup_start_element,
        markup_end_element,
        markup_text,
        NULL,
        NULL
    };

    state->ctxt = g_markup_parse_context_new(&parser, 0, (gpointer)state, NULL);
    state->dir_name = dir_name;
    state->filename = filename;
    state->open_empty_element = NULL;
    state->current_module = NULL;
    state->current_plugin = NULL;
}

static void
markup_parser_cleanup(
    struct markup_parser_state *state)
{
    if (state->ctxt)
        g_markup_parse_context_free(state->ctxt);
    if (state->open_empty_element)
        g_free(state->open_empty_element);
}

gboolean
zc_load_module_xml(
    const gchar *xml,
    GError **error)
{
    gboolean success = FALSE;
    struct markup_parser_state state;

    markup_parser_init(&state, ".", "(string)");

    if (!g_markup_parse_context_parse(state.ctxt, xml, strlen(xml), error)) {
        goto error;
    }

    if (!g_markup_parse_context_end_parse(state.ctxt, error)) {
        goto error;
    }

    success = TRUE;

error:
    /* free everything */
    markup_parser_cleanup(&state);

    return success;
}

static gboolean
load_module_xml_file(
    const gchar *dir_name,
    const gchar *file,
    GError **error)
{
    int fd = -1;
    gboolean success = FALSE;
    struct markup_parser_state state;

    g_debug("loading module XML file '%s/%s'", dir_name, file);
    markup_parser_init(&state, dir_name, g_build_path("/", dir_name, file, NULL));

    fd = open(state.filename, O_RDONLY, 0);
    if (fd < 0) {
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_UNKNOWN,
                    "Error opening '%s': %s",
                        state.filename, strerror(errno));
        goto error;
    }

    while (1) {
        gchar buf[4096];
        gsize len;

        if ((len = read(fd, buf, sizeof(buf))) <= 0) {
            if (len == 0)
                break;
            g_set_error(error,
                        ZCLOUD_ERROR,
                        ZCERR_UNKNOWN,
                        "Error reading '%s': %s",
                            state.filename, strerror(errno));
            goto error;
        }

        if (!g_markup_parse_context_parse(state.ctxt, buf, len, error)) {
            goto error;
        }
    }

    if (!g_markup_parse_context_end_parse(state.ctxt, error)) {
        goto error;
    }

    success = TRUE;

error:
    /* free everything */
    markup_parser_cleanup(&state);
    if (fd >= 0)
        close(fd);
    if (state.filename)
        g_free(state.filename);

    return success;
}

/* Get the list of directories to search for plugins.  This comes from $ZCPLUGINPATH
 * or, if that's not set, contains only the ZCPLUGINDIR.  The returned list should be
 * freed with g_strfreev when no longer needed.
 *
 * @returns: allocated array of strings
 */
static gchar **
get_plugin_dirs(void)
{
    char *envstr = NULL;

    if ((envstr = getenv(ZCLOUD_PLUGIN_PATH_ENV)) && envstr[0] != '\0') {
        return g_strsplit(envstr, ":", -1);
    } else {
        gchar **result = g_new0(gchar *, 2);
        result[0] = g_strdup(ZCPLUGINDIR);
        return result;
    }
}

/* Scan a single plugin directory for XML files defining plugins
 *
 * Side effect: populates all_store_plugins and all_modules
 *
 * @returns: FALSE on error, with ERROR set properly
 */
static gboolean
scan_plugin_dir(
    gchar *dir_name,
    GError **error)
{
    GDir *dir;
    const gchar *elt;

    g_debug("scanning plugin directory '%s'", dir_name);
    dir = g_dir_open(dir_name, 0, error);
    if (!dir)
        return FALSE;

    while ((elt = g_dir_read_name(dir))) {
        size_t len = strlen(elt);
        /* if the file ends in .xml, load it */
        if (len > 4 && 0 == g_ascii_strcasecmp(".xml", elt + len - 4)) {
            if (!load_module_xml_file(dir_name, elt, error)) {
                g_dir_close(dir);
                return FALSE;
            }
        }
    }

    g_dir_close(dir);
    return TRUE;
}

/* Scan all plugin directories for XML files defining plugins
 *
 * Side effect: populates all_store_plugins and all_modules
 *
 * @returns: FALSE on error, with ERROR set properly
 */
static gboolean
scan_plugin_dirs(
    GError **error)
{
    gchar **dirs = get_plugin_dirs();
    gchar **iter;

    for (iter = dirs; *iter; iter++) {
        if (!scan_plugin_dir(*iter, error))
            return FALSE;
    }

    g_strfreev(dirs);

    return TRUE;
}

/*
 * Public functions
 */

gchar *
zcloud_register_store_plugin(
    const gchar *module_name,
    const gchar *prefix,
    GType type)
{
    ZCloudStorePlugin *plugin;

    g_debug("registering prefix '%s' with module '%s'", prefix, module_name);
    plugin = zcloud_get_store_plugin_by_prefix(prefix);
    if (!plugin) {
        /* (this probably leaks memory -- GLib is vague on the topic) */
        return g_strdup_printf("zcloud_register_store_plugin: no plugin with "
                "prefix '%s' is defined", prefix);
    }

    if (plugin->type != G_TYPE_INVALID) {
        return g_strdup_printf("zcloud_register_store_plugin: "
                "prefix '%s' is already registered", prefix);
    }

    if (0 != strcmp(plugin->module->basename, module_name)) {
        return g_strdup_printf("zcloud_register_store_plugin: "
                "prefix '%s' belongs to module '%s'",
                prefix, plugin->module->basename);
    }

    plugin->type = type;
    return NULL;
}

ZCloudStorePlugin *
zcloud_get_store_plugin_by_prefix(
    const gchar *prefix)
{
    GSList *iter;
    for (iter = all_store_plugins; iter; iter = iter->next) {
        ZCloudStorePlugin *plugin = (ZCloudStorePlugin *)iter->data;

        if (0 == strcmp(plugin->prefix, prefix))
            return (ZCloudStorePlugin *)plugin;
    }

    return NULL;
}

GSList *
zcloud_get_all_store_plugins(void)
{
    return all_store_plugins;
}

gboolean
zc_load_store_plugin(
    ZCloudStorePlugin *store_plugin,
    GError **error)
{
    static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
    GSList *iter;
    ZCloudModule *zcmod;
    GModule *gmod;

    g_assert(store_plugin != NULL);

    /* this entire function gets serialized */
    g_static_mutex_lock(&mutex);

    /* check whether the type is loaded after acquiring the mutex */
    if (store_plugin->type != G_TYPE_INVALID) {
        g_static_mutex_unlock(&mutex);
        return TRUE;
    }

    zcmod = store_plugin->module;

    if (zcmod->loaded) {
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_MODULE,
                    "module '%s' has already failed to load", zcmod->basename);
        goto error;
    }

    /* this process should load all of the plugins for this module, and return
     * NULL if there is an error */
    gmod = g_module_open(zcmod->module_path, 0);

    if (!gmod) {
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_MODULE,
                    "%s", g_module_error());
        goto error;
    }

    /* keep this module around for the duration */
    g_module_make_resident(gmod);

    /* check that it lived up to its advertisement */
    for (iter = all_store_plugins; iter; iter = iter->next) {
        ZCloudStorePlugin *pl = (ZCloudStorePlugin *)iter->data;
        if (pl->module == zcmod && pl->type == G_TYPE_INVALID) {
            g_set_error(error,
                        ZCLOUD_ERROR,
                        ZCERR_MODULE,
                "module '%s' did not register store prefix '%s' as promised in '%s'",
                    zcmod->basename, pl->prefix, zcmod->xml_path);
            goto error;
        }
    }

    zcmod->loaded = TRUE;

    g_static_mutex_unlock(&mutex);
    return TRUE;

error:
    g_static_mutex_unlock(&mutex);
    return FALSE;
}

/*
 * Internal functions
 */

gboolean
zc_plugins_init(GError **error)
{
    if (!g_module_supported()) {
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_UNKNOWN,
                    "%s", g_module_error());
        return FALSE;
    }

    if (!scan_plugin_dirs(error))
        return FALSE;

    return TRUE;
}

void
zc_plugins_clear(void)
{
    GSList *iter;

    for (iter = all_store_plugins; iter; iter = iter->next) {
        ZCloudStorePlugin *plugin = (ZCloudStorePlugin *)iter->data;
        guint i;

        if (plugin->prefix)
            g_free(plugin->prefix);

        for (i = 0; i < plugin->paramspecs->len; i++) {
            GParamSpec *spec = (GParamSpec *)g_ptr_array_index(plugin->paramspecs, i);
            g_param_spec_unref(spec);
        }
        g_ptr_array_free(plugin->paramspecs, TRUE);

        g_free(plugin);
    }
    g_slist_free(all_store_plugins);
    all_store_plugins = NULL;

    for (iter = all_modules; iter; iter = iter->next) {
        ZCloudModule *module = (ZCloudModule *)iter->data;

        g_assert(!module->loaded);

        if (module->basename)
            g_free(module->basename);

        if (module->module_path)
            g_free(module->module_path);

        if (module->xml_path)
            g_free(module->xml_path);

        g_free(module);
    }
    g_slist_free(all_modules);
    all_modules = NULL;
}
