#  -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#  vi: set tabstop=4 shiftwidth=4 noexpandtab: */
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

#
# File Lists
#

public_headers = \
	zcloud/download_consumer.h \
	zcloud/error.h \
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
    list_consumer.c \
    memory_upload_producer.c \
    plugins.c \
    progress_listener.c \
    store.c \
    upload_producer.c \
    zcloud.c


test_source = \
	test-checks.c \
	memory_upload_producer-test.c \
	plugins-test.c

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
