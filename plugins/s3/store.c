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
#ifdef HAVE_CONFIG_H
/* use a relative path here to avoid conflicting with Perl's config.h. */
#include "../config/config.h"
#endif
#include <string.h>
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <curl/curl.h>

/* Constant renamed after version 7.10.7 */
#ifndef CURLINFO_RESPONSE_CODE
#define CURLINFO_RESPONSE_CODE CURLINFO_HTTP_CODE
#endif

/* We don't need OpenSSL's kerberos support, and it's broken in
 * RHEL 3 anyway. */
#define OPENSSL_NO_KRB5

#ifdef HAVE_OPENSSL_HMAC_H
# include <openssl/hmac.h>
#else
# ifdef HAVE_CRYPTO_HMAC_H
#  include <crypto/hmac.h>
# else
#  ifdef HAVE_HMAC_H
#   include <hmac.h>
#  endif
# endif
#endif

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/md5.h>

#include <glib.h>

#include "store.h"
#include "util.h"

/* Maximum key length as specified in the S3 documentation
 * (*excluding* null terminator) */
#define S3_MAX_KEY_LENGTH 1024

#define AMAZON_SECURITY_HEADER "x-amz-security-token"
#define AMAZON_BUCKET_CONF_TEMPLATE "\
  <CreateBucketConfiguration>\n\
    <LocationConstraint>%s</LocationConstraint>\n\
  </CreateBucketConfiguration>"

#define AMAZON_WILDCARD_LOCATION "*"

/* parameters for exponential backoff in the face of retriable errors */

/* start at 0.01s */
#define EXPONENTIAL_BACKOFF_START_USEC G_USEC_PER_SEC/100
/* double at each retry */
#define EXPONENTIAL_BACKOFF_BASE 2
/* retry 14 times (for a total of about 3 minutes spent waiting) */
#define EXPONENTIAL_BACKOFF_MAX_RETRIES 14

/* general "reasonable size" parameters */
#define MAX_ERROR_RESPONSE_LEN (100*1024)


/* Results which should always be marked as missing */
#define RESULT_HANDLING_ALWAYS_MISSING \
        { 404,  S3_ERROR_NoSuchBucket,     0,                          ZCERR_MISSING }, \
        { 404,  S3_ERROR_Unknown,          0,                          ZCERR_MISSING }

/* Results which should always be retried */
#define RESULT_HANDLING_ALWAYS_RETRY \
        { 400,  S3_ERROR_RequestTimeout,     0,                          ZCERR_EPHEMERAL }, \
        { 409,  S3_ERROR_OperationAborted,   0,                          ZCERR_EPHEMERAL }, \
        { 412,  S3_ERROR_PreconditionFailed, 0,                          ZCERR_EPHEMERAL }, \
        { 500,  S3_ERROR_InternalError,      0,                          ZCERR_EPHEMERAL }, \
        { 501,  S3_ERROR_NotImplemented,     0,                          ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_COULDNT_CONNECT,      ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_COULDNT_RESOLVE_HOST, ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_PARTIAL_FILE,         ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_OPERATION_TIMEOUTED,  ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_SEND_ERROR,           ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_RECV_ERROR,           ZCERR_EPHEMERAL }, \
        { 0,    0,                           CURLE_GOT_NOTHING,          ZCERR_EPHEMERAL }

typedef struct S3InternalData_s {
    S3Store *self;
    ZCloudUploadProducer *up_prod;
    ZCloudGrowingMemoryDownloadConsumer *err_con;
    ZCloudDownloadConsumer *user_con;
    gboolean err_too_big;
    GError *user_con_err;
    GError *up_prod_err;

    gboolean headers_done;
    char *etag;
} S3InternalData;

/*
 * CURL functions
 */

/*
 * a CURLOPT_WRITEFUNCTION to write part of the response to err_con and
 * everything to user_con
 */
static size_t
s3_internal_write_func(void *ptr, size_t size, size_t nmemb, void *stream);

/*
 * a CURLOPT_READFUNCTION to read data from a ZCloudUploadProducer
 */
static size_t
s3_internal_read_func(void *ptr, size_t size, size_t nmemb, void *stream);

/*
 * a CURLOPT_HEADERFUNCTION to save the ETag header only.
 */
static size_t
s3_internal_header_func(void *ptr, size_t size, size_t nmemb, void *stream);

/*
 * a CURLOPT_HEADERFUNCTION to save the ETag header only.
 */
static int
curl_debug_func(CURL *curl G_GNUC_UNUSED,
    curl_infotype type,
    char *s,
    size_t len,
    void *unused G_GNUC_UNUSED);

/*
 * S3 errors
 */

/* This preprocessor magic will enumerate constants named S3_ERROR_XxxYyy for
 * each of the errors in parentheses.
 *
 * see http://docs.amazonwebservices.com/AmazonS3/2006-03-01/ErrorCodeList.html
 * for Amazon's breakdown of error responses.
 */
#define S3_ERROR_LIST \
    S3_ERROR(None), \
    S3_ERROR(AccountProblem), \
    S3_ERROR(AllAccessDisabled), \
    S3_ERROR(AmbiguousGrantByEmailAddress), \
    S3_ERROR(OperationAborted), \
    S3_ERROR(BadDigest), \
    S3_ERROR(BucketAlreadyExists), \
    S3_ERROR(BucketAlreadyOwnedByYou), \
    S3_ERROR(BucketNotEmpty), \
    S3_ERROR(CredentialsNotSupported), \
    S3_ERROR(EntityTooLarge), \
    S3_ERROR(IncompleteBody), \
    S3_ERROR(InternalError), \
    S3_ERROR(InvalidAccessKeyId), \
    S3_ERROR(InvalidArgument), \
    S3_ERROR(InvalidBucketName), \
    S3_ERROR(InvalidDigest), \
    S3_ERROR(InvalidRange), \
    S3_ERROR(InvalidSecurity), \
    S3_ERROR(InvalidSOAPRequest), \
    S3_ERROR(InvalidStorageClass), \
    S3_ERROR(InvalidTargetBucketForLogging), \
    S3_ERROR(KeyTooLong), \
    S3_ERROR(InvalidURI), \
    S3_ERROR(MalformedACLError), \
    S3_ERROR(MaxMessageLengthExceeded), \
    S3_ERROR(MetadataTooLarge), \
    S3_ERROR(MethodNotAllowed), \
    S3_ERROR(MissingAttachment), \
    S3_ERROR(MissingContentLength), \
    S3_ERROR(MissingSecurityElement), \
    S3_ERROR(MissingSecurityHeader), \
    S3_ERROR(NoLoggingStatusForKey), \
    S3_ERROR(NoSuchBucket), \
    S3_ERROR(NoSuchKey), \
    S3_ERROR(NotImplemented), \
    S3_ERROR(NotSignedUp), \
    S3_ERROR(PreconditionFailed), \
    S3_ERROR(RequestTimeout), \
    S3_ERROR(RequestTimeTooSkewed), \
    S3_ERROR(RequestTorrentOfBucketError), \
    S3_ERROR(SignatureDoesNotMatch), \
    S3_ERROR(TooManyBuckets), \
    S3_ERROR(UnexpectedContent), \
    S3_ERROR(UnresolvableGrantByEmailAddress), \
    S3_ERROR(Unknown), \
    S3_ERROR(END)

typedef enum {
#define S3_ERROR(NAME) S3_ERROR_ ## NAME
    S3_ERROR_LIST
#undef S3_ERROR
} s3_error_code_t;

static char * s3_error_code_names[] = {
#define S3_ERROR(NAME) #NAME
    S3_ERROR_LIST
#undef S3_ERROR
};
/* Convert an s3 error name to an error code.  This function
 * matches strings case-insensitively, and is appropriate for use
 * on data from the network.
 *
 * @param s3_error_code: the error name
 * @returns: the error code (see constants in s3.h)
 */
static s3_error_code_t
s3_error_code_from_name(char *s3_error_name);

/*
 * Result handling
 */

