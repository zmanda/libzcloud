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

#ifndef __S3_UTIL_H__
#define __S3_UTIL_H__

#ifdef HAVE_REGEX_H
#  ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#  endif
#include <regex.h>
#endif
#include <glib.h>

/*
 * Constants
 */

/* number of raw bytes in MD5 hash */
#define S3_MD5_HASH_BYTE_LEN 16
/* length of an MD5 hash encoded as base64 (not including terminating NULL) */
#define S3_MD5_HASH_B64_LEN 25
/* length of an MD5 hash encoded as hexadecimal (not including terminating NULL) */
#define S3_MD5_HASH_HEX_LEN 32

/*
 * Types
 */

#ifndef HAVE_REGEX_H
typedef GRegex* regex_t;

typedef gint regoff_t;
typedef struct
{
    regoff_t rm_so;  /* Byte offset from string's start to substring's start.  */
    regoff_t rm_eo;  /* Byte offset from string's start to substring's end.  */
} regmatch_t;

typedef enum
{
    REG_NOERROR = 0,      /* Success.  */
    REG_NOMATCH          /* Didn't find a match (for regexec).  */
} reg_errcode_t;
#endif

/*
 * Functions
 */

#ifndef USE_GETTEXT
/* we don't use gettextize, so hack around this ... */
#define _(str) (str)
#endif

/*
 * Wrapper around regexec to handle programmer errors.
 * Only returns if the regexec returns 0 (match) or REG_NOMATCH.
 * See regexec(3) documentation for the rest.
 */
int
s3_regexec_wrap(regex_t *regex,
           const char *str,
           size_t nmatch,
           regmatch_t pmatch[],
           int eflags);

#ifndef HAVE_AMANDA_H
char*
find_regex_substring(const char* base_string,
           const regmatch_t match);
#endif

/*
 * Encode bytes using Base-64
 *
 * @note: GLib 2.12+ has a function for this (g_base64_encode)
 *     but we support much older versions. gnulib does as well, but its
 *     hard to use correctly (see its notes).
 *
 * @param to_enc: The data to encode.
 * @returns:  A new, null-terminated string or NULL if to_enc is NULL.
 */
gchar*
s3_base64_encode(const GByteArray *to_enc);

/*
 * Encode bytes using hexadecimal
 *
 * @param to_enc: The data to encode.
 * @returns:  A new, null-terminated string or NULL if to_enc is NULL.
 */
gchar*
s3_hex_encode(const GByteArray *to_enc);

/*
 * Compute the MD5 hash of a blob of data.
 *
 * @param to_hash: The data to compute the hash for.
 * @returns:  A new GByteArray containing the MD5 hash of data or
 * NULL if to_hash is NULL.
 */
GByteArray*
s3_compute_md5_hash(const GByteArray *to_hash);

#endif
