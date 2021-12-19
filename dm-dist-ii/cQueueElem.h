#pragma once

#include "essential.h"

#include <cstdint>

class cQueueElem
{
   friend class cQueue;

public:
   cQueueElem(char *c, bool bCopy = true);
   cQueueElem(uint8_t *d, uint32_t n, bool bCopy = true);
   ~cQueueElem()
   {
      if(pData != nullptr)
      {
         free(pData);
      }
   }

   auto Bytes() const -> uint32_t const { return nSize; }
   auto Data() -> uint8_t * { return pData; }
   void SetNull()
   {
      pData = nullptr;
      nSize = 0;
   }

   auto PeekNext() -> cQueueElem * { return pNext; }

private:
   uint8_t    *pData;
   uint32_t    nSize; // Optional number of bytes
   cQueueElem *pNext;
};