/* result handling is specified by a static array of result_handling structs,
 * which match based on response_code (from HTTP) and S3 error code.  The result
 * given for the first match is used.  0 acts as a wildcard for both response_code
 * and s3_error_code.  The list is terminated with a struct containing 0 for both
 * response_code and s3_error_code; the result for that struct is the default
 * result.
 *
 * See RESULT_HANDLING_ALWAYS_RETRY for an example.
 */

typedef struct result_handling {
    long response_code;
    s3_error_code_t s3_error_code;
    CURLcode curl_code;
    ZCloudError result;
} result_handling_t;

/* Construct the URL for an Amazon S3 REST request.
 *
 * A new string is allocated and returned; it is the responsiblity of the caller.
 *
 * @param self: the S3Store object
 * @param verb: capitalized verb for this request ('PUT', 'GET', etc.)
 * @param bucket: the bucket being accessed, or NULL for none
 * @param key: the key being accessed, or NULL for none
 * @param subresource: the sub-resource being accessed (e.g. "acl"), or NULL for none
 * @param use_subdomain: if TRUE, a subdomain of s3.amazonaws.com will be used
 */
static char *
build_url(const char *bucket,
      const char *key,
      const char *subresource,
      const char *query,
      gboolean use_subdomain,
      gboolean use_ssl);

/* Create proper authorization headers for an Amazon S3 REST
 * request to C{headers}.
 *
 * @note: C{X-Amz} headers (in C{headers}) must
 *  - be in lower-case
 *  - be in alphabetical order
 *  - have no spaces around the colon
 * (don't yell at me -- see the Amazon Developer Guide)
 *
 * @param self: the S3Store object
 * @param verb: capitalized verb for this request ('PUT', 'GET', etc.)
 * @param bucket: the bucket being accessed, or NULL for none
 * @param key: the key being accessed, or NULL for none
 * @param subresource: the sub-resource being accessed (e.g. "acl"), or NULL for none
 * @param md5_hash: the MD5 hash of the request body, or NULL for none
 * @param use_subdomain: if TRUE, a subdomain of s3.amazonaws.com will be used
 */
static struct curl_slist *
authenticate_request(S3Store *self,
                     const char *verb,
                     const char *bucket,
                     const char *key,
                     const char *subresource,
                     const char *md5_hash,
                     gboolean use_subdomain);

/* Interpret the response to an S3 request.
 *
 * @param self: The S3Store object
 * @param curl_code: return value from CURL
 * @param curl_error_buffer: human-readable message from CURL
 * @param int_data: pointer to some info saved from the last response
 * @param content_md5: The hex-encoded MD5 hash of the request body,
 *     which will be checked against the response's ETag header.
 *     If NULL, the header is not checked.
 *     If non-NULL, then the body should have the response headers at its beginnning.
 * @param result_handling: instructions for handling the results; see above.
 * @param err: storage for a GError pointer; will be NULL or a GError with
 *     domain ZCLOUD_ERROR and a code that is one of:
 *     ZCERR_EPHEMERAL, ZCERR_LASTING, ZCERR_MISSING
 */
static void
interpret_response(S3Store *self,
                   CURLcode curl_code,
                   char *curl_error_buffer,
                   S3InternalData *int_data,
                   const char *content_md5,
                   const result_handling_t *result_handling,
                   GError **err);


/* Perform an S3 operation.  This function handles all of the details
 * of retryig requests and so on.
 *
 * The concepts of bucket and keys are defined by the Amazon S3 API.
 * See: "Components of Amazon S3" - API Version 2006-03-01 pg. 8
 *
 * Individual sub-resources are defined in several places. In the REST API,
 * they they are represented by a "flag" in the "query string".
 * See: "Constructing the CanonicalizedResource Element" - API Version 2006-03-01 pg. 60
 *
 * @param self: the S3Store object
 * @param verb: the HTTP request method
 * @param bucket: the bucket to access, or NULL for none
 * @param key: the key to access, or NULL for none
 * @param subresource: the "sub-resource" to request (e.g. "acl") or NULL for none
 * @param query: the query string to send (not including th initial '?'),
 * or NULL for none
 * @param up_prod: source of data to upload
 * @param down_con: sink for downloaded data
 * @param prog_lis: listener for progress updates
 * @param result_handling: instructions for handling the results; see above.
 * @param error: storage for a GError pointer
 * @returns: TRUE on success, FALSE on failure
 */
static gboolean
perform_request(S3Store *self,
                const char *verb,
                const char *bucket,
                const char *key,
                const char *subresource,
                const char *query,
                ZCloudUploadProducer *up_prod,
                ZCloudDownloadConsumer *down_con,
                ZCloudProgressListener *prog_lis,
                const result_handling_t *result_handling,
                GError **error);

static gboolean
s3_curl_supports_ssl(void);

static gboolean
s3_bucket_location_compat(S3StoreClass *klass, const char *bucket, GError **error);

/*
 * Static function implementations
 */
static size_t
s3_internal_write_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
    S3InternalData *data = (S3InternalData *) stream;
    gsize err_bytes_written, bytes_written, bytes_desired = (gsize) size * nmemb;
    GError *err = NULL;

    g_assert(ZCLOUD_IS_DOWNLOAD_CONSUMER(data->err_con));

    if (!data->headers_done) return bytes_desired;

    /* ignore whatever err_con tells us, so we can keep trying to dl */
    err_bytes_written = zcloud_download_consumer_write(
        ZCLOUD_DOWNLOAD_CONSUMER(data->err_con), ptr, bytes_desired, NULL);
    if (err_bytes_written < bytes_desired) data->err_too_big = TRUE;

    if (data->user_con) {
        bytes_written = zcloud_download_consumer_write(data->user_con, ptr, bytes_desired, &err);
        if (err) {
            bytes_written = 0;
            data->user_con_err = err;
        }
    } else {
        bytes_written = bytes_desired;
    }

    return bytes_written;
}

/*
 * a CURLOPT_READFUNCTION to read data from a ZCloudUploadProducer
 */
static size_t
s3_internal_read_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
    S3InternalData *data = (S3InternalData *) stream;
    gsize bytes_read, bytes_desired = (gsize) size * nmemb;
    GError *err = NULL;

    if (!data->up_prod) return bytes_desired;

    bytes_read = zcloud_upload_producer_read(data->up_prod, ptr, bytes_desired, &err);
    if (err) {
        bytes_read = CURL_READFUNC_ABORT;
        data->up_prod_err = err;
    }

    return bytes_read;
}

/*
 * a CURLOPT_HEADERFUNCTION to save the ETag header only.
 */
static size_t
s3_internal_header_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
    static const char *final_header = "\r\n";
    char *header;
    regmatch_t pmatch[2];
    S3InternalData *data = (S3InternalData *) stream;
    S3StoreClass *klass = S3_STORE_GET_CLASS(data->self);

    header = g_strndup((gchar *) ptr, (gsize) size*nmemb);
    if (!s3_regexec_wrap(&klass->etag_regex, header, 2, pmatch, 0))
        data->etag = find_regex_substring(header, pmatch[1]);
    if (!strcmp(final_header, header))
        data->headers_done = TRUE;

    return size*nmemb;
}

/*
 * a CURLOPT_HEADERFUNCTION to save the ETag header only.
 */
static int
curl_debug_func(CURL *curl G_GNUC_UNUSED,
    curl_infotype type,
    char *s,
    size_t len,
    void *unused G_GNUC_UNUSED)
{
    char *lineprefix;
    char *message;
    char **lines, **line;

    switch (type) {
    case CURLINFO_TEXT:
        lineprefix="";
        break;

    case CURLINFO_HEADER_IN:
        lineprefix="Hdr In: ";
        break;

    case CURLINFO_HEADER_OUT:
        lineprefix="Hdr Out: ";
        break;

    default:
        /* ignore data in/out -- nobody wants to see that in the
         * debug logs! */
        return 0;
    }

    /* split the input into lines */
    message = g_strndup(s, (gsize) len);
    lines = g_strsplit(message, "\n", -1);
    g_free(message);

    for (line = lines; *line; line++) {
        if (**line == '\0') continue; /* skip blank lines */
        g_debug("%s%s", lineprefix, *line);
    }
    g_strfreev(lines);

    return 0;
}

