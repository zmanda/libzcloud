#  -*- Mode: makefile; tab-width: 4; indent-tabs-mode: t -*- */
#  vi: set tabstop=4 shiftwidth=4 noexpandtab: */
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

ZCLOUD_DEFINE_PLUGIN([s3], [yes], [Amazon S3 plugin], [
    case "$host" in
        sparc-sun-solaris2.10) # Solaris 10
        # curl is not in the LD_LIBRARY_PATH on Solaris 10, so we search
        # for it in a few common paths; we then extract the -L flags and
        # translate them to -R flags, as required by the runtime linker.
        AC_PATH_PROG(CURL_CONFIG, curl-config, [], $LOCSYSPATH:/opt/csw/bin:/usr/local/bin:/opt/local/bin)
        if test -n "$CURL_CONFIG"; then
            curlflags=`$CURL_CONFIG --libs 2>/dev/null`
            for flag in curlflags; do
                case $flag in
                    -L*) LIBCURL_LDFLAGS=`echo "x$flag" | sed -e 's/^x-L/-R/'`;;
                esac
            done
        fi
        ;;
    esac

    LIBCURL_CHECK_CONFIG(yes, 7.10.0, HAVE_CURL=yes, HAVE_CURL=no)
    if test x"$HAVE_CURL" != x"yes"; then
        AC_MSG_ERROR([** curl-7.10.0 or later is required to support S3])
    fi

    # rename this variable; we also get LBCURL_CPPFLAGS
    LIBCURL_LIBS=$LIBCURL

    # check the openssl headers for openssl/hmac.c
    found_hmac_h=no
    AC_CHECK_HEADERS([openssl/hmac.h crypto/hmac.h hmac.h],
                    [found_hmac_h=yes; break])
    if test x"$found_hmac_h" != x"yes"; then
        AC_MSG_ERROR([** openssl/hmac.h or equivalent not found; cannot build S3 plugin])
    fi

    AC_SUBST(LIBCURL_LIBS)
    AC_SUBST(LIBCURL_CPPFLAGS)
    AC_SUBST(LIBCURL_LDFLAGS)

	AC_CHECK_HEADERS([ \
		regex.h \
	])
])

# TODO: search for regex.h here instead of in toplevel configure, and if is not present, 
# check that the glib version is new enough to support GRegex
