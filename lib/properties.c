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

ZCloudPropertySpec *
zc_propspec_new(
    const gchar *name,
    GType type,
    const gchar *description)
{
    ZCloudPropertySpec *rv = g_new0(ZCloudPropertySpec, 1);
    rv->name = g_strdup(name);
    rv->type = type;
    rv->description = g_strdup(description);
    return rv;
}

void
zc_propspec_free(
    ZCloudPropertySpec *spec)
{
    if (spec) {
        if (spec->name)
            g_free(spec->name);
        if (spec->description)
            g_free(spec->description);
        g_free(spec);
    }
}

gboolean
zc_propspec_string_to_gtype(
    const gchar *type,
    GType *gtype)
{
    if (0 == g_strcasecmp(type, "string")) {
        *gtype = G_TYPE_STRING;
    } else if (0 == g_strcasecmp(type, "int")) {
        *gtype = G_TYPE_INT;
    } else if (0 == g_strcasecmp(type, "boolean")) {
        *gtype = G_TYPE_BOOLEAN;
    } else {
        return FALSE;
    }

    return TRUE;
}

gboolean
zc_property_value_from_string(
    ZCloudPropertySpec *spec,
    const gchar *str,
    GValue *destination)
{
    g_assert(spec != NULL);
    g_assert(str != NULL);

    if (spec->type == G_TYPE_STRING) {
        g_value_init(destination, spec->type);
        g_value_set_string(destination, str);
    } else if (spec->type == G_TYPE_INT) {
        gchar *end = NULL;
        gint64 rv;
        if (*str == '\0')
            return FALSE;
        rv = g_ascii_strtoll(str, &end, 10);
        if (*end != '\0')
            return FALSE;
        /* TODO: range-checking */
        g_value_init(destination, spec->type);
        g_value_set_int(destination, (gint)rv);
    } else if (spec->type == G_TYPE_BOOLEAN) {
        static char *names[] = {
            /* even indices are false, odd indices are true */
            "n",        "y",
            "no",       "yes",
            "f",        "t",
            "false",    "true",
            "off",      "on",
            NULL
        };
        gboolean result;
        gchar **iter;

        for (iter = names; *iter; iter++) {
            if (0 == g_strcasecmp(str, *iter))
                break;
        }
        if (!*iter)
            return FALSE;

        result = (iter - names) & 1;

        g_value_init(destination, spec->type);
        g_value_set_boolean(destination, result);
    } else {
        g_assert_not_reached();
    }

    return TRUE;
}