static s3_error_code_t
s3_error_code_from_name(char *s3_error_name)
{
    int i;

    if (!s3_error_name) return S3_ERROR_Unknown;

    /* do a brute-force search through the list, since it's not sorted */
    for (i = 0; i < S3_ERROR_END; i++) {
        if (g_strcasecmp(s3_error_name, s3_error_code_names[i]) == 0)
            return i;
    }

    return S3_ERROR_Unknown;
}

static char *
build_url(const char *bucket,
      const char *key,
      const char *subresource,
      const char *query,
      gboolean use_subdomain,
      gboolean use_ssl)
{
    GString *url = NULL;
    char *esc_bucket = NULL, *esc_key = NULL;

    /* scheme */
    url = g_string_new("http");
    if (use_ssl)
        g_string_append(url, "s");

    g_string_append(url, "://");

    /* domain */
    if (use_subdomain && bucket)
        g_string_append_printf(url, "%s.s3.amazonaws.com/", bucket);
    else
        g_string_append(url, "s3.amazonaws.com/");

    /* path */
    if (!use_subdomain && bucket) {
        esc_bucket = curl_escape(bucket, 0);
    if (!esc_bucket) goto cleanup;
        g_string_append_printf(url, "%s", esc_bucket);
        if (key)
            g_string_append(url, "/");
    }

    if (key) {
        esc_key = curl_escape(key, 0);
    if (!esc_key) goto cleanup;
        g_string_append_printf(url, "%s", esc_key);
    }

    /* query string */
    if (subresource || query)
        g_string_append(url, "?");

    if (subresource)
        g_string_append(url, subresource);

    if (subresource && query)
        g_string_append(url, "&");

    if (query)
        g_string_append(url, query);

cleanup:
    if (esc_bucket) curl_free(esc_bucket);
    if (esc_key) curl_free(esc_key);

    return g_string_free(url, FALSE);
}

static struct curl_slist *
authenticate_request(S3Store *self,
                     const char *verb,
                     const char *bucket,
                     const char *key,
                     const char *subresource,
                     const char *md5_hash,
                     gboolean use_subdomain)
{
    time_t t;
    struct tm tmp;
    char *date = NULL;
    char *buf = NULL;
    HMAC_CTX ctx;
    GByteArray *md = NULL;
    char *auth_base64 = NULL;
    struct curl_slist *headers = NULL;
    char *esc_bucket = NULL, *esc_key = NULL;
    GString *auth_string = NULL;

    /* From RFC 2616 */
    static const char *wkday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};



    /* Build the string to sign, per the S3 spec.
     * See: "Authenticating REST Requests" - API Version 2006-03-01 pg 58
     */

    /* verb */
    auth_string = g_string_new(verb);
    g_string_append(auth_string, "\n");

    /* Content-MD5 header */
    if (md5_hash)
        g_string_append(auth_string, md5_hash);
    g_string_append(auth_string, "\n");

    /* Content-Type is empty*/
    g_string_append(auth_string, "\n");


    /* calculate the date */
    t = time(NULL);
#ifdef _WIN32
    if (!gmtime_s(&tmp, &t)) g_debug("localtime error");
#else
    if (!gmtime_r(&t, &tmp)) perror("localtime");
#endif
    date = g_strdup_printf("%s, %02d %s %04d %02d:%02d:%02d GMT",
        wkday[tmp.tm_wday], tmp.tm_mday, month[tmp.tm_mon], 1900+tmp.tm_year,
        tmp.tm_hour, tmp.tm_min, tmp.tm_sec);

    g_string_append(auth_string, date);
    g_string_append(auth_string, "\n");

    if (self->user_token) {
        g_assert(self->product_token);

        g_string_append(auth_string, AMAZON_SECURITY_HEADER);
        g_string_append(auth_string, ":");
        g_string_append(auth_string, self->user_token);
        g_string_append(auth_string, ",");
        g_string_append(auth_string, self->product_token);
        g_string_append(auth_string, "\n");
    }

    /* CanonicalizedResource */
    g_string_append(auth_string, "/");
    if (bucket) {
        if (use_subdomain)
            g_string_append(auth_string, bucket);
        else {
            esc_bucket = curl_escape(bucket, 0);
            if (!esc_bucket) goto cleanup;
            g_string_append(auth_string, esc_bucket);
        }
    }

    if (bucket && (use_subdomain || key))
        g_string_append(auth_string, "/");

    if (key) {
            esc_key = curl_escape(key, 0);
            if (!esc_key) goto cleanup;
            g_string_append(auth_string, esc_key);
    }

    if (subresource) {
        g_string_append(auth_string, "?");
        g_string_append(auth_string, subresource);
    }

    /* run HMAC-SHA1 on the canonicalized string */
    md = g_byte_array_sized_new(EVP_MAX_MD_SIZE+1);
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, self->secret_key, (int) strlen(self->secret_key), EVP_sha1(), NULL);
    HMAC_Update(&ctx, (unsigned char*) auth_string->str, auth_string->len);
    HMAC_Final(&ctx, md->data, &md->len);
    HMAC_CTX_cleanup(&ctx);
    auth_base64 = s3_base64_encode(md);

    /* append the new headers */
    if (self->user_token) {
        /* Devpay headers are included in hash. */
        buf = g_strdup_printf(AMAZON_SECURITY_HEADER ": %s", self->user_token);
        headers = curl_slist_append(headers, buf);
        g_free(buf);

        buf = g_strdup_printf(AMAZON_SECURITY_HEADER ": %s", self->product_token);
        headers = curl_slist_append(headers, buf);
        g_free(buf);
    }

    buf = g_strdup_printf("Authorization: AWS %s:%s",
                          self->access_key, auth_base64);
    headers = curl_slist_append(headers, buf);
    g_free(buf);

    if (md5_hash && '\0' != md5_hash[0]) {
        buf = g_strdup_printf("Content-MD5: %s", md5_hash);
        headers = curl_slist_append(headers, buf);
        g_free(buf);
    }

    buf = g_strdup_printf("Date: %s", date);
    headers = curl_slist_append(headers, buf);
    g_free(buf);
cleanup:
    g_free(date);
    g_free(esc_bucket);
    g_free(esc_key);
    g_byte_array_free(md, TRUE);
    g_free(auth_base64);
    g_string_free(auth_string, TRUE);

    return headers;
}

