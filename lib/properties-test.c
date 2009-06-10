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


#include "test.h"

void
test_properties(void)
{
    GType type;
    ZCloudPropertySpec *spec;
    GValue val = {0, };
    
    /*
     * bogus values and edge cases
     */

    not_ok(zc_propspec_string_to_gtype("joules", &type),
            "zc_propspec_string_to_gtype of bogus type");

    ok(zc_propspec_string_to_gtype("bOOlean", &type),
            "zc_propspec_string_to_gtype(bOOlean) (case insensitivity)");
    is_int(type, G_TYPE_BOOLEAN, "..returns the correct type");

    spec = zc_propspec_new("testprop", G_TYPE_BOOLEAN, "for testing");
    ok(spec != NULL, "zc_propspec_new");
    is_string(spec->name, "testprop", "..name");
    is_int(spec->type, G_TYPE_BOOLEAN, "..type");
    is_string(spec->description, "for testing", "..description");
    zc_propspec_free(spec);

    /*
     * property types
     */

    /* string */
    {   
        ok(zc_propspec_string_to_gtype("string", &type),
                "zc_propspec_string_to_gtype(string) succeeds");
        is_int(type, G_TYPE_STRING, "..returns the correct type");
        spec = zc_propspec_new("testprop", G_TYPE_STRING, "for testing");

        /* there's no need to test all of the possible values */
        ok(zc_property_value_from_string(spec, "hello, world", &val), "parse property value");
        is_string(g_value_get_string(&val), "hello, world", "..correctly");
        g_value_unset(&val);

        zc_propspec_free(spec);
    }

    /* integer */
    {   
        ok(zc_propspec_string_to_gtype("int", &type),
                "zc_propspec_string_to_gtype(int) succeeds");
        is_int(type, G_TYPE_INT, "..returns the correct type");
        spec = zc_propspec_new("testprop", G_TYPE_INT, "for testing");

        /* there's no need to test all of the possible values */
        ok(zc_property_value_from_string(spec, "299", &val), "parse integer property value");
        is_int(g_value_get_int(&val), 299, "..correctly");
        g_value_unset(&val);

        ok(zc_property_value_from_string(spec, "-80", &val), "parse negative property value");
        is_int(g_value_get_int(&val), -80, "..correctly");
        g_value_unset(&val);

        zc_propspec_free(spec);
    }

    /* boolean */
    {   
        ok(zc_propspec_string_to_gtype("boolean", &type),
                "zc_propspec_string_to_gtype(boolean) succeeds");
        is_int(type, G_TYPE_BOOLEAN, "..returns the correct type");
        spec = zc_propspec_new("testprop", G_TYPE_BOOLEAN, "for testing");

        /* there's no need to test all of the possible values */
        ok(zc_property_value_from_string(spec, "oN", &val), "parse property value 'oN'");
        is_gboolean(g_value_get_boolean(&val), TRUE, "..correctly");
        g_value_unset(&val);

        ok(zc_property_value_from_string(spec, "falSe", &val), "parse property value 'falSe'");
        is_gboolean(g_value_get_boolean(&val), FALSE, "..correctly");
        g_value_unset(&val);

        not_ok(zc_property_value_from_string(spec, "sorta", &val), "handle invalid value 'sorta'");
        not_ok(zc_property_value_from_string(spec, "sorta", &val), "handle invalid empty string");

        zc_propspec_free(spec);
    }
}
