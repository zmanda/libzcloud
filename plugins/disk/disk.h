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


#ifndef DISK_H
#define DISK_H

#include "zcloud.h"

G_BEGIN_DECLS

/* store.c */

GType disk_store_get_type(void);
#define DISK_TYPE_STORE (disk_store_get_type())
#define DISK_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), DISK_TYPE_STORE, DiskStore))
#define DISK_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), DISK_TYPE_STORE, DiskStoreClass))
#define DISK_IS_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DISK_TYPE_STORE))
#define DISK_IS_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DISK_TYPE_STORE))
#define DISK_STORE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), DISK_TYPE_STORE, DiskStoreClass))

G_END_DECLS

#endif