static void
interpret_response(S3Store *self,
                   CURLcode curl_code,
                   char *curl_error_buffer,
                   S3InternalData *int_data,
                   const char *content_md5,
                   const result_handling_t *result_handling,
                   GError **err)
{
    S3StoreClass *klass = S3_STORE_GET_CLASS(self);
    long response_code = 0;
    regmatch_t pmatch[2];
    char *curl_message, *error_name = NULL, *message = NULL, *s3_message = NULL;
    gchar *body;
    s3_error_code_t s3_error_code = S3_ERROR_None;
    ZCloudError result;

    g_assert(S3_IS_STORE(self));

    /* bail out from a CURL error */
    if (curl_code != CURLE_OK) {
        curl_message = g_strdup_printf("CURL error: (code %d) %s", (int) curl_code,
                curl_error_buffer);
        if (CURLE_ABORTED_BY_CALLBACK == curl_code && int_data->user_con_err) {
            message = g_strdup_printf("%s; upload producer's read returned: "
                "(domain %s) (code %u) %s", curl_message,
                g_quark_to_string(int_data->user_con_err->domain),
                int_data->user_con_err->code, int_data->user_con_err->message);
            g_free(curl_message);
        } else if (CURLE_WRITE_ERROR == curl_code && int_data->up_prod_err) {
            message = g_strdup_printf("%s; download consumer's write returned: "
                "(domain %s) (code %u) %s", curl_message,
                g_quark_to_string(int_data->up_prod_err->domain),
                int_data->up_prod_err->code, int_data->up_prod_err->message);
            g_free(curl_message);
        } else {
            message = curl_message;
        }
        goto lookup_handling;
    }

    /* CURL seems to think things were OK, so get its response code */
    curl_easy_getinfo(self->curl, CURLINFO_RESPONSE_CODE, &response_code);

    /* check ETag == MD5 if both are present */
    if (int_data->etag && content_md5 && 200 == response_code) {
        if (g_strcasecmp(int_data->etag, content_md5)) {
            /* on mismatch, always mark retry*/
            g_set_error(err, ZCLOUD_ERROR, ZCERR_EPHEMERAL,
                "S3 Error: Possible data corruption. ETag returned by Amazon "
                "(%s) did not match the MD5 hash of the data sent (%s)",
                int_data->etag, content_md5);
            goto cleanup;
        }
    }

    /* Now look at the body to try to get the actual Amazon error message. Rather
     * than parse out the XML, just use some regexes. */

    if (int_data->err_too_big) {
        s3_error_code = S3_ERROR_Unknown;
        message = g_strdup_printf("S3 Error: (HTTP response %ld) Unknown "
            "(response body too large to parse)", response_code);
        goto lookup_handling;
    }

    body = zcloud_growing_memory_download_consumer_get_contents_as_string(int_data->err_con);
    if (!body) {
        s3_error_code = S3_ERROR_Unknown;
        message = g_strdup_printf("S3 Error: (HTTP response %ld) Unknown "
            "(empty response body)", response_code);
        goto lookup_handling;
    }

    if (!s3_regexec_wrap(&klass->error_name_regex, body, 2, pmatch, 0))
        error_name = find_regex_substring(body, pmatch[1]);

    if (!s3_regexec_wrap(&klass->message_regex, body, 2, pmatch, 0))
        s3_message = find_regex_substring(body, pmatch[1]);

    if (error_name) {
        s3_error_code = s3_error_code_from_name(error_name);
    }

    message = g_strdup_printf("S3 Error: (HTTP response %ld) (name %s) %s",
        response_code, error_name, s3_message);

lookup_handling:
    while (result_handling->response_code
        || result_handling->s3_error_code
        || result_handling->curl_code) {
        if ((result_handling->response_code && result_handling->response_code != response_code)
         || (result_handling->s3_error_code && result_handling->s3_error_code != s3_error_code)
         || (result_handling->curl_code && result_handling->curl_code != curl_code)) {
            result_handling++;
            continue;
        }

        break;
    }
    result = result_handling->result;

    if (ZCERR_EPHEMERAL == result ||
        ZCERR_LASTING == result ||
        ZCERR_MISSING == result) {
        g_set_error(err, ZCLOUD_ERROR, result, "%s", message);
    } else if (ZCERR_NONE == result) {
        g_clear_error(err);
    } else {
        g_assert_not_reached();
    }

cleanup:
    g_free(body);
    g_free(message);
    g_free(error_name);
    g_free(s3_message);
}

static gboolean
perform_request(S3Store *self,
                const char *verb,
                const char *bucket,
                const char *key,
                const char *subresource,
                const char *query,
                ZCloudUploadProducer *up_prod,
                ZCloudDownloadConsumer *down_con,
                ZCloudProgressListener *prog_lis,
                const result_handling_t *result_handling,
                GError **error)
{
    gboolean use_subdomain = (NULL != self->bucket_location);
    char *url = NULL;
    gboolean ret = FALSE;
    CURLcode curl_code = CURLE_OK;
    char curl_error_buffer[CURL_ERROR_SIZE] = "";
    struct curl_slist *headers = NULL;
    S3InternalData int_data = {self, up_prod, zcloud_growing_memory_download_consumer(MAX_ERROR_RESPONSE_LEN), down_con, FALSE, NULL, NULL, FALSE, NULL};
    guint retries = 0;
    gulong backoff = EXPONENTIAL_BACKOFF_START_USEC;
    /* corresponds to PUT, HEAD, GET, and POST */
    int curlopt_upload = 0, curlopt_nobody = 0, curlopt_httpget = 0, curlopt_post = 0;
    /* do we want to examine the headers */
    const char *curlopt_customrequest = NULL;
    /* for MD5 calculation */
    GByteArray *md5_hash = NULL;
    gchar *md5_hash_hex = NULL, *md5_hash_b64 = NULL;
    gsize request_body_size;

    /* these are used to detect errors in functions we call and report them */
    GError *tmp_err = NULL;
    char *tmp_err_msg = NULL;

    g_assert(S3_IS_STORE(self));
    g_assert(!up_prod || ZCLOUD_UPLOAD_PRODUCER(up_prod));
    g_assert(!down_con || ZCLOUD_DOWNLOAD_CONSUMER(down_con));
    g_assert(!prog_lis || ZCLOUD_PROGRESS_LISTENER(prog_lis));
    g_assert(self->curl != NULL);

    url = build_url(bucket, key, subresource, query, use_subdomain, self->use_ssl);
    g_assert(url);

    /*
     * libcurl may behave strangely if these are not set correctly
     * also do some simple paramater checking
     */
    if (!strncmp(verb, "PUT", 4)) {
        curlopt_upload = 1;
        g_assert(up_prod);
        g_assert(!down_con);
    } else if (!strncmp(verb, "GET", 4)) {
        curlopt_httpget = 1;
        g_assert(!up_prod);
    } else if (!strncmp(verb, "POST", 5)) {
        curlopt_post = 1;
    } else if (!strncmp(verb, "HEAD", 5)) {
        curlopt_nobody = 1;
        g_assert(!up_prod);
        g_assert(!down_con);
    } else {
        curlopt_customrequest = verb;
    }

    if (up_prod) {
        request_body_size = zcloud_upload_producer_get_size(up_prod, &tmp_err);
        if (tmp_err) {
            tmp_err_msg = "An error occurred  while calculating the MD5 hash of the upload";
            goto tmp_error;
        }
        md5_hash = zcloud_upload_producer_calculate_md5(up_prod, &tmp_err);
        if (tmp_err) {
            tmp_err_msg = "An error occurred  while calculating the MD5 hash of the upload";
            goto tmp_error;
        }
        if (md5_hash) {
            md5_hash_b64 = s3_base64_encode(md5_hash);
            md5_hash_hex = s3_hex_encode(md5_hash);
            g_byte_array_free(md5_hash, TRUE);
        }
    } else {
        request_body_size = 0;
        md5_hash_b64 = NULL;
        md5_hash_hex = NULL;
    }

    int_data.user_con = down_con;

    while (1) {
        /* reset things */
        if (headers) {
            curl_slist_free_all(headers);
        }
        curl_error_buffer[0] = '\0';
        if (up_prod) {
            if (!zcloud_upload_producer_reset(up_prod, &tmp_err)) {
                tmp_err_msg = "while reseting the upload producer";
                goto tmp_error;
            }
        }
        /* reset the write stuffs */
        if (!zcloud_download_consumer_reset(ZCLOUD_DOWNLOAD_CONSUMER(int_data.err_con), &tmp_err)) {
            tmp_err_msg = "while reseting the internal download consumer";
            goto tmp_error;
        }
        if (int_data.user_con) {
            if (!zcloud_download_consumer_reset(ZCLOUD_DOWNLOAD_CONSUMER(int_data.user_con), &tmp_err)) {
                tmp_err_msg = "while reseting the download consumer";
                goto tmp_error;
            }
        }
        int_data.err_too_big = FALSE;
        g_clear_error(&int_data.user_con_err);
        g_clear_error(&int_data.up_prod_err);
        int_data.headers_done = FALSE;
        int_data.etag = NULL;

        /* set up the request */
        headers = authenticate_request(self, verb, bucket, key, subresource,
            md5_hash_b64, use_subdomain);

        if (self->use_ssl && self->ca_info) {
            if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_CAINFO, self->ca_info)))
                goto curl_error;
        }

        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_VERBOSE, self->verbose)))
            goto curl_error;
        if (self->verbose) {
            if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_DEBUGFUNCTION,
                              curl_debug_func)))
                goto curl_error;
        }
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_ERRORBUFFER,
                                          curl_error_buffer)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_NOPROGRESS, 1)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_FOLLOWLOCATION, 1)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_URL, url)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_HTTPHEADER,
                                          headers)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_WRITEFUNCTION, s3_internal_write_func)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_WRITEDATA, &int_data)))
            goto curl_error;
        /* Note: we always have to set this apparently, for consistent "end of header" detection */
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_HEADERFUNCTION, s3_internal_header_func)))
            goto curl_error;
        /* Note: if set, CURLOPT_HEADERDATA seems to also be used for CURLOPT_WRITEDATA ? */
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_HEADERDATA, &int_data)))
            goto curl_error;
        /* XXX: enable progress
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_PROGRESSFUNCTION, progress_func)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_PROGRESSDATA, progress_data)))
            goto curl_error;
        */

