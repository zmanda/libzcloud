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
gboolean is_byte_array(const GByteArray *got, const GByteArray *expected, const gchar *msg);

/* generate functions for integer types, comparable with == */
#define ZC_INT_TYPE_LIST \
    ZC_INT_TYPE(char, "hhi") \
    ZC_INT_TYPE(short, "hi") \
    ZC_INT_TYPE(int, "i") \
    ZC_INT_TYPE(long, "li") \
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

#define ZC_INT_TYPE(T, FS) gboolean is_ ##T(T got, T expected, const gchar *msg);
  ZC_INT_TYPE_LIST
#undef ZC_INT_TYPE

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
    TEST_MODULE(plugins)

/* generate test module prototypes */
#define TEST_MODULE(n) void test_##n(void);
ALL_TESTS
#undef TEST_MODULE

G_END_DECLS

#endif
