/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/*  ***** BEGIN LICENSE BLOCK *****
 * Copyright (C) 2008,2009 Zmanda Incorporated. All Rights Reserved.
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

#ifndef S3_STORE_H
#define S3_STORE_H

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

#include "zcloud.h"

G_BEGIN_DECLS

GType s3_store_get_type(void);
#define S3_TYPE_STORE (s3_store_get_type())
#define S3_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), S3_TYPE_STORE, S3Store))
#define S3_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), S3_TYPE_STORE, S3StoreClass))
#define S3_IS_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), S3_TYPE_STORE))
#define S3_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), S3_TYPE_STORE))
#define S3_STORE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), S3_TYPE_STORE, S3StoreClass))

typedef struct S3Store_s {
    ZCloudStore parent;

    /* authentication information */
    char *access_key;
    char *secret_key;
    char *user_token;
    char *product_token;

    /* other properties */
    char *bucket_location;
    char *ca_info;

    gboolean use_ssl;
    gboolean verbose;

    /* saved libcurl handle */
    CURL *curl;

    /* the bucket that the keys are held in, if any */
    char *bucket;
} S3Store;

typedef struct S3StoreClass_s {
    ZCloudStoreClass parent_class;

    regex_t etag_regex;
    regex_t error_name_regex;
    regex_t message_regex;
    regex_t subdomain_regex;
    regex_t location_con_regex;
} S3StoreClass;

G_END_DECLS
#endif