#ifdef CURLOPT_INFILESIZE_LARGE
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)request_body_size)))
            goto curl_error;
#else
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_INFILESIZE, (long)request_body_size)))
            goto curl_error;
#endif

        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_HTTPGET, curlopt_httpget)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_UPLOAD, curlopt_upload)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_POST, curlopt_post)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_NOBODY, curlopt_nobody)))
            goto curl_error;
        if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_CUSTOMREQUEST,
                                          curlopt_customrequest)))
            goto curl_error;

        if (curlopt_upload) {
            if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_READFUNCTION, s3_internal_read_func)))
                goto curl_error;
            if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_READDATA, &int_data)))
                goto curl_error;
        } else {
            /* Clear request_body options. */
            if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_READFUNCTION,
                                              NULL)))
                goto curl_error;
            if ((curl_code = curl_easy_setopt(self->curl, CURLOPT_READDATA,
                                              NULL)))
                goto curl_error;
        }
        /* Perform the request */
        curl_code = curl_easy_perform(self->curl);


        /* interpret the response into self->last* */
    curl_error: /* (label for short-circuiting the curl_easy_perform call) */
        interpret_response(self, curl_code, curl_error_buffer,
            &int_data, md5_hash_hex, result_handling, &tmp_err);

        if (tmp_err) {
            if (ZCLOUD_ERROR == tmp_err->domain &&
                ZCERR_EPHEMERAL == tmp_err->code &&
                retries < EXPONENTIAL_BACKOFF_MAX_RETRIES) {
                /* keep trying ... */
                g_clear_error(&tmp_err);
            } else {
                ret = FALSE;
                g_propagate_error(error, tmp_err);
                tmp_err = NULL;
                break;
            }
        } else {
            /* everything okay, we're done! */
            ret = TRUE;
            break;
        }

        g_usleep(backoff);
        retries++;
        backoff *= EXPONENTIAL_BACKOFF_BASE;
    }

tmp_error:
    if (tmp_err) {
        ret = FALSE;
        g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING,
            "%s: (domain %s) (code %u) %s", tmp_err_msg,
            g_quark_to_string(tmp_err->domain), tmp_err->code, tmp_err->message);
        g_clear_error(&tmp_err);
    }

    /* cleanup */
    g_free(url);
    if (headers) curl_slist_free_all(headers);
    g_free(md5_hash_b64);
    g_free(md5_hash_hex);
    g_object_unref(int_data.err_con);
    g_clear_error(&int_data.user_con_err);
    g_clear_error(&int_data.up_prod_err);

    return ret;
}

static gboolean
s3_curl_supports_ssl(void)
{
    static int supported = -1;
    if (supported == -1) {
#if defined(CURL_VERSION_SSL)
    curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);
    if (info->features & CURL_VERSION_SSL)
        supported = 1;
    else
        supported = 0;
#else
    supported = 0;
#endif
    }

    return supported;
}

static gboolean
s3_bucket_location_compat(S3StoreClass *klass, const char *bucket, GError **error)
{
    gboolean ret = !s3_regexec_wrap(&klass->subdomain_regex, bucket, 0, NULL, 0);
    if (!ret) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
            "bucket location set, but bucket name (%s) is not usable as an S3 "
            "subdomain", bucket);
    }
    return ret;
}

/*
 * Method implementations
 */

static gboolean
setup_impl(
    ZCloudStore *zself,
    const gchar *suffix,
    gint n_parameters,
    GParameter *parameters,
    GError **error)
{
    S3Store *self = S3_STORE(zself);
    S3StoreClass *klass = S3_STORE_GET_CLASS(self);
    gint i, j;
    struct {const char *name; char **str;} str_props[] =
        {{"access-key", &self->access_key},
         {"secret-key", &self->secret_key},
         {"user-token", &self->user_token},
         {"product-token", &self->product_token},
         {"ca-info", &self->ca_info},
         {"bucket-location", &self->bucket_location},
         {NULL, NULL},
        };
    struct {const char *name; gboolean *bool;} bool_props[] =
        {{"use-ssl", &self->use_ssl},
         {"verbose", &self->verbose},
         {NULL, NULL},
        };

    if (suffix && suffix[0]) self->bucket = g_strdup(suffix);

    for (i = 0; i < n_parameters; i++) {
        const char *name = parameters[i].name;
        GValue *val = &parameters[i].value;

        for (j = 0; str_props[j].name; j++) {
            if (0 == strcmp(name, str_props[j].name)) {
                g_assert(G_VALUE_HOLDS_STRING(val));
                char *str_val = g_value_dup_string(val);
                /* don't bother with empty strings */
                if (str_val && str_val[0])
                    *str_props[j].str = str_val;
                else
                    g_free(str_val);

                goto got_match;
            }
        }

        for (j = 0; bool_props[j].name; j++) {
            if (0 == strcmp(name, bool_props[j].name)) {
                g_assert(G_VALUE_HOLDS_BOOLEAN(val));
                *bool_props[j].bool = g_value_get_boolean(val);
                goto got_match;
            }
        }

        /* we shouldn't get here.
         * zcloud_store_newv validates the paramater names.
         */
        g_error("unknown parameter '%s'", name);

    /* semi-colon to avoid warning that label is at end of block*/
    got_match: /* nothing */ ;
    }

    if (!self->access_key) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                    "must set a non-empty access key");
        return FALSE;
    }
    if (!self->secret_key) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                    "must set a non-empty secret key");
        return FALSE;
    }
    if (self->user_token) {
        if (!self->product_token || !self->product_token[0]) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                        "must set a non-empty product token");
            return FALSE;
        }
    }

    /* XXX: check SSL and bucket location compat...*/
    if (self->use_ssl) {
        if (!s3_curl_supports_ssl()) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                "SSL support requested, but CURL does not support it");
            return FALSE;
        }
        if (self->bucket_location) {
            curl_version_info_data *info;
            info = curl_version_info(CURLVERSION_NOW);
            if (info->version_num <= 0x070a02) {
                g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
                    "SSL support requested and bucket location set, "
                    "but CURL isn't new enough");
                return FALSE;
            }
        }
    }

    /* is the bucket name compatible with locations (subdomain)? */
    if (self->bucket && self->bucket_location &&
        /* note s3_bucket_location_compat will set error for us */
        !s3_bucket_location_compat(klass, self->bucket, error)) {
        return FALSE;
    }

    self->curl = curl_easy_init();
    if (!self->curl) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_UNKNOWN ,
                "an error occurred while creating the CURL handle");
            return FALSE;
    }

    return TRUE;
}

