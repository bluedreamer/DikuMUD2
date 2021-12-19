#include "ban.h"

#include "ban_t.h"
#include "comm.h"
#include "common.h"
#include "files.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>

ban_t *ban_list = nullptr;

void BanFile::save_ban()
{
   ban_t *tmp;
   FILE  *bf = fopen(filename.c_str(), "w");
   assert(bf);

   for(tmp = ban_list; tmp != nullptr; tmp = tmp->next)
   {
      fprintf(bf, "%s %c %ld %s\n", tmp->site, tmp->type, tmp->until, tmp->textfile);
   }

   fclose(bf);
}

void BanFile::load_ban()
{
   std::ifstream in(filename_);

   std::string line;
   while(getline(in, line))
   {
      std::string site{};
      std::string textfile{};
      char type{};
      time_t until{0};
      std::istringstream strm(line);
      strm >> site >> (char)type >> until >> textfile;
      tmp->site     = str_dup(site);
      tmp->textfile = str_dup(textfile);
      tmp->next     = ban_list;
      ban_list      = tmp;
   }
}

auto BanFile::ban_timer(char *arg) -> time_t
{
   time_t now = time(nullptr);

   while(*arg != 0)
   {
      int mult = 0;

      arg = skip_spaces(arg);
      while(isdigit(*arg) != 0)
      {
         mult = 10 * mult + (*arg++ - '0');
      }
      arg = skip_spaces(arg);
      switch(*arg++)
      {
         case 'd':
            mult *= SECS_PER_REAL_DAY;
            break;
         case 'h':
            mult *= SECS_PER_REAL_HOUR;
            break;
         case 'm':
            mult *= SECS_PER_REAL_MIN;
            break;
         default:
            return 0;
      }
      arg = skip_spaces(arg);
      now += mult;
   }

   return now;
}

void BanFile::add_ban(unit_data *ch, char *site, char type, time_t *until, char *textfile)
{
   ban_t *entry;
   char   d[50];
   char   buf[MAX_STRING_LENGTH];

   for(entry = ban_list; entry != nullptr; entry = entry->next)
   {
      if(str_ccmp(entry->site, site) == 0)
      {
         break;
      }
   }

   if(entry == nullptr)
   {
      CREATE(entry, ban_t, 1);
      entry->site = str_dup(site);
      if(static_cast<unsigned int>(str_is_empty(textfile)) == 0U)
      {
         entry->textfile = str_dup(textfile);
      }
      else
      {
         if(type == BAN_TOTAL)
         {
            entry->textfile = str_dup("bantotal.txt");
         }
         else
         {
            entry->textfile = str_dup("bannew.txt");
         }
      }
      entry->until = 0;
      entry->next  = ban_list;
      ban_list     = entry;
   }

   entry->type  = type;
   entry->until = *until;

   if(*until != 0)
   {
      strftime(d, 50, "Lasting 'til %a %b %d %H:%M.", localtime(until));
   }
   else
   {
      strcpy(d, "");
   }

   sprintf(buf, "Ban on %s, type %s, textfile %s. %s\n\r", entry->site, type == BAN_TOTAL ? "TOTAL" : "NEW CHARS", entry->textfile, d);
   send_to_char(buf, ch);
   save_ban();
}

void BanFile::kill_entry(ban_t *entry)
{
   if(entry == ban_list)
   {
      ban_list = entry->next;
   }
   else
   {
      ban_t *tmp;

      for(tmp = ban_list; tmp != nullptr; tmp = tmp->next)
      {
         if(tmp->next == entry)
         {
            break;
         }
      }
      tmp->next = entry->next;
   }

   free(entry->site);
   if(entry->textfile != nullptr)
   {
      free(entry->textfile);
   }
   free(entry);

   save_ban();
}

void BanFile::del_ban(unit_data *ch, char *site)
{
   ban_t *entry;

   for(entry = ban_list; entry != nullptr; entry = entry->next)
   {
      if(str_ccmp(entry->site, site) == 0)
      {
         break;
      }
   }

   if(entry != nullptr)
   {
      kill_entry(entry);
      act("$2t taken succesfully off ban list.", A_ALWAYS, ch, site, nullptr, TO_CHAR);
   }
   else
   {
      act("No entry $2t in ban list.", A_ALWAYS, ch, site, nullptr, TO_CHAR);
   }
}

