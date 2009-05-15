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

#include "internal.h"
#include <glib/gprintf.h>

#define XML_FILE "mod.xml"

/*
 * XML tests
 */

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
    g_assert((res && !*error) || (!res && *error));
}

static void
xml_ok(const gchar *xml)
{
    GError *error = NULL;
    zc_plugins_clear();
    write_xml_and_load(xml, &error);
    if (error) {
        g_fprintf(stderr, "Unexpected error: %s\n", error->message);
    }
    g_assert(error == NULL);
}

static void
xml_not_ok(const gchar *xml, gint code)
{
    GError *error = NULL;
    zc_plugins_clear();
    write_xml_and_load(xml, &error);
    if (error && (error->domain != G_MARKUP_ERROR || error->code != code)) {
        g_fprintf(stderr, "Unexpected code/domain: %s\n", error->message);
    }
    g_assert(error != NULL);
    g_assert(error->domain == G_MARKUP_ERROR);
    g_assert(error->code == code);
    g_clear_error(&error);
}

static void
test_xml_simple(void)
{
    xml_ok("<zcloud-module basename=\"disk\">"
    "<store-plugin prefix=\"disk\"></store-plugin>"
    "</zcloud-module>");

    g_assert(zcloud_get_store_plugin_by_prefix("disk"));
}

static void
test_xml_badattr(void)
{
    xml_not_ok("<zcloud-module foo=\"bar\" basename=\"myext\">"
    "<store-plugin prefix=\"myext\"></store-plugin>"
    "</zcloud-module>", G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE);

    xml_not_ok("<zcloud-module basename=\"myext\">"
    "<store-plugin foo=\"bar\" prefix=\"myext\"></store-plugin>"
    "</zcloud-module>", G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE);

    xml_not_ok("<zcloud-module>"
    "<store-plugin prefix=\"myext\"></store-plugin>"
    "</zcloud-module>", G_MARKUP_ERROR_INVALID_CONTENT);

    xml_not_ok("<zcloud-module basename=\"foo\">"
    "<store-plugin></store-plugin>"
    "</zcloud-module>", G_MARKUP_ERROR_INVALID_CONTENT);
}

static void
test_xml_badtext(void)
{
    xml_not_ok("<zcloud-module basename=\"myext\">"
    "random text"
    "</zcloud-module>", G_MARKUP_ERROR_INVALID_CONTENT);
}

static void
test_xml_badtags(void)
{
    xml_not_ok("<zcloud-module basename=\"myext\">"
    "<blink>hi mom</blink>"
    "</zcloud-module>", G_MARKUP_ERROR_UNKNOWN_ELEMENT);

    xml_not_ok("<store-plugin></store-plugin>",
    G_MARKUP_ERROR_INVALID_CONTENT);

    xml_not_ok("<zcloud-module basename=\"myext\">"
    "<store-plugin prefix=\"myext\">"
    "<store-plugin prefix=\"myext\">", G_MARKUP_ERROR_INVALID_CONTENT);

    xml_not_ok("<zcloud-module basename=\"myext\">"
    "<zcloud-module>", G_MARKUP_ERROR_INVALID_CONTENT);
}

static void
test_xml_multiplugins(void)
{
    xml_ok("<zcloud-module basename=\"myext\">"
    "<store-plugin prefix=\"myext1\"></store-plugin>"
    "<store-plugin prefix=\"myext2\"></store-plugin>"
    "</zcloud-module>");

    g_assert(zcloud_get_store_plugin_by_prefix("myext1"));
    g_assert(zcloud_get_store_plugin_by_prefix("myext2"));

    xml_not_ok("<zcloud-module basename=\"myext\">"
    "<store-plugin prefix=\"dupe\"></store-plugin>"
    "<store-plugin prefix=\"dupe\"></store-plugin>"
    "</zcloud-module>", G_MARKUP_ERROR_INVALID_CONTENT);
}

static void
test_xml_multimodules(void)
{
    xml_ok("<zcloud-module basename=\"myext1\">"
    "<store-plugin prefix=\"one\"></store-plugin>"
    "</zcloud-module><zcloud-module basename=\"myext2\">"
    "<store-plugin prefix=\"two\"></store-plugin>"
    "</zcloud-module>");

    g_assert(zcloud_get_store_plugin_by_prefix("one"));
    g_assert(0 == strcmp(
        zcloud_get_store_plugin_by_prefix("one")->module->basename,
        "myext1"));
    g_assert(zcloud_get_store_plugin_by_prefix("two"));
    g_assert(0 == strcmp(
        zcloud_get_store_plugin_by_prefix("two")->module->basename,
        "myext2"));

    xml_not_ok("<zcloud-module basename=\"foo\">"
    "<store-plugin prefix=\"one\"></store-plugin>"
    "</zcloud-module><zcloud-module basename=\"foo\">"
    "<store-plugin prefix=\"another\"></store-plugin>"
    "</zcloud-module>", G_MARKUP_ERROR_INVALID_CONTENT);
}

static void
test_plugin_info(void)
{
    GSList *iter;
    gboolean seen_one = FALSE, seen_two = FALSE;

    xml_ok("<zcloud-module basename=\"myext\">"
    "<store-plugin prefix=\"one\"></store-plugin>"
    "<store-plugin prefix=\"two\"></store-plugin>"
    "</zcloud-module>");

    /* evaluate all of the fields in the structs to make sure they're set
     * correctly */
    for (iter = zcloud_get_all_store_plugins(); iter; iter = iter->next) {
        ZCloudStorePlugin *plugin = (ZCloudStorePlugin *)iter->data;

        if (0 == strcmp(plugin->prefix, "one"))
            seen_one = TRUE;
        if (0 == strcmp(plugin->prefix, "two"))
            seen_two = TRUE;

        g_assert(0 == strcmp(plugin->module->basename, "myext"));
        g_assert(0 == strcmp(plugin->module->xml_path, "./mod.xml"));
        g_assert(plugin->module->module_path != NULL);
    }

    g_assert(seen_one && seen_two);
}

/*
 * Initialization
 */

void
zc_test_plugins()
{
    g_test_add_func("/plugins/xml/simple", test_xml_simple);
    g_test_add_func("/plugins/xml/badattr", test_xml_badattr);
    g_test_add_func("/plugins/xml/badtext", test_xml_badtext);
    g_test_add_func("/plugins/xml/badtags", test_xml_badtags);
    g_test_add_func("/plugins/xml/multiplugins", test_xml_multiplugins);
    g_test_add_func("/plugins/xml/multimodules", test_xml_multimodules);
    g_test_add_func("/plugins/plugin_info", test_plugin_info);
}
