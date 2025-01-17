#include "blkfile.h"

#include "files.h"
#include "textutil.h"
#include "utility.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#define BLK_RESERVED 0 /* Reserved block for special exceptions */
#define BLK_FREE     -1
#define BLK_END      -2
/* BLK_NULL == -3 in blkfile.h */
#define BLK_START_V1 -4 /* This is the start of a new block, used */
                        /* for integrity checking and versioning. */

#define BLK_MIN_SIZE (blk_length)(2 * (2 * sizeof(blk_handle) + sizeof(blk_length)))

/*
   Format of blocked file:

   <blk_handle next_block>
   if start of data then <blk_length length>
   <data>                data[]

*/

/*
   I could use the first free block as start of a linked list
   of free blocks. But this would require quite a lot of reading
   and writing of blocks when writing blocks (in order to find
   the next free block). But it would save memory (just a little),
   and it wouldn't be nessecary to create the list when the
   blocked file is opened
*/

static auto blk_fsize(BLK_FILE *bf) -> blk_handle
{
   return fsize(bf->f) / bf->bsize;
}

static void blk_read_block(BLK_FILE *bf, blk_handle index)
{
   long p;

   p = index * bf->bsize;

   if(fseek(bf->f, p, SEEK_SET) != 0)
   {
      error(HERE, "%s: Unable to seek.", bf->name);
   }

   if(fread(bf->buf, bf->bsize, 1, bf->f) != 1)
   {
      error(HERE, "%s: Unable to read.", bf->name);
   }
}

static void blk_write_block(BLK_FILE *bf, blk_handle index)
{
   if(fseek(bf->f, index * bf->bsize, SEEK_SET) != 0)
   {
      error(HERE, "%s: Unable to seek.", bf->name);
   }

   if(fwrite(bf->buf, bf->bsize, 1, bf->f) != 1)
   {
      error(HERE, "%s: Unable to write.", bf->name);
   }
}

static void register_free_block(BLK_FILE *bf, blk_handle idx)
{
   int i;

   bf->listtop++;

   if(bf->listtop > bf->listmax)
   {
      if(bf->listtop <= 1)
      {
         CREATE(bf->list, blk_handle, bf->listtop);
      }
      else
      {
         RECREATE(bf->list, blk_handle, bf->listtop);
      }
      bf->listmax = bf->listtop;
   }

   bf->list[bf->listtop - 1] = idx;

   /* Insert the block in descending order so we get a nice and  */
   /* straight FORWARD writing of blocks (may help w/ readahead) */
   for(i = 0; i < bf->listtop - 1; i++)
   {
      if(bf->list[i] < idx)
      {
         memmove(&bf->list[i + 1], &bf->list[i], (bf->listtop - i - 1) * sizeof(blk_handle));
         bf->list[i] = idx;
         break;
      }
   }
}

/* Returns a free block and occupies it at the same time. */
static auto find_occupy_free_blk(BLK_FILE *bf) -> blk_handle
{
   uint8_t *b;

   if(bf->listtop > 0) /* We got a free space in current list */
   {
      bf->listtop--;

      if(bf->list[bf->listtop] == BLK_RESERVED)
      {
         error(HERE, "%s: Reserved block mess.", bf->name);
      }

      return bf->list[bf->listtop];
   }

   if(bf->blktop == BLK_RESERVED)
   {
      b = (uint8_t *)bf->buf;
      bwrite_uint16_t(&b, (uint16_t)BLK_FREE);
      blk_write_block(bf, BLK_RESERVED);

      bf->blktop++; /* Skip the reserved block */
   }

   return bf->blktop++;
}

/* Copy data starting at blk_start into data, and return pointer */
/* to immediate next free space in data.                         */
static auto blk_extract_data(BLK_FILE *bf, void *blk_start, void *data, blk_length *len) -> void *
{
   int no;
   int used;

   if(*len == 0)
   {
      return nullptr;
   }

   used = (uint8_t *)blk_start - (uint8_t *)bf->buf;
   no   = bf->bsize - used;

   no   = std::min(*len, no); /* Make sure we don't copy too much */

   if(no < 0)
   {
      error(HERE, "%s: Illegal length.", bf->name);
   }

   if(no == 0)
   {
      return data;
   }

   memcpy(data, blk_start, no);

   *len -= no; /* We've not got len-no left to copy */

   return (uint8_t *)data + no;
}

