/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/*  ***** BEGIN LICENSE BLOCK *****
 * Copyright (C) 2009 Zmanda Incorporated. All Rights Reserved.
 *
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
 *  ***** END LICENSE BLOCK ***** */


#ifndef ZCLOUD_FD_DOWNLOAD_CONSUMER_H
#define ZCLOUD_FD_DOWNLOAD_CONSUMER_H

#include "download_consumer.h"

G_BEGIN_DECLS

GType zcloud_fd_download_consumer_get_type(void);
#define ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER (zcloud_fd_download_consumer_get_type())
#define ZCLOUD_FD_DOWNLOAD_CONSUMER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER, ZCloudFDDownloadConsumer))
#define ZCLOUD_FD_DOWNLOAD_CONSUMER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER, ZCloudFDDownloadConsumerClass))
#define ZCLOUD_IS_FD_DOWNLOAD_CONSUMER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER))
#define ZCLOUD_IS_FD_DOWNLOAD_CONSUMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER))
#define ZCLOUD_FD_DOWNLOAD_CONSUMER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_FD_DOWNLOAD_CONSUMER, ZCloudFDDownloadConsumerClass))

typedef struct ZCloudFDDownloadConsumer_s {
    ZCloudDownloadConsumer parent;

    int fd;
    gsize bytes_written;
} ZCloudFDDownloadConsumer;

typedef struct ZCloudFDDownloadConsumerClass_s {
    ZCloudDownloadConsumerClass parent_class;

} ZCloudFDDownloadConsumerClass;

/* constructor */
ZCloudFDDownloadConsumer *
zcloud_fd_download_consumer(
    int fd);

G_END_DECLS

#endif
