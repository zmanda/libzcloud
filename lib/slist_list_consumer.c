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

#include "internal.h"

static void
got_result_impl(
    ZCloudListConsumer *zself,
    const gchar *key)
{
    ZCloudSListListConsumer *self = ZCLOUD_SLIST_LIST_CONSUMER(zself);
    self->list = g_slist_prepend(self->list, g_strdup(key));
}

static GSList *
grab_contents_impl(
    ZCloudSListListConsumer *self)
{
    return self->list;
}

GSList *
zcloud_slist_list_consumer_grab_contents(
    ZCloudSListListConsumer *self)
{
    ZCloudSListListConsumerClass *c = ZCLOUD_SLIST_LIST_CONSUMER_GET_CLASS(self);
    g_assert(c->grab_contents != NULL);
    return (c->grab_contents)(self);
}

static void
finalize_impl(GObject *o)
{
    ZCloudSListListConsumer *self = ZCLOUD_SLIST_LIST_CONSUMER(o);

    g_slist_free(self->list);
}

static void
class_init(ZCloudSListListConsumerClass *klass)
{
    GObjectClass *go_class = G_OBJECT_CLASS(klass);
    ZCloudListConsumerClass *up_class = ZCLOUD_LIST_CONSUMER_CLASS(klass);

    go_class->finalize = finalize_impl;
    up_class->got_result = got_result_impl;
    klass->grab_contents = grab_contents_impl;
}

GType
zcloud_slist_list_consumer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudSListListConsumerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudSListListConsumer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_LIST_CONSUMER,
                                      "ZCloudSListListConsumer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

ZCloudSListListConsumer *
zcloud_slist_list_consumer(void)
{
    ZCloudSListListConsumer *ret;

    ret = g_object_new(ZCLOUD_TYPE_SLIST_LIST_CONSUMER, NULL);
    ret->list = NULL;

    return ret;
}
