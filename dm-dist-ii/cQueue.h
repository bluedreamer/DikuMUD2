#pragma once

#include "cQueueElem.h"
#include <cstdint>

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

   void Append(cQueueElem *pe);
   void Prepend(cQueueElem *pe);

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
