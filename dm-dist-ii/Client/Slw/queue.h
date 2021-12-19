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
   cQueueElem(uint8_t *d, uint32_t n, int bCopy = TRUE);
   ~cQueueElem(void)
   {
      if(pData)
         free(pData);
   }

   uint32_t Bytes(void) { return nSize; }
   uint8_t *Data(void) { return pData; }
   void     SetNull(void)
   {
      pData = NULL;
      nSize = 0;
   }

   cQueueElem *PeekNext(void) { return pNext; }

private:
   uint8_t    *pData;
   uint32_t    nSize; // Optional number of bytes
   cQueueElem *pNext;
};

class cQueue
{
public:
   cQueue();
   ~cQueue();

   int               IsEmpty(void);
   uint32_t          Size(void);
   uint32_t          Bytes(void);

   void              Copy(uint8_t *data, uint32_t nLen);
   void              CutCopy(uint8_t *data, uint32_t nLen);
   void              Cut(uint32_t nLen);

   void              Append(class cQueueElem *pe);
   void              Prepend(class cQueueElem *pe);

   cQueueElem       *GetHead(void);

   const cQueueElem *PeekHead(void);
   const cQueueElem *PeekTail(void);

   void              Flush(void);

private:
   cQueueElem *pHead;
   cQueueElem *pTail;
   cQueueElem *pPreTail;

   uint32_t    nEntries;
   uint32_t    nBytes;
};

#endif
