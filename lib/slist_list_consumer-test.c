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

#include "test.h"

void
test_slist_list_consumer(void)
{
    static const gchar *to_insert[] = {"hi", "bye", "1!", NULL};
    ZCloudSListListConsumer *o;
    ZCloudListConsumer *lc;
    GSList *l;
    guint i;
    gboolean have_all;

    g_type_init();  /* TODO */

    o = zcloud_slist_list_consumer();
    lc = ZCLOUD_LIST_CONSUMER(o);

    l = zcloud_slist_list_consumer_grab_contents(o);
    is_null(l, "grabbing a fresh list returns NULL");

    for (i = 0; to_insert[i]; i++) {
        zcloud_list_consumer_got_result(lc, to_insert[i]);
    }

    l = zcloud_slist_list_consumer_grab_contents(o);
    isnt_null(l, "grabbing a list after some got_result()s returns non-NULL");

    have_all = TRUE;
    for (i = 0; to_insert[i]; i++) {
        if (!g_slist_find_custom(l, to_insert[i], (GCompareFunc) strcmp)) {
            have_all = FALSE;
        }
    }
    ok(have_all, "found all the results we got in the list");

    g_object_unref(o);
}
