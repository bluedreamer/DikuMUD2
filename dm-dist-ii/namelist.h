/* *********************************************************************** *
 * File   : namelist.h                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ Class of the popular Namelist                              *
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

#pragma once

#include "bytestring.h"
#include "hashstring.h"

class cNamelist
{
public:
   cNamelist();
   cNamelist(const char **list);
   ~cNamelist();

   void Free();
   void CopyList(const char *const *list);
   void CopyList(class cNamelist *);

   void AppendBuffer(CByteBuffer *pBuf);
   auto ReadBuffer(CByteBuffer *pBuf) -> int;
   void bread(ubit8 **b);

   void catnames(char *buf);

   void Remove(ubit32 idx);
   void RemoveName(const char *name);

   void Substitute(ubit32 idx, const char *newname);
   auto Name(ubit32 idx = 0) -> const char *;
   auto InstanceName(ubit32 idx = 0) -> cStringInstance *;

   void AppendName(const char *name);
   void PrependName(const char *name);

   auto Duplicate() -> cNamelist *;

   auto IsNameIdx(const char *name) -> const int;
   auto IsNameRawIdx(const char *name) -> const int;
   auto IsNameRaw(const char *name) -> const char *;
   auto IsName(const char *name) -> const char *;

   inline auto Length() -> ubit32 const { return length; }

private:
   class cStringInstance **namelist;
   ubit32                  length;
};
