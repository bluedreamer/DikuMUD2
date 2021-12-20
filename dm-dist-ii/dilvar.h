#pragma once

#include "unit_data.h"

#include <cstdint>

/* DIL variable structure */
class dilvar
{
public:
   uint8_t type; /* variable type */
   union
   {
      unit_data        *unitptr;
      int32_t           integer;
      extra_descr_data *extraptr;
      char             *string;
      cStringInstance  *pHash;
      cNamelist        *namelist;
   } val;
};
