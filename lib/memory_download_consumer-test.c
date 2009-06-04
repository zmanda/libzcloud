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

void
test_memory_download_consumer(void)
{
    ZCloudMemoryDownloadConsumer *o;
    ZCloudDownloadConsumer *o_p;
    GError *err = NULL;
    const gchar *data_str = "The quick brown fox jumps over the lazy dog";
    gsize data_str_len, wrote, get_size;
    guint8 *buf;
    gboolean ok;

    g_type_init();  /* TODO */
    data_str_len = strlen(data_str);

    o = zcloud_memory_download_consumer(data_str_len+1);
    o_p = ZCLOUD_DOWNLOAD_CONSUMER(o);

    wrote = zcloud_download_consumer_write(o_p, data_str, 0, &err);
    gerror_is_clear(&err, "no error writing 0 bytes to buffer");
    is_gsize(wrote, 0, "wrote 0 bytes to buffer");

    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, FALSE);
    is_gsize(get_size, 0, "after writing zero bytes, buffer has size 0");

    wrote = zcloud_download_consumer_write(o_p, data_str, 1, &err);
    gerror_is_clear(&err, "no error writing 1 bytes to buffer");
    is_gsize(wrote, 1, "wrote 1 bytes to buffer");

    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, TRUE);
    is_gsize(get_size, 1, "after writing 1 bytes, buffer has size 1");
    is_gchar((gchar) buf[0], 'T', "got back the one character we wrote");
    buf[0] = 'i';
    g_free(buf);
    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, FALSE);
    is_gsize(get_size, 1, "buffer still has size 1");
    is_gchar((gchar) buf[0], 'T', "after changing a copy, the buffer contents remain unchanged");

    wrote = zcloud_download_consumer_write(o_p, data_str+1, data_str_len, &err);
    gerror_is_clear(&err, "no error writing remaining bytes to buffer");
    is_gsize(wrote, data_str_len, "filled remaining buffer space");

    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, FALSE);
    is_gsize(get_size, data_str_len+1, "after writing all data, buffer size matches");
    is_string((gchar*) buf, data_str, "buffer and input data match");

    wrote = zcloud_download_consumer_write(o_p, data_str, data_str_len, &err);
    gerror_is_clear(&err, "no error writing more bytes to buffer");
    is_gsize(wrote, 0, "wrote no bytes to full buffer");

    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, FALSE);
    is_gsize(get_size, data_str_len+1, "after writing more data to full buffer, buffer size is the same");
    is_string((gchar*) buf, data_str, "buffer and input data still match");

    ok = zcloud_download_consumer_reset(o_p, &err);
    is_gboolean(ok, TRUE, "reset test buffer returned true");
    gerror_is_clear(&err, "no error reseting buffer");

    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, FALSE);
    is_gsize(get_size, 0, "after reset, size is 0");

    wrote = zcloud_download_consumer_write(o_p, data_str+2, 1, &err);
    gerror_is_clear(&err, "no error writing 1 bytes to buffer");
    is_gsize(wrote, 1, "wrote 1 bytes to buffer");

    buf = zcloud_memory_download_consumer_get_contents(o, &get_size, FALSE);
    is_gsize(get_size, 1, "after writing 1 bytes, buffer has size 1");
    is_gchar((gchar) buf[0], 'e', "got back the one character we wrote");

    g_object_unref(o);
}
