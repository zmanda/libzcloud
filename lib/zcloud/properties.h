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

#ifndef ZCLOUD_PROPERTIES_H
#define ZCLOUD_PROPERTIES_H

G_BEGIN_DECLS

/*
 * Property specifications
 */

typedef struct ZCloudPropertySpec_s {
    /* name of the property - lowercase, underscores */
    gchar *name;

    /* property type, represented as a GType */
    GType type;

    /* short description of the property */
    gchar *description;
} ZCloudPropertySpec;

G_END_DECLS

#endif

