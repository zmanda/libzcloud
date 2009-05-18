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

static gboolean
is_md5(
    GByteArray *got,
    const gchar *bytestring,
    const gchar *msg)
{
    gboolean rv;
    GByteArray *arr = g_byte_array_sized_new(16);
    g_byte_array_append(arr, (const guint8 *)bytestring, 16);

    rv = is_byte_array(got, arr, msg);

    g_byte_array_free(arr, 1);
    return rv;
}

void
test_memory_upload_producer(void)
{
    ZCloudMemoryUploadProducer *o;
    ZCloudUploadProducer *o_p;
    GError *err = NULL;
    const gchar *buf = "The quick brown fox jumps over the lazy dog",
        *buf_md5 = "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82\x6b\xd8\x1d\x35\x42\xa4\x19\xd6",
        *empty_md5 = "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e";
    gsize buf_len, size;
    GByteArray *md5;

    g_type_init();  /* TODO */
    buf_len = strlen(buf);

    o = zcloud_memory_upload_producer(buf, 0);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);

    size = zcloud_upload_producer_get_size(o_p, &err);
    gerror_is_clear(&err, "no error while getting size of zero-length buffer");
    is_gsize(size, 0, "check reported size for zero-length buffer");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    gerror_is_clear(&err, "no error while calculating MD5 hash of zero-length buffer");
    is_md5(md5, empty_md5, "check MD5 hash for zero-length buffer");

    g_byte_array_free(md5, TRUE);
    g_object_unref(o);

    o = zcloud_memory_upload_producer(buf, buf_len);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);

    size = zcloud_upload_producer_get_size(o_p, &err);
    gerror_is_clear(&err, "no error while getting size of test string");
    is_gsize(size, buf_len, "check reported size for test string");

    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    gerror_is_clear(&err, "no error while calculating MD5 hash of test string");
    is_md5(md5, buf_md5, "check MD5 hash for test string");

    g_byte_array_free(md5, TRUE);
    g_object_unref(o);
}
