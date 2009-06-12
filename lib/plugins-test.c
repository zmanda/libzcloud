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

static void
load_xml(const gchar *xml, GError **error)
{
    gboolean res;

    zc_plugins_clear();
    res = zc_load_module_xml(xml, error);

    /* check that error and res agree */
    g_assert((res && !*error) || (!res && *error));
}

#define get_paramspec(pl, i) ((GParamSpec *)g_ptr_array_index((pl)->paramspecs, (i)))

static void test_xml_parser(void) {
    GError *error = NULL; ZCloudStorePlugin *pl;

    load_xml("<zcloud-module basename=\"disk\">"
        "<store-plugin prefix=\"disk\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "XML: single plugin in a simple module");
    ok(NULL != zcloud_get_store_plugin_by_prefix("disk"), "plugin found");

    load_xml("<zcloud-module foo=\"bar\" basename=\"myext\">"
        "<store-plugin prefix=\"myext\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*'zcloud-module' attribute 'foo' not recognized",
                G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE, "bogus zcloud-module attribute");

    load_xml("<zcloud-module basename=\"myext\">"
        "<store-plugin foo=\"bar\" prefix=\"myext\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*'store-plugin' attribute 'foo' not recognized",
                G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE, "bogus store-plugin attribute");

    load_xml("<zcloud-module>"
        "<store-plugin prefix=\"myext\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*'zcloud-module' attribute 'basename' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing basename attribute");

    load_xml("<zcloud-module basename=\"foo\">"
        "<store-plugin></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*'store-plugin' attribute 'prefix' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing prefix attribute");

    load_xml("<zcloud-module basename=\"myext\">"
        "random text"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*unexpected text",
            G_MARKUP_ERROR_INVALID_CONTENT, "unexpected text");

    load_xml("<zcloud-module basename=\"myext\">"
        "<blink>hi mom</blink>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*element 'blink' not recognized",
            G_MARKUP_ERROR_UNKNOWN_ELEMENT, "bogus element");

    load_xml("<store-plugin></store-plugin>", &error);
    gerror_is_set(&error, "*element 'store-plugin' must appear in a 'zcloud-module' element",
            G_MARKUP_ERROR_INVALID_CONTENT, "store-plugin with no module");

    load_xml("<zcloud-module basename=\"myext\">"
        "<zcloud-module>", &error);
    gerror_is_set(&error, "*element 'zcloud-module' cannot be nested",
            G_MARKUP_ERROR_INVALID_CONTENT, "nested zcloud-module");

    load_xml("<zcloud-module basename=\"myext\">"
        "<store-plugin prefix=\"myext\">"
        "<store-plugin prefix=\"myext\">", &error);
    gerror_is_set(&error, "*element 'store-plugin' cannot be nested",
            G_MARKUP_ERROR_INVALID_CONTENT, "nested store-plugin");

    load_xml("<zcloud-module basename=\"myext\">"
        "<store-plugin prefix=\"myext\">"
        "<store-plugin prefix=\"myext\">", &error);
    gerror_is_set(&error, "*element 'store-plugin' cannot be nested",
            G_MARKUP_ERROR_INVALID_CONTENT, "nested store-plugin");

    load_xml("<parameter name=\"foo\" type=\"string\" blurb=\"bar\" />",
         &error);
    gerror_is_set(&error, "*element 'parameter' must appear in a 'store-plugin' element",
            G_MARKUP_ERROR_INVALID_CONTENT, "property with no module");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter type=\"string\" blurb=\"bar\" />", &error);
    gerror_is_set(&error, "*'parameter' attribute 'name' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing name attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"foo\" blurb=\"bar\" />", &error);
    gerror_is_set(&error, "*'parameter' attribute 'type' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing type attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"foo\" type=\"string\" />", &error);
    gerror_is_set(&error, "*'parameter' attribute 'blurb' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing blurb attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"foo\" hype=\"string\" />", &error);
    gerror_is_set(&error, "*'parameter' attribute 'hype' not recognized",
                G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE, "bad property attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"foo\" type=\"rectangle\" blurb=\"bar\"/>", &error);
    gerror_is_set(&error, "*invalid parameter type 'rectangle'",
                G_MARKUP_ERROR_INVALID_CONTENT, "bad parameter type");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"1foo\" type=\"string\" blurb=\"bar\"/>", &error);
    gerror_is_set(&error, "*invalid parameter name '1foo'",
                G_MARKUP_ERROR_INVALID_CONTENT, "bad parameter name (starts with number)");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"f_o_o\" type=\"string\" blurb=\"bar\"/>", &error);
    gerror_is_set(&error, "*invalid parameter name 'f_o_o'",
                G_MARKUP_ERROR_INVALID_CONTENT, "bad parameter name (non-alphanumeric)");

    load_xml("<zcloud-module basename=\"mod\">"
        "<store-plugin prefix=\"withprop\">"
        " <parameter name=\"ver-bose\" type=\"string\" blurb=\"lots to say\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading a plugin with a property with dashes in its name");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"foo\" type=\"sTrInG\" blurb=\"bar\">"
        "<foo>"
        "</parameter>", &error);
    gerror_is_set(&error, "*'parameter' element must be empty",
                G_MARKUP_ERROR_INVALID_CONTENT, "non-empty property");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<parameter name=\"foo\" type=\"string\" blurb=\"bar\" />"
        "<parameter name=\"foo\" type=\"string\" blurb=\"bar\" />", &error);
    gerror_is_set(&error, "*duplicate parameter name 'foo'",
                G_MARKUP_ERROR_INVALID_CONTENT, "duplicate property");

    load_xml("<zcloud-module basename=\"myext\">"
        "<store-plugin prefix=\"myext1\"></store-plugin>"
        "<store-plugin prefix=\"myext2\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading a module with two store plugins");
    ok(NULL != zcloud_get_store_plugin_by_prefix("myext1"), "one of two plugins exists");
    ok(NULL != zcloud_get_store_plugin_by_prefix("myext2"), ".. as does the other");

    load_xml("<zcloud-module basename=\"myext\">"
        "<store-plugin prefix=\"dupe\"></store-plugin>"
        "<store-plugin prefix=\"dupe\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*store plugin prefix 'dupe' is already defined",
            G_MARKUP_ERROR_INVALID_CONTENT, "duplicate prefixes generate an error");

    load_xml("<zcloud-module basename=\"myext1\">"
        "<store-plugin prefix=\"one\"></store-plugin>"
        "</zcloud-module><zcloud-module basename=\"myext2\">"
        "<store-plugin prefix=\"two\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading two modules");

    pl = zcloud_get_store_plugin_by_prefix("one");
    ok(pl != NULL, "prefix one exists");
    ok(0 == strcmp(pl->module->basename, "myext1"),
        "..and has the right module name");

    pl = zcloud_get_store_plugin_by_prefix("two");
    ok(pl != NULL, "prefix two exists");
    ok(0 == strcmp(pl->module->basename, "myext2"),
        "..and has the right module name");

    load_xml("<zcloud-module basename=\"mod\">"
        "<store-plugin prefix=\"withprop\">"
        " <parameter name=\"verbose\" type=\"string\" blurb=\"talkative; wordy\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading a plugin with a property without a nick");
    pl = zcloud_get_store_plugin_by_prefix("withprop");
    is_string(get_paramspec(pl, 0)->name, "verbose",
            "..name is set correctly");
    is_string(g_param_spec_get_blurb(get_paramspec(pl, 0)), "talkative; wordy",
            "..blurb is set correctly");
    is_int(get_paramspec(pl, 0)->value_type, G_TYPE_STRING,
            "..type is set correctly");
    is_string(g_param_spec_get_nick(get_paramspec(pl, 0)), "verbose",
            "..and the nick defaults correctly");

    load_xml("<zcloud-module basename=\"mod\">"
        "<store-plugin prefix=\"withprop\">"
        " <parameter name=\"verbose\" type=\"string\" nick=\"loud\" blurb=\"talkative; wordy\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading a plugin with a property with a nick");
    pl = zcloud_get_store_plugin_by_prefix("withprop");
    is_string(g_param_spec_get_nick(get_paramspec(pl, 0)), "loud",
            "..and the nick defaults correctly");

    load_xml("<zcloud-module basename=\"foo\">"
        "<store-plugin prefix=\"one\"></store-plugin>"
        "</zcloud-module><zcloud-module basename=\"foo\">"
        "<store-plugin prefix=\"another\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*zcloud-module with basename 'foo' is already defined",
            G_MARKUP_ERROR_INVALID_CONTENT,
            "two extensions with the same basename generate an error");

    load_xml("<zcloud-module basename=\"myext\">"
    "<store-plugin prefix=\"one\"></store-plugin>"
    "<store-plugin prefix=\"two\"></store-plugin>"
    "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading two plugins");

    {
        GSList *iter, *all;
        gboolean seen_one = FALSE, seen_two = FALSE;

        /* evaluate all of the fields in the structs to make sure they're set
         * correctly */
        all = zcloud_get_all_store_plugins();
        for (iter = all; iter; iter = iter->next) {
            pl = (ZCloudStorePlugin *)iter->data;

            if (0 == strcmp(pl->prefix, "one"))
                seen_one = TRUE;
            if (0 == strcmp(pl->prefix, "two"))
                seen_two = TRUE;

            is_string(pl->module->basename, "myext",
                "module basename is correct");
            is_string(pl->module->xml_path, "(string)",
                "module xml_path is correct");
            ok(pl->module->module_path != NULL,
                "module module_path is non-NULL");
        }

        ok(g_slist_length(all) == 2 && seen_one && seen_two,
            "saw the correct two plugins");
    }
}

void
test_plugins(void)
{
    test_xml_parser();
}
