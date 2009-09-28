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

INCLUDES = -I $(top_srcdir)/lib $(OPENSSL_INCLUDES)
AM_CPPFLAGS = $(GLIB_CFLAGS) $(LIBCURL_CPPFLAGS)
AM_LDFLAGS = $(ZC_LDFLAGS) $(LIBCURL_LDFLAGS)
AM_LIBTOOLFLAGS = --silent

zcplugin_DATA = s3.xml
EXTRA_DIST = $(zcplugin_DATA)

zcplugin_LTLIBRARIES = libs3.la
libs3_la_LDFLAGS = -avoid-version -module $(LIBCURL_LDFLAGS)
libs3_la_LIBADD = $(LIBCURL_LIBS) $(OPENSSL_LIBS)
libs3_la_SOURCES = \
	init.c \
	s3_store.h store.c\
	util.h util.c

TESTS = s3test
s3test_SOURCES = s3test.c $(top_builddir)/lib/test-checks.c
s3test_LDADD = $(top_builddir)/lib/libzcloud.la
