#include <cstdio>
#include <cstdlib>
#include <ctime>

auto main(int argc, char *argv[]) -> int
{
   int       month = -1;
   int       n;
   int       operation;
   int       drunk = 0;
   time_t    when;
   struct tm tm_when;
   char      Buf[200];
   char      pName[200];

   if(argc > 1)
   {
      month = atoi(argv[1]);
      if((month < 1) || (month > 12))
      {
         printf("Argument 1 must be an optional month [1..12].\n");
         exit(1);
      }

      month--;
   }

   while(feof(stdin) == 0)
   {
      char *msbuf = fgets(Buf, sizeof(Buf), stdin);
      n           = sscanf(Buf, "TUBORG %s %d %ld", pName, &operation, &when);

      if(n == 3)
      {
         tm_when = *localtime(&when);

         if((month == -1) || (tm_when.tm_mon == month))
         {
            drunk++;
            printf("%s %2d:%2d:%2d (%d/%d) year %d\n",
                   pName,
                   tm_when.tm_hour,
                   tm_when.tm_min,
                   tm_when.tm_sec,
                   1 + tm_when.tm_mday,
                   1 + tm_when.tm_mon,
                   tm_when.tm_year);
         }
      }
   }

   printf("\nTuborgs consumed : %d\n", drunk);

   return 0;
}
