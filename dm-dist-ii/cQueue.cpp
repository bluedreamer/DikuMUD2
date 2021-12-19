#include <algorithm>
#include <cstring>
#include "cQueue.h"
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

auto cQueue::Size() const -> uint32_t
{
   return nEntries;
}

auto cQueue::Bytes() const -> uint32_t
{
   return nBytes;
}

auto cQueue::IsEmpty() const -> int
{
   return static_cast<int>(nEntries == 0);
}

void cQueue::Copy(uint8_t *data, uint32_t nLen)
{
   assert(nLen <= nBytes);

   int   now;
   auto *qe = (cQueueElem *)PeekHead();

   do
   {
      assert(qe);
      now = std::min(nLen, qe->Bytes());
      memcpy(data, qe->Data(), now);
      nLen -= now;
      data += now;
      qe = qe->PeekNext();
   } while(nLen > 0);
}

void cQueue::Cut(uint32_t nLen)
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

      nLen -= std::min(qe->Bytes(), nLen);
      delete qe;
   } while(nLen > 0);
}

void cQueue::CutCopy(uint8_t *data, uint32_t nLen)
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
