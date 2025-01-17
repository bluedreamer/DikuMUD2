#include "blkfile.h"
#include "bytestring.h"
#include "structs.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#undef free

int          sunlight       = 0;
//int8_t time_light[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define TFSIZE 128
#define MAXH   4000
blk_handle h[MAXH];

#ifdef SUSPEKT
FILE *fopen_cache(char *name, char *mode)
{
   static FILE *stream = 0;

   if(stream)
      fclose(stream);

   stream = fopen(name, mode);

   return stream;
}
#endif

auto gen_data(int len) -> char *
{
   uint8_t       *ptr;
   static uint8_t data[4 * TFSIZE];
   static int     count = 0;
   int            i;

   ptr = data;

   for(i = 0; i < 4 * TFSIZE / 4; i++)
   {
      bwrite_uint32_t(&ptr, len++);
   }

   count++;

   return (char *)data;
}

void verify_data(uint8_t *data, int len)
{
   uint8_t *ptr;
   int      i;
   int      j;

   ptr = data;
   j   = len;

   for(i = 0; i < len / 4; i++)
   {
      i = bread_uint32_t(&ptr);
      if(i != j++)
      {
         assert(FALSE);
      }
   }
}

void write_data(BLK_FILE *tf, int no)
{
   int   i;
   char *c;

   for(i = 0; i < no; i++)
   {
      c             = gen_data(1);
      h[i * 10 + 0] = blk_write(tf, c, 1); /* One Block */

      c             = gen_data(TFSIZE - 7);
      h[i * 10 + 1] = blk_write(tf, c, TFSIZE - 7); /* One block */

      c             = gen_data(TFSIZE - 6);
      h[i * 10 + 2] = blk_write(tf, c, TFSIZE - 6); /* One block */

      c             = gen_data(TFSIZE - 5);
      h[i * 10 + 3] = blk_write(tf, c, TFSIZE - 5); /* Two blocks */

      c             = gen_data(TFSIZE - 6 + TFSIZE - 3);
      h[i * 10 + 4] = blk_write(tf, c, TFSIZE - 6 + TFSIZE - 3); /* Two blocks */

      c             = gen_data(TFSIZE - 6 + TFSIZE - 2);
      h[i * 10 + 5] = blk_write(tf, c, TFSIZE - 6 + TFSIZE - 2); /* Two blocks */

      c             = gen_data(TFSIZE - 6 + TFSIZE - 1);
      h[i * 10 + 6] = blk_write(tf, c, TFSIZE - 6 + TFSIZE - 1); /* Three blocks */

      c             = gen_data(TFSIZE - 6 + 2 * (TFSIZE - 2) - 1);
      h[i * 10 + 7] = blk_write(tf, c, TFSIZE - 6 + 2 * (TFSIZE - 2) - 1); /* Three blocks */

      c             = gen_data(TFSIZE - 6 + 2 * (TFSIZE - 2));
      h[i * 10 + 8] = blk_write(tf, c, TFSIZE - 6 + 2 * (TFSIZE - 2)); /* Three blocks */

      c             = gen_data(TFSIZE - 6 + 2 * (TFSIZE - 2) + 1);
      h[i * 10 + 9] = blk_write(tf, c, TFSIZE - 6 + 2 * (TFSIZE - 2) + 1); /* Four blocks */
   }
   /* Each "run" is 1+1+1+2+2+2+3+3+3+4 = 22 records */
   /* A total of 440 (+1) records = 56448 bytes */
}

void new_data(BLK_FILE *tf, int no)
{
   int   i;
   int   j;
   int   l;
   char *c;

   for(j = 0; j < no; j++)
   {
      while(h[i = (rand() % MAXH)] != BLK_NULL)
      {
         ;
      }
      l    = 1 + (rand() % (TFSIZE - 6 + 3 - 1 * (TFSIZE - 2)));
      c    = gen_data(l);
      h[i] = blk_write(tf, c, l); /* One Block */
   }
}

void verify_read(BLK_FILE *tf)
{
   uint8_t   *data;
   int        i;
   blk_length len;

   for(i = 0; i < MAXH; i++)
   {
      if(h[i] != BLK_NULL)
      {
         data = (uint8_t *)blk_read(tf, h[i], &len);
         verify_data(data, len);
      }
   }
}

void fragment_data(BLK_FILE *tf, int no)
{
   int i;
   int j;

   for(j = 0; j < no; j++)
   {
      /* In extreme cases this might cause an endless loop */
      /* but that shouldn't be a problem... :)             */
      while(h[i = (rand() % MAXH)] == BLK_NULL)
      {
         ;
      }
      blk_delete(tf, h[i]);
      h[i] = BLK_NULL;
   }
}

auto main() -> int
{
   FILE     *f;
   BLK_FILE *tf;
   int       i;
   int       j;

   srand(time(nullptr));

   f = fopen("tmp.testfile", "wb");
   fclose(f);

   tf = blk_open("tmp.testfile", TFSIZE);
   for(i = 0; i < MAXH; i++)
   {
      h[i] = BLK_NULL;
   }

   printf("Writing data\n");
   write_data(tf, 50); /* Write 50 * 10 units */
   printf("Verifying data by reading\n");
   verify_read(tf);

   for(i = 0; i < 1000; i++)
   {
      j = 1 + (rand() % 25);
      printf("Fragmenting file by %d records.\n", j);
      fragment_data(tf, j);
      printf("Verifying data by reading\n");
      verify_read(tf);

      j = 1 + (rand() % 35);
      printf("Generating %d new records.\n", j);
      new_data(tf, j);
      printf("Verifying data by reading\n");
      verify_read(tf);

      if((rand() % 10) == 1)
      {
         blk_close(tf);
         printf("\nRe-indexing blockfile\n");
         tf = blk_open("tmp.testfile", TFSIZE);
      }
   }

   return 0;
}

void destroy_fptr(unit_data *u, unit_fptr *f)
{
}

void unlink_affect(struct unit_data *u, unit_affected_type *af)
{
}
