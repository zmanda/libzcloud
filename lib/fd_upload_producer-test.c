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
test_fd_upload_producer(void)
{
    ZCloudFDUploadProducer *o;
    ZCloudUploadProducer *o_p;
    GError *err = NULL;
    const gchar *data_str = "The quick brown fox jumps over the lazy dog",
        *buf_md5 = "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82\x6b\xd8\x1d\x35\x42\xa4\x19\xd6",
        *empty_md5 = "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e";
    gchar *temp_fn = NULL, *buf;
    gsize data_str_len, size, read;
    GByteArray *md5;
    gboolean op_ok;
    int file_fd, fds[2];
    /* sighandler_t isn't defined, so... */
    void (*prev_handler)(int);

    g_type_init();  /* TODO */

    data_str_len = strlen(data_str);

    file_fd = g_file_open_tmp("zc-dc-XXXXXX", &temp_fn, &err);
    isnt_gint(file_fd, -1, "successfully opened a temp file");
    gerror_is_clear(&err, "opening temp file didn't cause an error");

    o = zcloud_fd_upload_producer(file_fd);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);

    size = zcloud_upload_producer_get_size(o_p, &err);
    gerror_is_clear(&err, "no error while getting size of empty file");
    is_gsize(size, 0, "check reported size for empty file");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    gerror_is_clear(&err, "no error while calculating MD5 hash of empty file");
    is_md5(md5, empty_md5, "check MD5 hash for empty file");
    g_byte_array_free(md5, TRUE);

    buf = g_malloc0(data_str_len+1);
    read = zcloud_upload_producer_read(o_p, buf, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from empty file");
    is_gsize(read, 0, "read 0 bytes from empty file");
    is_string(buf, "", "empty read from empty file");
    g_free(buf);
    buf = NULL;

    is_gssize((gssize) write(file_fd, data_str, data_str_len), data_str_len,
        "wrote test string to file");
    is_gssize((gssize) lseek(file_fd, 0, SEEK_SET), 0, "seek to beginning after write");

    size = zcloud_upload_producer_get_size(o_p, &err);
    gerror_is_clear(&err, "no error while getting size of test file");
    is_gsize(size, data_str_len, "check reported size for test file");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    gerror_is_clear(&err, "no error while calculating MD5 hash of test file");
    is_md5(md5, buf_md5, "check MD5 hash for test file");
    g_byte_array_free(md5, TRUE);
    is_gssize((gssize) lseek(file_fd, 0, SEEK_CUR), 0,
        "at beginning after md5 calc");

    buf = g_malloc0(data_str_len+1);
    read = zcloud_upload_producer_read(o_p, buf, data_str_len+1, &err);
    gerror_is_clear(&err, "no error reading from file");
    is_gsize(read, data_str_len, "read all bytes from file");
    is_string((gchar*) buf, data_str, "read test string from file");
    g_free(buf);
    buf = NULL;

    buf = g_malloc0(data_str_len+1);
    read = zcloud_upload_producer_read(o_p, buf, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from file");
    is_gsize(read, 0, "read 0 bytes from exhausted file");
    is_string((gchar*) buf, "", "empty read exhausted file");
    g_free(buf);
    buf = NULL;

    op_ok = zcloud_upload_producer_reset(o_p, &err);
    is_gboolean(op_ok, TRUE, "reset producer returned true");
    gerror_is_clear(&err, "no error reseting producer");
    is_gssize((gssize) lseek(file_fd, 0, SEEK_CUR), 0,
        "at beginning after reset");

    buf = g_malloc0(data_str_len+1);

    read = zcloud_upload_producer_read(o_p, buf, 1, &err);
    gerror_is_clear(&err, "no error reading from file");
    is_gsize(read, 1, "read one bytes from file");
    is_gchar(buf[0], 'T', "read first char from file");

    read = zcloud_upload_producer_read(o_p, buf+1, 1, &err);
    gerror_is_clear(&err, "no error reading from file");
    is_gsize(read, 1, "read one bytes from file");
    is_gchar(buf[1], 'h', "read second char from file");

    read = zcloud_upload_producer_read(o_p, buf+2, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from buffer");
    is_gsize(read, data_str_len-2, "read remaining bytes from file");
    is_string((gchar*) buf, data_str, "completed read matches data");

    g_free(buf);
    buf = NULL;

    close(file_fd);
    unlink(temp_fn);
    g_free(temp_fn);
    g_object_unref(o);

    if (pipe(fds) < 0) {
        gchar *tmp = g_strdup_printf("error occurred creating pipe (%s); aborting",
            strerror(errno));
        fail(tmp);
        g_free(tmp);
        return;
    }

    o = zcloud_fd_upload_producer(fds[0]);
    isnt_null(o, "created fd upload producer w/ pipe");
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);

    /*write, read, try reset */
    is_gssize((gsize) write(fds[1], "h", 1), 1,
        "write a byte for the producer to read");

    buf = g_malloc0(1);
    read = zcloud_upload_producer_read(o_p, buf, 1, &err);
    is_gsize(read, 1, "reading 1 byte from pipe");
    gerror_is_clear(&err, "reading 1 byte to pipe didn't cause error");
    is_gchar(buf[0], 'h', "got the char we expected");
    g_free(buf);
    buf = NULL;

    op_ok = zcloud_upload_producer_reset(o_p, &err);
    is_gboolean(op_ok, FALSE, "reset didn't return ok w/ pipe");
    gerror_is_set(&err, "failed to *", ZCERR_UNKNOWN, "reset set the proper error");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    is_null(md5, "didn't get MD5 w/ pipe");
    gerror_is_set(&err, "failed to *", ZCERR_UNKNOWN, "calculate MD5 set the proper error");

    close(fds[0]);

    prev_handler = signal(SIGPIPE, SIG_IGN);
    buf = g_malloc0(1);
    read = zcloud_upload_producer_read(o_p, buf, 1, &err);
    signal(SIGPIPE, prev_handler);
    is_gsize(read, 0, "reading 1 byte after closing one end shouldn't work");
    gerror_is_set(&err, "an error * while reading *", ZCERR_UNKNOWN,
        "reading 1 byte after closing one end caused proper error");

    close(fds[1]);
    g_object_unref(o);
}
