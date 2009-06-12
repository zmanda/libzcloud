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


#
# File Lists
#

public_headers = \
	zcloud/download_consumer.h \
	zcloud/error.h \
	zcloud/fixed_memory_download_consumer.h \
	zcloud/growing_memory_download_consumer.h \
	zcloud/list_consumer.h \
	zcloud/memory_upload_producer.h \
	zcloud/plugins.h \
	zcloud/progress_listener.h \
	zcloud/store.h \
	zcloud/upload_producer.h

internal_headers = \
    internal/plugins.h

libzcloud_source = \
    download_consumer.c \
    error.c \
    fixed_memory_download_consumer.c \
    growing_memory_download_consumer.c \
    list_consumer.c \
    memory_upload_producer.c \
    plugins.c \
    progress_listener.c \
    store.c \
    upload_producer.c \
    zcloud.c

test_source = \
	test-checks.c \
	test-util.c \
	fixed_memory_download_consumer-test.c \
	growing_memory_download_consumer-test.c \
	memory_upload_producer-test.c \
	plugins-test.c \
	store-test.c

EXTRA_DIST = \
    libzcloud.pc.in

##
# compiler settings

INCLUDES = -I. -I$(top_srcdir)/config $(OPENSSL_INCLUDES)
AM_CPPFLAGS = $(GLIB_CFLAGS) $(ZC_WARNING_CPPFLAGS)
AM_LDFLAGS = $(ZC_LDFLAGS)
AM_LIBTOOLFLAGS = --silent

##
# install the headers

zcincludedir = $(includedir)/zcloud
zcinclude_HEADERS = $(public_headers)
include_HEADERS = zcloud.h

##
# install the library and pkgconfig source file

lib_LTLIBRARIES = libzcloud.la

libzcloud_la_LIBADD = $(GLIB_LIBS) $(OPENSSL_LIBS)
libzcloud_la_LDFLAGS = $(OPENSSL_LDFLAGS)
libzcloud_la_SOURCES = internal.h $(internal_headers) $(libzcloud_source)

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libzcloud.pc

##
# test library

# due to the need to access internal functions, tests are built within a
# specially-created library and invoked from a "test" binary.

check_LTLIBRARIES = libzcloud-test.la
libzcloud_test_la_LIBADD = $(libzcloud_la_LIBADD)
libzcloud_test_la_LDFLAGS = $(libzcloud_la_LDFLAGS)
libzcloud_test_la_SOURCES = $(libzcloud_la_SOURCES) $(test_source)

TESTS = libzcloud-test
check_PROGRAMS = libzcloud-test
libzcloud_test_SOURCES = test-main.c test.h
libzcloud_test_LDADD = libzcloud-test.la
