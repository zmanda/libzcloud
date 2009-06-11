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


#ifndef ZCLOUD_H
#define ZCLOUD_H

#include <glib.h>
#include <glib-object.h>

/* classes */
#include "zcloud/store.h"
#include "zcloud/download_consumer.h"
#include "zcloud/fixed_memory_download_consumer.h"
#include "zcloud/growing_memory_download_consumer.h"
#include "zcloud/list_consumer.h"
#include "zcloud/memory_upload_producer.h"
#include "zcloud/progress_listener.h"
#include "zcloud/upload_producer.h"

/* miscellaneous */
#include "zcloud/error.h"
#include "zcloud/plugins.h"

G_BEGIN_DECLS

/* Call this before calling any other zcloud functions.  If this fails,
 * do not call it again.
 *
 * @returns: FALSE on error, with ERROR set properly
 */
gboolean zcloud_init(GError **error);

G_END_DECLS

#endif
