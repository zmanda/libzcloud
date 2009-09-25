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


#ifndef ZCLOUD_SLIST_LIST_CONSUMER_H
#define ZCLOUD_SLIST_LIST_CONSUMER_H

#include "list_consumer.h"

G_BEGIN_DECLS

GType zcloud_slist_list_consumer_get_type(void);
#define ZCLOUD_TYPE_SLIST_LIST_CONSUMER (zcloud_slist_list_consumer_get_type())
#define ZCLOUD_SLIST_LIST_CONSUMER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_SLIST_LIST_CONSUMER, ZCloudSListListConsumer))
#define ZCLOUD_SLIST_LIST_CONSUMER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_SLIST_LIST_CONSUMER, ZCloudSListListConsumerClass))
#define ZCLOUD_IS_SLIST_LIST_CONSUMER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_SLIST_LIST_CONSUMER))
#define ZCLOUD_IS_SLIST_LIST_CONSUMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_SLIST_LIST_CONSUMER))
#define ZCLOUD_SLIST_LIST_CONSUMER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_SLIST_LIST_CONSUMER, ZCloudSListListConsumerClass))

typedef struct ZCloudSListListConsumer_s {
    ZCloudListConsumer parent;

    GSList *list;
} ZCloudSListListConsumer;

typedef struct ZCloudSListListConsumerClass_s {
    ZCloudListConsumerClass parent_class;

    GSList * (*grab_contents)(ZCloudSListListConsumer *self);
} ZCloudSListListConsumerClass;

/* constructor */
ZCloudSListListConsumer *
zcloud_slist_list_consumer(void);

GSList *
zcloud_slist_list_consumer_grab_contents(
    ZCloudSListListConsumer *self);

G_END_DECLS

#endif
