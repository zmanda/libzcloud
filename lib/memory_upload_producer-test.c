#include <stdio.h>
#include <string.h>

#include "zcloud.h"

gboolean
not_error(GError *err, const char *desc)
{
    if (err) {
        fprintf(stderr, "OK %s\n", desc? desc : "");
        return TRUE;
    } else {
        fprintf(stderr, "NOT OK %s\n", desc? desc : "");
        fprintf(stderr, "\tgot error: %s\n", err->message? err->message : "");
        return FALSE;
    }
}

gboolean
is_gsize(gsize got, gsize expected, const char *desc)
{
    if (got == expected) {
        fprintf(stderr, "OK %s\n", desc? desc : "");
        return TRUE;
    } else {
        fprintf(stderr, "NOT OK %s\n", desc? desc : "");
        /* XXX: G_GSIZE_FORMAT only exists in GLib 2.3.1+ */
        fprintf(stderr, "\tgot: " G_GSIZE_FORMAT " expected: " G_GSIZE_FORMAT "\n",
            got, expected);
        return FALSE;
    }
}

gboolean
is_string(const gchar *got, const gchar *expected, const char *desc)
{
    if (0 == strcmp(got, expected)) {
        fprintf(stderr, "OK %s\n", desc? desc : "");
        return TRUE;
    } else {
        fprintf(stderr, "NOT OK %s\n", desc? desc : "");
        fprintf(stderr, "\tgot: %s expected: %s\n", got, expected);
        return FALSE;
    }
}

gchar *
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
is_byte_array(const GByteArray *got, const GByteArray *expected, const char *desc)
{
    gchar *got_s, *expected_s;
    gboolean ret;

    got_s = hex_encode(got);
    expected_s = hex_encode(expected);

    ret = is_string(got_s, expected_s, desc);

    g_free(got_s);
    g_free(expected_s);

    return ret;
}

int
main()
{
    ZCloudMemoryUploadProducer *o;
    ZCloudUploadProducer *o_p;
    GError *err;
    const gchar *buf = "The quick brown fox jumps over the lazy dog",
        *empty_md5 = "d41d8cd98f00b204e9800998ecf8427e",
        *buf_md5 = "9e107d9d372bb6826bd81d3542a419d6";
    gchar *md5_s;
    gsize buf_len, size;
    GByteArray *md5;
    int ret = 0;

    g_type_init();
    buf_len = strlen(buf);

    o = g_object_new(ZCLOUD_TYPE_MEMORY_UPLOAD_PRODUCER,
        "buffer", buf, "buffer-length", (guint) 0);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);
    size = zcloud_upload_producer_get_size(o_p, &err);
    if (!not_error(err, "no error while getting size of zero-length buffer")) {
        g_error_free(err);
        ret = 1;
    }
    if (!is_gsize(size, 0, "check reported size for zero-length buffer")) {
        ret = 1;
    }
    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    if (!not_error(err, "no error while calculating MD5 hash of zero-length buffer")) {
        g_error_free(err);
        ret = 1;
    }
    md5_s = hex_encode(md5);
    if (!is_string(md5_s, empty_md5, "check MD5 hash for zero-length buffer")) {
        ret = 1;
    }
    g_byte_array_free(md5, TRUE);
    g_free(md5_s);
    g_object_unref(o);

    o = g_object_new(ZCLOUD_TYPE_MEMORY_UPLOAD_PRODUCER,
        "buffer", buf, "buffer-length", (guint) buf_len);
    o_p = ZCLOUD_UPLOAD_PRODUCER(o);
    size = zcloud_upload_producer_get_size(o_p, &err);
    if (!not_error(err, "no error while getting size of test string")) {
        g_error_free(err);
        ret = 1;
    }
    if (!is_gsize(size, 0, "check reported size for test string")) {
        ret = 1;
    }
    md5 = zcloud_upload_producer_calculate_md5(o_p, &err);
    if (!not_error(err, "no error while calculating MD5 hash of test string")) {
        g_error_free(err);
        ret = 1;
    }
    md5_s = hex_encode(md5);
    if (!is_string(md5_s, buf_md5, "check MD5 hash for test string")) {
        ret = 1;
    }
    g_byte_array_free(md5, TRUE);
    g_free(md5_s);
    g_object_unref(o);

    return ret;
}
