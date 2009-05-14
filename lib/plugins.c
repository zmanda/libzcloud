/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: LGPL 2.1/GPL 2.0
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
 *
 * The Original Code is Zmanda Incorporated code.
 *
 * The Initial Developer of the Original Code is
 *  Zmanda Incorporated
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Nikolas Coukouma <atrus@zmanda.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License Version 2 or later (the "GPL"),
 * in which case the provisions of the GPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL and not to allow others to
 * use your version of this file under the terms of the LGPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of either the the GPL or the LGPL.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * and GNU General Public License along with libzcloud. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * ***** END LICENSE BLOCK ***** */

#include "internal.h"
#include "zcloud/zcloud.h"

/* List of all ZCStorePlugin objects */
static GSList *all_store_plugins;

/* List of all ZCModule objects */
static GSList *all_modules;

/*
 * Plugin initialization support
 */

/* user_data for the markup parser */
struct markup_parser_state {
    /* directory in which the parse is taking place */
    const gchar *dir_name;

    /* filename of the XML file */
    gchar *filename;

    ZCModule *current_module;
    ZCStorePlugin *current_plugin;
};

/* Functions for a SAX parser to parse the module XML files */

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

static void
markup_start_element(GMarkupParseContext *context,
                   const gchar *element_name,
                   const gchar **attribute_names,
                   const gchar **attribute_values,
                   gpointer user_data,
                   GError **error)
{
    struct markup_parser_state *state = (struct markup_parser_state *)user_data;

    if (g_strcasecmp(element_name, "zcloud-module") == 0) {
        GSList *iter;
        ZCModule *module;
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
            if (0 == strcasecmp("basename", *i1)) {
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
            module = (ZCModule *)iter->data;
            if (0 == strcmp(module->basename, basename)) {
                markup_error(state, context, error,
                            G_MARKUP_ERROR_INVALID_CONTENT,
                            "zcloud-module with basename '%s' is already defined", basename);
                return;
            }
        }

        module = state->current_module = g_new0(ZCModule, 1);
        module->basename = basename;
        module->module_path = g_module_build_path(state->dir_name, basename);
        module->xml_path = g_strdup(state->filename);
        module->module = NULL;

        all_modules = g_slist_append(all_modules, module);
    } else if (g_strcasecmp(element_name, "store-plugin") == 0) {
        ZCStorePlugin *plugin;
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
            if (0 == strcasecmp("prefix", *i1)) {
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
        if (zc_get_store_plugin_by_prefix(prefix)) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "store plugin prefix '%s' is already defined", prefix);
            return;
        }

        plugin = state->current_plugin = g_new0(ZCStorePlugin, 1);
        plugin->module = state->current_module;
        plugin->prefix = prefix;
        plugin->type = NULL;

        all_store_plugins = g_slist_append(all_store_plugins, plugin);
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

    if (g_strcasecmp(element_name, "zcloud-module") == 0) {
        if (!state->current_module) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "unexpected '</zcloud-module>'");
            return;
        }
        if (state->current_plugin) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "expected '</store-plugin>'");
            return;
        }
        state->current_module = NULL;
    } else if (g_strcasecmp(element_name, "store-plugin") == 0) {
        if (!state->current_plugin) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "unexpected '</store-plugin>'");
            return;
        }
        state->current_plugin = NULL;
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

    /* if there's any non whitespace, then this is unexpected */
    for (i = 0; i < text_len; i++) {
        if (!isspace(text[i])) {
            markup_error(state, context, error,
                        G_MARKUP_ERROR_INVALID_CONTENT,
                        "unexpected text");
            return;
        }
    }

}

/* Load and parse a single XML file, creating the corresponding ZCStorePlugin and
 * ZCModule objects along the way.
 *
 * Side effect: populates all_store_plugins and all_modules
 *
 * @param dir_name: directory containing the file
 * @param file: filename
 * @returns: FALSE on error, with ERROR set properly
 */
static gboolean
load_module_xml(
    const gchar *dir_name,
    const gchar *file,
    GError **error)
{
    GMarkupParseContext *ctxt = NULL;
    int fd = -1;
    struct markup_parser_state state = { NULL, NULL, NULL, NULL };
    static GMarkupParser parser = {
        markup_start_element,
        markup_end_element,
        markup_text,
        NULL,
        NULL
    };

    state.dir_name = dir_name;
    state.filename = g_build_path("/", dir_name, file, NULL);

    fd = open(state.filename, O_RDONLY, 0);
    if (fd < 0) {
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_UNKNOWN,
                    "Error opening '%s': %s",
                        state.filename, strerror(errno));
        goto error;
    }

    ctxt = g_markup_parse_context_new(&parser, 0, (gpointer)&state, NULL);
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

        if (!g_markup_parse_context_parse(ctxt, buf, len, error)) {
            goto error;
        }
    }

    if (!g_markup_parse_context_end_parse(ctxt, error)) {
        goto error;
    }

error:
    /* free everything */
    if (ctxt)
        g_markup_parse_context_free(ctxt);
    if (fd >= 0)
        close(fd);
    if (state.filename)
        g_free(state.filename);

    return (*error)? FALSE : TRUE;
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

    if ((envstr = getenv("ZCPLUGINPATH")) && envstr[0] != '\0') {
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
    
    dir = g_dir_open(dir_name, 0, error);
    if (!dir)
        return FALSE;

    while ((elt = g_dir_read_name(dir))) {
        size_t len = strlen(elt);
        /* if the file ends in .xml, load it */
        if (len > 4 && 0 == g_ascii_strcasecmp(".xml", elt + len - 4)) {
            if (!load_module_xml(dir_name, elt, error)) {
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





static gboolean
zcloud_load_plugin(
    const gchar *plugin_name,
    GError **error)
{
    gchar *path;
    GModule *module;

    path = g_module_build_path(ZCPLUGINDIR, plugin_name);

    module = g_module_open(path, 0);

    if (!module) {
        g_set_error(error,
                    ZCLOUD_ERROR,
                    ZCERR_UNKNOWN,
                    "%s", g_module_error());
        return FALSE;
    }

    return TRUE;
}

/*
 * Public functions
 */

void
zcloud_register_plugin(
    const gchar *module_name,
    const gchar *prefix,
    GType *type)
{
}

/*
 * Internal functions
 */

ZCStorePlugin *
zc_get_store_plugin_by_prefix(
    gchar *prefix)
{
    GSList *iter = all_store_plugins;
    while (iter) {
        ZCStorePlugin *plugin = (ZCStorePlugin *)iter->data;

        if (0 == strcmp(plugin->prefix, prefix))
            return plugin;
    }

    return NULL;
}

GSList *
zc_get_all_store_plugins(void)
{
    return all_store_plugins;
}


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

