/* *********************************************************************** *
 * File   : structs.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

#include "structs.h"
#include "handler.h"
#include "utils.h"

int world_norooms   = 0; /* number of rooms in the world   */
int world_noobjects = 0; /* number of objects in the world */
int world_nochars   = 0; /* number of chars in the world   */
int world_nonpc     = 0; /* number of chars in the world   */
int world_nopc      = 0; /* number of chars in the world   */
int world_nozones   = 0; /* number of zones in the world   */

/* Descriptor stuff is in system.c */