static gboolean
create_bucket(
    S3Store *self,
    const gchar *bucket,
    const gchar *bucket_location,
    ZCloudProgressListener *progress,
    GError **error)
{
    static result_handling_t result_handling[] = {
        { 200,  0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,    0, 0, /* default: */ ZCERR_LASTING },
        };
    S3StoreClass *klass = S3_STORE_GET_CLASS(self);
    gchar *loc_spec = NULL;
    ZCloudUploadProducer *up_prod = NULL;
    ZCloudGrowingMemoryDownloadConsumer *down_con = NULL;
    gboolean ret, perf_res;
    GError *put_err = NULL;
    regmatch_t pmatch[4];
    char *body = NULL, *loc_end_open, *loc_content;

    static const char *wildcard_conf_err = "S3 Error: A wildcard location "
        "constraint is configured, but the bucket has a non-empty location "
        "constraint";

    g_assert(S3_IS_STORE_CLASS(klass));
    g_assert(bucket);

    if (bucket_location &&
        0 != strcmp(AMAZON_WILDCARD_LOCATION, bucket_location)) {
        /* note s3_bucket_location_compat will set error for us */
        if (s3_bucket_location_compat(klass, bucket, error)) {
            loc_spec = g_strdup_printf(AMAZON_BUCKET_CONF_TEMPLATE, bucket_location);
        } else {
            ret = FALSE;
            g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING, "S3 Error: location "
                "constraint is configured, but the bucket name (%s) is "
                "not usable as an S3 subdomain", bucket);
            goto cleanup;
        }
    } else {
        /* so we can both call strlen and g_free */
        loc_spec = g_strdup("");
    }

    up_prod = ZCLOUD_UPLOAD_PRODUCER(zcloud_memory_upload_producer(loc_spec, strlen(loc_spec)));

    ret = perform_request(self /* self */, "PUT" /*verb*/,
        bucket /* bucket */, NULL /* key */, NULL /* subresource */,
        NULL /* query */, up_prod /* up_prod */, NULL /* down_con */,
        progress /* prog_lis */, result_handling /*result_handling*/,
        &put_err /* error */);

    /* if everything went okay, there's no need to investigate */
    if (ret) goto cleanup;

    /* see if the error is due to the bucket already existing... */
    down_con = zcloud_growing_memory_download_consumer(MAX_ERROR_RESPONSE_LEN);
    perf_res = perform_request(self /* self */, "GET" /*verb*/,
        bucket /* bucket */, NULL /* key */, "location" /* subresource */,
        NULL /* query */, NULL /* up_prod */,
        ZCLOUD_DOWNLOAD_CONSUMER(down_con) /* down_con */,
        progress /* prog_lis */, result_handling /*result_handling*/,
        NULL /* error */);

    if (!perf_res) {
        /* propogate the original error */
        if (put_err) g_propagate_error(error, put_err);
        goto cleanup;
    }

    /* don't bother checking the location if the user doesn't care about it */
    if (!bucket_location) {
        goto loc_okay;
    }

    body = zcloud_growing_memory_download_consumer_get_contents_as_string(down_con);

    if (!body) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING, "S3 Error: No body "
            "received for location request");
        goto cleanup;
    }

    if (s3_regexec_wrap(&klass->location_con_regex, body, 4, pmatch, 0)) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING, "S3 Error: Unexpected "
            "location response from Amazon S3");
        goto cleanup;
    }
    loc_end_open = find_regex_substring(body, pmatch[1]);
    loc_content = find_regex_substring(body, pmatch[3]);

    /* check for self-closing XML tag */
    if ('/' != loc_end_open[0]) {
        /* if wildcard */
        if (!strcmp(AMAZON_WILDCARD_LOCATION, bucket_location)) {
            g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING, "%s", wildcard_conf_err);
                goto cleanup;
        }
    } else { /* not self-closing ...*/

        /* if not a wildcard */
        if (strcmp(AMAZON_WILDCARD_LOCATION, bucket_location)) {

            /* loc_content doesn't match bucket_location */
            if (strcmp(loc_content, bucket_location)) {
                g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING, "S3 Error: "
                    "The location constraint configured does not match the "
                    "constraint currently on the bucket");
                goto cleanup;
            }
        } else { /* if wildcard ... */

            if ('\0' != loc_content[0]) {
                g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING, "%s", wildcard_conf_err);
                goto cleanup;
            }
        }
    }

loc_okay:
    /* everything looks okay */
    ret = TRUE;

cleanup:
    g_object_unref(up_prod);
    if (down_con) g_object_unref(down_con);
    g_free(body);
    g_free(loc_spec);

    return ret;
}

static gboolean
create_impl(
    ZCloudStore *zself,
    const gchar *key,
    ZCloudProgressListener *progress,
    GError **error)
{
    static result_handling_t result_handling[] = {
        { 200,  0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,    0, 0, /* default: */ ZCERR_LASTING },
        };
    S3Store *self = S3_STORE(zself);
    ZCloudUploadProducer *up_prod = NULL;
    gboolean perf_res;

    if (self->bucket) {
        /* create an object (upload empty body) */
        up_prod = ZCLOUD_UPLOAD_PRODUCER(zcloud_memory_upload_producer("", 0));

        perf_res = perform_request(self /* self */, "PUT" /*verb*/,
            self->bucket /* bucket */, key /* key */, NULL /* subresource */,
            NULL /* query */, up_prod /* up_prod */, NULL /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            error /* error */);

        g_object_unref(up_prod);

        return perf_res;
    } else {
        return create_bucket(self, key, self->bucket_location, progress, error);
    }
}
static gboolean
upload_impl(
    ZCloudStore *zself,
    const gchar *key,
    ZCloudUploadProducer *upload,
    ZCloudProgressListener *progress,
    GError **error)
{
    static result_handling_t result_handling[] = {
        { 200,  0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,    0, 0, /* default: */ ZCERR_LASTING },
        };
    S3Store *self = S3_STORE(zself);

    if (self->bucket) {
        return perform_request(self /* self */, "PUT" /*verb*/,
            self->bucket /* bucket */, key /* key */, NULL /* subresource */,
            NULL /* query */, upload /* up_prod */, NULL /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            error /* error */);
    } else {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER, "S3 Error: "
            "Can not download a bucket");
        return FALSE;
    }
}

static gboolean
download_impl(
    ZCloudStore *zself,
    const gchar *key,
    ZCloudDownloadConsumer *download,
    ZCloudProgressListener *progress,
    GError **error)
{
    static result_handling_t result_handling[] = {
        { 200,  0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,    0, 0, /* default: */ ZCERR_LASTING },
        };
    S3Store *self = S3_STORE(zself);

    if (self->bucket) {
        return perform_request(self /* self */, "GET" /*verb*/,
            self->bucket /* bucket */, key /* key */, NULL /* subresource */,
            NULL /* query */, NULL /* up_prod */, download /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            error /* error */);
    } else {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER, "S3 Error: "
            "Can not download a bucket");
        return FALSE;
    }
}

static gboolean
exists_impl(
    ZCloudStore *zself,
    const gchar *key,
    ZCloudProgressListener *progress,
    GError **error)
{
    static result_handling_t result_handling[] = {
        { 200,  0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,    0, 0, /* default: */ ZCERR_LASTING },
        };
    S3Store *self = S3_STORE(zself);
    GError *tmp_err = NULL;
    gboolean perf_res;

    if (self->bucket) {
        perf_res = perform_request(self /* self */, "HEAD" /*verb*/,
            self->bucket /* bucket */, key /* key */, NULL /* subresource */,
            NULL /* query */, NULL /* up_prod */, NULL /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            error /* error */);
    } else {
        /* can't use HEAD on a bucket, so GET the location instead */
        perf_res = perform_request(self /* self */, "GET" /*verb*/,
            key /* bucket */, NULL /* key */, "location" /* subresource */,
            NULL /* query */, NULL /* up_prod */, NULL /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            error /* error */);
    }

    return perf_res;
}

static gboolean
delete_impl(
    ZCloudStore *zself,
    const gchar *key,
    ZCloudProgressListener *progress,
    GError **error)
{
    static result_handling_t result_handling[] = {
        { 200,  0, 0, ZCERR_NONE },
        { 204,  0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,    0, 0, /* default: */ ZCERR_LASTING },
        };
    S3Store *self = S3_STORE(zself);
    GError *tmp_err = NULL;
    gboolean perf_res;

    if (self->bucket) {
        perf_res = perform_request(self /* self */, "DELETE" /*verb*/,
            self->bucket /* bucket */, key /* key */, NULL /* subresource */,
            NULL /* query */, NULL /* up_prod */, NULL /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            &tmp_err /* error */);
    } else {
        perf_res = perform_request(self /* self */, "DELETE" /*verb*/,
            key /* bucket */, NULL /* key */, NULL /* subresource */,
            NULL /* query */, NULL /* up_prod */, NULL /* down_con */,
            progress /* prog_lis */, result_handling /*result_handling*/,
            &tmp_err /* error */);
    }
    if (tmp_err) {
        if (ZCLOUD_ERROR == tmp_err->domain && ZCERR_MISSING == tmp_err->code) {
            g_clear_error(&tmp_err);
            perf_res = TRUE; /* repeated delete should be fine */
        } else {
            g_propagate_error(error, tmp_err);
        }
    }
    return perf_res;
}

