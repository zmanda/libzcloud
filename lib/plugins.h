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

#ifndef ZC_PLUGINS_H
#define ZC_PLUGINS_H

G_BEGIN_DECLS

typedef struct ZCModule_s {
    /* short name of the module (e.g., "disk" or "cloudsplosion") */
    gchar *basename;

    /* full pathname of the loadable module (shared object or DLL) */
    gchar *module_path;

    /* full pathname of the XML file defining this module */
    gchar *xml_path;

    /* the module itself, or NULL if not loaded */
    GModule *module;
} ZCModule;

typedef struct ZCStorePlugin_s {
    /* module defining this store plugin */
    ZCModule *module;

    /* prefix identifying the plugin */
    gchar *prefix;

    /* GType for the store class, or NULL if the plugin isn't loaded */
    GType *type;
} ZCStorePlugin;

/* Initialize the plugins, loading all of the relevant plugin information.
 *
 * @returns: FALSE on error, with ERROR set properly
 */
G_GNUC_INTERNAL
gboolean zc_plugins_init(GError **error);

/* Get a ZCStorePlugin object by prefix
 *
 * @param prefix: the store prefix
 * @returns: corresponding ZCStorePlugin, or NULL if not found
 */
G_GNUC_INTERNAL
ZCStorePlugin *zc_get_store_plugin_by_prefix(gchar *prefix);

/* Get all ZCStorePlugin objects
 *
 * @returns: GSList containing all ZCStorePlugin objects
 */
G_GNUC_INTERNAL
GSList *zc_get_all_store_plugins(void);

/* Load the module for the given store plugin, and ensure
 * that the ZCStorePlugin's 'type' field is non-NULL on
 * return.
 *
 * @param store_plugin: the plugin to load
 * @returns: FALSE on error, with ERROR set properly
 */
//G_GNUC_INTERNAL
gboolean zc_load_store_plugin(ZCStorePlugin *store_plugin);

G_END_DECLS

#endif
