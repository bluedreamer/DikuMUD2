#pragma once
/* *********************************************************************** *
 * File   : queue.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Queue in c++                                                   *
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

#include "essential.h"

// To use, inherit this type into your data structure.
class cQueueElem
{
   friend class cQueue;

public:
   cQueueElem(char *c, int bCopy = TRUE);
   cQueueElem(uint8_t *d, uint32_t n, int bCopy = TRUE);
   ~cQueueElem()
   {
      if(pData != nullptr)
      {
         free(pData);
      }
   }

   auto Bytes() -> uint32_t const { return nSize; }
   auto Data() -> uint8_t * { return pData; }
   void SetNull()
   {
      pData = nullptr;
      nSize = 0;
   }

   auto PeekNext() -> cQueueElem * { return pNext; }

private:
   uint8_t      *pData;
   uint32_t      nSize; // Optional number of bytes
   cQueueElem *pNext;
};

class cQueue
{
public:
   cQueue();
   ~cQueue();

   [[nodiscard]] auto IsEmpty() const -> int;
   [[nodiscard]] auto Size() const -> uint32_t;
   [[nodiscard]] auto Bytes() const -> uint32_t;

   void Copy(uint8_t *data, uint32_t nLen);
   void CutCopy(uint8_t *data, uint32_t nLen);
   void Cut(uint32_t nLen);

   void Append(class cQueueElem *pe);
   void Prepend(class cQueueElem *pe);

   auto GetHead() -> cQueueElem *;

   auto PeekHead() -> const cQueueElem *;
   auto PeekTail() -> const cQueueElem *;

   void Flush();

private:
   cQueueElem *pHead;
   cQueueElem *pTail;
   cQueueElem *pPreTail;

   uint32_t nEntries;
   uint32_t nBytes;
};