void BanFile::show_site(unit_data *ch, ban_t *entry)
{
   char buf[200];
   char d[40];

   if(entry->until != 0)
   {
      strftime(d, 40, "until %a %b %d %H:%M", localtime(&entry->until));
   }
   else
   {
      strcpy(d, " ");
   }
   sprintf(buf, " %-30s : %-9s %s %s\n\r", entry->site, (entry->type == BAN_TOTAL) ? "TOTAL" : "NEW CHARS", entry->textfile, d);
   send_to_char(buf, ch);
}

void BanFile::do_ban(unit_data *ch, char *arg, const command_info *cmd)
{
   ban_t *tmp;
   char   site[MAX_INPUT_LENGTH];
   char   textfile[MAX_INPUT_LENGTH];
   char   mode;
   char   type;
   time_t until = 0;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      if(ban_list != nullptr)
      {
         send_to_char("Sites banned:\n\r-------------\n\r", ch);
         for(tmp = ban_list; tmp != nullptr; tmp = tmp->next)
         {
            show_site(ch, tmp);
         }
      }
      else
      {
         send_to_char("No sites banned.\n\r", ch);
      }

      return;
   }

   mode = tolower(*arg);
   ++arg;

   if(static_cast<unsigned int>(str_is_empty(arg)) == 0U)
   {
      arg = one_argument(arg, site);
      arg = one_argument(arg, textfile);

      switch(mode)
      {
         case 'n':
         case 't':
            type = (mode == 't') ? BAN_TOTAL : BAN_NEW_CHARS;

            if((static_cast<unsigned int>(str_is_empty(arg)) == 0U) && (until = ban_timer(arg)) == 0)
            {
               send_to_char("Wrong syntax in time. Not banned.\n\r.", ch);
            }
            else
            {
               add_ban(ch, site, type, &until, textfile);
            }
            return;

         case 'd':
            del_ban(ch, site);
            return;
      }
   }

   send_to_char("Usage:\n\r  >ban <t|n> <site> [textfile] [<period>]\n\r"
                "  >ban d <site>\n\r  >ban\n\r",
                ch);
}

/* Complex little recursive bugger */
auto BanFile::ban_check(char *ban, char *site) -> bool /* TRUE, if banned */
{
   if(*ban == '\0' && *site == '\0')
   {
      return TRUE;
   }
   if(*ban == '*')
   {
      if(*site == '\0' || *++ban == '\0')
      {
         return TRUE;
      }
      for(;;)
      {
         if(ban_check(ban, site))
         {
            return TRUE;
         }
         if(*++site == '\0')
         {
            return FALSE;
         }
      }
   }
   else if(*site == '\0')
   {
      return FALSE;
   }
   else if(*ban == '?' || *ban == tolower(*site))
   {
      return ban_check(++ban, ++site);
   }
   else
   {
      return FALSE;
   }
}

auto BanFile::site_banned(char *cur_site) -> char
{
   ban_t *entry;
   ban_t *next_entry;
   time_t now = time(nullptr);

   for(entry = ban_list; entry != nullptr; entry = next_entry)
   {
      next_entry = entry->next;

      if((entry->until != 0) && entry->until < now)
      {
         kill_entry(entry);
      }
      else if(ban_check(entry->site, cur_site))
      {
         return entry->type;
      }
   }

   return NO_BAN;
}

void BanFile::show_ban_text(char *site, descriptor_data *d)
{
   ban_t *entry;
   char   bantext[MAX_STRING_LENGTH];
   char   formtext[MAX_STRING_LENGTH];

   for(entry = ban_list; entry != nullptr; entry = entry->next)
   {
      if(ban_check(entry->site, site))
      {
         if(file_to_string(str_cc(libdir, entry->textfile), bantext, sizeof(bantext) - 1) == -1)
         {
            send_to_descriptor("Your site has been banned.\n\r", d);
         }
         else
         {
            str_escape_format(bantext, formtext, sizeof(formtext));
            send_to_descriptor(formtext, d);
         }
         return;
      }
   }
}
BanFile::BanFile()
   : filename_(libdir + BAN_FILE)
{
}
