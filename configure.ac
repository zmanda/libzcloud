#  -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#  vi: set tabstop=4 shiftwidth=4 expandtab: */
#  ***** BEGIN LICENSE BLOCK *****
#  Version: LGPL 2.1/GPL 2.0
#  This file is part of libzcloud.
# 
#  libzcloud is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License (the LGPL)
#  as published by the Free Software Foundation, either version 2.1 of
#  the LGPL, or (at your option) any later version.
# 
#  libzcloud is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
# 
#  The Original Code is Zmanda Incorporated code.
# 
#  The Initial Developer of the Original Code is
#   Zmanda Incorporated
#  Portions created by the Initial Developer are Copyright (C) 2009
#  the Initial Developer. All Rights Reserved.
# 
#  Contributor(s):
#    Nikolas Coukouma <atrus@zmanda.com>
# 
#  Alternatively, the contents of this file may be used under the terms of
#  the GNU General Public License Version 2 or later (the "GPL"),
#  in which case the provisions of the GPL are applicable instead
#  of those above. If you wish to allow use of your version of this file only
#  under the terms of either the GPL and not to allow others to
#  use your version of this file under the terms of the LGPL, indicate your
#  decision by deleting the provisions above and replace them with the notice
#  and other provisions required by the GPL. If you do not delete
#  the provisions above, a recipient may use your version of this file under
#  the terms of either the the GPL or the LGPL.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  and GNU General Public License along with libzcloud. If not, see
#  <http://www.gnu.org/licenses/>.
# 
#  ***** END LICENSE BLOCK ***** */

AC_INIT
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_SRCDIR([lib/store.h])
AC_CONFIG_HEADER([config/config.h])
AM_INIT_AUTOMAKE(libzcloud, "1.0alpha")
AC_PREREQ(2.59)

# Libtool
AM_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
ZCLOUD_CHECK_GLIB

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero regcomp])

AC_CONFIG_FILES([
    Makefile
    lib/Makefile
])

AC_CONFIG_SUBDIRS([
])

AC_OUTPUT