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


#include <assert.h>
#include <string.h>

#include "queue.h"
#include "utility.h"

cQueueElem::cQueueElem(char *pStr, int bCopy)
{
   assert(pStr);

   ubit32 n = strlen(pStr) + 1;

   if (bCopy)
   {
      CREATE(pData, ubit8, n);
      memcpy(pData, pStr, n);
   }
   else
     pData = (ubit8 *) pStr;

   nSize = n; 
}

cQueueElem::cQueueElem(ubit8 *d, ubit32 n, int bCopy)
{
   if (bCopy)
   {
      if (n > 0)
      {
	 CREATE(pData, ubit8, n);
	 memcpy(pData, d, n);
      }
      else
	pData = NULL;
   }
   else
     pData = d;
   
   nSize = n; 
}

cQueue::cQueue()
{
   pHead    = NULL;
   pTail    = NULL;
   nEntries = 0;
   nBytes   = 0;
}


cQueue::~cQueue()
{
   Flush();
}


ubit32 cQueue::Size(void)
{
   return nEntries;
}


ubit32 cQueue::Bytes(void)
{
   return nBytes;
}



int cQueue::IsEmpty(void)
{
   return nEntries == 0;
}


void cQueue::Copy(ubit8 *data, ubit32 nLen)
{
   assert(nLen <= nBytes);

   int now;
   cQueueElem *qe = (cQueueElem *) PeekHead();

   do
   {
      assert(qe);
      now = MIN(nLen, qe->Bytes());
      memcpy(data, qe->Data(), now);
      nLen -= now;
      data += now;
      qe = qe->PeekNext();
   }
   while (nLen > 0);
}


void cQueue::Cut(ubit32 nLen)
{
   if (nLen < 1)
     return;

   assert(nLen <= nBytes);

   cQueueElem *qe;

   do
   {
      qe = GetHead();

      assert(qe);

      if (nLen < qe->Bytes())
	Prepend(new cQueueElem(qe->Data()+nLen, qe->Bytes()-nLen));

      nLen -= MIN(qe->Bytes(), nLen);
      delete qe;
   }
   while (nLen > 0);
}


void cQueue::CutCopy(ubit8 *data, ubit32 nLen)
{
   Copy(data, nLen);
   Cut(nLen);
}


void cQueue::Append(class cQueueElem *pe)
{
   if (IsEmpty())
   {
      pHead = pTail = pe;
   }
   else
   {
      pTail->pNext  = pe;
      pTail         = pe;
   }

   pe->pNext = NULL;

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
   if (IsEmpty())
   {
      pe->pNext = NULL;
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

const cQueueElem * cQueue::PeekHead(void)
{
   return pHead;
}

const class cQueueElem * cQueue::PeekTail(void)
{
   return pTail;
}

class cQueueElem * cQueue::GetHead(void)
{
   class cQueueElem *pTmp;

   if (IsEmpty())
     return NULL;

   pTmp = pHead;

   if (nEntries == 1)
     pHead = pTail = NULL;
   else
     pHead = pHead->pNext;

   nEntries--;
   nBytes -= pTmp->Bytes();

   return pTmp;
}


void cQueue::Flush(void)
{
   class cQueueElem *pTmp;

   while ((pTmp = GetHead()))
     delete pTmp;
}
