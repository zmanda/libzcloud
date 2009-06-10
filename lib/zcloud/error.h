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


/*
 * Error handling
 */

#ifndef ZCLOUD_ERROR_H
#define ZCLOUD_ERROR_H

G_BEGIN_DECLS

/* a quark which can be used to identify GError objects from libzcloud */
#define ZCLOUD_ERROR zcloud_error_quark()
GQuark zcloud_error_quark(void);

/* error codes for GErrors in the ZCLOUD domain */
typedef enum ZCloudError_e {
    ZCERR_NONE = 0,
    ZCERR_UNKNOWN = 1,
    ZCERR_MODULE = 2,           /* error loading a module */
    ZCERR_PLUGIN = 3,           /* error from a plugin, if nothing else is more suitable */
    ZCERR_PARAMETER = 4,        /* parameter-related error */
} ZCloudError;

G_END_DECLS

#endif
