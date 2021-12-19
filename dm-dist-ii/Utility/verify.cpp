#include "essential.h"

#include <cstdio>
#include <ctime>

auto main(int argc, char *argv[]) -> int
{
   char   Buf[1000];
   time_t created;
   time_t lastcrc;
   time_t first_crc;

   char  *msbuf = fgets(Buf, sizeof(Buf), stdin);
   sscanf(Buf, "%08x%08x", (uint32_t *)&created, (uint32_t *)&lastcrc);
   first_crc = created;
   created ^= 0xAF876162;

   printf("Account File Created at [%d] %s", (uint32_t)created, ctime(&created));
   printf("Initial CRC is [%08x]\n", (uint32_t)first_crc);

   while(feof(stdin) == 0)
   {
      char   name1[100];
      char   name2[100];
      char   action;
      int    amount1;
      int    mxor;
      int    gid;
      int    pid;
      int    crc;
      int    amount;
      int    total;
      int    next_crc;
      int    check;
      time_t now;

      char  *msbuf2 = fgets(Buf, sizeof(Buf), stdin);
      if(feof(stdin) != 0)
      {
         break;
      }

      sscanf(Buf,
             "%c %s %s %d %*01x%08x%08x%08x%08x%08x%08x%08x%08x\n",
             &action,
             name1,
             name2,
             &amount1,
             (uint32_t *)&mxor,
             (uint32_t *)&gid,
             (uint32_t *)&crc,
             (uint32_t *)&pid,
             (uint32_t *)&amount,
             (uint32_t *)&total,
             (uint32_t *)&next_crc,
             (uint32_t *)&now);

      check = gid + pid + total + amount + (uint32_t)now;

      mxor  = ~mxor;
      next_crc ^= mxor;

      gid ^= mxor;
      pid ^= (mxor << 1);

      amount ^= mxor << 2;
      amount -= 13;
      total ^= mxor << 3;
      total -= 17;

      crc ^= mxor << 4;

      printf("%c %s(%4d) %s(%4d) %5d [%6d]  %s", action, name1, gid, name2, pid, amount, total, ctime(&now));

      if(amount != amount1)
      {
         printf("Amount mismatch!\n");
      }

      if(crc != check)
      {
         printf("\nCRC mismatch: %08x versus %08x\n", crc, check);
      }

      if(first_crc != next_crc)
      {
         printf(
            "Dependancy check [%08x] [%08x] [%08x] [%08x]!\n", (uint32_t)first_crc, next_crc, (uint32_t)first_crc ^ mxor, next_crc ^ mxor);
      }

      first_crc = next_crc ^ mxor;
   }

   return 0;
}
