#pragma once

#include <cstdint>

class cStringConstant
{
   friend class cStringInstance;
   friend class cHashSystem;

   /* None of these definitions ought to be used by anything but the
      friends of this class */

private:
   cStringConstant(const char *c, uint32_t len, uint32_t h);
   ~cStringConstant();
   inline auto      String() -> const char      *{ return pStr; }
   inline auto      Next() -> cStringConstant      *{ return pNext; }

   char            *pStr;
   uint32_t         nReferences;
   cStringConstant *pNext;
   uint32_t         nStrLen;
   // Possibly consider storing the Hash value for speed on destruction
};
