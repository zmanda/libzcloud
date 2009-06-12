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

#ifndef ZCLOUD_PLUGINS_H
#define ZCLOUD_PLUGINS_H

#include "store.h"

G_BEGIN_DECLS

/*
 * Plugin registration
 */

/* Modules call this function from g_module_check_init.  It is invalid to
 * call this at any other time.
 *
 * @param module_name: name of the module registering this plugin
 * @param prefix: prefix for this plugin
 * @param type: GType to instantiate for this plugin
 * @returns: NULL on success, or an error string on failure (return this string
 *   from g_module_check_init)
 */
gchar *zcloud_register_store_plugin(
            const gchar *module_name,
            const gchar *prefix,
            GType type);

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
} ZCloudModule;

typedef struct ZCloudStorePlugin_s {
    /* prefix identifying the plugin */
    gchar *prefix;

    /* the module defining this store plugin */
    ZCloudModule *module;

    /* type of the store class to be instantiated */
    GType type;

    /* array of GParamSpecs */
    GPtrArray *paramspecs;
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

G_END_DECLS

#endif
