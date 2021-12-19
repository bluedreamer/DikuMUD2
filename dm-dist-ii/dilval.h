#pragma once

#include "dil.h"
#include <cstdint>

/* DIL evaluation result. */
class dilval
{
public:
   dilval() { type = DILV_FAIL; }
   ~dilval();

   uint8_t type; /* result type     */
   uint8_t atyp; /* allocation type */
   union
   {
      void   *ptr; /* result pointer  */
      int64_t num; /* result integer  MS 2020 (32->64 bit)*/
   } val;
   void *ref; /* result reference (NULL=Rexpr) */
};
