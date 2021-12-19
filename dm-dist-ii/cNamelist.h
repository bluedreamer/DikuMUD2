#pragma once

#include "CByteBuffer.h"
#include "cStringInstance.h"

#include <cstdint>

class cNamelist
{
public:
   cNamelist();
   cNamelist(const char **list);
   ~cNamelist();

   void        Free();
   void        CopyList(const char *const *list);
   void        CopyList(cNamelist *);

   void        AppendBuffer(CByteBuffer *pBuf);
   auto        ReadBuffer(CByteBuffer *pBuf) -> int;
   void        bread(uint8_t **b);

   void        catnames(char *buf);

   void        Remove(uint32_t idx);
   void        RemoveName(const char *name);

   void        Substitute(uint32_t idx, const char *newname);
   auto        Name(uint32_t idx = 0) -> const char *;
   auto        InstanceName(uint32_t idx = 0) -> cStringInstance *;

   void        AppendName(const char *name);
   void        PrependName(const char *name);

   auto        Duplicate() -> cNamelist *;

   auto        IsNameIdx(const char *name) -> const int;
   auto        IsNameRawIdx(const char *name) -> const int;
   auto        IsNameRaw(const char *name) -> const char *;
   auto        IsName(const char *name) -> const char *;

   inline auto Length() const -> uint32_t const { return length; }

private:
   class cStringInstance **namelist;
   uint32_t                length;
};
