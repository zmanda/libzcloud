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

#
# SYNOPSIS
#
#   ZCLOUD_WITH_ZCPLUGINDIR
#
# OVERVIEW
#
#   Allow user to specify the zcplugindir, defaulting to ${libdir}/libzcloud-plugins.
#   Define ZCPLUGINDIR and substitute $zcplugindir with the result.
#
AC_DEFUN([ZCLOUD_WITH_ZCPLUGINDIR],
[
    AC_ARG_WITH(zcplugindir,
	AS_HELP_STRING([--with-zcplugindir=DIR],
	    [plugin directory @<:@libdir/libzcloud-plugins@:>@]),
	[
	    case "$withval" in
	    "" | y | ye | yes | n | no)
		AC_MSG_ERROR([Invalid --with-zcplugindir value])
	      ;;
	    *) zcplugindir="$withval"
	      ;;
	    esac
	], [
	    : ${zcplugindir='${libdir}/libzcloud-plugins'} # (variable will be evaluated below)
	]
    )

    AC_SUBST([zcplugindir])
    AC_DEFINE_DIR([ZCPLUGINDIR], [zcplugindir],
      [Default libzcloud plugin directory])
])
