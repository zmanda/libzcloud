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
#include <glib/gprintf.h>

gint tests_run = 0;
gint tests_failed = 0;
gint tests_passed = 0;

/*
 * Output
 */

void
diag(const gchar *fmt, ...)
{
    va_list ap;
    gchar *msg;
    gchar **msglines, **i;

    va_start(ap, fmt);
    msg = g_strdup_vprintf(fmt, ap);
    va_end(ap);

    if (!msg || !msg[0])
        return;

    /* strip a final newline, if one is present */
    if (msg[strlen(msg)-1] == '\n')
        msg[strlen(msg)-1] = '\0';

    /* split on newlines */
    msglines = g_strsplit(msg, "\n", -1);
    for (i = msglines; *i; i++) {
        g_printf("# %s\n", *i);
    }
}

gboolean
pass(const gchar *msg)
{
    tests_passed++;
    g_printf("ok %d - %s\n", ++tests_run, msg);
    fflush(stdout);
    return TRUE;
}

gboolean
fail(const gchar *msg)
{
    tests_failed++;
    g_printf("not ok %d - %s\n", ++tests_run, msg);
    fflush(stdout);
    return FALSE;
}

gboolean
ok(
    gboolean test,
    const gchar *msg)
{
    if (test)
        pass(msg);
    else
        fail(msg);
    return test;
}

gboolean
not_ok(
    gboolean test,
    const gchar *msg)
{
    return ok(!test, msg);
}

gboolean
gerror_is_clear(
    GError **error,
    const gchar *msg)
{
    if (!*error) {
        pass(msg);
        return TRUE;
    } else {
        diag("Got unexpected GError: { message = '%s'; code = %d }",
            (*error)->message? (*error)->message : "(nil)", (*error)->code);
        fail(msg);
        g_clear_error(error);
        return FALSE;
    }
}

gboolean
gerror_is_set(
    GError **error,
    const gchar *expected_message_glob,
    gint expected_code,
    const gchar *msg)
{
    gboolean success = TRUE;
    if (!error || !*error)
        return fail(msg);

    if (expected_message_glob) {
        if (!(*error)->message) {
            diag(" error is non-NULL but has message = NULL");
            success = FALSE;
        } else if (!g_pattern_match_simple(expected_message_glob, (*error)->message)) {
            diag(" message '%s' does not match pattern", (*error)->message);
            success = FALSE;
        }
    }

    if (expected_code != -1) {
        if (expected_code != (*error)->code) {
            diag(" got code %d; expected %d", (*error)->code, expected_code);
            success = FALSE;
        }
    }

    g_clear_error(error);
    return ok(success, msg);
}

#define ZC_INT_TYPE_NAME(T, N, FS) \
    gboolean \
    is_ ##N ( \
        T got, \
        T expected, \
        const gchar *msg) \
    { \
        if (got == expected) { \
            return pass(msg); \
        } else { \
            diag(" got: %" FS "; expected: %" FS , got, expected); \
            return fail(msg); \
        } \
    } \
    \
    gboolean \
    isnt_ ##N ( \
        T got, \
        T expected, \
        const gchar *msg) \
    { \
        if (got != expected) { \
            return pass(msg); \
        } else { \
            diag(" unexpectedly got: %" FS, got); \
            return fail(msg); \
        } \
    }
  ZC_INT_TYPE_LIST
#undef ZC_INT_TYPE_NAME

gboolean
is_null(
    gconstpointer got,
    const gchar *msg)
{
    if (got == NULL) {
        return pass(msg);
    } else {
        diag(" got: %p; expected: NULL\n", got);
        return fail(msg);
    }
}

gboolean
isnt_null(
    gconstpointer got,
    const gchar *msg)
{
    if (got != NULL) {
        return pass(msg);
    } else {
        diag(" unexpectedly got: %p\n", got);
        return fail(msg);
    }
}

gboolean
is_string(
    const gchar *got,
    const gchar *expected,
    const gchar *msg)
{
    if (got == NULL) {
        diag(" got NULL expected: '%s'\n", expected);
        return fail(msg);
    } else if (0 == strcmp(got, expected)) {
        return pass(msg);
    } else {
        diag(" got: '%s'; expected: '%s'\n", got, expected);
        return fail(msg);
    }
}

gboolean
isnt_string(
    const gchar *got,
    const gchar *expected,
    const gchar *msg)
{
    if (0 != strcmp(got, expected)) {
        return pass(msg);
    } else {
        diag(" got: %s expected: %s\n", got, expected);
        return fail(msg);
    }
}

static gchar *
hex_encode(const GByteArray *to_enc)
{
    guint i;
    gchar *ret = NULL, table[] = "0123456789abcdef";
    if (!to_enc) return NULL;

    ret = g_new(gchar, to_enc->len*2 + 1);
    for (i = 0; i < to_enc->len; i++) {
        /* most significant 4 bits */
        ret[i*2] = table[to_enc->data[i] >> 4];
        /* least significant 4 bits */
        ret[i*2 + 1] = table[to_enc->data[i] & 0xf];
    }
    ret[to_enc->len*2] = '\0';

    return ret;
}

gboolean
is_byte_array(
    const GByteArray *got,
    const GByteArray *expected,
    const char *msg)
{
    gchar *got_s, *expected_s;
    gboolean ret;

    got_s = hex_encode(got);
    expected_s = hex_encode(expected);

    ret = is_string(got_s, expected_s, msg);

    g_free(got_s);
    g_free(expected_s);

    return ret;
}

gboolean
isnt_byte_array(
    const GByteArray *got,
    const GByteArray *expected,
    const char *msg)
{
    gchar *got_s, *expected_s;
    gboolean ret;

    got_s = hex_encode(got);
    expected_s = hex_encode(expected);

    ret = isnt_string(got_s, expected_s, msg);

    g_free(got_s);
    g_free(expected_s);

    return ret;
}
