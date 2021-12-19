/* *********************************************************************** *
 * File   : spec_assign.c                             Part of Valhalla MUD *
 * Version: 1.04                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Assignment of special functions constants.                     *
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

#include "spec_assign.h"

#include "handler.h"
#include "interpreter.h"
#include "utils.h"

#include <cstdio>

auto spec_unused(struct spec_arg *sarg) -> int
{
   if(sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      slog(LOG_ALL, 0, "Unit %s@%s had undefined special routine.", UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
   }

   return SFR_SHARE;
}

/* ********************************************************************
 *  Assignments                                                       *
 ******************************************************************** */

/* macro definitions for array indices are in values.h */
