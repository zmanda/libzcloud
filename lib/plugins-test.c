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

#define XML_FILE "mod.xml"

static void
write_xml_and_load(const gchar *xml, GError **error)
{
    int fd;
    const gchar *p = xml;
    gsize remaining = strlen(xml);
    gboolean res;

    fd = open(XML_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    g_assert(fd >= 0);

    while (remaining) {
        gsize written = write(fd, p, remaining);
        g_assert(written > 0);
        p += written;
        remaining -= written;
    }
    g_assert(close(fd) == 0);

    res = zc_load_module_xml(".", XML_FILE, error);

    /* double-check that the boolean result agrees with ERROR */
    g_assert((res && !*error) || (!res && *error));
}

static void
load_xml(const gchar *xml, GError **error)
{
    zc_plugins_clear();
    write_xml_and_load(xml, error);
}

void
test_plugins(void)
{
    GError *error = NULL;
    ZCloudStorePlugin *pl;
    ZCloudStorePluginPropertySpec *prop;

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
    gerror_is_set(&error, "*element 'property' must appear in another element",
            G_MARKUP_ERROR_INVALID_CONTENT, "property with no module");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property type=\"string\" description=\"bar\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'name' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing name attribute");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" description=\"bar\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'type' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing type attribute");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" type=\"string\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'description' is required",
                G_MARKUP_ERROR_INVALID_CONTENT, "missing description attribute");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" hype=\"string\" />", &error);
    gerror_is_set(&error, "*'property' attribute 'hype' not recognized",
                G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE, "bad property attribute");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" type=\"rectangle\" description=\"bar\"/>", &error);
    gerror_is_set(&error, "*invalid property type 'rectangle'",
                G_MARKUP_ERROR_INVALID_CONTENT, "bad property type");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" type=\"sTrInG\" description=\"bar\">"
        "<foo>"
        "<property>", &error);
    gerror_is_set(&error, "*'property' element must be empty",
                G_MARKUP_ERROR_INVALID_CONTENT, "non-empty property");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" type=\"string\" description=\"bar\" />"
        "<property name=\"foo\" type=\"boolean\" description=\"bar\" />", &error);
    gerror_is_set(&error, "*duplicate property name 'foo'",
                G_MARKUP_ERROR_INVALID_CONTENT, "duplicate property");

    load_xml("<zcloud-module basename=\"myext\">"
        "<property name=\"foo\" type=\"string\" description=\"bar\" />"
        "<store-plugin prefix=\"myext\">"
        "<property name=\"foo\" type=\"boolean\" description=\"bar\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_set(&error, "*duplicate property name 'foo'",
                G_MARKUP_ERROR_INVALID_CONTENT, "duplicate property across scopes");

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
        "<property name=\"prop\" type=\"string\" description=\"desc ript\" />"
        "<store-plugin prefix=\"withprop\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error with a property at the module level");

    pl = zcloud_get_store_plugin_by_prefix("withprop");
    ok(pl != NULL, "prefix withprop exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "prop"),
        "..and has the right property name");
    ok(prop->type == G_TYPE_STRING,
        "..and has the right property type");
    ok(0 == strcmp(prop->description, "desc ript"),
        "..and has the right property description");

    load_xml("<zcloud-module basename=\"mod\">"
        "<store-plugin prefix=\"withprop\">"
        " <property name=\"prop\" type=\"boolean\" description=\"verbose\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error with a property at the plugin level");

    pl = zcloud_get_store_plugin_by_prefix("withprop");
    ok(pl != NULL, "prefix withprop exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "prop"),
        "..and has the right property name");
    ok(prop->type == G_TYPE_BOOLEAN,
        "..and has the right property type");
    ok(0 == strcmp(prop->description, "verbose"),
        "..and has the right property description");

    load_xml("<zcloud-module basename=\"mod\">"
        "<property name=\"outer\" type=\"int\" description=\"out\" />"
        "<store-plugin prefix=\"withprop\">"
        " <property name=\"inner\" type=\"int\" description=\"in\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error with a property at both levels");

    pl = zcloud_get_store_plugin_by_prefix("withprop");
    ok(pl != NULL, "prefix withprop exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "outer"),
        "..and ends with the module-level property");
    ok(prop->type == G_TYPE_INT,
        "..and has the right property type");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->next->data;
    ok(0 == strcmp(prop->name, "inner"),
        "..and begins with the plugin-level property");
    ok(prop->type == G_TYPE_INT,
        "..and has the right property type");

    load_xml("<zcloud-module basename=\"mod\">"
        "<store-plugin prefix=\"withprop\">"
        " <property name=\"inner\" type=\"int\" description=\"in\" />"
        "</store-plugin>"
        "<property name=\"outer\" type=\"int\" description=\"out\" />"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error with a property at both levels, inner property first");

    pl = zcloud_get_store_plugin_by_prefix("withprop");
    ok(pl != NULL, "prefix withprop exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "inner"),
        "..and begins with the plugin-level property");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->next->data;
    ok(0 == strcmp(prop->name, "outer"),
        "..and ends with the module-level property");

    load_xml("<zcloud-module basename=\"mod\">"
        "<property name=\"outer\" type=\"int\" description=\"out\" />"
        "<store-plugin prefix=\"withprop1\">"
        " <property name=\"inner1\" type=\"int\" description=\"in1\" />"
        "</store-plugin>"
        "<store-plugin prefix=\"withprop2\">"
        " <property name=\"inner2\" type=\"boolean\" description=\"in2\" />"
        "</store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error with a property at both levels with two plugins");

    pl = zcloud_get_store_plugin_by_prefix("withprop1");
    ok(pl != NULL, "prefix withprop1 exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "outer"),
        "..and ends with the module-level property");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->next->data;
    ok(0 == strcmp(prop->name, "inner1"),
        "..and begins with the correct plugin-level property");

    pl = zcloud_get_store_plugin_by_prefix("withprop2");
    ok(pl != NULL, "prefix withprop2 exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "outer"),
        "..and begins with the module-level property");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->next->data;
    ok(0 == strcmp(prop->name, "inner2"),
        "..and ends with the correct plugin-level property");

    load_xml("<zcloud-module basename=\"mod1\">"
        "<property name=\"prop1\" type=\"int\" description=\"1\" />"
        "<store-plugin prefix=\"withprop1\"></store-plugin>"
        "</zcloud-module>"
        "<zcloud-module basename=\"mod2\">"
        "<property name=\"prop2\" type=\"int\" description=\"2\" />"
        "<store-plugin prefix=\"withprop2\"></store-plugin>"
        "</zcloud-module>", &error);
    gerror_is_clear(&error, "no error with a property at module level with two modules");

    pl = zcloud_get_store_plugin_by_prefix("withprop1");
    ok(pl != NULL, "prefix withprop1 exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "prop1"),
        "..and has the correct module-level property");

    pl = zcloud_get_store_plugin_by_prefix("withprop2");
    ok(pl != NULL, "prefix withprop2 exists");
    prop = (ZCloudStorePluginPropertySpec *)pl->property_specs->data;
    ok(0 == strcmp(prop->name, "prop2"),
        "..and has the correct module-level property");

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

            ok(0 == strcmp(pl->module->basename, "myext"),
                "module basename is correct");
            ok(0 == strcmp(pl->module->xml_path, "./mod.xml"),
                "module xml_path is correct");
            ok(pl->module->module_path != NULL,
                "module module_path is non-NULL");
        }

        ok(g_slist_length(all) == 2 && seen_one && seen_two,
            "saw the correct two plugins");
    }

    unlink(XML_FILE);
}
