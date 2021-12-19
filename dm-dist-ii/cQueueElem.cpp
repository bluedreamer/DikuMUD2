#include "cQueueElem.h"

#include <cassert>
#include <cstring>

cQueueElem::cQueueElem(char *c, bool bCopy)
{
   assert(c);

   uint32_t n = strlen(c) + 1;

   if(bCopy != 0)
   {
      CREATE(pData, uint8_t, n);
      memcpy(pData, c, n);
   }
   else
   {
      pData = (uint8_t *)c;
   }

   nSize = n;
}

cQueueElem::cQueueElem(uint8_t *d, uint32_t n, bool bCopy)
{
   if(bCopy != 0)
   {
      if(n > 0)
      {
         CREATE(pData, uint8_t, n);
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
