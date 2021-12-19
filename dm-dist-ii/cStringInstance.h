#pragma once

#include "cStringConstant.h"
#include <cstdint>

class cStringInstance
{
public:
   cStringInstance();
   cStringInstance(const char *str);
   ~cStringInstance();
   inline auto StringPtr() -> const char * { return pConst != nullptr ? pConst->pStr : nullptr; }
   inline auto String() -> const char * { return pConst != nullptr ? pConst->pStr : ""; }
   void        Reassign(const char *c);
   inline auto Length() -> const uint32_t { return pConst != nullptr ? pConst->nStrLen : 0; }

private:
   void                   Make(const char *str);
   cStringConstant *pConst;
};
