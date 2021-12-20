#include "essential.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

long hours[24];

auto update(struct tm *ptm_on, struct tm *ptm_off) -> int
{
   int secs = 0;

   if(ptm_on->tm_hour != ptm_off->tm_hour)
   {
      secs += (60 - ptm_on->tm_sec);
      secs += 60 * (59 - ptm_on->tm_min);

      hours[ptm_on->tm_hour] += (60 - ptm_on->tm_sec);
      hours[ptm_on->tm_hour] += 60 * (59 - ptm_on->tm_min);

      ptm_on->tm_hour = (ptm_on->tm_hour + 1) % 24;
      ptm_on->tm_min  = 0;
      ptm_on->tm_sec  = 0;

      return secs + update(ptm_on, ptm_off);
   }

   if(ptm_off->tm_sec >= ptm_on->tm_sec)
   {
      secs += (ptm_off->tm_sec - ptm_on->tm_sec);
      secs += 60 * (ptm_off->tm_min - ptm_on->tm_min);

      hours[ptm_on->tm_hour] += (ptm_off->tm_sec - ptm_on->tm_sec);
      hours[ptm_on->tm_hour] += 60 * (ptm_off->tm_min - ptm_on->tm_min);
   }
   else
   {
      secs += (60 - ptm_on->tm_sec) + ptm_off->tm_sec;
      secs += 60 * (ptm_off->tm_min - ptm_on->tm_min - 1);

      hours[ptm_on->tm_hour] += (60 - ptm_on->tm_sec) + ptm_off->tm_sec;
      hours[ptm_on->tm_hour] += 60 * (ptm_off->tm_min - ptm_on->tm_min - 1);
   }

   return secs;
}

void udskriv()
{
   int  i;
   long sum = 0;

   for(i = 0; i < 24; i++)
   {
      printf("O'Clock %2d: %4ld hours\n", i, hours[i] / 3600);
      sum += hours[i];
   }

   printf("\n   Total: %6ld hours\n", sum / 3600);
}

void ShowUsage(int argc, char *argv[])
{
   printf("\nUsage:\n  %s (day <1..366> | week <1..53> | month <1..12>) "
          " [<1990..2020>].\n\n",
          argv[0]);

   printf("Example: cat lib/connections.dat | %s month 2 | less\n\n", argv[0]);

   printf("Copyright (C) 1996 by Valhalla.\n\n");
}

auto is_in(int a, int from, int to) -> int
{
   return static_cast<int>((a >= from) && (a <= to));
}

auto main(int argc, char *argv[]) -> int
{
   int       n;
   int       i;
   uint8_t   Buf[12];
   struct tm tm_on;
   struct tm tm_off;
   int       error = static_cast<int>(FALSE);
   uint32_t  count = 1;

   int day   = -1;
   int week  = -1;
   int month = -1;
   int year  = -1;

   if(argc >= 2)
   {
      if(argc >= 3 && strcmp(argv[1], "day") == 0)
      {
         day = atoi(argv[2]);
         if(is_in(day, 1, 366) == 0)
         {
            error = static_cast<int>(TRUE);
         }
         day--;
      }
      else if(argc >= 3 && strcmp(argv[1], "week") == 0)
      {
         week = atoi(argv[2]);
         if(is_in(week, 1, 53) == 0)
         {
            error = static_cast<int>(TRUE);
         }
      }
      else if(argc >= 3 && strcmp(argv[1], "month") == 0)
      {
         month = atoi(argv[2]);
         if(is_in(month, 1, 12) == 0)
         {
            error = static_cast<int>(TRUE);
         }
         month--;
      }
      else
      {
         error = static_cast<int>(TRUE);
      }

      if(argc > 3)
      {
         year = atoi(argv[3]);
         if(is_in(year, 1990, 2020) == 0)
         {
            error = static_cast<int>(TRUE);
         }
         else
         {
            year -= 1900;
         }
      }

      if(error != 0)
      {
         ShowUsage(argc, argv);
         exit(1);
      }
   }
   else
   {
      printf("Analysing entire database.\n\n");
   }

   for(i = 0; i < 24; i++)
   {
      hours[i] = 0;
   }

   while(feof(stdin) == 0)
   {
      uint32_t id;
      uint32_t on;
      uint32_t off;

      n = 0;
      n += fread(&id, sizeof(uint32_t), 1, stdin);
      n += fread(&on, sizeof(uint32_t), 1, stdin);
      n += fread(&off, sizeof(uint32_t), 1, stdin);

      if(n != 3)
      {
         if(feof(stdin) != 0)
         {
            break;
         }

         printf("Unable to read record!\n");
         exit(1);
      }

      tm_on  = *localtime((time_t *)&on);
      tm_off = *localtime((time_t *)&off);

      tm_on.tm_wday  = (tm_on.tm_yday / 7) + 1; // 1..53
      tm_off.tm_wday = (tm_off.tm_yday / 7) + 1;

      if(((((day != -1) && ((tm_on.tm_yday == day) || (tm_off.tm_yday == day))) ||
           ((week != -1) && ((tm_on.tm_wday == week) || (tm_off.tm_wday == week))) ||
           ((month != -1) && ((tm_on.tm_mon == month) || (tm_off.tm_mon == month)))) &&
          ((year == -1) || (tm_on.tm_year == year) || (tm_off.tm_year == year))) ||
         (day == -1 && week == -1 && month == -1 && year == -1))

      {
         printf("%5d) ID %5d   %2d:%2d:%2d (%2d/%2d) / "
                "%2d:%2d:%2d (%2d/%2d) =",
                count++,
                id,
                tm_on.tm_hour,
                tm_on.tm_min,
                tm_on.tm_sec,
                tm_on.tm_mday,
                tm_on.tm_mon + 1,

                tm_off.tm_hour,
                tm_off.tm_min,
                tm_off.tm_sec,
                tm_off.tm_mday,
                tm_off.tm_mon + 1);

         int secs = update(&tm_on, &tm_off);
         printf("%5d secs\n", secs);
      }
   }

   printf("\n\n");

   udskriv();

   return 0;
}
