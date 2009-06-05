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


/* This header file is *only* for use internally to libzcloud - that is, it is
 * only included by source files that become part of libzcloud.la.
 */

#ifndef ZCLOUD_INTERNAL_H
#define ZCLOUD_INTERNAL_H

/* get our autoconf definitions */
#include "config.h"

/* include glib and friends */
#include <glib.h>
#include <gmodule.h>
#include <glib-object.h>

/* basic system includes - anything purpose-specific should be included only
 * in the source file(s) that need it */

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
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

/* set up G_GNUC_INTERNAL to work regardless of glib version, even if that means
 * leaking symbols on older systems.  This macro is used to mark any functions
 * which should not be available from the final, linked libzcloud, but which are
 * referenced in multiple source files within the library -- in other words, any
 * functions defined in internal header files.
 *
 * Note that internal functions have the prefix "zc_" rather than "zcloud_"
 */
#ifndef G_HAVE_GNUC_VISIBILITY
#ifndef G_GNUC_INTERNAL
#define G_GNUC_INTERNAL
#endif
#endif

/* include internal headers */
#include "internal/plugins.h"

/* finish by including the public header */
#include "zcloud.h"

#endif
