/* *********************************************************************** *
 * File   : queue.cc                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ FIFO List Queue                                            *
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

/*
   I will make a GetTail operation if / when it is needed. In such case,
   the linked list should be a doubly linked list for speed.
*/

#include <cassert>
#include <cstring>

#include "queue.h"
#include "utility.h"

cQueueElem::cQueueElem(char *pStr, int bCopy)
{
   assert(pStr);

   ubit32 n = strlen(pStr) + 1;

   if(bCopy != 0)
   {
      CREATE(pData, ubit8, n);
      memcpy(pData, pStr, n);
   }
   else
   {
      pData = (ubit8 *)pStr;
   }

   nSize = n;
}

cQueueElem::cQueueElem(ubit8 *d, ubit32 n, int bCopy)
{
   if(bCopy != 0)
   {
      if(n > 0)
      {
         CREATE(pData, ubit8, n);
         memcpy(pData, d, n);
      }
      else
      {
         pData = nullptr;
      }
   }
   else
   {
      pData = d;
   }

   nSize = n;
}

cQueue::cQueue()
{
   pHead    = nullptr;
   pTail    = nullptr;
   nEntries = 0;
   nBytes   = 0;
}

cQueue::~cQueue()
{
   Flush();
}

auto cQueue::Size() const -> ubit32
{
   return nEntries;
}

auto cQueue::Bytes() const -> ubit32
{
   return nBytes;
}

auto cQueue::IsEmpty() const -> int
{
   return static_cast<int>(nEntries == 0);
}

void cQueue::Copy(ubit8 *data, ubit32 nLen)
{
   assert(nLen <= nBytes);

   int   now;
   auto *qe = (cQueueElem *)PeekHead();

   do
   {
      assert(qe);
      now = MIN(nLen, qe->Bytes());
      memcpy(data, qe->Data(), now);
      nLen -= now;
      data += now;
      qe = qe->PeekNext();
   } while(nLen > 0);
}

void cQueue::Cut(ubit32 nLen)
{
   if(nLen < 1)
   {
      return;
   }

   assert(nLen <= nBytes);

   cQueueElem *qe;

   do
   {
      qe = GetHead();

      assert(qe);

      if(nLen < qe->Bytes())
      {
         Prepend(new cQueueElem(qe->Data() + nLen, qe->Bytes() - nLen));
      }

      nLen -= MIN(qe->Bytes(), nLen);
      delete qe;
   } while(nLen > 0);
}

void cQueue::CutCopy(ubit8 *data, ubit32 nLen)
{
   Copy(data, nLen);
   Cut(nLen);
}

void cQueue::Append(class cQueueElem *pe)
{
   if(IsEmpty() != 0)
   {
      pHead = pTail = pe;
   }
   else
   {
      pTail->pNext = pe;
      pTail        = pe;
   }

   pe->pNext = nullptr;

   nEntries++;
   nBytes += pe->Bytes();
}

/*  Quinn's Immortal Remark...
 *
 *  Listen...
 *  The sound of a thousand comp.sci. professors rotating in their graves.
 */
void cQueue::Prepend(class cQueueElem *pe)
{
   if(IsEmpty() != 0)
   {
      pe->pNext = nullptr;
      pTail     = pe;
   }
   else
   {
      pe->pNext = pHead;
   }

   pHead = pe;

   nEntries++;
   nBytes += pe->Bytes();
}

auto cQueue::PeekHead() -> const cQueueElem *
{
   return pHead;
}

auto cQueue::PeekTail() -> const class cQueueElem *
{
   return pTail;
}

auto cQueue::GetHead() -> class cQueueElem *
{
   class cQueueElem *pTmp;

   if(IsEmpty() != 0)
   {
      return nullptr;
   }

   pTmp = pHead;

   if(nEntries == 1)
   {
      pHead = pTail = nullptr;
   }
   else
   {
      pHead = pHead->pNext;
   }

   nEntries--;
   nBytes -= pTmp->Bytes();

   return pTmp;
}

void cQueue::Flush()
{
   class cQueueElem *pTmp;

   while((pTmp = GetHead()) != nullptr)
   {
      delete pTmp;
   }
}
