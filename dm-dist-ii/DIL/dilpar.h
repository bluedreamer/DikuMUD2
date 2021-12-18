/* *********************************************************************** *
 * File   : dilpar.h                                  Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk                                                *
 *                                                                         *
 * Purpose: Prototypes, types and defines for the rest of dil              *
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

#ifndef _MUD_DILPAR_H
#define _MUD_DILPAR_H

#include "essential.h"

#define DIL_VERSION "v3.30"

#define SYMSIZE 40

/* This is for the compiler */

#define CODESIZE 16768 /* maximum expression size */

struct exptype
{
   uint8_t  dsl, typ, rtyp; /* static/dynamic/lvalue, type, req type */
   uint8_t  boolean;        /* if expression contains boolean op */
   int32_t  num;            /* static value INT or field type */
   uint8_t *code;           /* code / static value SP */
   uint8_t *codep;          /* code write pointer */
};
#endif /* _MUD_DILPAR_H */
