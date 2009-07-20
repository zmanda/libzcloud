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
#   ZCLOUD_CHECK_OPENSSL
#
# OVERVIEW
#
#   Look for OpenSSL in a number of default spots, or in a user-selected
#   spot (via --with-openssl).  Sets
#     OPENSSL_INCLUDES to the include directives required
#     OPENSSL_LIBS to the -l directives required
#     OPENSSL_LDFLAGS to the -L or -R flags required
#   If OpenSSL is not found, the configure run fails.
#

AC_DEFUN([ZCLOUD_CHECK_OPENSSL], [
    dnl this is just a thin wrapper so we fail immediately with no OpenSSL
    AX_CHECK_OPENSSL([
            # cool!
        ], [
            AC_MSG_FAILURE([No working OpenSSL found])
        ])

    ZCLOUD_SUMMARY_ADD([OPENSSL_INCLUDES=$OPENSSL_INCLUDES])
    ZCLOUD_SUMMARY_ADD([OPENSSL_LIBS=$OPENSSL_LIBS])
    ZCLOUD_SUMMARY_ADD([OPENSSL_LDFLAGS=$OPENSSL_LDFLAGS])
])
