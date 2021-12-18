/* *********************************************************************** *
 * File   : hashstring.h                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ Public Headers for the String Reuse system.                *
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

#ifndef _MUD_STRING_H
#define _MUD_STRING_H

#include "essential.h"

void string_statistics(struct unit_data *ch);

class cStringConstant
{
   friend class cStringInstance;
   friend class cHashSystem;

   /* None of these definitions ought to be used by anything but the
      friends of this class */

private:
   cStringConstant(const char *c, uint32_t len, uint32_t h);
   ~cStringConstant();
   inline auto String() -> const char * { return pStr; }
   inline auto Next() -> cStringConstant * { return pNext; }

   char                  *pStr;
   uint32_t                 nReferences;
   class cStringConstant *pNext;
   uint32_t                 nStrLen;
   // Possibly consider storing the Hash value for speed on destruction
};

class cStringInstance
{
public:
   cStringInstance();
   cStringInstance(const char *str);
   ~cStringInstance();
   inline auto StringPtr() -> const char * { return pConst != nullptr ? pConst->pStr : nullptr; }
   inline auto String() -> const char * { return pConst != nullptr ? pConst->pStr : ""; }
   void        Reassign(const char *c);
   inline auto Length() -> const uint32_t { return pConst != nullptr ? pConst->nStrLen : 0; }

private:
   void                   Make(const char *str);
   class cStringConstant *pConst;
};

#endif
