/* *********************************************************************** *
 * File   : config.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Server boot time configuration.                                *
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

#include "config.h"

#include "common.h"
#include "db.h"
#include "essential.h"
#include "files.h"
#include "str_parse.h"
#include "textutil.h"

#include <cstdio>
#include <cstring>

void ShowUsage(const char *name)
{
   fprintf(stderr,
           "Usage: %s [-h] [-d pathname] "
           "[-z pathname] [-c {players}]\n",
           name);
   fprintf(stderr, "  -h: This help screen.\n");
   fprintf(stderr, "  -c: List or Convert playerfile\n");
   fprintf(stderr, "  -d: Alternate lib/ dir\n");
   fprintf(stderr, "  -z: Alternate zon/ dir\n");
   fprintf(stderr, "  -p: Persistant containers list\n");
   fprintf(stderr, "Copyright (C) 1994 - 1996 by Valhalla.\n");
}
