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
#   ZCLOUD_DEFINE_PLUGIN(plugin, install_by_default, help, [if-enabled])
#
# OVERVIEW
#
#   * Add an --enable-$plugin-plugin or --disable-$plugin-plugin command
#   * adds plugins/$plugin/Makefile to AC_CONFIG_FILES
#
#   If the plugin is enabled
#   * execute $if-enabled
#   * add plugins/$plugin to PLUGIN_SUBDIRS
#
AC_DEFUN([ZCLOUD_DEFINE_PLUGIN], [
    AC_ARG_ENABLE([$1],
        AS_HELP_STRING(
            m4_if([$2], [no], [--enable-$1-plugin], [--disable-$1-plugin]),
            [$3]),
        [ PLUGIN_$1="$enableval" ],
        [ PLUGIN_$1="$2" ])

    AC_CONFIG_FILES([plugins/$1/Makefile])

    if test x"$PLUGIN_$1" = x"yes"; then
        $4
        PLUGIN_SUBDIRS="$PLUGIN_SUBDIRS plugins/$1"
    fi

    AC_SUBST(PLUGIN_SUBDIRS)

    plugins=`echo $PLUGIN_SUBDIRS | sed 's,plugins/,,g'`
    ZCLOUD_SUMMARY_ADD([Enabled plugins: $plugins])
    plugins=
])

