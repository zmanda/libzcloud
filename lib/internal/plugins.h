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


#ifndef ZC_PLUGINS_H
#define ZC_PLUGINS_H

G_BEGIN_DECLS

/* Initialize the plugins, loading all of the relevant plugin information.
 *
 * @returns: FALSE on error, with ERROR set properly
 */
G_GNUC_INTERNAL
gboolean zc_plugins_init(GError **error);

/*
 * testing utility functions
 */

/* Clear all of the plugin metadata; note that this can *not* be called after
 * modules have been loaded, although it is safe to add plugin constructors.
 */
G_GNUC_INTERNAL
void zc_plugins_clear(void);

/* Parse the given XML, creating the corresponding ZCloudStorePlugin and
 * ZCloudModule objects along the way.  The base directory for the module
 * is ".".
 *
 * Side effect: populates all_store_plugins and all_modules
 *
 * @param xml: xml data to load
 * @returns: FALSE on error, with ERROR set properly
 */
G_GNUC_INTERNAL
gboolean
zc_load_module_xml(
    const gchar *xml,
    GError **error);

G_END_DECLS

#endif
