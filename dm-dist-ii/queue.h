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

#ifndef _MUD_QUEUE_HCC
#define _MUD_QUEUE_HCC

#include "essential.h"

// To use, inherit this type into your data structure.
class cQueueElem
{
   friend class cQueue;

public:
   cQueueElem(char *c, int bCopy = TRUE);
   cQueueElem(ubit8 *d, ubit32 n, int bCopy = TRUE);
   ~cQueueElem()
   {
      if(pData != nullptr)
      {
         free(pData);
      }
   }

   auto Bytes() -> ubit32 const { return nSize; }
   auto Data() -> ubit8 * { return pData; }
   void SetNull()
   {
      pData = nullptr;
      nSize = 0;
   }

   auto PeekNext() -> cQueueElem * { return pNext; }

private:
   ubit8      *pData;
   ubit32      nSize; // Optional number of bytes
   cQueueElem *pNext;
};

class cQueue
{
public:
   cQueue();
   ~cQueue();

   [[nodiscard]] auto IsEmpty() const -> int;
   [[nodiscard]] auto Size() const -> ubit32;
   [[nodiscard]] auto Bytes() const -> ubit32;

   void Copy(ubit8 *data, ubit32 nLen);
   void CutCopy(ubit8 *data, ubit32 nLen);
   void Cut(ubit32 nLen);

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

   ubit32 nEntries;
   ubit32 nBytes;
};

#endif
