#include "comm.h"
#include "common.h"
#include "db.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "main.h"
#include "money.h"
#include "skills.h"
#include "spec_assign.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "unit_vector_data.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define DEFAULT_ENTRY_ZONE "udgaard"
#define DEFAULT_ENTRY_NAME "temple"

#define PC_DEATHLOC_NAME    "heaven_entry"
#define PC_PREDEATHLOC_NAME "deathseq_room"
#define PC_TIMEOUTLOC_NAME  "timeout"
#define VOID_ROOM           "void"
#define ZOMBIE_NAME         "zombie"
#define DEMIGOD_NAME        "demigod"
#define MAIL_NOTE_NAME      "letter"
#define CORPSE_NAME         "corpse"
#define HEAD_NAME           "head"

#define PC_DEATHOBJ_NAME "death_seq"
#define DESTROY_ROOM     "destroy_room"

unit_data       *void_room     = nullptr;
unit_data       *destroy_room  = nullptr;
unit_data       *heaven_room   = nullptr;
unit_data       *seq_room      = nullptr;
unit_data       *time_room     = nullptr;
unit_data       *entry_room    = nullptr;

file_index_type *demigod_fi    = nullptr; /* Default demigod shape */
file_index_type *zombie_fi     = nullptr;
file_index_type *letter_fi     = nullptr;
file_index_type *corpse_fi     = nullptr;
file_index_type *head_fi       = nullptr;
file_index_type *deathobj_fi   = nullptr;
file_index_type *beginner_note = nullptr;

void             basis_boot()
{
   void_room = world_room(BASIS_ZONE, VOID_ROOM);
   assert(void_room);

   destroy_room = world_room(BASIS_ZONE, DESTROY_ROOM);
   assert(destroy_room);

   heaven_room = world_room(BASIS_ZONE, PC_DEATHLOC_NAME);
   assert(heaven_room);

   seq_room = world_room(BASIS_ZONE, PC_PREDEATHLOC_NAME);
   assert(seq_room);

   time_room = world_room(BASIS_ZONE, PC_TIMEOUTLOC_NAME);
   assert(time_room);

   letter_fi = find_file_index(BASIS_ZONE, MAIL_NOTE_NAME);
   assert(letter_fi);

   zombie_fi = find_file_index(BASIS_ZONE, ZOMBIE_NAME);
   assert(zombie_fi);

   demigod_fi = find_file_index(BASIS_ZONE, DEMIGOD_NAME);
   if(demigod_fi == nullptr)
   {
      demigod_fi = zombie_fi;
   }

   head_fi = find_file_index(BASIS_ZONE, HEAD_NAME);
   assert(head_fi);

   corpse_fi = find_file_index(BASIS_ZONE, CORPSE_NAME);
   assert(corpse_fi);

   deathobj_fi = find_file_index(BASIS_ZONE, PC_DEATHOBJ_NAME);
   assert(deathobj_fi);

   entry_room = world_room(DEFAULT_ENTRY_ZONE, DEFAULT_ENTRY_NAME);
   if(entry_room == nullptr)
   {
      slog(LOG_ALL, 0, "Entry room does not exist, using void.");
      entry_room = void_room;
   }
}

/* These events happen rarely (daemon is every 30 minutes) */
void random_event_world()
{
   unit_data *u;

   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_NPC(u) && (number(0, 100) == 0))
      {
         set_reward_char(u, 0);
         return;
      }
   }
}

