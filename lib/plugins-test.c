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

static void
test_xml_parser(void)
{
    GError *error = NULL;
    ZCloudStorePlugin *pl;
    ZCloudPropertySpec *prop;

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

    load_xml("<property name=\"foo\" type=\"string\" description=\"bar\" />",
         &error);
    gerror_is_set(&error, "*element 'property' must appear in a 'store-plugin' element",
            G_MARKUP_ERROR_INVALID_CONTENT, "property with no module");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property type=\"string\" description=\"bar\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'name' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing name attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property name=\"foo\" description=\"bar\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'type' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing type attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property name=\"foo\" type=\"string\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'description' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing description attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property name=\"foo\" hype=\"string\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'hype' not recognized",
                G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE, "bad property attribute");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property name=\"foo\" type=\"rectangle\" description=\"bar\"/>", &error);
    gerror_is_set(&error, "*invalid property type 'rectangle'",
                G_MARKUP_ERROR_INVALID_CONTENT, "bad property type");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property name=\"foo\" type=\"sTrInG\" description=\"bar\">"
        "<foo>"
        "<property>", &error);
    gerror_is_set(&error, "*'property' element must be empty",
                G_MARKUP_ERROR_INVALID_CONTENT, "non-empty property");

    load_xml("<zcloud-module basename=\"myext\"><store-plugin prefix=\"foo\">"
        "<property name=\"foo\" type=\"string\" description=\"bar\" />"
        "<property name=\"foo\" type=\"boolean\" description=\"bar\" />", &error);
    gerror_is_set(&error, "*duplicate property name 'foo'",
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
        " <property name=\"prop\" type=\"boolean\" description=\"verbose\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error loading a plugin with a property");

    pl = zcloud_get_store_plugin_by_prefix("withprop");
    ok(pl != NULL, "prefix withprop exists");
    prop = (ZCloudPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "prop"),
        "..and has the right property name");
    ok(prop->type == G_TYPE_BOOLEAN,
        "..and has the right property type");
    ok(0 == strcmp(prop->description, "verbose"),
        "..and has the right property description");

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

static void
test_store_construction(void)
{
    ZCloudStore *store;
    GError *error = NULL;

    mock_setup();

    store = zcloud_store_new("mock:foo", &error);
    gerror_is_clear(&error, "create mock object");
}

void
test_plugins(void)
{
    test_xml_parser();
    test_store_construction();
}
