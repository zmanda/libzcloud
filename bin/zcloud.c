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


#include <stdio.h>
#include "zcloud.h"

int main(int argc, char **argv)
{
    GError *error = NULL;
    ZCloudStore *store;

    if (argc != 2) {
        fprintf(stderr, "USAGE: zcloud <storespec>\n");
        return 1;
    }

    if (!zcloud_init(&error)) {
        fprintf(stderr, "Could not initialize libzcloud: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    store = zcloud_store_new(argv[1], &error);
    if (!store) {
        fprintf(stderr, "Could not load store '%s': %s\n", argv[1], error->message);
        g_error_free(error);
        return 1;
    }

    fprintf(stderr, "SUCCESS\n");
    return 0;
}