/* These events happen rarely (daemon is every 30 minutes) */
void random_event_player(unit_data *u, unit_data *daemon)
{
   int        i;
   unit_data *tmpu;

   if(u == nullptr)
   {
      random_event_world();
      return;
   }

   char mbuf[MAX_INPUT_LENGTH] = {0};

   switch(number(0, 11))
   {
      case 0:
      case 1:
      case 2:
      case 3:
         random_event_world();
         return;

      case 4:
         act("You feel an itch where you can not scratch.", A_ALWAYS, u, nullptr, nullptr, TO_CHAR);
         break;

      case 5:
         act("You feel happy.", A_ALWAYS, u, nullptr, nullptr, TO_CHAR);
         break;

      case 6:
         act("You feel sad.", A_ALWAYS, u, nullptr, nullptr, TO_CHAR);
         break;

      case 7:
         act("You feel lucky.", A_ALWAYS, u, nullptr, nullptr, TO_CHAR);
         break;

      case 8:
         tmpu = read_unit(find_file_index("udgaard", "tuborg"));
         if(tmpu != nullptr)
         {
            unit_to_unit(tmpu, u);
            act("The Gods grant you nectar to quench your thirst.", A_ALWAYS, u, nullptr, nullptr, TO_CHAR);
            act("Suddenly the sky opens and $2n drops into the hands of $1n.", A_ALWAYS, u, tmpu, nullptr, TO_ROOM);
         }
         else
         {
            act("You have an inexplainable craving for a refreshing Green Tuborg.", A_ALWAYS, u, nullptr, nullptr, TO_CHAR);
         }
         break;

      case 9:
         for(u = unit_list; u != nullptr; u = u->gnext)
         {
            /* Only outlaw the ones that have signed */
            if(IS_PC(u) && (number(0, 1000) == 0) && IS_SET(CHAR_FLAGS(u), PC_PK_RELAXED))
            {
               set_reward_char(u, 0);
               return;
            }
         }
         break;

      case 10:
         i = number(1, 1000 * CHAR_LEVEL(u));
         act("You learn that you have won $2t in the lottery.", A_ALWAYS, u, money_string(i, DEF_CURRENCY, TRUE), nullptr, TO_CHAR);
         money_to_unit(u, i, DEF_CURRENCY);
         break;

      case 11:
         if(number(0, 1) != 0)
         {
            spell_perform(SPL_DISEASE_1, MEDIA_SPELL, daemon, daemon, u, mbuf, nullptr);
         }
         else
         {
            spell_perform(SPL_DISEASE_2, MEDIA_SPELL, daemon, daemon, u, mbuf, nullptr);
         }
         break;

         /*  MANY OPTIONS! Ideas:
          *  Stumble upon random item
          *  Word of failure from bank
          *  etc. etc.
          */
   }
}

auto error_rod(struct spec_arg *sarg) -> int
{
   struct zone_type *zone;
   FILE             *fl;
   char              filename[256];

   if((sarg->cmd->no != CMD_USE) || (!IS_PC(sarg->activator)) || (OBJ_EQP_POS(sarg->owner) != WEAR_HOLD))
   {
      return SFR_SHARE;
   }

   zone = unit_zone(sarg->activator);

   strcpy(filename, UNIT_NAME(sarg->activator));

   if(!IS_ADMINISTRATOR(sarg->activator) && (zone->creators.IsName(filename) == nullptr))
   {
      send_to_char("You are only allowed to erase errors "
                   "in your own zone.\n\r",
                   sarg->activator);
      return SFR_BLOCK;
   }

   sprintf(filename, "%s%s.err", zondir, zone->filename);

   if((fl = fopen(filename, "w")) == nullptr)
   {
      slog(LOG_ALL, 0, "Could not clear the zone error-file");
      send_to_char("Could not clear the zone error-file.\n\r", sarg->activator);
      return SFR_BLOCK;
   }
   fclose(fl);

   act("$1n uses $2n.", A_HIDEINV, sarg->activator, sarg->owner, nullptr, TO_ROOM);
   send_to_char("Error file was erased.\n\r", sarg->activator);
   slog(LOG_ALL, UNIT_MINV(sarg->activator), "%s cleared %s", UNIT_NAME(sarg->activator), filename);
   return SFR_BLOCK;
}

auto info_rod(struct spec_arg *sarg) -> int
{
   struct zone_type *zone;
   FILE             *fl;
   char              filename[256];

   if((static_cast<unsigned int>(is_command(sarg->cmd, "wave")) == 0U) || !IS_PC(sarg->activator) || OBJ_EQP_POS(sarg->owner) != WEAR_HOLD)
   {
      return SFR_SHARE;
   }

   zone = unit_zone(sarg->activator);

   strcpy(filename, UNIT_NAME(sarg->activator));

   if(!IS_ADMINISTRATOR(sarg->activator) && (zone->creators.IsName(filename) == nullptr))
   {
      send_to_char("You are only allowed to erase user-information"
                   " in your own zone.",
                   sarg->activator);
      return SFR_BLOCK;
   }

   sprintf(filename, "%s%s.inf", zondir, zone->filename);

   if((fl = fopen(filename, "w")) == nullptr)
   {
      slog(LOG_ALL, 0, "Could not clear the zone user info-file");
      send_to_char("Could not clear the zone user info-file.\n\r", sarg->activator);
      return SFR_BLOCK;
   }
   fclose(fl);

   act("$1n uses $2n.", A_HIDEINV, sarg->activator, sarg->owner, nullptr, TO_ROOM);
   send_to_char("Zone user information file was erased.\n\r", sarg->activator);
   slog(LOG_ALL, UNIT_MINV(sarg->activator), "%s cleared %s", UNIT_NAME(sarg->activator), filename);
   return SFR_BLOCK;
}

