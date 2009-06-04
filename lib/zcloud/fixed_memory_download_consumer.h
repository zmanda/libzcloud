/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: LGPL 2.1/GPL 2.0
 * This file is part of libzcloud.
 *
 * libzcloud is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License (the LGPL)
 * as published by the Free Software Foundation, either version 2.1 of
 * the LGPL, or (at your option) any later version.
 *
 * libzcloud is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * The Original Code is Zmanda Incorporated code.
 *
 * The Initial Developer of the Original Code is
 *  Zmanda Incorporated
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Nikolas Coukouma <atrus@zmanda.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License Version 2 or later (the "GPL"),
 * in which case the provisions of the GPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL and not to allow others to
 * use your version of this file under the terms of the LGPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of either the the GPL or the LGPL.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * and GNU General Public License along with libzcloud. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER_H
#define ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER_H

#include "download_consumer.h"

G_BEGIN_DECLS

GType zcloud_fixed_memory_download_consumer_get_type(void);
#define ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER (zcloud_fixed_memory_download_consumer_get_type())
#define ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER, ZCloudFixedMemoryDownloadConsumer))
#define ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER, ZCloudFixedMemoryDownloadConsumerClass))
#define ZCLOUD_IS_FIXED_MEMORY_DOWNLOAD_CONSUMER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER))
#define ZCLOUD_IS_FIXED_MEMORY_DOWNLOAD_CONSUMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER))
#define ZCLOUD_FIXED_MEMORY_DOWNLOAD_CONSUMER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_FIXED_MEMORY_DOWNLOAD_CONSUMER, ZCloudFixedMemoryDownloadConsumerClass))

enum {
    ZCLOUD_PROP_FIXED_MEMORY_DOWNLOAD_CONSUMER_BUFFER = 1,
    ZCLOUD_PROP_FIXED_MEMORY_DOWNLOAD_CONSUMER_BUFFER_LENGTH,
    ZCLOUD_PROP_FIXED_MEMORY_DOWNLOAD_CONSUMER_BUFFER_POSITION,
};

typedef struct ZCloudFixedMemoryDownloadConsumer_s {
    ZCloudDownloadConsumer parent;

    guint8 *buffer;
    guint buffer_length;
    guint buffer_position;
} ZCloudFixedMemoryDownloadConsumer;

typedef struct ZCloudFixedMemoryDownloadConsumerClass_s {
    ZCloudDownloadConsumerClass parent_class;

    guint8 * (*get_contents)(ZCloudFixedMemoryDownloadConsumer *self, gsize *length);
} ZCloudFixedMemoryDownloadConsumerClass;

/* constructor */
/* Note: specifying 0 as max_buffer_length results in an unbounded buffer */
ZCloudFixedMemoryDownloadConsumer *
zcloud_fixed_memory_download_consumer(
    guint8 *buffer,
    guint buffer_length);

guint8 *
zcloud_fixed_memory_download_consumer_get_contents(
    ZCloudFixedMemoryDownloadConsumer *self,
    gsize *length);

G_END_DECLS

#endif
