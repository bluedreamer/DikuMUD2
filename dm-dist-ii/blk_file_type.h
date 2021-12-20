#pragma once
#include <cstdint>
#include <cstdio>

using blk_handle = int16_t;
using blk_length = int32_t;

class blk_file_type
{
public:
   FILE       *f;       /* Pointer to file, closed after each call   */
   char       *name;    /* name of the file to contain blocks        */
   void       *buf;     /* Buffer to store blocks read from *f       */
   blk_handle *list;    /* Memory list of free indexes               */
   blk_handle  listtop; /* Top of memory list                        */
   blk_handle  listmax; /* Number of alloced elements                */
   blk_handle  blktop;  /* The maximum block number                  */
   blk_length  bsize;   /* The size of each block in the file        */
};
