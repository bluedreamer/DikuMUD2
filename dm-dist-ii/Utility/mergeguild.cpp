#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

auto skip_blanks(const char *string) -> char *
{
   assert(string);

   for(; (*string != 0) && (isspace(*string) != 0); string++)
   {
      ;
   }

   return (char *)string;
}

/* Returns true is arg is empty */
auto str_is_empty(const char *arg) -> int
{
   return static_cast<int>(*(skip_blanks(arg)) == '\0');
}

void str_char_subst(char *str, char c)
{
   char *cp;

   if(c == 0)
   {
      return;
   }

   while((cp = strchr(str, c)) != nullptr)
   {
      *cp++ = 0;
   }
}

auto mystrtok(char *s, char delim) -> char *
{
   static char *cp = nullptr;
   char        *cp2;

   if(delim == 0)
   {
      return nullptr;
   }

   if(s != nullptr)
   {
      cp = s;
   }

   if(cp == nullptr || *cp == 0)
   {
      return nullptr;
   }

   cp2 = cp;

   cp  = strchr(cp, delim);
   if(cp == nullptr)
   {
      cp = nullptr;
      return cp2;
   }

   *cp++ = 0;

   return cp2;
}

void convert(int idx)
{
   char  buf[4096];
   char *name;
   char *code;
   int   i;
   int   section     = 0;

   int   costs[5][3] = {{5, 10, 15}, {8, 16, 24}, {10, 20, 30}, {15, 30, 45}, {20, 40, 60}};

   while(feof(stdin) == 0)
   {
      if(fgets(buf, sizeof(buf), stdin) == nullptr)
      {
         break;
      }

      name = mystrtok(buf, ',');

      if(strncmp(name, "SECTION", 7) == 0)
      {
         section++;
         continue;
      }

      if((name != nullptr) && (str_is_empty(name) == 0))
      {
         for(i = 0; i < idx; i++)
         {
            code = mystrtok(nullptr, ',');
            if(code == nullptr)
            {
               break;
            }
         }

         if((code != nullptr) && (str_is_empty(code) == 0))
         {
            int   level;
            int   cost;
            int   max;
            int   nums;
            char *nc;

            nc    = strtok(code, "/");

            cost  = nc[0] - 'A';
            nums  = nc[1] - '0';

            nc    = strtok(nullptr, "/");
            level = atoi(nc);

            nc    = strtok(nullptr, "/");
            max   = atoi(nc);

            fprintf(stdout, "%d %2d; %4d; %-30s; %4d; %4d; ", section, level, max, name, (cost + 1) * 50, (cost + 1) * 500);

            if((nums >= 0 && nums <= 3) && (cost >= 0 && cost <= 4))
            {
               for(i = 0; i < nums; i++)
               {
                  fprintf(stdout, " %2d;", costs[cost][i]);
               }

               for(; i < 3; i++)
               {
                  fprintf(stdout, "    ");
               }

               fprintf(stdout, "  0;\n");
            }
            else
            {
               fprintf(stdout, "Error\n");
            }
         }
      }
   }
}

auto main(int argc, char *argv[]) -> int
{
   if(argc < 2)
   {
      fprintf(stderr, "Usage: %s <1..8>\n", argv[0]);
      return 0;
   }

   convert(atoi(argv[1]));

   return 0;
}
