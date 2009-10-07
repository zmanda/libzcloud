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

void
test_memory_upload_producer(void)
{
    ZCloudMemoryUploadProducer *o;
    ZCloudUploadProducer *o_p;
    GError *err = NULL;
    const gchar *data_str = "The quick brown fox jumps over the lazy dog",
        *buf_md5 = "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82\x6b\xd8\x1d\x35\x42\xa4\x19\xd6",
        *empty_md5 = "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e";
    gsize data_str_len, size, read;
    GByteArray *md5;
    guint8 *buf;
    gboolean ok;

    g_type_init();  /* TODO */
    data_str_len = strlen(data_str);

    o = zcloud_memory_upload_producer(data_str, 0);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);

    size = zcloud_upload_producer_get_size(o_p, &err);
    gerror_is_clear(&err, "no error while getting size of zero-length buffer");
    is_gsize(size, 0, "check reported size for zero-length buffer");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    gerror_is_clear(&err, "no error while calculating MD5 hash of zero-length buffer");
    is_md5(md5, empty_md5, "check MD5 hash for zero-length buffer");
    g_byte_array_free(md5, TRUE);

    buf = g_malloc0(data_str_len+1);
    read = zcloud_upload_producer_read(o_p, buf, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from zero-length buffer");
    is_gsize(read, 0, "read 0 bytes from zero-length buffer");
    is_string((gchar*) buf, "", "empty read from zero-length buffer");
    g_free(buf);

    g_object_unref(o);

    o = zcloud_memory_upload_producer(data_str, data_str_len);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);

    size = zcloud_upload_producer_get_size(o_p, &err);
    gerror_is_clear(&err, "no error while getting size of test string");
    is_gsize(size, data_str_len, "check reported size for test string");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    gerror_is_clear(&err, "no error while calculating MD5 hash of test string");
    is_md5(md5, buf_md5, "check MD5 hash for test string");
    g_byte_array_free(md5, TRUE);

    buf = g_malloc0(data_str_len+1);
    read = zcloud_upload_producer_read(o_p, buf, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from buffer");
    is_gsize(read, data_str_len, "read all bytes from buffer");
    is_string((gchar*) buf, data_str, "read test string from buffer");
    g_free(buf);

    buf = g_malloc0(data_str_len+1);
    read = zcloud_upload_producer_read(o_p, buf, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from buffer");
    is_gsize(read, 0, "read 0 bytes from exhausted test buffer");
    is_string((gchar*) buf, "", "empty read exhausted buffer");
    g_free(buf);

    ok = zcloud_upload_producer_reset(o_p, &err);
    is_gboolean(ok, TRUE, "reset test buffer returned true");
    gerror_is_clear(&err, "no error reseting buffer");

    buf = g_malloc0(data_str_len+1);

    read = zcloud_upload_producer_read(o_p, buf, 1, &err);
    gerror_is_clear(&err, "no error reading from buffer");
    is_gsize(read, 1, "read one bytes from buffer");
    is_gchar(buf[0], 'T', "read first char from buffer");

    read = zcloud_upload_producer_read(o_p, buf+1, 1, &err);
    gerror_is_clear(&err, "no error reading from buffer");
    is_gsize(read, 1, "read one bytes from buffer");
    is_gchar(buf[1], 'h', "read second char from buffer");

    read = zcloud_upload_producer_read(o_p, buf+2, data_str_len, &err);
    gerror_is_clear(&err, "no error reading from buffer");
    is_gsize(read, data_str_len-2, "read remaining bytes from buffer");
    is_string((gchar*) buf, data_str, "complete read matches data");

    g_free(buf);

    g_object_unref(o);
}