/* Copy from data into blk_start the apropriate number of bytes */
/* Returns pointer just past last copied element from data      */
static auto blk_put_data(BLK_FILE *bf, void *blk_start, const void *data) -> const void *
{
   int no;
   int used;

   used = (uint8_t *)blk_start - (uint8_t *)bf->buf;
   no   = bf->bsize - used;

   if(no < 0)
   {
      error(HERE, "%s: Illegal length.", bf->name);
   }

   if(no == 0)
   {
      return data;
   }

   memcpy(blk_start, data, no); /* Copy data */

   return (uint8_t *)data + no;
}

/* Delete message starting at index "index" */
void blk_delete(BLK_FILE *bf, blk_handle index)
{
   blk_handle next_block;
   int        first;
   uint8_t   *b;

   if((bf->f = fopen_cache(bf->name, "r+b")) == nullptr)
   {
      error(HERE, "%s: Cache open.", bf->name);
   }

   first = static_cast<int>(TRUE);

   do
   {
      blk_read_block(bf, index); /* Read the block to find next block */

      b          = (uint8_t *)bf->buf;

      next_block = (blk_handle)bread_uint16_t(&b);

      if(next_block == BLK_FREE)
      {
         slog(LOG_ALL,
              0,
              "BLK_DELETE: Attempt to access free disk "
              "or mem block!");
         return;
      }

      if(first != 0)
      {
         if(next_block != BLK_START_V1)
         {
            slog(LOG_ALL,
                 0,
                 "BLK_DELETE: Illegal delete handle "
                 "in '%s'",
                 bf->name);
            return;
         }
         next_block = (blk_handle)bread_uint16_t(&b);
         first      = static_cast<int>(FALSE);
      }

      b = (uint8_t *)bf->buf;
      bwrite_uint16_t(&b, (uint16_t)BLK_FREE);

      blk_write_block(bf, index); /* Save on disk                      */

      register_free_block(bf, index);

      index = next_block;
   } while(index != BLK_END);

   if(fflush(bf->f) != 0)
   {
      error(HERE, "%s: Flush", bf->name);
   }
}

/* Read message starting at index "index", and return pointer to the */
/* result buffer, if given pointer to a blk_length type, save the    */
/* length of the buffer in this variable                             */
/* Returns NULL if error                                             */
auto blk_read(BLK_FILE *bf, blk_handle index, blk_length *blen) -> void *
{
   void      *blk_ptr;
   void      *data  = nullptr;
   void      *odata = nullptr;
   blk_handle next_block;
   blk_handle oindex;
   blk_length len = 0;

   if((bf->f = fopen_cache(bf->name, "r+b")) == nullptr)
   {
      error(HERE, "%s: Cache open.", bf->name);
   }

   oindex = index;
   do
   {
      blk_read_block(bf, index); /* Read the block into bf->buf */

      blk_ptr    = bf->buf;
      next_block = bread_uint16_t((uint8_t **)&blk_ptr);

      if(next_block == BLK_FREE)
      {
         slog(LOG_ALL,
              0,
              "BLK_READ: Attempt to access free "
              "block in '%s'",
              bf->name);
         if(blen != nullptr)
         {
            *blen = 0;
         }
         return nullptr;
      }

      if(oindex == index)
      {
         if(next_block != BLK_START_V1)
         {
            slog(LOG_ALL,
                 0,
                 "BLK_READ: Illegal read handle "
                 "in '%s'.",
                 bf->name);
            if(blen != nullptr)
            {
               *blen = 0;
            }
            return nullptr;
         }

         /* This was the first block, now read the real next_block */
         next_block = bread_uint16_t((uint8_t **)&blk_ptr);

         len        = bread_uint32_t((uint8_t **)&blk_ptr);
         if(len > 0)
         {
            CREATE(data, uint8_t, len); /* Alloc space for the buffer */
         }
         else
         {
            data = nullptr;
         }
         odata = data;
         if(blen != nullptr)
         {
            *blen = len;
         }
      }

      data  = blk_extract_data(bf, blk_ptr, data, &len);

      index = next_block;
   } while(index != BLK_END);

   return odata;
}