/*
 * Class mechanics
 */

void
instance_init(
    S3Store *self)
{
    /* initialize all members */
    self->access_key = NULL;
    self->secret_key = NULL;
    self->user_token = NULL;
    self->product_token = NULL;
    self->bucket_location = NULL;
    self->ca_info = NULL;
    self->use_ssl = TRUE;
    self->verbose = TRUE;
    self->curl = NULL;
    self->bucket = NULL;
}

void
finalize_impl(
    GObject *obj)
{
    S3Store *self = S3_STORE(obj);

    /* free all strings */
    g_free(self->access_key);
    g_free(self->secret_key);
    g_free(self->user_token);
    g_free(self->product_token);
    g_free(self->bucket_location);
    g_free(self->ca_info);
    g_free(self->bucket);

    /* release curl */
    if (self->curl) curl_easy_cleanup(self->curl);
}/* Private structure for our "thunk", which tracks where the user is in the list
 * of keys. */
struct list_keys_thunk {
    gchar *prefix;
    regex_t pattern;
    ZCloudListConsumer *consumer;

    gboolean in_contents; /* look for "key" entities in here */

    gboolean is_truncated;
    gchar *next_marker;

    gboolean want_text;

    gchar *text;
    gsize text_len;
};

/* Functions for a SAX parser to parse the XML from Amazon */

static void
list_start_element(GMarkupParseContext *context G_GNUC_UNUSED,
                   const gchar *element_name,
                   const gchar **attribute_names G_GNUC_UNUSED,
                   const gchar **attribute_values G_GNUC_UNUSED,
                   gpointer user_data,
                   GError **error G_GNUC_UNUSED)
{
    struct list_keys_thunk *thunk = (struct list_keys_thunk *)user_data;

    thunk->want_text = 0;
    if (g_strcasecmp(element_name, "contents") == 0) {
        thunk->in_contents = 1;
    } else if (g_strcasecmp(element_name, "key") == 0 && thunk->in_contents) {
        thunk->want_text = 1;
    } else if (g_strcasecmp(element_name, "istruncated")) {
        thunk->want_text = 1;
    } else if (g_strcasecmp(element_name, "nextmarker")) {
        thunk->want_text = 1;
    }
}

static void
list_end_element(GMarkupParseContext *context G_GNUC_UNUSED,
                 const gchar *element_name,
                 gpointer user_data,
                 GError **error G_GNUC_UNUSED)
{
    struct list_keys_thunk *thunk = (struct list_keys_thunk *)user_data;
    gchar *full_key = NULL;

    if (g_strcasecmp(element_name, "contents") == 0) {
        thunk->in_contents = 0;
    } else if (g_strcasecmp(element_name, "key") == 0 && thunk->in_contents) {
        if (thunk->prefix)
            full_key = g_strconcat(thunk->prefix, thunk->text, NULL);
        else /* strconcat would stop early */
            full_key = g_strdup(thunk->text);

        if (!s3_regexec_wrap(&thunk->pattern, full_key, 0, NULL, 0))
            zcloud_list_consumer_got_result(thunk->consumer, full_key);

        g_free(thunk->text);
        thunk->text = NULL;
    } else if (g_strcasecmp(element_name, "istruncated") == 0) {
        if (thunk->text && g_strncasecmp(thunk->text, "false", 5) != 0)
            thunk->is_truncated = TRUE;
    } else if (g_strcasecmp(element_name, "nextmarker") == 0) {
        if (thunk->next_marker) g_free(thunk->next_marker);
        thunk->next_marker = thunk->text;
        thunk->text = NULL;
    }
    g_free(full_key);
}

static void
list_text(GMarkupParseContext *context G_GNUC_UNUSED,
          const gchar *text,
          gsize text_len,
          gpointer user_data,
          GError **error G_GNUC_UNUSED)
{
    struct list_keys_thunk *thunk = (struct list_keys_thunk *)user_data;

    if (thunk->want_text) {
        if (thunk->text) g_free(thunk->text);
        thunk->text = g_strndup(text, text_len);
    }
}

/* Perform a fetch from S3; several fetches may be involved in a
 * single listing operation. this is really just a query string builder */
static gboolean
list_fetch(S3Store *self,
           const char *bucket,
           const char *prefix,
           const char *marker,
           const char *max_keys,
           ZCloudGrowingMemoryDownloadConsumer *down_con,
           ZCloudProgressListener *prog_lis,
           GError **error)
{
    static result_handling_t result_handling[] = {
        { 200, 0, 0, ZCERR_NONE },
        RESULT_HANDLING_ALWAYS_MISSING,
        RESULT_HANDLING_ALWAYS_RETRY,
        { 0,   0, 0, /* default: */ ZCERR_LASTING  }
        };
    const char* pos_parts[][2] = {
        {"prefix", prefix},
        {"marker", marker},
        {"max-keys", max_keys},
        {NULL, NULL}
        };
    gboolean perf_res;
    char *esc_value;
    GString *query;
    guint i;
    gboolean have_prev_part = FALSE;

    /* loop over possible parts to build query string */
    query = g_string_new("");
    for (i = 0; pos_parts[i][0]; i++) {
        if (pos_parts[i][1]) {
            if (have_prev_part)
                g_string_append(query, "&");
            else
                have_prev_part = TRUE;
            esc_value = curl_escape(pos_parts[i][1], 0);
            g_string_append_printf(query, "%s=%s", pos_parts[i][0], esc_value);
            curl_free(esc_value);
        }
    }

    /* and perform the request on that URI */
    perf_res = perform_request(self /* self */, "GET" /*verb*/,
            bucket /* bucket */, NULL /* key */, NULL /* subresource */,
            query->str /* query */, NULL /* up_prod */,
            ZCLOUD_DOWNLOAD_CONSUMER(down_con) /* down_con */,
            prog_lis /* prog_lis */, result_handling /*result_handling*/,
            error /* error */);

    if (query) g_string_free(query, TRUE);

    return perf_res;
}

