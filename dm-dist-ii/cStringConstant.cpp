#include <cassert>
#include "textutil.h"
#include "essential.h"
#include "cStringConstant.h"
#include "cHashSystem.h"

cStringConstant::cStringConstant(const char *str, uint32_t len, uint32_t nHash)
{
   class cStringConstant *tmp;

   assert(str);

   if((tmp = Hash.Lookup(str, len, nHash)) != nullptr)
   {
      tmp->nReferences++;
   }
   else
   {
      nReferences = 1;
      pStr        = str_dup(str);
      nStrLen     = len;

      Hash.Insert(this, nHash);

#if STATISTICS
      ++alloced_strings;
      alloced_memory += nStrLen;
#endif
   }
}

cStringConstant::~cStringConstant()
{
   Hash.Remove(this);
   free(pStr);

#if STATISTICS
   --alloced_strings;
   alloced_memory -= nStrLen;
#endif
}
