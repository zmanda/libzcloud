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


/* The sole purpose of this file is to provide a main() which can invoke
 * the various tests. */

#include "test.h"
#include <glib/gprintf.h>

struct test_t {
    const gchar *test_name;
    void (*test_fn)(void);
};

#define TEST_MODULE(n) { G_STRINGIFY(n), test_##n },
struct test_t all_tests[] = {
    ALL_TESTS
    { NULL, NULL }
};
#undef TEST_MODULE

static void
null_log(const gchar *log_domain G_GNUC_UNUSED,
     GLogLevelFlags log_level G_GNUC_UNUSED,
     const gchar *message G_GNUC_UNUSED,
     gpointer user_data G_GNUC_UNUSED)
{
}

int
main(int argc, char **argv)
{
    struct test_t *t;

    if (argc > 1) {
        g_fprintf(stderr, "usage: %s\n", argv[0]);
        return 1;
    }

    g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_MASK, null_log, NULL);

    for (t = all_tests; t->test_name; t++) {
        g_fprintf(stderr, "TESTING %s\n", t->test_name);
        t->test_fn();
    }

    if (tests_failed + tests_passed) {
        g_fprintf(stderr, "RESULTS: %d passed, %d failed (%d%% success)\n",
            tests_passed, tests_failed,
            tests_passed * 100 / (tests_passed + tests_failed));
    }

    return tests_failed? 1 : 0;
}
