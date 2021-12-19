#include "char_data.h"

#include <cstring>
extern int world_nochars;

char_data::char_data()
{
   world_nochars++;

   specific.pc = nullptr;
   money       = nullptr;
   descriptor  = nullptr;
   Combat      = nullptr;
   followers   = nullptr;
   master      = nullptr;
   last_room   = nullptr;

   memset(&points, 0, sizeof(points));
}

char_data::~char_data()
{
   world_nochars--;
}
