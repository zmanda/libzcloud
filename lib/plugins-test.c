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