auto recep_daemon(struct spec_arg *sarg) -> int
{
#ifdef SUSPEKT
   static int idx = -1;
   time_t     t1;

   if(sarg->cmd->no == CMD_AUTO_TICK || (is_command(sarg->cmd, "tickle") && daemon == find_unit(activator, &arg, 0, FIND_UNIT_SURRO)))
   {
      if(idx == -1)
         idx = number(0, (int)top_of_player_idx);

      act("$1n checks $3t's inventory ($2d)", A_ALWAYS, daemon, &idx, player_idx_info[idx].name, TO_ROOM);

      t1 = time(0) - player_idx_info[idx].keep_period;

      if(t1 >= 0)
      {
         act("$1n says, 'Overdue by $2d seconds.'", A_ALWAYS, daemon, (int *)&t1, 0, TO_ROOM);
         act("$1n says, 'Snip Snap Snude, now the inventory is ude.'", A_ALWAYS, daemon, 0, 0, TO_ROOM);
         /* Unless player is in game, load him and erase his inventory */
      }

      if(++idx > top_of_player_idx)
         idx = 0;
   }
#endif

   return SFR_SHARE;
}

auto chaos_daemon(struct spec_arg *sarg) -> int
{
   char      *arg = (char *)sarg->arg;
   unit_data *u;

   if(sarg->cmd->no == CMD_AUTO_TICK ||
      ((static_cast<unsigned int>(is_command(sarg->cmd, "tickle")) != 0U) && IS_ULTIMATE(sarg->activator) &&
       sarg->owner == find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_SURRO)))
   {
      u = random_unit(sarg->owner, FIND_UNIT_WORLD, UNIT_ST_PC);
      if((u != nullptr) && IS_MORTAL(u))
      {
         act("$1n grins evilly at $3n.", A_ALWAYS, sarg->owner, nullptr, u, TO_ROOM);
         random_event_player(u, sarg->owner);
      }
      else
      {
         random_event_player(nullptr, sarg->owner);
      }
   }

   return SFR_SHARE;
}

auto death_room(struct spec_arg *sarg) -> int
{
   int dam;
   int i;

   if(sarg->cmd->no != CMD_AUTO_TICK)
   {
      return SFR_SHARE;
   }

   if(sarg->fptr->data != nullptr)
   {
      dam = atoi((char *)sarg->fptr->data);
   }
   else
   {
      dam = 10000;
   }

   scan4_unit_room(sarg->owner, UNIT_ST_PC | UNIT_ST_NPC);

   for(i = 0; i < unit_vector.top; i++)
   {
      /* Damage below can theoretically cause death of other persons */
      if(is_destructed(DR_UNIT, unit_vector.units[i]) != 0)
      {
         continue;
      }

      damage(unit_vector.units[i], unit_vector.units[i], nullptr, dam, MSG_TYPE_OTHER, MSG_OTHER_BLEEDING, COM_MSG_EBODY);
   }

   return SFR_SHARE;
}

/* Log stuff below */

extern struct log_buffer log_buf[];

