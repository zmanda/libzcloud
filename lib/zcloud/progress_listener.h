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


#ifndef ZCLOUD_PROGRESS_LISTENER_H
#define ZCLOUD_PROGRESS_LISTENER_H

G_BEGIN_DECLS

GType zcloud_progress_listener_get_type(void);
#define ZCLOUD_TYPE_PROGRESS_LISTENER (zcloud_progress_listener_get_type())
#define ZCLOUD_PROGRESS_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_PROGRESS_LISTENER, ZCloudProgressListener))
#define ZCLOUD_PROGRESS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_PROGRESS_LISTENER, ZCloudProgressListenerClass))
#define ZCLOUD_IS_PROGRESS_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_PROGRESS_LISTENER))
#define ZCLOUD_IS_PROGRESS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_PROGRESS_LISTENER))
#define ZCLOUD_PROGRESS_LISTENER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_PROGRESS_LISTENER, ZCloudProgressListenerClass))

typedef struct ZCloudProgressListener_s {
    GObject parent;

} ZCloudProgressListener;

typedef struct ZCloudProgressListenerClass_s {
    GObjectClass parent_class;

} ZCloudProgressListenerClass;

G_END_DECLS

#endif
