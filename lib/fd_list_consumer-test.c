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

#include <stdio.h>
#include <signal.h>
#include "test.h"

/*
 * Reads bytes from a blocking fd one at a time until it reaches term or
 * encounters eof.
 * Returns a malloc-ed string.
 */
gchar*
read_until(int fd, gchar term, gboolean *eof);

gchar*
read_until(int fd, gchar term, gboolean *eof)
{
    gchar tmp, *ret;
    GString *str = g_string_new(NULL);
    if (eof) *eof = FALSE;

    while (TRUE) {
        ssize_t r_ret = read(fd, &tmp, 1);
        if (r_ret < 0) {
            g_warning("reading from fd %d caused an error %s", fd, strerror(errno));
        } else if (0 == r_ret) {/* eof */
            if (eof) *eof = TRUE;
            break;
        } else if (1 == r_ret) {
            if (tmp == term)
                break;
            else
                g_string_append_c(str, tmp);
        } else {
            g_assert_not_reached();
        }
    }

    ret = str->str;
    g_string_free(str, FALSE);
    return ret;
}

void
test_fd_list_consumer(void)
{
    static const gchar *to_insert[] = {"hi", "bye", "1!", NULL};
    ZCloudFDListConsumer *o;
    ZCloudListConsumer *lc;
    int fds[2];
    gchar *got;
    gboolean eof;
    guint i;
    /* sighandler_t isn't defined, so... */
    void (*prev_handler)(int);

    if (pipe(fds) < 0) {
        gchar *tmp = g_strdup_printf("error occurred creating pipe (%s); aborting",
            strerror(errno));
        fail(tmp);
        g_free(tmp);
        return;
    }

    o = zcloud_fd_list_consumer(fds[1], '\n');
    isnt_null(o, "created fd list consumer w/ newline suffix");
    lc = ZCLOUD_LIST_CONSUMER(o);

    for (i = 0; to_insert[i]; i++) {
        zcloud_list_consumer_got_result(lc, to_insert[i]);
        got = read_until(fds[0], '\n', &eof);
        is_gboolean(eof, FALSE, "didn't get eof while reading back");
        is_string(got, to_insert[i], "read the item we expected");
    }

    g_object_unref(o);

    o = zcloud_fd_list_consumer(fds[1], '\0');
    isnt_null(o, "created fd list consumer w/ NUL suffix");
    lc = ZCLOUD_LIST_CONSUMER(o);

    for (i = 0; to_insert[i]; i++) {
        zcloud_list_consumer_got_result(lc, to_insert[i]);
        got = read_until(fds[0], '\0', &eof);
        is_gboolean(eof, FALSE, "didn't get eof while reading back");
        is_string(got, to_insert[i], "read the item we expected");
    }

    close(fds[0]);

    prev_handler = signal(SIGPIPE, SIG_IGN);
    zcloud_list_consumer_got_result(lc, "foo");
    signal(SIGPIPE, prev_handler);
    pass("tried writing to closed pipe and din't explode");

    close(fds[1]);
    g_object_unref(o);
}