auto                     log_object(struct spec_arg *sarg) -> int
{
   uint8_t       *ip;
   enum log_level lev = LOG_OFF;
   char           c;
   unit_data     *ch = UNIT_IN(sarg->owner);

   if(sarg->fptr->data == nullptr)
   {
      CREATE(ip, uint8_t, 1);
      *ip                       = 0;

      OBJ_VALUE(sarg->owner, 0) = 'b';
      sarg->fptr->data          = ip;
   }
   else
   {
      ip = (uint8_t *)sarg->fptr->data;
   }

   c = OBJ_VALUE(sarg->owner, 0);

   switch(sarg->cmd->no)
   {
      case CMD_AUTO_EXTRACT:
         free(ip);
         sarg->fptr->data = nullptr;
         return SFR_SHARE;

      case CMD_AUTO_TICK:
         switch(c)
         {
            case 'o':
               lev = LOG_OFF;
               break;
            case 'b':
               lev = LOG_BRIEF;
               break;
            case 'e':
               lev = LOG_EXTENSIVE;
               break;
            case 'a':
               lev = LOG_ALL;
               break;
         }

         if(LOG_OFF < lev && IS_PC(ch) && PC_IMMORTAL(ch))
         {
            while(static_cast<unsigned int>(str_is_empty(log_buf[*ip].str)) == 0U)
            {
               if(log_buf[*ip].level <= lev && log_buf[*ip].wizinv_level <= CHAR_LEVEL(ch))
               {
                  act("<LOG: $2t>", A_ALWAYS, ch, log_buf[*ip].str, nullptr, TO_CHAR);
               }
               *ip = ((*ip + 1) % MAXLOG);
            }
            return SFR_BLOCK;
         }
         return SFR_SHARE;

      default:
         if((sarg->cmd->cmd_str != nullptr) && sarg->activator == UNIT_IN(sarg->owner) && (strcmp("log", sarg->cmd->cmd_str) == 0))
         {
            sarg->arg = skip_spaces(sarg->arg);
            if(static_cast<unsigned int>(is_abbrev(sarg->arg, "all")) != 0U)
            {
               c = 'a';
            }
            else if(static_cast<unsigned int>(is_abbrev(sarg->arg, "extensive")) != 0U)
            {
               c = 'e';
            }
            else if(static_cast<unsigned int>(is_abbrev(sarg->arg, "brief")) != 0U)
            {
               c = 'b';
            }
            else if(static_cast<unsigned int>(is_abbrev(sarg->arg, "off")) != 0U)
            {
               act("Ok, log is now off.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
               OBJ_VALUE(sarg->owner, 0) = 'o';
               return SFR_BLOCK;
            }
            else if(static_cast<unsigned int>(is_abbrev(sarg->arg, "help")) != 0U)
            {
               act("Possible settings are:\n\r off, brief, extensive, all.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
               return SFR_BLOCK;
            }
            else
            {
               act("Current log level is `$2t'.",
                   A_ALWAYS,
                   ch,
                   c == 'o'   ? "off"
                   : c == 'b' ? "brief"
                   : c == 'a' ? "all"
                              : "extensive",
                   nullptr,
                   TO_CHAR);
               return SFR_BLOCK;
            }

            act("You will now see the $2t log.", A_ALWAYS, ch, c == 'b' ? "brief" : c == 'a' ? "entire" : "extensive", nullptr, TO_CHAR);
            OBJ_VALUE(sarg->owner, 0) = c;
            return SFR_BLOCK;
         }

         return SFR_SHARE;
   }
}

/* Return TRUE if ok, FALSE if not */
auto system_check(unit_data *pc, char *buf) -> int
{
   /* Check for `` and ; in system-string */
   if((strchr(buf, '`') != nullptr) || (strchr(buf, ';') != nullptr))
   {
      send_to_char("You can not use the ' and ; characters\n\r", pc);
      slog(LOG_ALL, 0, "%s may have tried to break security with %s", UNIT_NAME(pc), buf);
      return static_cast<int>(FALSE);
   }

   return static_cast<int>(TRUE);
}

void execute_append(unit_data *pc, char *str)
{
   FILE *f;

   f = fopen(str_cc(libdir, EXECUTE_FILE), "ab+");

   if(f == nullptr)
   {
      slog(LOG_ALL, 0, "Error appending to execute file.");
      return;
   }

   fprintf(f, "%s\n", str);

   slog(LOG_ALL, UNIT_MINV(pc), "EXECUTE(%s): %s", UNIT_NAME(pc), str);

   fclose(f);
}

auto admin_obj(struct spec_arg *sarg) -> int
{
   char              buf[512];
   int               zonelist;
   struct zone_type *zone;
   extra_descr_data *exdp;

   if(sarg->cmd->no != CMD_AUTO_UNKNOWN)
   {
      return SFR_SHARE;
   }

   if(!IS_PC(sarg->activator))
   {
      return SFR_SHARE;
   }

   if(str_ccmp(sarg->cmd->cmd_str, "email") == 0)
   {
      zonelist = static_cast<int>(FALSE);
   }
   else if(str_ccmp(sarg->cmd->cmd_str, "zonelist") == 0)
   {
      if(!IS_ADMINISTRATOR(sarg->activator))
      {
         send_to_char("Only administrators can use this function.\n\r", sarg->activator);
         return SFR_BLOCK;
      }
      zonelist = static_cast<int>(TRUE);
   }
   else
   {
      return SFR_SHARE;
   }

   if((exdp = PC_INFO(sarg->activator)->find_raw("$email")) == nullptr)
   {
      send_to_char("You do not have an email address registered.\n\r", sarg->activator);
      return SFR_BLOCK;
   }

   if(static_cast<unsigned int>(str_is_empty(exdp->descr.String())) != 0U)
   {
      send_to_char("Your email is incorrectly registered.\n\r", sarg->activator);
      return SFR_BLOCK;
   }

   if(zonelist != 0)
   {
      sprintf(buf, "mail zone zonelist %s", exdp->descr.String());
   }
   else if((zone = unit_zone(sarg->activator)) == nullptr)
   {
      send_to_char("You are inside no zone?", sarg->activator);
      return SFR_BLOCK;
   }
   else
   {
      if((zone->creators.IsName(UNIT_NAME(sarg->activator)) == nullptr) && (!IS_OVERSEER(sarg->activator)))
      {
         send_to_char("Only overseers can use this function.\n\r", sarg->activator);
         return SFR_BLOCK;
      }
      sprintf(buf, "mail zone %s %s", zone->filename, exdp->descr.String());
   }

   if(system_check(sarg->activator, buf) == 0)
   {
      return SFR_BLOCK;
   }

   execute_append(sarg->activator, buf);

   strcat(buf, "\n\r");
   send_to_char(buf, sarg->activator);

   return SFR_BLOCK;
}
