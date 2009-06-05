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

#ifndef ZC_PROPERTIES_H
#define ZC_PROPERTIES_H

G_BEGIN_DECLS

/* Create a new store plugin property spec object
 *
 * @param name: name for new object (will be strdup'd)
 * @param type: property type
 * @param description: description (will be strdup'd)
 * @returns: new object
 */
G_GNUC_INTERNAL
ZCloudStorePluginPropertySpec *
zc_propspec_new(
    const gchar *name,
    GType type,
    const gchar *description);

/* Free an allocated store plugin property spec's memory
 *
 * @param spec: spec to free
 */
G_GNUC_INTERNAL
void
zc_propspec_free(
    ZCloudStorePluginPropertySpec *spec);

/* Convert a string, as given in a <property> element, into a GType.
 *
 * @param type: string naming the type
 * @param gtype: (output) resulting GType
 * @returns: TRUE on success
 */
G_GNUC_INTERNAL
gboolean
zc_propspec_string_to_gtype(
    const gchar *type,
    GType *gtype);

G_END_DECLS

#endif
