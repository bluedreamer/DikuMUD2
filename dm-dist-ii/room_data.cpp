#include "room_data.h"
extern int world_norooms;
room_data::room_data()
{
   world_norooms++;
}

room_data::~room_data()
{
   world_norooms--;

   for(int i = 0; i <= 5; i++)
   {
      if(dir_option[i] != nullptr)
      {
         delete dir_option[i];
      }
   }
}
