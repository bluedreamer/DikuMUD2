/* *********************************************************************** *
 * File   : select.cc                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Select hooks & buffer handling for sockets                     *
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

#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "cHook.h"
#include "select.h"
#include "utility.h"

/* ------------------------------------------------------------------- */
/*                            CAPTAIN HOOK                             */
/* ------------------------------------------------------------------- */
