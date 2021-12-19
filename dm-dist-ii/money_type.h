#pragma once

#include "file_index_type.h"
#include <cstdint>

using currency_t = int16_t;

struct money_type
{
   currency_t       currency;         /* Which currency for money */
   char           **strings;          /* The money-strings */
   char            *tails;            /* What to print on the dark side of the coin */
   char            *abbrev;           /* Small string for lists */
   uint8_t          pl_idx;           /* The index in above to first plural string */
   int32_t          relative_value;   /* Relative to the internal value */
   int32_t          min_value;        /* Minimum internal value of the currency */
   uint8_t          coins_per_weight; /* How many coins per weight unit */
   file_index_type *fi;               /* Where is coin object in file */
};

/* You shouldn't have to refer to this array yourself.
 * Use the macros instead.
 */
extern money_type money_types[];
