#include "comm.h"
#include "db.h"
#include "files.h"
#include "handler.h"
#include "help_index_type.h"
#include "interpreter.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>

/*  Help-file contained 468 keywords on 12-Sep-94
 *  Potential waste: 8 * (HELP_INCREMENT - 1) bytes
 */

#define HELP_INCREMENT 100

struct help_file_type
{
   help_index_type *help_idx;       /* the help keyword list */
   int              elements, size; /* info about the list   */
   char            *filename;
};

static struct help_file_type help_file[3];

extern char                  libdir[]; /* from dikumud.c        */

auto                         search_help_cmp(const void *keyval, const void *datum) -> int
{
   if(static_cast<unsigned int>(is_abbrev((char *)keyval, ((help_index_type *)datum)->keyword)) != 0U)
   {
      return 0;
   }
   return str_ccmp((char *)keyval, ((help_index_type *)datum)->keyword);
}

/* Returns TRUE if help was presented */
static auto help(struct help_file_type *hlp, descriptor_data *d, char *arg) -> int
{
   char             buf[MAX_STRING_LENGTH];
   char             line[256];
   char             buf2[MAX_STRING_LENGTH];
   help_index_type *tmp;
   FILE            *help_fl;

   if((hlp->help_idx == nullptr) || (hlp->elements < 1))
   {
      return static_cast<int>(FALSE);
   }

   if((tmp = (help_index_type *)bsearch(arg, hlp->help_idx, hlp->elements + 1, sizeof(help_index_type), search_help_cmp)) != nullptr)
   {
      int i = (tmp - hlp->help_idx);

      /*  Have to unroll backwards to make sure we find FIRST
       *  occurence of argument
       */
      while(0 < i && (static_cast<unsigned int>(is_abbrev(arg, hlp->help_idx[i - 1].keyword)) != 0U))
      {
         --i;
      }

      help_fl = fopen_cache(hlp->filename, "r");
      assert(help_fl);

      fseek(help_fl, hlp->help_idx[i].pos, 0);

      char *ms2020 = fgets(line, sizeof(line), help_fl);
      strcat(line, "\n\r");

      send_to_descriptor(line, d);

      *buf = '\0';
      for(;;)
      {
         ms2020 = fgets(line, sizeof(line), help_fl);
         if(*line == '#')
         {
            break;
         }
         strcat(buf, line);
      }

      str_escape_format(buf, buf2, sizeof(buf2));
      page_string(d, buf2);
      return static_cast<int>(TRUE);
   }

   return static_cast<int>(FALSE);
}

/* Returns TRUE if help was found and displayed */

auto help_base(descriptor_data *d, char *arg) -> int
{
   uint8_t bHelp = static_cast<uint8_t>(FALSE);

   arg           = skip_spaces(arg);
   str_lower(arg);

   if((CHAR_LEVEL(d->character) >= IMMORTAL_LEVEL) && (help(&help_file[2], d, arg) != 0))
   {
      bHelp = static_cast<uint8_t>(TRUE);
   }

   if((help(&help_file[0], d, arg) == 0) && (help(&help_file[1], d, arg) == 0))
   {
      return bHelp;
   }

   return static_cast<int>(TRUE);
}

void do_help(unit_data *ch, char *arg, const struct command_info *cmd)
{
   if(!IS_PC(ch) || (CHAR_DESCRIPTOR(ch) == nullptr))
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      struct zone_type *zone = unit_zone(ch);

      page_string(CHAR_DESCRIPTOR(ch), zone->help);
      return;
   }

   if(help_base(CHAR_DESCRIPTOR(ch), arg) == 0)
   {
      act("There is no help available on the subject '$2t'.", A_ALWAYS, ch, arg, nullptr, TO_CHAR);
   }
}

/* BOOT stuff below */

/*  one_word is like one_argument, except that words in quotes "" are
 *  regarded as ONE word
 */
auto one_word(char *arg, char *first_arg) -> char *
{
   do
   {
      int look_at = 0;

      arg         = skip_spaces(arg);

      if(*arg == '\"') /* is it a quote " */
      {
         for(arg++; arg[look_at] >= ' ' && arg[look_at] != '\"'; ++look_at)
         {
            first_arg[look_at] = tolower(arg[look_at]);
         }

         if(arg[look_at] == '\"')
         { /* " (damn dumb emacs syntax colouring) */
            arg++;
         }
      }
      else
      {
         for(look_at = 0; ' ' < arg[look_at]; look_at++)
         {
            first_arg[look_at] = tolower(arg[look_at]);
         }
      }

      first_arg[look_at] = '\0';
      arg += look_at;
   } while(fill_word(first_arg) != 0);

   return arg;
}

auto build_help_cmp(const void *keyval, const void *datum) -> int
{
   return str_ccmp(((help_index_type *)keyval)->keyword, ((help_index_type *)datum)->keyword);
}

static void generate_help_idx(struct help_file_type *hlp, const char *name)
{
   FILE *fl;
   char  buf[256];
   char  tmp[256];
   char *scan;
   long  pos;

   hlp->help_idx = nullptr;
   hlp->elements = 0;
   hlp->filename = str_dup(str_cc(libdir, name));

   touch_file(hlp->filename);

   if((fl = fopen_cache(hlp->filename, "r")) == nullptr)
   {
      slog(LOG_OFF, 0, "   Could not open help file [%s]", hlp->filename);
      exit(0);
   }

   for(;;)
   {
      pos                  = ftell(fl);

      char *ms2020         = fgets(buf, sizeof buf, fl);

      buf[sizeof buf - 1]  = '\0'; /* Just in case... */
      buf[strlen(buf) - 1] = '\0'; /* Cut off trailing newline */

      scan                 = buf;

      for(;;)
      {
         /* extract the keywords */
         scan = one_word(scan, tmp);
         if(*tmp == '\0')
         {
            break;
         }

         if(hlp->help_idx == nullptr)
         {
            hlp->size = HELP_INCREMENT;
            CREATE(hlp->help_idx, help_index_type, hlp->size);
         }
         else if(++hlp->elements == hlp->size)
         {
            hlp->size += HELP_INCREMENT;
            RECREATE(hlp->help_idx, help_index_type, hlp->size);
         }

         hlp->help_idx[hlp->elements].pos     = pos;
         hlp->help_idx[hlp->elements].keyword = str_dup(tmp);
      }

      /* skip the text */
      do
      {
         ms2020 = fgets(buf, sizeof buf, fl);
      } while(*buf != '#' && (feof(fl) == 0));

      if(feof(fl) != 0)
      {
         break;
      }

      if(buf[1] == '~')
      {
         break;
      }
   }

   qsort(hlp->help_idx, hlp->elements + 1, sizeof(help_index_type), build_help_cmp);
}

void boot_help()
{
   generate_help_idx(&help_file[0], HELP_FILE);
   generate_help_idx(&help_file[1], HELP_FILE_LOCAL);
   generate_help_idx(&help_file[2], HELP_FILE_WIZ);
}
