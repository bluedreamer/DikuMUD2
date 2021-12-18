/* *********************************************************************** *
 * File   : db_utils.c                                Part of Valhalla MUD *
 * Version: 1.15                                                           *
 * Author : seifert@diku.dk and quinn@diku.dk                              *
 *                                                                         *
 * Purpose: Assorted routines for database functions.                      *
 *                                                                         *
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

/* 27/09/92 gnort: Found something looking like a BAD memory-waster in
                   free_unit, caused by wrongly nested if-else's           */
/* Sun Jun 27 1993 HHS applied world status update */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "db.h"
#include "db_file.h"
#include "handler.h"
#include "limits.h"
#include "skills.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

/************************************************************************
 *  Memory Handling Procedures Below this line                          *
 ********************************************************************** */
