#define BLOCKSIZE = 256000 /* nothing bigger than this please */

void *blkalloc_start = nullptr;
void *blkalloc_end   = nullptr;

auto  blkalloc(size_t size) -> void *
{
   void *res = nullptr;

   if(size >= BLOCKSIZE)
      return (malloc(size));

   if(blkalloc_start + size >= blkalloc_end)
   {
      blkalloc_start = malloc(BLOCKSIZE);
      blkalloc_end   = blkalloc_start + BLOCKSIZE;
   }

   res = blkalloc_start;
   blkalloc_start += size;
   return res;
}
