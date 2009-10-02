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


#ifndef ZC_TEST_H
#define ZC_TEST_H

#include "internal.h"

G_BEGIN_DECLS

/*
 * Checks (test-checks.c)
 */

/* print a diagnostic message */
void diag(const gchar *fmt, ...) G_GNUC_PRINTF(1, 2);

/* print a message indicating that the named test passed or failed */
gboolean pass(const gchar *msg);
gboolean fail(const gchar *msg);

/* The remaining test functions return TRUE if the test passes, and take
 * a MSG parameter describing the test, for output to the user */

/* pass if TEST is true */
gboolean ok(gboolean test, const gchar *msg);

/* pass if TEST is false */
gboolean not_ok(gboolean test, const gchar *msg);

/* pass if ERROR is NULL.  The GError is cleared if necessary. */
gboolean gerror_is_clear(GError **error, const gchar *msg);

/* pass if ERROR is non-NULL, optionally matching its message against
 * EXPECTED_MESSAGE_GLOB, and (if EXPECTED_CODE is not -1) matching the
 * code against EXPECTED_CODE. The GError is cleared if necessary. */
gboolean gerror_is_set(GError **error, const gchar *expected_message_glob,
                    gint expected_code, const gchar *msg);

/* compare two objects of the same, given type */
gboolean is_string(const gchar *got, const gchar *expected, const gchar *msg);
gboolean isnt_string(const gchar *got, const gchar *expected, const gchar *msg);
gboolean is_byte_array(const GByteArray *got, const GByteArray *expected, const gchar *msg);
gboolean isnt_byte_array(const GByteArray *got, const GByteArray *expected, const gchar *msg);

gboolean is_null(gconstpointer got, const gchar *msg);
gboolean isnt_null(gconstpointer got, const gchar *msg);

/* generate functions for integer types, comparable with == */
#define ZC_INT_TYPE_LIST \
    ZC_INT_TYPE_NAME(void *, pointer, "p") \
    \
    ZC_INT_TYPE(char, "hhi") \
    ZC_INT_TYPE_NAME(unsigned char, u_char, "hhu") \
    ZC_INT_TYPE(short, "hi") \
    ZC_INT_TYPE_NAME(unsigned short, u_short, "hu") \
    ZC_INT_TYPE(int, "i") \
    ZC_INT_TYPE_NAME(unsigned int, u_int, "u") \
    ZC_INT_TYPE(long, "li") \
    ZC_INT_TYPE_NAME(unsigned long, u_long, "lu") \
    ZC_INT_TYPE_NAME(long long, long_long, "lli") \
    ZC_INT_TYPE_NAME(unsigned long long, u_long_long, "lli") \
    \
    ZC_INT_TYPE(gboolean, "i") \
    ZC_INT_TYPE(gchar, "hhi") \
    ZC_INT_TYPE(guchar, "hhu") \
    ZC_INT_TYPE(gint, "i") \
    ZC_INT_TYPE(guint, "u") \
    ZC_INT_TYPE(gshort, "hi") \
    ZC_INT_TYPE(gushort, "hu") \
    ZC_INT_TYPE(glong, "li") \
    ZC_INT_TYPE(gulong, "lu") \
    ZC_INT_TYPE(gint8, "hhi") \
    ZC_INT_TYPE(guint8, "hhu") \
    ZC_INT_TYPE(gint16, G_GINT16_FORMAT) \
    ZC_INT_TYPE(guint16, G_GUINT16_FORMAT) \
    ZC_INT_TYPE(gint32, G_GINT32_FORMAT) \
    ZC_INT_TYPE(guint32, G_GUINT32_FORMAT) \
    ZC_INT_TYPE(gint64, G_GINT64_FORMAT) \
    ZC_INT_TYPE(guint64, G_GUINT64_FORMAT) \
    ZC_INT_TYPE(gsize, G_GSIZE_FORMAT) \
    ZC_INT_TYPE(gssize, G_GSSIZE_FORMAT) \
    ZC_INT_TYPE(goffset, G_GINT64_FORMAT)

#define ZC_INT_TYPE(T, FS) ZC_INT_TYPE_NAME(T, T, FS)

#define ZC_INT_TYPE_NAME(T, N, FS)                          \
    gboolean is_##N(T got, T expected, const gchar *msg);  \
    gboolean isnt_##N(T got, T expected, const gchar *msg);
  ZC_INT_TYPE_LIST
#undef ZC_INT_TYPE_NAME

/*
 * Utilities (test-util.c)
 */

/* Load the given xml file as a plugin file, using "." as the
 * plugin directory */
gboolean load_plugin_xml(const gchar *xml, GError **error);

/*
 * Mock classes (test-mock.c)
 */

/* Set up to use the mock plugin; this clears the plugin list and
 * adds the a module and a plugin with prefix "mock:" */
void mock_setup(void);

/*
 * Global state tracking
 */

extern gint tests_failed, tests_passed, tests_run;

/*
 * All test modules
 */

/* list of all test modules */
#define ALL_TESTS \
    TEST_MODULE(memory_upload_producer) \
    TEST_MODULE(growing_memory_download_consumer) \
    TEST_MODULE(fixed_memory_download_consumer) \
    TEST_MODULE(slist_list_consumer) \
    TEST_MODULE(fd_list_consumer) \
    TEST_MODULE(plugins) \
    TEST_MODULE(store)

/* generate test module prototypes */
#define TEST_MODULE(n) void test_##n(void);
ALL_TESTS
#undef TEST_MODULE

G_END_DECLS

#endif
