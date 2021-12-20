#pragma once

#include "essential.h"

#define DIL_VERSION "v3.30"

#define SYMSIZE 40

/* This is for the compiler */

#define CODESIZE 16768 /* maximum expression size */

class exptype
{
public:
   uint8_t  dsl, typ, rtyp; /* static/dynamic/lvalue, type, req type */
   uint8_t  boolean;        /* if expression contains boolean op */
   int32_t  num;            /* static value INT or field type */
   uint8_t *code;           /* code / static value SP */
   uint8_t *codep;          /* code write pointer */
};
