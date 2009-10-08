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
#include <stdlib.h>
#include "zcloud.h"
#include "test.h"
int main(int argc, char **argv)
{
    GError *error = NULL;
    ZCloudStore *store;
    char *spec;

    zcloud_init(&error);
    gerror_is_clear(&error, "initialize libzcloud");

    /* die on anything worse than a message */
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);

    spec = "s3:";
    store = zcloud_store_new(spec, &error, NULL);
    is_null(store, "creating S3 store with no parameters returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with no parameters triggers an error");

    store = zcloud_store_new(spec, &error, "access-key", "a", NULL);
    is_null(store, "creating S3 store with only access key return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with only access key triggers an error");

    store = zcloud_store_new(spec, &error, "secret-key", "a", NULL);
    is_null(store, "creating S3 store with only access key return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with only secret key triggers an error");

    store = zcloud_store_new(spec, &error, "product-token", "a", NULL);
    is_null(store, "creating S3 store with only access key return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with only product token triggers an error");

    store = zcloud_store_new(spec, &error, "user-token", "a", NULL);
    is_null(store, "creating S3 store with only user token return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with only user token triggers an error");

    store = zcloud_store_new(spec, &error, "access-key", "a", "product-token", "b", NULL);
    is_null(store, "creating S3 store with access key and product token returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with access key and product token triggers an error");

    store = zcloud_store_new(spec, &error, "access-key", "a", "user-token", "b", NULL);
    is_null(store, "creating S3 store with access key and product token returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with access key and product token triggers an error");

    store = zcloud_store_new(spec, &error, "access-key", "a", "user-token", "b", NULL);
    is_null(store, "creating S3 store with access key and user token returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with access key and user token triggers an error");

    store = zcloud_store_new(spec, &error, "access-key", "a", "user-token", "b", NULL);
    is_null(store, "creating S3 store with access key and user token returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with access key and user token triggers an error");

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "b", NULL);
    isnt_null(store, "creating S3 store with both access and secret keys returns non-NULL");
    ok(ZCLOUD_IS_STORE(store), "creating S3 store with both access and secret keys returns a store");
    gerror_is_clear(&error, "creating S3 store with both access and secret keys is okay");
    g_object_unref(store);

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "", NULL);
    is_null(store, "creating S3 store with access key and empty secret key returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with access key and empty secret key triggers an error");

    store = zcloud_store_new(spec, &error,
        "access-key", "", "secret-key", "b", NULL);
    is_null(store, "creating S3 store with access key and empty secret key returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                   "creating S3 store with access key and empty secret key triggers an error");

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "user-token", "b", "product-token", "c", NULL);
    is_null(store, "creating S3 store with access key, user token, and "
            "product token (but not secret key) return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                  "creating S3 store with access key, user token, and "
                  "product token (but not secret key) triggers an error");

    store = zcloud_store_new(spec, &error,
        "secret-key", "a", "user-token", "b", "product-token", "c", NULL);
    is_null(store, "creating S3 store with secret key, user token, and "
            "product token (but not secret key) return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                  "creating S3 store with access key, user token, and "
                  "product token (but not secret key) triggers an error");

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "b", "user-token", "c", NULL);
    is_null(store, "creating S3 store with access key, secret key, and "
            "user token (but not product token) return NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                  "creating S3 store with access key, secret key, and "
                  "user token (but not product token) triggers an error");

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "b", "product-token", "c", NULL);
    isnt_null(store, "creating S3 store with access key, secret key, and "
            "product token (but not user token) returns non-NULL");
    ok(ZCLOUD_IS_STORE(store), "creating S3 store with access key, secret key, and "
            "product token (but not user token) returns a store");
    gerror_is_clear(&error, "creating S3 store with access key, secret key, and "
                  "product token (but not user token) is okay");
    g_object_unref(store);

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "b", "user-token", "c", "product-token", "d", NULL);
    isnt_null(store, "creating S3 store with access key, secret key, "
              "user token, and product token returns non-NULL");
    ok(ZCLOUD_IS_STORE(store), "creating S3 store with access key, secret key, "
       "user token, and product token returns a store");
    gerror_is_clear(&error, "creating S3 store with access key, secret key, "
                    "user token, and product token is okay");
    g_object_unref(store);

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "b", "user-token", "", "product-token", "d", NULL);
    isnt_null(store, "creating S3 store with access key, secret key, "
              "empty user token, and product token returns non-NULL");
    ok(ZCLOUD_IS_STORE(store), "creating S3 store with access key, secret key, "
       "empty user token, and product token returns a store");
    gerror_is_clear(&error, "creating S3 store with access key, secret key, "
                    "empty user token, and product token is okay");
    g_object_unref(store);

    store = zcloud_store_new(spec, &error,
        "access-key", "a", "secret-key", "b", "user-token", "c", "product-token", "", NULL);
    is_null(store, "creating S3 store with access key, secret key, "
              "user token, and empty product token returns NULL");
    gerror_is_set(&error, "must set a non-empty*", ZCERR_PARAMETER,
                  "creating S3 store with access key, secret key, and "
                  "user token, and empty product token triggers an error");

    {
        char *test_bucket, *test_bucket_spec,
            *access_key, *secret_key, *user_token, *product_token;
        const char *test_key = "test-key";
        ZCloudStore *test_store;
        gboolean op_ok;
        access_key = getenv("S3_ACCESS_KEY");
        secret_key = getenv("S3_SECRET_KEY");
        user_token = getenv("S3_USER_TOKEN");
        product_token = getenv("S3_PRODUCT_TOKEN");

        if (!access_key || !secret_key) {
            fprintf(stderr, "set environment variables S3_ACCESS_KEY and S3_SECRET_KEY to run full tests.\n"
                    "optionally set S3_USER_TOKEN and S3_PRODUCT_TOKEN too.\n");
            goto end;
        }

        {
            char host_buf[512];
            const size_t host_buf_size = sizeof(host_buf)/sizeof(char);
            gchar *tmp;
            if (gethostname(host_buf, host_buf_size) < 0) {
                g_error("error getting hostname: %s", strerror(errno));
            }
            host_buf[host_buf_size-1] = '\0';
            tmp = g_strconcat(access_key, "-s3test-", host_buf, NULL);
            test_bucket = g_ascii_strup(tmp, -1);
            test_bucket_spec = g_strconcat("s3:", test_bucket, NULL);
            g_free(tmp);
        }

        test_store = zcloud_store_new(test_bucket_spec, &error,
            "access-key", access_key, "secret-key", secret_key,
            "user-token", user_token, "product-token", product_token, NULL);
        isnt_null(test_store, "creating S3 store with credentials from env returns non-NULL");
        ok(ZCLOUD_IS_STORE(test_store), "creating S3 store with credentials from env returns a store");
        gerror_is_clear(&error, "creating S3 store with from env credentials is okay");

        store = zcloud_store_new(spec, &error,
            "access-key", access_key, "secret-key", secret_key,
            "user-token", user_token, "product-token", product_token, NULL);
        isnt_null(test_store, "creating S3 store with credentials from env returns non-NULL");
        ok(ZCLOUD_IS_STORE(test_store), "creating S3 store with credentials from env returns a store");
        gerror_is_clear(&error, "creating S3 store with from env credentials is okay");

        op_ok = zcloud_store_exists(store, test_bucket, NULL, &error);
        if (op_ok) {
            fprintf(stderr, "Test bucket (%s) seems to already exist. "
                    "Stopping early to avoid data loss\n", test_bucket);
            goto end;
        }
        /* error may be unexpected, but run the tests to learn more */
        g_clear_error(&error);

        op_ok = zcloud_store_create(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "create test bucket returned ok");
        gerror_is_clear(&error, "create test bucket didn't cause error");

        op_ok = zcloud_store_exists(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "test bucket exists now");
        gerror_is_clear(&error, "test bucket exist check didn't cause error");

        op_ok = zcloud_store_create(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "create test bucket (again) returned ok");
        gerror_is_clear(&error, "create test bucket (again) didn't cause error");

        op_ok = zcloud_store_delete(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "delete test bucket returned ok");
        gerror_is_clear(&error, "delete test bucket didn't cause error");

        /* would like to call exists (and expect to get FALSE w/o error), but
           the delete might not be visible */

        op_ok = zcloud_store_delete(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "delete test bucket (again) returned ok");
        gerror_is_clear(&error, "delete test bucket (again) didn't cause error");

        /* TODO: list buckets, should NOT have test bucket */

        op_ok = zcloud_store_create(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "create test bucket (after delete) returned ok");
        gerror_is_clear(&error, "create test bucket (after delete) didn't cause error");

        /* TODO: list buckets, should have test_bucket */

        op_ok = zcloud_store_create(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "create test bucket (again after delete) returned ok");
        gerror_is_clear(&error, "create test bucket (again after delete) didn't cause error");

        /* try some operations within the test bucket... */
        spec = test_bucket_spec;

        op_ok = zcloud_store_exists(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, FALSE, "test key doesn't exist in empty bucket");
        gerror_is_set(&error, NULL, ZCERR_MISSING,
            "testing for existence of test key (w/ empty bucket) caused the expected error");

        {
            ZCloudSListListConsumer *list_con = zcloud_slist_list_consumer();
            GSList *l;

            op_ok = zcloud_store_list(test_store, "%s", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket");
            gerror_is_clear(&error, "listing keys for empty bucket didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            is_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "didn't find (as expected) the test key in the empty bucket listing");

            g_object_unref(list_con);
        }

        op_ok = zcloud_store_create(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, TRUE, "create test key returned ok");
        gerror_is_clear(&error, "create test key didn't cause error");

        op_ok = zcloud_store_exists(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, TRUE, "test key now exists");
        gerror_is_clear(&error, "testing for existence of test key didn't cause error");

        op_ok = zcloud_store_delete(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, TRUE, "delete test key returned ok");
        gerror_is_clear(&error, "delete test key didn't cause error");

        op_ok = zcloud_store_delete(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, TRUE, "delete test key (again) returned ok");
        gerror_is_clear(&error, "delete test key (again) didn't cause error");

        op_ok = zcloud_store_create(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, TRUE, "create test key (after delete) returned ok");
        gerror_is_clear(&error, "create test key (after delete) didn't cause error");

        {
            ZCloudSListListConsumer *list_con;
            GSList *l;

            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, test_key, ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (exact)");
            gerror_is_clear(&error, "listing keys for empty bucket (exact) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            isnt_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "found the test key in the bucket listing (exact)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "%s", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (wild)");
            gerror_is_clear(&error, "listing keys for empty bucket didn't cause error (wild)");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            isnt_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "found the test key in the bucket listing (wild)");

            op_ok = zcloud_store_list(test_store, NULL, ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (implicit wild)");
            gerror_is_clear(&error, "listing keys for empty bucket didn't cause error (implicit wild)");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            isnt_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "found the test key in the bucket listing (implicit wild)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "te%s", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (pre)");
            gerror_is_clear(&error, "listing keys for empty bucket (pre) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            isnt_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "found the test key in the bucket listing (w/ pre)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "%sey", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (suf)");
            gerror_is_clear(&error, "listing keys for empty bucket (suf) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            isnt_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "found the test key in the bucket listing (w/ suf)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "t%sy", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (mid)");
            gerror_is_clear(&error, "listing keys for empty bucket (mid) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            isnt_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "found the test key in the bucket listing (w/ mid)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "%%s", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (escaped)");
            gerror_is_clear(&error, "listing keys for empty bucket (escaped) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            is_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "didn't find (as expected) the test key in the bucket listing (escaped)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "te", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (pre no wild)");
            gerror_is_clear(&error, "listing keys for empty bucket (pre no wild) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            is_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "didn't find (as expected) the test key in the bucket listing (pre no wild)");

            g_object_unref(list_con);
            list_con = zcloud_slist_list_consumer();

            op_ok = zcloud_store_list(test_store, "", ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
            is_gboolean(op_ok, TRUE, "listing keys for empty bucket (empty)");
            gerror_is_clear(&error, "listing keys for empty bucket (empty) didn't cause error");

            l = zcloud_slist_list_consumer_grab_contents(list_con);
            is_null(g_slist_find_custom(l, test_key, (GCompareFunc) strcmp),
                "didn't find the test key in the bucket listing (w/ empty)");

            g_object_unref(list_con);
        }

        /* upload and download tests */
        {
            const gchar *data_str = "The quick brown fox jumps over the lazy dog";
            gsize data_str_len;
            /* note: doesn't include NULL */
            data_str_len = strlen(data_str);

            /* upload */
            {
                ZCloudUploadProducer *up_prod;
                up_prod = ZCLOUD_UPLOAD_PRODUCER(
                    zcloud_memory_upload_producer(data_str, data_str_len));

                op_ok = zcloud_store_upload(test_store, test_key, up_prod, NULL, &error);
                is_gboolean(op_ok, TRUE, "uploading to test key returned ok");
                gerror_is_clear(&error, "uploading to test key didn't cause error");

                g_object_unref(up_prod);
            }

            /* download */
            {
                ZCloudGrowingMemoryDownloadConsumer *gm_down_con;
                ZCloudDownloadConsumer *down_con;
                guint8 *down_buf;
                gsize down_buf_size;

                gm_down_con = zcloud_growing_memory_download_consumer(data_str_len*2);
                down_con = ZCLOUD_DOWNLOAD_CONSUMER(gm_down_con);

                op_ok = zcloud_store_download(test_store, test_key,
                    ZCLOUD_DOWNLOAD_CONSUMER(down_con), NULL, &error);
                is_gboolean(op_ok, TRUE, "downloading from test key returned ok");
                gerror_is_clear(&error, "downloading from test key didn't cause error");

                down_buf = zcloud_growing_memory_download_consumer_get_contents(gm_down_con, &down_buf_size);
                {
                    GByteArray got = {down_buf, down_buf_size};
                    GByteArray exp = {(guint8 *)data_str, data_str_len};
                    is_byte_array(&got, &exp, "downloaded data matches uploaded data");
                }

                g_object_unref(down_con);
            }

            /* TODO test progress listener */
        }

        /* try to delete non-empty bucket, get error */
        op_ok = zcloud_store_delete(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, FALSE, "deleting non-empty bucket returned not ok (as expected)");
        gerror_is_set(&error, "*BucketNotEmpty*",
            ZCERR_LASTING, "deleting non-empty bucket caused the expected error");

        /* delete test key */
        op_ok = zcloud_store_delete(test_store, test_key, NULL, &error);
        is_gboolean(op_ok, TRUE, "delete test key (after roundtrip) returned ok");
        gerror_is_clear(&error, "delete test key (after roundtrip) didn't cause error");

        /* try to delete empty bucket */
        op_ok = zcloud_store_delete(store, test_bucket, NULL, &error);
        is_gboolean(op_ok, TRUE, "deleting now-empty bucket returned ok");
        gerror_is_clear(&error, "deleting now-empty bucket didn't cause error");

        /* try to upload a bucket, get error */
        {
            ZCloudUploadProducer *up_prod;
            up_prod = ZCLOUD_UPLOAD_PRODUCER(
                zcloud_memory_upload_producer("", 0));

            op_ok = zcloud_store_upload(store, test_bucket, up_prod, NULL, &error);
            is_gboolean(op_ok, FALSE, "uploading test bucket returned not ok (as expected)");
            gerror_is_set(&error, NULL,
                ZCERR_PARAMETER, "uploading test bucket caused the expected error");

            g_object_unref(up_prod);
        }

        /* try to download a bucket, get error */
        {
            ZCloudDownloadConsumer *down_con;
            down_con = ZCLOUD_DOWNLOAD_CONSUMER(
                zcloud_growing_memory_download_consumer(1));

            op_ok = zcloud_store_download(store, test_bucket, down_con, NULL, &error);
            is_gboolean(op_ok, FALSE, "downloading test bucket returned not ok (as expected)");
            gerror_is_set(&error, NULL,
                ZCERR_PARAMETER, "downloading test bucket caused the expected error");

            g_object_unref(down_con);
        }

        g_object_unref(store);
        g_object_unref(test_store);
        g_free(test_bucket);
        g_free(test_bucket_spec);
    }

end:
    fprintf(stderr, tests_failed? "One or more tests FAILED\n" : "SUCCESS\n");
    return tests_failed? 1 : 0;
}
