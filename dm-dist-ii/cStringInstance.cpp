#include "cStringInstance.h"

#include "cHashSystem.h"
#include "cstring"
#include "textutil.h"

void cStringInstance::Make(const char *str)
{
   if(static_cast<unsigned int>(str_is_empty(str)) != 0U)
   {
      pConst = nullptr;
      return;
   }

   uint32_t h   = Hash.Hash(str);
   uint32_t len = strlen(str);

   pConst = Hash.Lookup(str, len, h);

   if(pConst == nullptr)
   {
      pConst = new cStringConstant(str, len, h);
   }
   else
   {
      pConst->nReferences++;
   }

#if STATISTICS
   ++referenced_strings;
   potential_memory += Length();
#endif
}

void cStringInstance::Reassign(const char *str)
{
   if(pConst != nullptr)
   {
#if STATISTICS
      --referenced_strings;
      potential_memory -= Length();
#endif

      if(--pConst->nReferences == 0)
      {
         delete pConst;
      }
   }

   Make(str);
}

cStringInstance::cStringInstance()
{
   Make(nullptr);
}

cStringInstance::cStringInstance(const char *str)
{
   Make(str);
}

cStringInstance::~cStringInstance()
{
   if(pConst != nullptr)
   {
#if STATISTICS
      --referenced_strings;
      potential_memory -= Length();
#endif

      if(--pConst->nReferences == 0)
      {
         delete pConst;
      }
   }
}
