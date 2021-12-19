#pragma once
#include "file_index_type.h"
#include <cstdint>

/* A linked list of commands to execute */
struct zone_reset_cmd
{
   uint8_t cmd_no; /* Index to array of func() ptrs */
   uint8_t cmpl;   /* Complete flag                 */

   file_index_type *fi[2];
   int16_t          num[3];

   zone_reset_cmd *next;
   zone_reset_cmd *nested;
};
