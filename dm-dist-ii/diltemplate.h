#pragma once

#include "dilxref.h"
#include "zone_type.h"

#include <cstdint>

/*
 *  A DIL template for registering DIL programs/functions/procedures.
 *  Inline code is registered as local instances.
 *  Uppon loading old dil programs, an unlinked template is created.
 */
class zone_type;
class diltemplate
{
public:
   const char   *prgname;      /* program name @ zone */
   zone_type    *zone;         /* Pointer to owner of structure    */
   uint8_t       flags;        /* recall, etc. */
   uint16_t      intrcount;    /* max number of interrupts */
   uint16_t      varcrc;       /* variable crc from compiler */
   uint16_t      corecrc;      /* core crc from compiler */
   uint8_t       rtnt;         /* return type */
   uint8_t       argc;         /* number of arguments */
   uint8_t      *argt;         /* argument types */
   uint32_t      coresz;       /* size of coreblock */
   uint8_t      *core;         /* instructions, expressions and statics */
   uint16_t      varc;         /* number of variables */
   uint8_t      *vart;         /* variable types */
   uint16_t      xrefcount;    /* number of external references   */
   diltemplate **extprg;       /* external programs (SERVER only) */
   dilxref      *xrefs;        /* external references (DMC only)  */
   uint32_t      nActivations; /* Number of activations           */
   diltemplate  *next;         /* for zone templates              */
};
extern diltemplate *playerinit_tmpl;
