#pragma once
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
#include "CByteBuffer.h"
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
   void bread(uint8_t **b);

   void catnames(char *buf);

   void Remove(uint32_t idx);
   void RemoveName(const char *name);

   void Substitute(uint32_t idx, const char *newname);
   auto Name(uint32_t idx = 0) -> const char *;
   auto InstanceName(uint32_t idx = 0) -> cStringInstance *;

   void AppendName(const char *name);
   void PrependName(const char *name);

   auto Duplicate() -> cNamelist *;

   auto IsNameIdx(const char *name) -> const int;
   auto IsNameRawIdx(const char *name) -> const int;
   auto IsNameRaw(const char *name) -> const char *;
   auto IsName(const char *name) -> const char *;

   inline auto Length() -> uint32_t const { return length; }

private:
   class cStringInstance **namelist;
   uint32_t                length;
};
