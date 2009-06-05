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


/* This file need only be included by plugin modules' source files, not by
 * libzcloud users.  It defines the data formats and functions used to
 * set up communication between libzcloud and its plugins. */

#ifndef ZCLOUD_PLUGINS_H
#define ZCLOUD_PLUGINS_H

G_BEGIN_DECLS

/*
 * Plugin registration
 */

/* plugin-creation function
 *
 * @param storenode: the node (store specification string without prefix)
 * @returns: a new object or NULL on error (with ERROR set correctly)
 */
typedef ZCloudStore *(* ZCloudStoreConstructor)(const gchar *storenode, GError **error);

/* Plugins call this function from g_module_check_init.  It is invalid to
 * call this at any other time.
 *
 * @param module_name: name of the module registering this plugin
 * @param prefix: prefix for this plugin
 * @param type: GType to instantiate for this plugin
 * @returns: NULL on success, or an error string on failure (return this string
 *   from g_module_check_init)
 */
gchar *zcloud_register_store_plugin(const gchar *module_name,
                    const gchar *prefix, ZCloudStoreConstructor constructor);

/*
 * Types
 */

typedef struct ZCloudModule_s {
    /* short name of the module (e.g., "disk" or "cloudsplosion") */
    gchar *basename;

    /* full pathname of the loadable module (shared object or DLL) */
    gchar *module_path;

    /* full pathname of the XML file defining this module */
    gchar *xml_path;

    /* TRUE if this module is loaded */
    gboolean loaded;

    /* list of ZCloudStorePluginPropertySpec objects */
    GSList *property_specs;
} ZCloudModule;

typedef struct ZCloudStorePluginPropertySpec_s {
    /* name of the property - lowercase, underscores */
    gchar *name;

    /* property type, translated to a GType */
    GType type;

    /* short description of the property */
    gchar *description;
} ZCloudStorePluginPropertySpec;

typedef struct ZCloudStorePlugin_s {
    /* prefix identifying the plugin */
    gchar *prefix;

    /* the module defining this store plugin */
    ZCloudModule *module;

    /* constructor for the store class, or NULL if the plugin isn't loaded */
    ZCloudStoreConstructor constructor;

    /* list of ZCloudStorePluginPropertySpec objects */
    GSList *property_specs;
} ZCloudStorePlugin;

/*
 * Low-level interface
 */

/* Get a ZCloudStorePlugin object by prefix
 *
 * @param prefix: the store prefix
 * @returns: corresponding ZCloudStorePlugin, or NULL if not found
 */
ZCloudStorePlugin *zcloud_get_store_plugin_by_prefix(const gchar *prefix);

/* Get all ZCloudStorePlugin objects
 *
 * @returns: GSList containing all ZCloudStorePlugin objects
 */
GSList *zcloud_get_all_store_plugins(void);

/* Load the module for the given store plugin if necessary, and ensure that the
 * ZCloudStorePlugin's 'type' field is non-NULL on return.
 *
 * @param store_plugin: the plugin to load
 * @returns: FALSE on error, with ERROR set properly
 */
gboolean zcloud_load_store_plugin(ZCloudStorePlugin *store_plugin, GError **error);

G_END_DECLS

#endif
