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


#ifndef ZCLOUD_FD_UPLOAD_PRODUCER_H
#define ZCLOUD_FD_UPLOAD_PRODUCER_H

#include "upload_producer.h"

G_BEGIN_DECLS

GType zcloud_fd_upload_producer_get_type(void);
#define ZCLOUD_TYPE_FD_UPLOAD_PRODUCER (zcloud_fd_upload_producer_get_type())
#define ZCLOUD_FD_UPLOAD_PRODUCER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_FD_UPLOAD_PRODUCER, ZCloudFDUploadProducer))
#define ZCLOUD_FD_UPLOAD_PRODUCER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_FD_UPLOAD_PRODUCER, ZCloudFDUploadProducerClass))
#define ZCLOUD_IS_FD_UPLOAD_PRODUCER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_FD_UPLOAD_PRODUCER))
#define ZCLOUD_IS_FD_UPLOAD_PRODUCER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_FD_UPLOAD_PRODUCER))
#define ZCLOUD_FD_UPLOAD_PRODUCER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_FD_UPLOAD_PRODUCER, ZCloudFDUploadProducerClass))

typedef struct ZCloudFDUploadProducer_s {
    ZCloudUploadProducer parent;

    int fd;
    gsize bytes_read;
} ZCloudFDUploadProducer;

typedef struct ZCloudFDUploadProducerClass_s {
    ZCloudUploadProducerClass parent_class;
} ZCloudFDUploadProducerClass;

/* constructor */
ZCloudFDUploadProducer *
zcloud_fd_upload_producer(int fd);

G_END_DECLS

#endif
