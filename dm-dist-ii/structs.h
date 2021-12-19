#pragma once
/* *********************************************************************** *
 * File   : structs.h                                 Part of Valhalla MUD *
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

#include "bin_search_type.h"
#include "blkfile.h"
#include "combat.h"
#include "essential.h"
#include "extra.h"
#include "hashstring.h"
#include "protocol.h"
#include "queue.h"
#include "snoop_data.h"
#include "system.h"
#include "values.h"
#include "zone_reset_cmd.h"

#define FI_MAX_ZONENAME 30 /* Max length of any zone-name    */
#define FI_MAX_UNITNAME 15 /* Max length of any unit-name    */

#define MESS_ATTACKER 1
#define MESS_VICTIM   2
#define MESS_ROOM     3

/* For use in spec_assign.c's unit_function_array[] */
#define SD_NEVER 0 /* Never save this function           */
#define SD_NULL  1 /* Ignore fptr->data (save as 0 ptr)  */
#define SD_ASCII 2 /* If pointer, then it's ascii char * */

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */

#include "pc_account_data.h"
#include "pc_time_data.h"

/* ----------------- UNIT GENERAL STRUCTURES ----------------------- */

/* ----------------- Destructed decalrations ----------------------- */

#define DR_UNIT   0
#define DR_AFFECT 1
#define DR_FUNC   2

auto is_destructed(int i, void *ptr) -> int;
