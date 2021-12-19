#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

long hours[24];

void update(struct tm *ptm_on, struct tm *ptm_off)
{
   if(ptm_on->tm_hour != ptm_off->tm_hour)
   {
      hours[ptm_on->tm_hour] += (60 - ptm_on->tm_sec);
      hours[ptm_on->tm_hour] += 60 * (59 - ptm_on->tm_min);

      ptm_on->tm_hour = (ptm_on->tm_hour + 1) % 24;
      ptm_on->tm_min  = 0;
      ptm_on->tm_sec  = 0;

      update(ptm_on, ptm_off);
   }
   else
   {
      if(ptm_off->tm_sec >= ptm_on->tm_sec)
      {
         hours[ptm_on->tm_hour] += (ptm_off->tm_sec - ptm_on->tm_sec);
         hours[ptm_on->tm_hour] += 60 * (ptm_off->tm_min - ptm_on->tm_min);
      }
      else
      {
         hours[ptm_on->tm_hour] += (60 - ptm_on->tm_sec) + ptm_off->tm_sec;
         hours[ptm_on->tm_hour] += 60 * (ptm_off->tm_min - ptm_on->tm_min - 1);
      }
   }
}

void udskriv()
{
   int  i;
   long sum = 0;

   for(i = 0; i < 24; i++)
   {
      printf("Klokken %2d: %4ld timer\n", i, hours[i] / 3600);
      sum += hours[i];
   }

   printf("Timer ialt: %ld\n", sum / 3600);
}

auto main(int argc, char *argv[]) -> int
{
   int       year  = -1;
   int       month = -1;
   int       n;
   int       i;
   time_t    on;
   time_t    off;
   char      Buf[200];
   struct tm tm_on;
   struct tm tm_off;

   if(argc > 1)
   {
      if(argc != 3)
      {
         printf("Syntax is <month [0..11]> <year [00..99]>\n");
         exit(1);
      }

      month = atoi(argv[1]);
      if((month < 0) || (month > 11))
      {
         printf("Argument 1 must be a month [0..11].\n");
         exit(1);
      }

      year = atoi(argv[2]);
      if((year < 0) || (year > 99))
      {
         printf("Argument 2 must be a year [00..99].\n");
         exit(1);
      }
   }

   for(i = 0; i < 24; i++)
   {
      hours[i] = 0;
   }

   while(feof(stdin) == 0)
   {
      char *msbuf = fgets(Buf, sizeof(Buf), stdin);
      n           = sscanf(Buf, "Connected %ld - %ld disconnected.", &on, &off);
      if(n == 2)
      {
         tm_on  = *localtime(&on);
         tm_off = *localtime(&off);

         if(((month == -1) || (tm_on.tm_mon == month) || (tm_off.tm_mon == month)) && ((year == -1) || (tm_on.tm_year == year)))

         {
            printf("%2d:%2d:%2d (%d/%d/%d) / %2d:%2d:%2d (%d/%d)\n",
                   tm_on.tm_hour,
                   tm_on.tm_min,
                   tm_on.tm_sec,
                   tm_on.tm_mday,
                   tm_on.tm_mon,
                   tm_on.tm_year,

                   tm_off.tm_hour,
                   tm_off.tm_min,
                   tm_off.tm_sec,
                   tm_off.tm_mday,
                   tm_off.tm_mon);

            update(&tm_on, &tm_off);
         }
      }
   }

   udskriv();

   return 0;
}