static gboolean
list_impl(
    ZCloudStore *zself,
    const gchar *template,
    ZCloudListConsumer *list_con,
    ZCloudProgressListener *prog_lis,
    GError **error)
{
    /*
     * max len of XML variables:
     * bucket: 255 bytes (p12 API Version 2006-03-01)
     * key: 1024 bytes (p15 API Version 2006-03-01)
     * size per key: 5GB bytes (p6 API Version 2006-03-01)
     * size of size 10 bytes (i.e. 10 decimal digits)
     * etag: 44 (observed+assumed)
     * owner ID: 64 (observed+assumed)
     * owner DisplayName: 255 (assumed)
     * StorageClass: const (p18 API Version 2006-03-01)
     */
    static const guint MAX_RESPONSE_LEN = 1000*2000;
    static const char *MAX_KEYS = "1000";
#ifdef HAVE_REGEX_H
    static const char *to_escape = "[].*^$\\";
#elif GLIB_CHECK_VERSION(2,14,0)
    static const char *to_escape = "[](){}|+?.*^$\\";
#else
#error must have POSIX or GLib/PCRE regular expressions
#endif
    S3Store *self = S3_STORE(zself);
    gboolean ret = FALSE;
    struct list_keys_thunk thunk;
    GMarkupParseContext *ctxt = NULL;
    static GMarkupParser parser = { list_start_element, list_end_element, list_text, NULL, NULL };
    GError *tmp_err = NULL;
    gchar *tmp_err_when = NULL;
    ZCloudGrowingMemoryDownloadConsumer *down_con = zcloud_growing_memory_download_consumer(MAX_RESPONSE_LEN);
    guint8 *body_raw;
    gsize body_len;
    gchar *reg_pat = g_malloc(strlen(template)*2+3);
    gsize i, j, k;

    if (!self->bucket) {
        g_set_error(error, ZCLOUD_ERROR, ZCERR_PARAMETER,
            "Listing all buckets is not currently supported");
        return FALSE;
    }

    g_assert(ZCLOUD_IS_LIST_CONSUMER(list_con));
    thunk.prefix = NULL;
    thunk.consumer = list_con;
    thunk.text = NULL;
    thunk.next_marker = NULL;

    /* TODO: figure out appropriate prefix for more effecient listing */
    reg_pat[0] = '^';
    k = 1;
    for (i = 0; template[i]; i++) {
        for (j = 0; to_escape[j]; j++) {
            if (to_escape[j] == template[i]) {
                reg_pat[k] = '\\';
                k++;
                reg_pat[k] = template[i];
                k++;
                goto next_chr;
            }
        }
        if ('%' == template[i]) {
            if ('%' == template[i+1]) {
                reg_pat[k] = '%';
                k++;
            } else if ('s' == template[i+1]) {
                reg_pat[k] = '.';
                k++;
                reg_pat[k] = '*';
                k++;
            } else {
                if ('\0' == template[i+1]) {
                    tmp_err = g_error_new(ZCLOUD_ERROR, ZCERR_PARAMETER,
                        "Invalid key template: '%%' can not appear unescaped "
                        "at the end of the string");
                } else {
                    tmp_err = g_error_new(ZCLOUD_ERROR, ZCERR_PARAMETER,
                        "Invalid key template, at \"%%%c\"", template[i+1]);
                }
                goto tmp_error;
            }
            i++;
        } else {
            reg_pat[k] = template[i];
            k++;
        }
    next_chr:
        continue;
    }
    reg_pat[k] = '$';
    k++;
    reg_pat[k] = '\0';

#ifdef HAVE_REGEX_H
    {
        int reg_result;
        char regmessage[1024];
        reg_result = regcomp(&thunk.pattern, reg_pat, REG_NOSUB);
        if (0 != reg_result) {
            regerror(reg_result, &thunk.pattern, regmessage, sizeof(regmessage));
            g_error("Regex error: %s", regmessage);
        }
    }
#else
    thunk.pattern = g_regex_new(reg_pat, G_REGEX_DOTALL | G_REGEX_DOLLAR_ENDONLY, 0, &tmp_err);
    if (tmp_err) g_error("Regex error: %s", tmp_err->message);
#endif

    /* Loop until S3 has given us the entire picture */
    do {
        if (!zcloud_download_consumer_reset(ZCLOUD_DOWNLOAD_CONSUMER(down_con), &tmp_err)) {
            tmp_err_when = "reseting the internal list download consumer";
            goto tmp_error;
        }
        /* get some data from S3 */
        ret = list_fetch(self, self->bucket, NULL, thunk.next_marker, MAX_KEYS, down_con, prog_lis, &tmp_err);
        if (!ret) goto tmp_error;

        /* run the parser over it */
        body_raw = zcloud_growing_memory_download_consumer_get_contents(down_con, &body_len);
        thunk.in_contents = FALSE;
        thunk.is_truncated = FALSE;
        thunk.want_text = FALSE;

        ctxt = g_markup_parse_context_new(&parser, 0, (gpointer)&thunk, NULL);

        if (!g_markup_parse_context_parse(ctxt, (gchar *) body_raw, body_len, &tmp_err)) {
            tmp_err_when = "parsing";
            goto tmp_error;
        }

        if (!g_markup_parse_context_end_parse(ctxt, &tmp_err)) {
            tmp_err_when = "ending the parse";
            goto tmp_error;
        }

        g_markup_parse_context_free(ctxt);
        ctxt = NULL;
    } while (thunk.next_marker);

tmp_error:
    if (tmp_err) {
        ret = FALSE;
        if (tmp_err_when) {
            /* produce errors similar to what perform_request would */
            g_set_error(error, ZCLOUD_ERROR, ZCERR_LASTING,
                "An error occurred while %s: (domain %s) (code %u) %s)",
                tmp_err_when, g_quark_to_string(tmp_err->domain), tmp_err->code,
                tmp_err->message);
            g_error_free(tmp_err);
        } else {
            g_propagate_error(error, tmp_err);
        }
    } else {
        ret = TRUE;
    }

    g_free(reg_pat);
#ifndef HAVE_REGEX_H /* when using GLib regex, this needs to be freed*/
    g_free(thunk.pattern);
#endif
    g_free(thunk.text);
    g_free(thunk.next_marker);
    if (ctxt) g_markup_parse_context_free(ctxt);
    g_object_unref(down_con);

    return ret;
}

void
class_init(
    S3StoreClass *cls)
{
    GObjectClass *goc = G_OBJECT_CLASS(cls);
    ZCloudStoreClass *store_class = ZCLOUD_STORE_CLASS(cls);

    goc->finalize = finalize_impl;

    store_class->setup = setup_impl;
    store_class->create = create_impl;
    store_class->upload = upload_impl;
    store_class->download = download_impl;
    store_class->exists = exists_impl;
    store_class->delete = delete_impl;
    store_class->list = list_impl;

#ifdef HAVE_REGEX_H
  /* using POSIX regular expressions */
  struct {const char * str; int flags; regex_t *regex;} regexes[] = {
        {"<Code>[[:space:]]*([^<]*)[[:space:]]*</Code>",
         REG_EXTENDED | REG_ICASE,
         &cls->error_name_regex},
        {"^ETag:[[:space:]]*\"([^\"]+)\"[[:space:]]*$",
         REG_EXTENDED | REG_ICASE | REG_NEWLINE,
         &cls->etag_regex},
        {"<Message>[[:space:]]*([^<]*)[[:space:]]*</Message>",
         REG_EXTENDED | REG_ICASE,
         &cls->message_regex},
        {"^[a-z0-9]((-*[a-z0-9])|(\\.[a-z0-9])){2,62}$",
         REG_EXTENDED | REG_NOSUB,
         &cls->subdomain_regex},
        {"(/>)|(>([^<]*)</LocationConstraint>)",
         REG_EXTENDED | REG_ICASE,
         &cls->location_con_regex},
        {NULL, 0, NULL}
    };
    char regmessage[1024];
    int size, i;
    int reg_result;

    for (i = 0; regexes[i].str; i++) {
        reg_result = regcomp(regexes[i].regex, regexes[i].str, regexes[i].flags);
        if (reg_result != 0) {
            size = regerror(reg_result, regexes[i].regex, regmessage, sizeof(regmessage));
            g_error("Regex error: %s", regmessage);
        }
    }
#elif GLIB_CHECK_VERSION(2,14,0)
  /* using PCRE via GLib */
  struct {const char * str; int flags; regex_t *regex;} regexes[] = {
        {"<Code>\\s*([^<]*)\\s*</Code>",
         G_REGEX_OPTIMIZE | G_REGEX_CASELESS,
         &cls->error_name_regex},
        {"^ETag:\\s*\"([^\"]+)\"\\s*$",
         G_REGEX_OPTIMIZE | G_REGEX_CASELESS,
         &cls->etag_regex},
        {"<Message>\\s*([^<]*)\\s*</Message>",
         G_REGEX_OPTIMIZE | G_REGEX_CASELESS,
         &cls->message_regex},
        {"^[a-z0-9]((-*[a-z0-9])|(\\.[a-z0-9])){2,62}$",
         G_REGEX_OPTIMIZE | G_REGEX_NO_AUTO_CAPTURE,
         &cls->subdomain_regex},
        {"(/>)|(>([^<]*)</LocationConstraint>)",
         G_REGEX_CASELESS,
         &cls->location_con_regex},
        {NULL, 0, NULL}
  };
  int i;
  GError *err = NULL;

  for (i = 0; regexes[i].str; i++) {
      *(regexes[i].regex) = g_regex_new(regexes[i].str, regexes[i].flags, 0, &err);
      if (err) g_error("Regex error: %s", err->message);
  }
#else
#error must have POSIX or GLib/PCRE regular expressions
#endif
}

GType
s3_store_get_type(void)
{
    static GType type = 0;

    if G_UNLIKELY(type == 0) {
        static const GTypeInfo info = {
            sizeof (S3StoreClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            (GClassFinalizeFunc) NULL,
            NULL /* class_data */,
            sizeof (S3Store),
            0 /* n_preallocs */,
            (GInstanceInitFunc) instance_init,
            NULL
        };

        type = g_type_register_static (ZCLOUD_TYPE_STORE, "S3Store", &info,
                                       (GTypeFlags)0);
    }

    return type;
}
