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
#ifdef HAVE_XXX_H
#include <xxx.h>
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

#include "plugins.h"

#endif
