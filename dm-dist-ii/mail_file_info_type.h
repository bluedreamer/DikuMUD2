#pragma once

#include "blk_file_type.h"

#include <cstdint>
#include <ctime>

class mail_file_info_type
{
public:
   time_t     date; /* Date of posting the letter      */
   blk_handle handle;
   int        sender;     /* PC ID */
   int        receipient; /* PC ID */
   uint8_t    loaded;     /* Boolean, has player got a note? */
};
