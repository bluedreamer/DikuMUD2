#include "file_index_type.h"
#include <cstdlib>
file_index_type::file_index_type()
{
   name     = nullptr;
   zone     = nullptr;
   next     = nullptr;
   room_ptr = nullptr;
}

file_index_type::~file_index_type()
{
   if(name != nullptr)
   {
      free(name);
   }
}
