#  -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#  vi: set tabstop=4 shiftwidth=4 expandtab: */
# ***** BEGIN LICENSE BLOCK *****
# Copyright (C) 2009 Zmanda Incorporated. All Rights Reserved.
#
# This file is part of libzcloud.
#
# libzcloud is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License (the LGPL)
# as published by the Free Software Foundation, either version 2.1 of
# the LGPL, or (at your option) any later version.
#
# libzcloud is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#  ***** END LICENSE BLOCK *****


# SYNOPSIS
#
#   ZCLOUD_INIT_COMPILER_FLAGS
#
# DESCRIPTION
#
#   Set up extra compiler flags, mostly for GCC.  Sets up ZC_WARNING_CPPFLAGS,
#   which can be added to AM_CPPFLAGS in Makefiles.  Sets up ZC_LDFLAGS, also for
#   inclusion in Makefiles
#
AC_DEFUN([ZCLOUD_INIT_COMPILER_FLAGS], [
    AC_REQUIRE([AC_PROG_GCC_TRADITIONAL])

    # Warn for just about everything
    AX_CFLAGS_GCC_OPTION(-Wall, ZC_WARNING_CPPFLAGS)
    
    AC_SUBST([ZC_WARNING_CPPFLAGS])
    AC_SUBST([ZC_LDFLAGS])
])

# SYNOPSIS
#
#   ZCLOUD_ADD_GCC_WARNING_OPTION(option)
#
# DESCRIPTION
#
#   Adds OPTION to ZC_WARNING_CPPFLAGS if the compiler is gcc and if it accepts
#   the option.
AC_DEFUN([ZCLOUD_ADD_GCC_WARNING_OPTION], [
    AC_REQUIRE([ZCLOUD_INIT_COMPILER_FLAGS])
    AX_CFLAGS_GCC_OPTION($1, ZC_WARNING_CPPFLAGS)
])
