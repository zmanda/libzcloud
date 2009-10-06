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

#include <signal.h>
#include <unistd.h>
#include "test.h"

void
test_fd_download_consumer(void)
{
    static const gchar *test_text = "Lorem ipsum te agam detracto voluptatum mei, eam natum dicam.";
    ZCloudFDDownloadConsumer *o;
    ZCloudDownloadConsumer *dc;
    size_t test_text_len;
    gchar *temp_fn = NULL, *read_buf;
    GError *err = NULL;
    gsize write_ret;
    gboolean op_ok;
    int file_fd, fds[2];
    /* sighandler_t isn't defined, so... */
    void (*prev_handler)(int);

    test_text_len = strlen(test_text);

    file_fd = g_file_open_tmp("zc-dc-XXXXXX", &temp_fn, &err);
    isnt_gint(file_fd, -1, "successfully opened a temp file");
    gerror_is_clear(&err, "opening temp file didn't cause an error");

    o = zcloud_fd_download_consumer(file_fd);
    isnt_null(o, "created fd download consumer");
    dc = ZCLOUD_DOWNLOAD_CONSUMER(o);

    write_ret = zcloud_download_consumer_write(dc, test_text, 1, &err);
    is_gsize(write_ret, 1, "writing 1 byte");
    gerror_is_clear(&err, "writing 1 byte didn't cause error");

    write_ret = zcloud_download_consumer_write(dc, test_text+1, test_text_len, &err);
    is_gsize(write_ret, test_text_len, "writing rest of string");
    gerror_is_clear(&err, "writing rest of string didn't cause error");

    is_goffset(lseek(file_fd, 0, SEEK_SET), 0, "moved to beginning of file");

    read_buf = g_malloc(test_text_len+2);
    is_gssize(read(file_fd, read_buf, test_text_len+2), test_text_len+1,
        "read right number of bytes back");
    is_string(read_buf, test_text, "contents of file match");

    /* note: position should now be at the end of the file */
    write_ret = zcloud_download_consumer_write(dc, test_text, 3, &err);
    is_gsize(write_ret, 3, "writing some more");
    gerror_is_clear(&err, "writing some didn't cause error");

    op_ok = zcloud_download_consumer_reset(dc, &err);
    is_gboolean(op_ok, TRUE, "reset returned ok");
    gerror_is_clear(&err, "reset didn't cause error");

    is_goffset(lseek(file_fd, 0, SEEK_SET), 0, "moved to beginning of file (after reset)");

    is_gssize(read(file_fd, read_buf, test_text_len+2), 0,
        "read right number of bytes back (after reset)");

    close(file_fd);
    unlink(temp_fn);
    g_free(temp_fn);
    g_object_unref(o);
    g_free(read_buf);

    if (pipe(fds) < 0) {
        gchar *tmp = g_strdup_printf("error occurred creating pipe (%s); aborting",
            strerror(errno));
        fail(tmp);
        g_free(tmp);
        return;
    }

    o = zcloud_fd_download_consumer(fds[1]);
    isnt_null(o, "created fd download consumer w/ pipe");
    dc = ZCLOUD_DOWNLOAD_CONSUMER(o);

    /*write, read, try reset */
    write_ret = zcloud_download_consumer_write(dc, "h", 1, &err);
    is_gsize(write_ret, 1, "writing 1 byte to pipe");
    gerror_is_clear(&err, "writing 1 byte to pipe didn't cause error");

    write_ret = zcloud_download_consumer_write(dc, "", 1, &err);
    is_gsize(write_ret, 1, "writing another byte");
    gerror_is_clear(&err, "writing anoter byte didn't cause error");

    read_buf = g_malloc(3);
    is_gssize(read(file_fd, read_buf, 3), 2,
        "read right number of bytes from other end");
    is_string(read_buf, "h", "got correct string from other end");

    g_free(read_buf);

    op_ok = zcloud_download_consumer_reset(dc, &err);
    is_gboolean(op_ok, FALSE, "reset didn't return ok w/ pipe");
    gerror_is_set(&err, "could not *", ZCERR_UNKNOWN, "reset set the proper error");

    close(fds[0]);

    prev_handler = signal(SIGPIPE, SIG_IGN);
    write_ret = zcloud_download_consumer_write(dc, "h", 1, &err);
    signal(SIGPIPE, prev_handler);
    is_gsize(write_ret, 0, "writing 1 byte after closing one end shouldn't work");
    gerror_is_set(&err, "an error * while writing *", ZCERR_UNKNOWN,
        "writing 1 byte after closing one end caused proper error");

    close(fds[1]);
    g_object_unref(o);
}