/* Save the letter pointed to by str in the blk file */
/* and return a handle to the message                */
auto blk_write(BLK_FILE *bf, const void *data, blk_length len) -> blk_handle
{
   blk_handle  index;
   blk_handle  next_block;
   blk_handle  first_block;
   const void *org;
   void       *blk_ptr;

   if((bf->f = fopen_cache(bf->name, "r+b")) == nullptr)
   {
      error(HERE, "%s: Cache open.", bf->name);
   }

   if(len == 0)
   {
      slog(LOG_ALL, 0, "Zero length save to file %s", bf->name);
   }

   first_block = index = find_occupy_free_blk(bf);

   org                 = data;

   do
   {
      blk_ptr = bf->buf;

      if(first_block == index)
      {
         bread_uint16_t((uint8_t **)&blk_ptr);
      }

      bread_uint16_t((uint8_t **)&blk_ptr);

      if(first_block == index)
      {
         bread_uint32_t((uint8_t **)&blk_ptr);
      }

      data = blk_put_data(bf, blk_ptr, data);

      if((uint8_t *)data - (uint8_t *)org < len)
      {
         next_block = find_occupy_free_blk(bf);
      }
      else
      {
         next_block = BLK_END;
      }

      blk_ptr = bf->buf;

      if(first_block == index)
      {
         bwrite_uint16_t((uint8_t **)&blk_ptr, (uint16_t)BLK_START_V1);
      }

      bwrite_uint16_t((uint8_t **)&blk_ptr, (uint16_t)next_block);

      if(first_block == index)
      {
         bwrite_uint32_t((uint8_t **)&blk_ptr, (uint32_t)len);
      }

      blk_write_block(bf, index);
      index = next_block;
   } while(index != BLK_END);

   if(fflush(bf->f) != 0)
   {
      error(HERE, "%s: Flush", bf->name);
   }

   return first_block;
}

void blk_write_reserved(BLK_FILE *bf, const void *data, blk_length len)
{
   blk_handle i;
   uint8_t   *b;

   blk_read_block(bf, BLK_RESERVED);
   b = (uint8_t *)bf->buf;
   i = bread_uint16_t(&b);

   if(i != BLK_FREE)
   {
      blk_delete(bf, i);
   }

   i = blk_write(bf, data, len);

   b = (uint8_t *)bf->buf;
   bwrite_uint16_t(&b, i);
   blk_write_block(bf, BLK_RESERVED);
}

auto blk_read_reserved(BLK_FILE *bf, blk_length *blen) -> void *
{
   blk_handle i;
   uint8_t   *b;

   if(bf->blktop == 0)
   {
      if(blen != nullptr)
      {
         *blen = 0;
      }
      return nullptr;
   }

   blk_read_block(bf, BLK_RESERVED);
   b = (uint8_t *)bf->buf;
   i = bread_uint16_t(&b);

   if(i == BLK_FREE)
   {
      if(blen != nullptr)
      {
         *blen = 0;
      }
      return nullptr;
   }

   return blk_read(bf, i, blen);
}

/* block size is the number of bytes contained in each block. Try 128+ */
auto blk_open(const char *name, blk_length block_size) -> BLK_FILE *
{
   blk_handle index;

   BLK_FILE  *bf;

   if(block_size < BLK_MIN_SIZE)
   {
      error(HERE, "%s: Illegal block size.", name);
   }

   CREATE(bf, BLK_FILE, 1);
   bf->name    = str_dup(name);
   bf->listtop = 0;
   bf->listmax = 0;
   bf->blktop  = 0;
   bf->list    = nullptr;
   bf->bsize   = block_size;
   bf->f       = nullptr;
   CREATE(bf->buf, uint8_t, bf->bsize);

   if((bf->f = fopen_cache(bf->name, "r+b")) == nullptr)
   {
      error(HERE, "%s: Cache open.", bf->name);
   }

   bf->blktop = blk_fsize(bf);

   for(index = 0;; index++)
   {
      if(fread(bf->buf, bf->bsize, 1, bf->f) != 1)
      {
         break;
      }

      if(index >= bf->blktop)
      {
         bf->blktop = index + 1;
         slog(LOG_ALL, 0, "Apparently blk_fsize doesn't work!");
      }

      if((*((blk_handle *)bf->buf) == BLK_FREE) && (index != BLK_RESERVED))
      {
         register_free_block(bf, index);
      }
   }

   return bf;
}

/* I only put this routine here since it makes the whole thing seem more */
/* complete. It will probably never be called, since there is no need to */
/* close a blocked file (it uses no resources other than a little mem).  */
void blk_close(BLK_FILE *bf)
{
   free(bf->name);
   free(bf->buf);
   if(bf->list != nullptr)
   {
      free(bf->list);
   }
   free(bf);
}
