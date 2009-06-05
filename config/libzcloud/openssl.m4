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
#   spot.  Set
#     OPENSSL_INCLUDES to the include directives required
#     OPENSSL_LIBS to the -l directives required
#     OPENSSL_LDFLAGS to the -L or -R flags required
#
AC_DEFUN([ZCLOUD_CHECK_OPENSSL], [
    AC_MSG_CHECKING(for OpenSSL)
    AC_REQUIRE([AC_CANONICAL_BUILD])

    AC_ARG_WITH(openssl,
        AS_HELP_STRING([--with-openssl=DIR],
            [root of the OpenSSL directory]),
        [
            case "$withval" in
            "" | y | ye | yes | n | no)
            AC_MSG_ERROR([Invalid --with-openssl value])
              ;;
            *) ssldirs="$withval"
              ;;
            esac
        ], [
            ssldirs="/usr/local/ssl /usr/lib/ssl /usr/ssl /usr/pkg /usr/local /usr"
        ]
        )
    
    # note that we #include <openssl/foo.h>, so the OpenSSL headers have to be in
    # an 'openssl' subdirectory

    OPENSSL_INCLUDES=
    found=false
    for ssldir in $ssldirs; do
        if test -f "$ssldir/include/openssl/ssl.h"; then
            found=true
            test "$ssldir" = "/usr" || OPENSSL_INCLUDES="-I$ssldir/include"
            break
        fi
    done

    if ! $found; then
        AC_MSG_RESULT([no])
        AC_MSG_FAILURE([Cannot find ssl.h])
    fi
    AC_MSG_RESULT([yes])

    # try linking with that directory, saving LIBS and LDFLAGS because
    # we do not want to link *everything* with openssl
    save_LIBS="$LIBS"
    save_LDFLAGS="$LDFLAGS"
    test "$ssldir" = "/usr" || LDFLAGS="$LDFLAGS -L $ssldir/lib"
    AC_SEARCH_LIBS(SSL_new, ssl, [], [
        AC_MSG_FAILURE([Cannot find ssl libraries])
    ])
    LDFLAGS="$save_LDFLAGS"
    LIBS="$save_LIBS"

    test "$ssldir" = "/usr" || OPENSSL_LDFLAGS="-L $ssldir/lib"
    OPENSSL_LIBS="-lssl -lcrypto"

    AC_SUBST([OPENSSL_INCLUDES])
    AC_SUBST([OPENSSL_LIBS])
    AC_SUBST([OPENSSL_LDFLAGS])
])
