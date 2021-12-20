#include <cerrno>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef DOS
   #include <sys/time.h>
struct timezone
{
   int tz_minuteswest;
   int tz_dsttime;
};
#endif

#include "sysport.h"

#include "comm.h"
#include "db.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "main.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include "unixshit.h"

void        type_validate(void);
void        run_the_game(void);
extern char libdir[]; /* directory for libraryfiles */

int main(int argc, char **argv)
{
   void cleanup_playerfile(int argc, char *argv[]);

   int pos = 1, sp;

   extern char    **player_name_list;
   extern cNamelist persist_namelist;

   slog(LOG_ALL, 0, "DMSERVER COMPILED AT %s %s", compile_date, compile_time);

   type_validate(); // MS2020

#ifdef LINUX
   {
      int           n;
      struct rlimit rlim;

      n = getrlimit(RLIMIT_CPU, &rlim);
      slog(LOG_ALL, 0, "RLIMIT CPU: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_DATA, &rlim);
      slog(LOG_ALL, 0, "RLIMIT DATA: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_STACK, &rlim);
      slog(LOG_ALL, 0, "RLIMIT STACK: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_CORE, &rlim);
      slog(LOG_ALL, 0, "RLIMIT CORE: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_RSS, &rlim);
      slog(LOG_ALL, 0, "RLIMIT RSS: %d / %d.", rlim.rlim_cur, rlim.rlim_max);
   }
#endif

   while((pos < argc) && (*(argv[pos]) == '-'))
   {
      switch(*(argv[pos] + 1))
      {
         case '?':
         case 'h':
            ShowUsage(argv[0]);
            exit(0);

         case 'c':
            slog(LOG_OFF, 0, "Converting player file mode.");
            player_convert = TRUE;
            pos++;
            player_name_list = create_namelist();
            while((pos < argc) && (*(argv[pos]) != '-'))
            {
               char *name = strrchr(argv[pos], '/');

               if(name && strchr(name, '.') == NULL)
                  player_name_list = add_name(name + 1, player_name_list);
               pos++;
            }

            if(pos >= argc)
               pos--;

            while(*(argv[pos]) == '-')
               pos--;
            break;

         case 'p':
            sp = pos;
            pos++;
            while((pos < argc) && (*(argv[pos]) != '-'))
            {
               char *name = strrchr(argv[pos], '/');

               if(!str_is_empty(name + 1))
                  persist_namelist.AppendName(name + 1);
               pos++;
            }

            if(pos >= argc)
               pos--;

            if(sp != pos)
               while(*(argv[pos]) == '-')
                  pos--;
            break;

         case 'd':
            if(*(argv[pos] + 2))
               strcpy(libdir, argv[pos] + 2);
            else if(++pos < argc)
               strcpy(libdir, argv[pos]);
            else
            {
               slog(LOG_OFF, 0, "Directory arg expected after option -d.");
               exit(1);
            }
            break;

         case 'z':
            if(*(argv[pos] + 2))
               strcpy(zondir, argv[pos] + 2);
            else if(++pos < argc)
               strcpy(zondir, argv[pos]);
            else
            {
               slog(LOG_OFF, 0, "Directory arg expected after option -z.");
               exit(1);
            }
            break;

         default:
            slog(LOG_OFF, 0, "Unknown option -% in argument string.", *(argv[pos] + 1));
            break;
      }
      pos++;
   }

   if(pos < argc)
      if(!isdigit(*argv[pos]))
      {
         ShowUsage(argv[0]);
         exit(1);
      }

   slog(LOG_OFF, 0, "Using 'root' as base directory.");

#if defined(GENERIC_SYSV) || defined(HPUX)
   srand48(time(0));
#elif defined(LINUX)
   /* Other used routines are declared obsolete by SVID3 */
   srand(time(0));
#else
   srandom(time(0));
#endif

   run_the_game();
   return (0);
}
