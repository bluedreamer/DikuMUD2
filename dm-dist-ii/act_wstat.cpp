#include "account.h"
#include "affect.h"
#include "apply_function_type.h"
#include "blkfile.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
#include "db.h"
#include "db_file.h"
#include "files.h"
#include "handler.h"
#include "hashstring.h"
#include "interpreter.h"
#include "magic.h"
#include "main.h"
#include "skills.h"
#include "spell_info_type.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "tick_function_type.h"
#include "time_info_data.h"
#include "tree_type.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "unit_function_array_type.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"
#include "zone_info_type.h"
#include "zone_reset_cmd.h"

#include <arpa/inet.h>
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <sys/socket.h>

extern void stat_bank(const unit_data *ch, unit_data *u); /* bank.c */

static void stat_world_extra(const unit_data *ch)
{
   char              buf[MAX_STRING_LENGTH];
   char             *b;
   struct zone_type *zp;
   int               i;

   b = buf;
   sprintf(b, "World zones (%d):\n\r", zone_info.no_of_zones);
   TAIL(b);

   for(i = 1, zp = zone_info.zone_list; zp != nullptr; zp = zp->next, i++)
   {
      sprintf(b, "%-24s", zp->name);
      TAIL(b);
      if(i % 3 == 0)
      {
         strcpy(b, "\n\r");
         send_to_char(buf, ch);
         *buf = 0;
         b    = buf;
      }
   }

   send_to_char(buf, ch);
   send_to_char("\n\r", ch);
}

static void stat_string(unit_data *ch)
{
   // TODO find out why this wont't link
   // void string_statistics(unit_data *ch);

   // string_statistics(ch);
   assert(0);
}

static void stat_swap(unit_data *ch)
{
   void swap_status(unit_data * ch);

   swap_status(ch);
}

static void stat_memory(unit_data *ch)
{
   char       buf[MAX_STRING_LENGTH];

   extern int events;

   void       memory_status(char *buf);
   void       system_memory(unit_data * ch);

   sprintf(buf, "Event queue entries: %d\n\r\n\r", events);
   send_to_char(buf, ch);
   system_memory(ch);
   memory_status(buf);
   send_to_char(buf, ch);
}

static void stat_world(unit_data *ch)
{
   extern int world_norooms, world_noobjects, world_nochars, world_nozones;
   extern int world_nonpc;
   extern int world_nopc;

   char       buf[MAX_STRING_LENGTH];
   time_t     now   = time(nullptr);

   char      *p     = ctime(&now);
   p[strlen(p) - 1] = '\0';

   sprintf(buf,
           "Server compiled at %s %s\n\r"
           "World status (tick %d):\n\r"
           "#rooms [%4d]  #objects [%4d]  #chars [%4d]  #npcs [%4d] "
           "  #pcs [%4d]\n\r"
           "#units [%4d]  #zones   [%4d]\n\r"
           "#connections [%2d / peak %2d]\n\r"
           "#players     [%2d / peak %2d]\n\r"
           "Boottime: %s\n\rTime now: %s\n\r",
           compile_date,
           compile_time,
           tics,
           world_norooms,
           world_noobjects,
           world_nochars,
           world_nonpc,
           world_nopc,
           world_norooms + world_noobjects + world_nochars,
           world_nozones,
           no_connections,
           max_no_connections,
           no_players,
           max_no_players,
           world_boottime,
           p);

   send_to_char(buf, ch);
}

static char *stat_buffer, *stat_p;

static void  stat_zone_reset(char *indnt, zone_reset_cmd *zrip, unit_data *ch)
{
   static const char *nums[] = {"max", "zonemax", "local"};

   char               buf[100];
   int                i;

   *stat_p = 0;

   STRCAT(stat_p, indnt);

   switch(zrip->cmd_no)
   {
      case 0:
         strcpy(stat_p, "Nop");
         break;

      case 1:
         sprintf(stat_p, "Load %s", zrip->fi[0]->name);
         TAIL(stat_p);

         if(zrip->fi[1] != nullptr)
         {
            sprintf(stat_p, " into %s", zrip->fi[1]->name);
            TAIL(stat_p);
         }

         for(i = 0; i < 3; i++)
         {
            if(zrip->num[i] != 0)
            {
               sprintf(stat_p, " %s %d", nums[i], zrip->num[i]);
               TAIL(stat_p);
            }
         }

         strcpy(stat_p, zrip->cmpl != 0U ? " Complete" : "");
         break;

      case 2:
         sprintf(stat_p, "Equip %s %s max %d %s", zrip->fi[0]->name, where[zrip->num[1]], zrip->num[0], zrip->cmpl != 0U ? "Complete" : "");
         break;

      case 3:
         sprintf(stat_p, "Door at %s : %s : %s", zrip->fi[0]->name, dirs[zrip->num[0]], sprintbit(buf, zrip->num[1], unit_open_flags));
         break;

      case 4:
         sprintf(stat_p, "Purge %s", zrip->fi[0]->name);
         break;

      case 5:
         sprintf(stat_p, "Remove %s in %s", zrip->fi[0]->name, zrip->fi[1]->name);
         break;
   }

   STRCAT(stat_p, "\n\r");

   /* make sure we get no overflow */
   if(stat_p - stat_buffer + MAX_INPUT_LENGTH > 2 * MAX_STRING_LENGTH)
   {
      STRCAT(stat_p, "Truncated .....\n\r");
      return;
   }

   if(zrip->nested != nullptr)
   {
      char whitespace[MAX_INPUT_LENGTH];

      sprintf(stat_p, "%s{\n\r", indnt);
      TAIL(stat_p);

      sprintf(whitespace, "%s  ", indnt);
      stat_zone_reset(whitespace, zrip->nested, ch);

      sprintf(stat_p, "%s}\n\r", indnt);
      TAIL(stat_p);
   }
   if(zrip->next != nullptr)
   {
      stat_zone_reset(indnt, zrip->next, ch);
   }
}

static void stat_zone(unit_data *ch, struct zone_type *zone)
{
   static const char *reset_modes[] = {"Never Reset", "Reset When Empty", "Reset Always", "UNKNOWN"};

   char               tmp[128];
   char               buf[MAX_STRING_LENGTH];
   bool               errors;
   bool               info;
   int                reset_mode = zone->reset_mode;

   if(is_in(reset_mode, 0, 2) == 0)
   {
      reset_mode = 3;
   }

   sprintf(tmp, "%s%s.err", zondir, zone->filename);
   errors = (static_cast<unsigned int>(file_exists(tmp)) != 0U);

   sprintf(tmp, "%s%s.inf", zondir, zone->filename);
   info = (static_cast<unsigned int>(file_exists(tmp)) != 0U);

   zone->creators.catnames(tmp);

   sprintf(buf,
           "Zone [%s]  File [%s]  Access [%d]\n\r"
           "Title: \"%s\"\n\r"
           "Load level [%d] Pay only [%d]\n\r"
           "Number of Units [%d]    Number of Rooms [%d]\n\r"
           "Reset Mode : %s (%d)    Reset Interval [%d]\n\r"
           "Pressure [%d] Change [%d] Sky [%d] Base [%d]\n\r\n\r"
           "Authors Mud Mail: %s\n\r\n\r%s\n\r\n\r"
           "%s\n\r%s\n\r",
           zone->name,
           zone->filename,
           zone->access,
           zone->title != nullptr ? zone->title : "",
           zone->loadlevel,
           zone->payonly,
           zone->no_of_fi,
           zone->no_rooms,
           reset_modes[reset_mode],
           reset_mode,
           zone->zone_time,
           zone->weather.pressure,
           zone->weather.change,
           zone->weather.sky,
           zone->weather.base,
           tmp,
           zone->notes,
           errors ? "Errors in zone (stat zone error)" : "No errors registered in zone.",
           info ? "User info feedback in zone (stat zone info)." : "No user info (feedback) in zone.");

   send_to_char(buf, ch);
}

static void stat_creators(unit_data *ch, char *arg)
{
   char              buf[4 * MAX_STRING_LENGTH];
   char             *b;
   char              tmp[1024];
   int               found;
   struct zone_type *z;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Requires name of creator.\n\r", ch);
      return;
   }

   b   = buf;

   arg = one_argument(arg, tmp);

   if(str_ccmp(tmp, "all") == 0)
   {
      sprintf(b, "List of all Zones with Creators.\n\r\n\r");
      TAIL(b);

      found = static_cast<int>(FALSE);

      for(z = zone_info.zone_list; z != nullptr; z = z->next)
      {
         z->creators.catnames(tmp);

         sprintf(b, "%-15s   %s\n\r", z->name, tmp);

         TAIL(b);
         found = static_cast<int>(TRUE);
      }

      page_string(CHAR_DESCRIPTOR(ch), buf);
      return;
   }

   sprintf(b, "Zones Created by %s.\n\r\n\r", tmp);
   TAIL(b);

   found = static_cast<int>(FALSE);
   for(z = zone_info.zone_list; z != nullptr; z = z->next)
   {
      if(z->creators.IsName(tmp) != nullptr)
      {
         sprintf(b, "%-15s   File: %s.zon\n\r", z->name, z->filename);
         TAIL(b);
         found = static_cast<int>(TRUE);
      }
   }

   if(found == 0)
   {
      sprintf(b, "None.\n\r");
   }

   TAIL(b);

   send_to_char(buf, ch);
}

// MS2020 modified to get rid of warnings
static void stat_dil(const unit_data *ch, const struct zone_type *zone)
{
   char         buf[MAX_STRING_LENGTH];
   char         buf2[MAX_STRING_LENGTH];
   diltemplate *tmpl;
   int          i;

   sprintf(buf, "List of DIL in zone %s:\n\r", zone->name);
   send_to_char(buf, ch);

   for(*buf = 0, tmpl = zone->tmpl, i = 0; tmpl != nullptr; tmpl = tmpl->next)
   {
      sprintf(buf2, "%s%6d %-19s", buf, tmpl->nActivations, tmpl->prgname);
      memcpy(buf, buf2, MAX_STRING_LENGTH);

      /* Enough to fill a whole line */
      if(++i == 3)
      {
         sprintf(buf2, "%s\n\r", buf);
         memcpy(buf, buf2, MAX_STRING_LENGTH);
         send_to_char(buf, ch);
         *buf = 0;
         i    = 0;
      }
   }

   if(i != 0)
   {
      sprintf(buf2, "%s\n\r", buf);
      memcpy(buf, buf2, MAX_STRING_LENGTH);
      send_to_char(buf, ch);
   }
}

// Preserved original warning ridden code :)
#ifdef MS2020
static void stat_dil(const unit_data *ch, const struct zone_type *zone)
{
   char         buf[MAX_STRING_LENGTH];
   diltemplate *tmpl;
   int          i;

   sprintf(buf, "List of DIL in zone %s:\n\r", zone->name);
   send_to_char(buf, ch);

   for(*buf = 0, tmpl = zone->tmpl, i = 0; tmpl; tmpl = tmpl->next)
   {
      sprintf(buf, "%s%6d %-19s", buf, tmpl->nActivations, tmpl->prgname);

      /* Enough to fill a whole line */
      if(++i == 3)
      {
         sprintf(buf, "%s\n\r", buf);
         send_to_char(buf, ch);
         *buf = 0;
         i    = 0;
      }
   }

   if(i)
   {
      sprintf(buf, "%s\n\r", buf);
      send_to_char(buf, ch);
   }
}
#endif

static void extra_stat_zone(unit_data *ch, char *arg, struct zone_type *zone)
{
   char               buf[MAX_STRING_LENGTH];
   char               filename[128];
   int                argno;
   file_index_type   *fi;
   int                search_type = 0;
   int                i;

   void               stat_dijkstraa(unit_data * ch, struct zone_type * z);

   static const char *zone_args[]    = {"mobiles", "objects", "rooms", "reset", "errors", "info", "path", "dil", nullptr};

   static int         search_types[] = {UNIT_ST_NPC, UNIT_ST_OBJ, UNIT_ST_ROOM};

   arg                               = one_argument(arg, buf);
   argno                             = search_block(buf, zone_args, 0);

   if(argno == -1) /* Asked for a specific zone? */
   {
      if((zone = find_zone(buf)) == nullptr)
      {
         send_to_char("Usage: wstat zone [name] "
                      "[mobiles|objects|rooms|reset|info|errors|path]\n\r",
                      ch);
         return;
      }

      arg   = one_argument(arg, buf);
      argno = search_block(buf, zone_args, 0);

      if(argno == -1)
      {
         stat_zone(ch, zone);
         return;
      }
   }

   switch(argno)
   {
      case 0:
      case 1:
      case 2:
         sprintf(buf, "List of %s in zone %s:\n\r", zone_args[argno], zone->name);
         send_to_char(buf, ch);
         search_type = search_types[argno];
         break;

      case 3:
         if(zone->zri != nullptr)
         {
            sprintf(buf, "Reset information for zone %s:\n\r", zone->name);
            send_to_char(buf, ch);
            CREATE(stat_buffer, char, 2 * MAX_STRING_LENGTH);
            stat_p                      = stat_buffer;
            char mbuf[MAX_INPUT_LENGTH] = {0};
            stat_zone_reset(mbuf, zone->zri, ch);
            page_string(CHAR_DESCRIPTOR(ch), stat_buffer);
            free(stat_buffer);
         }
         else
         {
            sprintf(buf, "No reset information for zone %s.\n\r", zone->name);
            send_to_char(buf, ch);
         }
         return;

      case 4:
      case 5:
         /* Errors/Info (Small hack, this :-) ) */
         sprintf(filename, "%s%s.%.3s", zondir, zone->filename, zone_args[argno]);
         if(static_cast<unsigned int>(file_exists(filename)) == 0U)
         {
            return;
         }
         file_to_string(filename, buf, MAX_STRING_LENGTH);
         page_string(CHAR_DESCRIPTOR(ch), buf);
         return;

      case 6:
         stat_dijkstraa(ch, zone);
         break;

      case 7:
         stat_dil(ch, zone);
         break;

      default:
         return;
   }

   char buf2[MAX_STRING_LENGTH];
   /* Search for mobs/objs/rooms and line in columns */
   for(*buf = 0, fi = zone->fi, i = 0; fi != nullptr; fi = fi->next)
   {
      if(fi->type == search_type)
      {
         sprintf(buf2, "%s%-20s", buf, fi->name);
         memcpy(buf, buf2, MAX_STRING_LENGTH); // MS2020 hack
         /* Enough to fill a whole line */
         if(++i == 4)
         {
            sprintf(buf2, "%s\n\r", buf);
            memcpy(buf, buf2, MAX_STRING_LENGTH); // MS2020 hack
            send_to_char(buf, ch);
            *buf = 0;
            i    = 0;
         }
      }
   }
   if(i != 0)
   {
      sprintf(buf2, "%s\n\r", buf);
      memcpy(buf, buf2, MAX_STRING_LENGTH); // MS2020 hack
      send_to_char(buf, ch);
   }
}

static void stat_ability(const unit_data *ch, unit_data *u)
{
   char  buf[MAX_STRING_LENGTH];
   char *b = buf;
   int   i;

   if(!IS_PC(u))
   {
      send_to_char("Unit is not a PC - use 'data' for NPC's\n\r", ch);
      return;
   }

   strcpy(b, "Char Abilities:\n\r");
   TAIL(b);

   for(i = 0; i < ABIL_TREE_MAX; i++)
   {
      sprintf(b, "%20s : %3d%% Lvl %3d Cost %3d\n\r", abil_text[i], CHAR_ABILITY(u, i), PC_ABI_LVL(u, i), PC_ABI_COST(u, i));
      TAIL(b);
   }

   page_string(CHAR_DESCRIPTOR(ch), buf);
}

static void stat_spell(const unit_data *ch, unit_data *u)
{
   char  tmpbuf1[100];
   char  tmpbuf2[100];
   char  buf[100 * (SPL_TREE_MAX + 1)];
   char *b = buf;
   int   i;
   int   max;

   if(!IS_CHAR(u))
   {
      send_to_char("Unit is not a char\n\r", ch);
      return;
   }

   strcpy(b, "Char magic skill\n\r");
   TAIL(b);

   max = IS_NPC(u) ? SPL_GROUP_MAX : SPL_TREE_MAX;

   for(i = 0; i < max; i++)
   {
      str_next_word(TREE_ISLEAF(spl_tree, i) ? spl_text[TREE_PARENT(spl_tree, i)] : "sphere", tmpbuf1);

      if(TREE_ISLEAF(spl_tree, i) && strcmp(tmpbuf1, "sphere") == 0)
      {
         if(spell_info[i].tmpl == nullptr && spell_info[i].spell_pointer == nullptr)
         {
            strcpy(tmpbuf1, "NOT IMPLEMENTED");
         }
      }

      sprintf(tmpbuf2,
              "%s %s (%s)",
              spell_info[i].cast_type == SPLCST_CHECK ? "CHECK " : (spell_info[i].cast_type == SPLCST_RESIST ? "RESIST" : "OTHER "),
              spl_text[i],
              tmpbuf1);

      sprintf(b,
              "%c%c%c%c%c] %3d%%/%d %c %-50s [%3dC]\n\r",
              IS_SET(spell_info[i].media, MEDIA_SPELL) ? 'C' : '-',
              IS_SET(spell_info[i].media, MEDIA_SCROLL) ? 'S' : '-',
              IS_SET(spell_info[i].media, MEDIA_POTION) ? 'P' : '-',
              IS_SET(spell_info[i].media, MEDIA_WAND) ? 'W' : '-',
              IS_SET(spell_info[i].media, MEDIA_STAFF) ? 'R' : '-',
              IS_NPC(u) ? NPC_SPL_SKILL(u, i) : PC_SPL_SKILL(u, i),
              IS_NPC(u) ? 0 : PC_SPL_LVL(u, i),
              spell_info[i].realm == ABIL_DIV ? 'D' : (spell_info[i].realm == ABIL_MAG ? 'M' : '!'),
              tmpbuf2,
              IS_NPC(u) ? 0 : PC_SPL_COST(u, i));
      TAIL(b);
   }

   page_string(CHAR_DESCRIPTOR(ch), buf);
   assert(strlen(buf) < sizeof(buf));
}

static void stat_skill(const unit_data *ch, unit_data *u)
{
   if(!IS_CHAR(u))
   {
      send_to_char("Unit is not a char\n\r", ch);
   }
   else if(IS_NPC(u))
   {
      send_to_char("NPC's have no skills.\n\r", ch);
   }
   else
   {
      char  buf[100 * (SKI_TREE_MAX + 1)];
      char *b = buf;
      int   i;

      strcpy(b, "Other skills:\n\r");
      TAIL(b);

      for(i = 0; i < SKI_TREE_MAX; i++)
      {
         sprintf(b, "%20s: %3d%% Lvl %3d Cost %3d\n\r", ski_text[i], PC_SKI_SKILL(u, i), PC_SKI_LVL(u, i), PC_SKI_COST(u, i));
         TAIL(b);
      }

      page_string(CHAR_DESCRIPTOR(ch), buf);
   }
}

static void stat_wskill(const unit_data *ch, unit_data *u)
{
   char  buf[100 * (WPN_TREE_MAX + 1)];
   char *b = buf;
   int   i;
   int   max;

   if(!IS_CHAR(u))
   {
      send_to_char("Unit is not a char\n\r", ch);
      return;
   }

   strcpy(b, "Char weapon skill:\n\r");
   TAIL(b);

   max = IS_NPC(u) ? WPN_GROUP_MAX : WPN_TREE_MAX;

   for(i = 0; i < max; i++)
   {
      sprintf(b,
              "%20s : %3d%% Lvl %3d Cost %3d\n\r",
              wpn_text[i],
              IS_NPC(u) ? NPC_WPN_SKILL(u, i) : PC_WPN_SKILL(u, i),
              IS_NPC(u) ? 0 : PC_WPN_LVL(u, i),
              IS_NPC(u) ? 0 : PC_WPN_COST(u, i));
      TAIL(b);
   }
   page_string(CHAR_DESCRIPTOR(ch), buf);
}

static void stat_affect(const unit_data *ch, unit_data *u)
{
   unit_affected_type *af;
   char                buf[1024];

   if(UNIT_AFFECTED(u) == nullptr)
   {
      send_to_char("It is not affected by anything.\n\r", ch);
      return;
   }

   send_to_char("Unit affects:\n\r", ch);

   for(af = UNIT_AFFECTED(u); af != nullptr; af = af->next)
   {
      sprintf(buf,
              "----------------------------------------------------\n\r"
              "Id [%d]   Duration [%d]   Beat [%d] Data [%d] [%d] [%d]\n\r"
              "First f() %s\n\r"
              "Tick  f() %s\n\r"
              "Last  f() %s\n\r"
              "Apply f() %s\n\r"
              "%s",
              af->id,
              af->duration,
              af->beat,
              af->data[0],
              af->data[1],
              af->data[2],
              af->firstf_i >= 0 ? STR(tif[af->firstf_i].descr) : "Not used (-1)",
              af->tickf_i >= 0 ? STR(tif[af->tickf_i].descr) : "Not used (-1)",
              af->lastf_i >= 0 ? STR(tif[af->lastf_i].descr) : "Not used (-1)",
              af->applyf_i >= 0 ? STR(apf[af->applyf_i].descr) : "Not used (-1)",
              u == af->owner ? "" : "Serious fuck up in owner pointer!\n\r");

      send_to_char(buf, ch);
   }
}

static void stat_func(const unit_data *ch, unit_data *u)
{
   extern struct unit_function_array_type unit_function_array[];

   char                                   buf[4096];
   char                                   buf2[512];
   unit_fptr                             *f;

   if(UNIT_FUNC(u) == nullptr)
   {
      send_to_char("It has no special routines.\n\r", ch);
      return;
   }

   send_to_char("Unit functions:\n\r", ch);

   for(f = UNIT_FUNC(u); f != nullptr; f = f->next)
   {
      if(f->index == SFUN_DIL_INTERNAL)
      {
         struct dilprg *prg;

         if((prg = (struct dilprg *)f->data) != nullptr)
         {
            sprintf(buf,
                    "DIL Name: %s@%s\n\r",
                    prg->stack[0].tmpl->prgname,
                    prg->stack[0].tmpl->zone != nullptr ? prg->stack[0].tmpl->zone->name : "IDLE");

            send_to_char(buf, ch);
         }
      }

      sprintf(buf,
              "%s Flags [%s] Index [%d] Beat [%d]\n\r"
              "%s\n\r\n\r",
              unit_function_array[f->index].name,
              sprintbit(buf2, f->flags, sfb_flags),
              f->index,
              f->heart_beat,
              f->data != nullptr ? unit_function_array[f->index].save_w_d == SD_ASCII ? (char *)f->data : "Has raw data." : "No data.");
      send_to_char(buf, ch);
   }
}

static void stat_normal(unit_data *ch, unit_data *u)
{
   char buf[MAX_STRING_LENGTH];
   char tmpbuf1[512];
   char tmpbuf2[256];

   /* Stat on the unit */

   UNIT_NAMES(u).catnames(tmpbuf1); /* Get names into tmpbuf1 */

   /* Even though type isn't a flag, we'd better show them all in case
    * more than one is set!
    */
   sprintf(buf,
           "Unit status: %s [%s@%s] %d copies (CRC %lu)\n\rNamelist: %s\n\r"
           "Title: \"%s\"\n\rOutside_descr:\n\r\"%s\"\n\r"
           "Inside_descr:\n\r\"%s\"\n\r",

           sprintbit(tmpbuf2, UNIT_TYPE(u), unit_status),
           UNIT_FI_NAME(u),
           UNIT_FI_ZONENAME(u),
           UNIT_FILE_INDEX(u) != nullptr ? UNIT_FILE_INDEX(u)->no_in_mem : -1,
           UNIT_FILE_INDEX(u) != nullptr ? (unsigned long)UNIT_FILE_INDEX(u)->crc : 0,
           tmpbuf1,
           STR(UNIT_TITLE_STRING(u)),
           STR(UNIT_OUT_DESCR_STRING(u)),
           STR(UNIT_IN_DESCR_STRING(u)));
   send_to_char(buf, ch);

   sprintf(buf,
           "Lights: [%d]  Bright: [%d]  TrnIllu: [%d]  "
           "Chars Within [%d] WIZI [%d]\n\r"
           "Unit is inside: %s -- Unit %s\n\r"
           "Manipulate: %s\n\r"
           "Flags: %s\n\r"
           "Hitpoints/max: [%ld/%ld]  Alignment: [%d]\n\r",

           UNIT_LIGHTS(u),
           UNIT_BRIGHT(u),
           UNIT_ILLUM(u),
           UNIT_CHARS(u),
           UNIT_MINV(u),
           UNIT_IN(u) != nullptr ? STR(TITLENAME(UNIT_IN(u))) : "Nothing",
           UNIT_CONTAINS(u) != nullptr ? "has contents" : "is empty",
           sprintbit(tmpbuf2, UNIT_MANIPULATE(u), unit_manipulate),
           sprintbit(tmpbuf1, UNIT_FLAGS(u), unit_flags),
           (signed long)UNIT_HIT(u),
           (signed long)UNIT_MAX_HIT(u),
           UNIT_ALIGNMENT(u));
   send_to_char(buf, ch);

   sprintf(buf,
           "Key name: [%s]  Open flags: %s\n\r"
           "Base weight : [%d] Weight : [%d] Capacity : [%d] Size [%d]\n\r",

           UNIT_KEY(u) != nullptr ? UNIT_FI_NAME(u) : "none",
           sprintbit(tmpbuf1, UNIT_OPEN_FLAGS(u), unit_open_flags),
           UNIT_BASE_WEIGHT(u),
           UNIT_WEIGHT(u),
           UNIT_CAPACITY(u),
           UNIT_SIZE(u));
   send_to_char(buf, ch);
}

static void stat_extra(const unit_data *ch, extra_descr_data *ed)
{
   /* MS: We used to do a TAIL here... bad idea as newspaper is VERY HUGE */
   /* This isn't nice either, but it works... */

   if(ed != nullptr)
   {
      char tmp[MAX_STRING_LENGTH];

      send_to_char("Extra descriptions:\n\r-------------------\n\r", ch);

      for(; ed != nullptr; ed = ed->next)
      {
         ed->names.catnames(tmp);
         send_to_char("Names ", ch);
         send_to_char(tmp, ch);
         send_to_char("\n\r\"", ch);
         send_to_char(ed->descr.String(), ch);
         send_to_char("\"\n\r-------------------\n\r", ch);
      }
   }
   else
   {
      send_to_char("None.\n\r", ch);
   }
}

static void stat_extra_descr(const unit_data *ch, unit_data *u)
{
   stat_extra(ch, UNIT_EXTRA_DESCR(u));
}

static void stat_extra_quest(const unit_data *ch, unit_data *u)
{
   if(IS_PC(u))
   {
      stat_extra(ch, PC_QUEST(u));
   }
   else
   {
      send_to_char("Quests only on Players.\n\r", ch);
   }
}

static void stat_extra_info(const unit_data *ch, unit_data *u)
{
   if(!IS_ADMINISTRATOR(ch))
   {
      send_to_char("This information is classified for administrators "
                   "only.\n\r",
                   ch);
      return;
   }

   if(IS_PC(u))
   {
      stat_extra(ch, PC_INFO(u));
   }
   else
   {
      send_to_char("Information is only on Players.\n\r", ch);
   }
}

static void stat_ip(const unit_data *ch, unit_data *u)
{
   if(!IS_ADMINISTRATOR(ch))
   {
      send_to_char("This information is classified for administrators "
                   "only.\n\r",
                   ch);
      return;
   }

   if(IS_PC(u))
   {
      char               buf[500];
      struct sockaddr_in sock;

      for(int i = 0; i < 5; i++)
      {
         sock.sin_addr.s_addr = PC_LASTHOST(u)[i];
         sprintf(buf, "IP [%s]\n\r", inet_ntoa(sock.sin_addr));
         send_to_char(buf, ch);
      }
   }
   else
   {
      send_to_char("Information is only on Players.\n\r", ch);
   }
}

#define STR_DATA(num)                                                                                                                      \
   (obj_data[idx].v[num] == 0                                                                                                              \
       ? int_str[num]                                                                                                                      \
       : (obj_data[idx].v[num] == 1 ? (OBJ_VALUE(u, num) ? sprinttype(NULL, OBJ_VALUE(u, num), spl_text) : "None")                         \
                                    : (obj_data[idx].v[num] == 2 ? sprinttype(NULL, OBJ_VALUE(u, num), wpn_text) : "")))

auto stat_obj_data(unit_data *u, struct obj_type_t *obj_data) -> char *
{
   static char result[512];
   const char *special_str = "";
   char        int_str[5][32];
   int         idx = OBJ_TYPE(u);
   int         i;

   switch(idx)
   {
      case ITEM_WEAPON:
         special_str = sprinttype(nullptr, OBJ_VALUE(u, 0), wpn_text);
         break;

      case ITEM_CONTAINER:
         special_str = affected_by_spell(u, ID_CORPSE) != nullptr ? "IS A CORPSE" : "";
         break;

      case ITEM_DRINKCON:
         special_str = sprinttype(nullptr, OBJ_VALUE(u, 2), drinks);
         break;
   }

   if(idx <= 0 || ITEM_SHIELD < idx)
   {
      idx = ITEM_OTHER;
   }

   for(i = 0; i < 5; ++i)
   { /* Init obj-value strings */
      sprintf(int_str[i], "%ld", (signed long)OBJ_VALUE(u, i));
   }

   sprintf(result, obj_data[idx].fmt, STR_DATA(0), STR_DATA(1), STR_DATA(2), STR_DATA(3), STR_DATA(4), special_str);

   return result;
}
#undef STR_DATA

static void stat_data(const unit_data *ch, unit_data *u)
{
   /*  This is a bit tricky:
    *    1: format for the sprintf, where all arguments are %s's.
    *  2-6: correspons to data[0] - data[4]
    *	   0 means print the integer-value
    *	   1 means print the spell associated
    *	   2 means print the weapon-type (not used for wstat())
    *	   3 means print the empty string (i.e. skip)
    *		A 6th trailing format-argument inserts a special string
    *		  as according to the switch following...
    */
   static struct obj_type_t wstat_obj_type[] = {
      {"Unused", {0, 0, 0, 0, 0}},                                /*UNUSED    */
      {"Hours Left %s   Light Sources %s", {0, 0, 0, 0, 0}},      /*LIGHT     */
      {"Power %s\n\rSpells : '%s', '%s', '%s'", {0, 1, 1, 1, 0}}, /*SCROLL    */
      {"Power : %s  Charges : %s\n\r"
       "Spells : '%s', '%s' Max Charge '%s'",
       {0, 0, 1, 1, 1}}, /*WAND      */
      {"Power : %s  Charges : %s\n\r"
       "Spells : '%s', '%s' Max Charge '%s'",
       {0, 0, 1, 1, 1}}, /*STAFF     */
      {"[0] Category (%s)\n\r"
       "[1] Craftsmanship %s\n\r"
       "[2] Magical Bonus %s\n\r"
       "[3] Slaying Race %s\n\r",
       {2, 0, 0, 0, 0}},                                         /*WEAPON    */
      {"Tohit : %s  Todam : %sD%s  Type : %s", {0, 0, 0, 0, 0}}, /*FIREWEAPON*/
      {"Tohit : %s  Todam : %s%s  Type : %s", {0, 0, 3, 0, 0}},  /*MISSILE   */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*TREASURE  */
      {"[0] Armour Category %s\n\r"
       "[1] Craftsmanship %s\n\r"
       "[2] Magical Bonus %s\n\r",
       {0, 0, 0, 0, 0}},                                          /*ARMOR     */
      {"Power: %s  Spells: '%s', '%s', '%s'", {0, 1, 1, 1, 0}},   /*POTION    */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},       /*WORN      */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},       /*OTHER     */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},       /*TRASH     */
      {"Spell : %s  - Hitpoints : %s", {1, 0, 0, 0, 0}},          /*TRAP      */
      {"Data : [%s] [%s] [%s] [%s] [%s]\n\r%s", {0, 0, 0, 0, 0}}, /*CONTAINER */
      {"Tounge : %s", {0, 0, 0, 0, 0}},                           /*NOTE      */
      {"Max-contains %s  Contains %s  "
       "%sPoison Factor: %s  %sLiquid : %s",
       {0, 0, 3, 0, 3}},                                         /*DRINKCON  */
      {"Keytype : %s", {0, 0, 0, 0, 0}},                         /*KEY       */
      {"Makes full %s - %s%sPoison Factor %s", {0, 3, 3, 0, 0}}, /*FOOD      */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*MONEY     */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*PEN       */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*BOAT      */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*SPELL     */
      {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*BOOK      */
      {"[0] Shield Category %s\n\r"
       "[1] Craftsmanship %s\n\r"
       "[2] Magical Bonus %s\n\r",
       {0, 0, 0, 0, 0}} /*SHIELD    */
   };

   char buf[4096];
   char tmpbuf1[256];
   char tmpbuf2[256];
   int  i;

   if(IS_CHAR(u))
   {
      sprintf(buf,
              "Char data:\n\r"
              "Descriptor: %s  Fighting: '%s'\n\r"
              "Master: '%s'  First Follower: '%s'\n\r"
              "Last room: '%s'\n\r"
              "Level [%d]  Sex [%s]  Position [%s]\n\r"
              "Race [%s]  Can Carry W [%d]  Can Carry N [%d]\n\r"
              "Char Flags [%s]\n\r"
              "Exp: [%ld] Offensive Bonus [%d] Defensive Bonus [%d]\n\r"
              "Attack [%s]   Speed [%d]   Natural-Armour [%d]\n\r"
              "HP [H%ld/L%d/G%d]   Mana [H%d/L%d/G%d]  END [H%d/L%d/G%d]\n\r"
              "STR [%d]  DEX [%d]  CON [%d]  CHA [%d]\n\r"
              "BRA [%d]  MAG [%d]  DIV [%d]  HP  [%d]\n\r",

              CHAR_DESCRIPTOR(u) != nullptr ? "Yes" : "No",
              CHAR_FIGHTING(u) ? STR(UNIT_NAME(CHAR_FIGHTING(u))) : "Nobody",
              CHAR_MASTER(u) ? STR(UNIT_NAME(CHAR_MASTER(u))) : "Nobody",
              CHAR_FOLLOWERS(u) ? STR(UNIT_NAME(CHAR_FOLLOWERS(u)->follower)) : "Nobody",
              CHAR_LAST_ROOM(u) != nullptr ? STR(UNIT_TITLE_STRING(CHAR_LAST_ROOM(u))) : "Nowhere",
              CHAR_LEVEL(u),
              sprinttype(nullptr, CHAR_SEX(u), char_sex),
              sprinttype(nullptr, CHAR_POS(u), char_pos),
              IS_PC(u) ? sprinttype(nullptr, CHAR_RACE(u), pc_races) : itoa(CHAR_RACE(u)),
              char_carry_w_limit(u),
              char_carry_n_limit(u),
              sprintbit(tmpbuf1, CHAR_FLAGS(u), char_flags),
              (signed long)CHAR_EXP(u),
              CHAR_OFFENSIVE(u),
              CHAR_DEFENSIVE(u),
              sprinttype(nullptr, CHAR_ATTACK_TYPE(u), wpn_text),
              CHAR_SPEED(u),
              CHAR_NATURAL_ARMOUR(u),
              (signed long)UNIT_HIT(u),
              hit_limit(u),
              hit_gain(u),
              CHAR_MANA(u),
              mana_limit(u),
              mana_gain(u),
              CHAR_ENDURANCE(u),
              move_limit(u),
              move_gain(u),
              CHAR_STR(u),
              CHAR_DEX(u),
              CHAR_CON(u),
              CHAR_CHA(u),
              CHAR_BRA(u),
              CHAR_MAG(u),
              CHAR_DIV(u),
              CHAR_HPP(u));
      send_to_char(buf, ch);

      if(IS_PC(u))
      {
         /* Stat on a player  */
         time_info_data tid1;
         time_info_data tid2;

         tid1 = age(u);
         tid2 = real_time_passed((time_t)PC_TIME(u).played, 0);

         strcpy(tmpbuf2, ctime(&PC_TIME(u).connect));
         sprintf(buf,
                 "----------------- PLAYER -------------------\n\r"
                 "Filename [%s]  Unique ID [%ld]  BBS [%3d]  Cracks [%2d]\n\r"
                 "Skill points: [%ld]  Ability points: [%ld]  CRIMES: [%d]\n\r"
                 "Hometown: [%s]\n\rVLVL: %d (%d lxp) Guild: [%s (%d member)]\n\r"
                 "Drunk: [%d]  Full: [%d]  Thirst: [%d]\n\r"
                 "CCInfo: %s     Setup: %s\n\r"
                 "Age: %dY %dM %dD %dH   Lifespan : %dY   Played: %hdDays %hdHours (%d)\n\r"
                 "Last connected at: %s\r"
                 "Created at       : %s\r",
                 PC_FILENAME(u),
                 (signed long)PC_ID(u),
                 PC_ACCESS_LEVEL(u),
                 PC_CRACK_ATTEMPTS(u),
                 (signed long)PC_SKILL_POINTS(u),
                 (signed long)PC_ABILITY_POINTS(u),
                 PC_CRIMES(u),
                 STR(PC_HOME(u)),
                 PC_VIRTUAL_LEVEL(u),
                 required_xp(PC_VIRTUAL_LEVEL(u) + 1) - required_xp(PC_VIRTUAL_LEVEL(u)),
                 STR(PC_GUILD(u)),
                 PC_TIME(u).played - PC_GUILD_TIME(u),
                 PC_COND(u, DRUNK),
                 PC_COND(u, FULL),
                 PC_COND(u, THIRST),
                 PC_ACCOUNT(u).last4 == -1 ? "NONE" : "SET",
                 sprintbit(tmpbuf1, PC_FLAGS(u), pc_flags),
                 tid1.year,
                 tid1.month,
                 tid1.day,
                 tid1.hours,
                 PC_LIFESPAN(u),
                 tid2.day,
                 tid2.hours,
                 PC_TIME(u).played,
                 tmpbuf2,
                 ctime(&PC_TIME(u).creation));

         send_to_char(buf, ch);
      }
      else /* Stat on a monster */
      {
         sprintf(buf,
                 "---------------- NON PLAYER ----------------\n\r"
                 "Default position: %s\n\r"
                 "NPC-flags: %s\n\r",

                 sprinttype(nullptr, NPC_DEFAULT(u), char_pos),
                 sprintbit(tmpbuf1, NPC_FLAGS(u), npc_flags));
         send_to_char(buf, ch);
      }
   }
   else if(IS_OBJ(u)) /* Stat on an object */
   {
      sprintf(buf,
              "Object data:\n\r"
              "Object type: %s (%d)\n\r"
              "Values: [%4ld] [%4ld] [%4ld] [%4ld] [%4ld]\n\r"
              "Magic resistance [%d]\n\r\n\r"
              "%s\n\r"
              "Extra flags: %s\n\r"
              "Cost: [%lu]  Cost/day: [%lu]  Equipped: %s\n\r",

              sprinttype(nullptr, OBJ_TYPE(u), obj_types),
              OBJ_TYPE(u),
              (signed long)OBJ_VALUE(u, 0),
              (signed long)OBJ_VALUE(u, 1),
              (signed long)OBJ_VALUE(u, 2),
              (signed long)OBJ_VALUE(u, 3),
              (signed long)OBJ_VALUE(u, 4),
              OBJ_RESISTANCE(u),
              stat_obj_data(u, wstat_obj_type),
              sprintbit(tmpbuf1, OBJ_FLAGS(u), obj_flags),
              (unsigned long)OBJ_PRICE(u),
              (unsigned long)OBJ_PRICE_DAY(u),
              sprinttype(nullptr, OBJ_EQP_POS(u), equip_pos));
      send_to_char(buf, ch);
   }
   else /* Stat on a room */
   {
      sprintf(buf,
              "Room data:\n\r"
              "%s [%s@%s]  Sector type: %s\n\r"
              "Magic resistance [%d]\n\rOutside Environment: %s\n\r",

              UNIT_TITLE_STRING(u),
              UNIT_FI_NAME(u),
              UNIT_FI_ZONENAME(u),
              sprinttype(nullptr, ROOM_LANDSCAPE(u), room_landscape),
              ROOM_RESISTANCE(u),
              UNIT_IN(u) != nullptr ? STR(TITLENAME(UNIT_IN(u))) : "Nothing");
      send_to_char(buf, ch);

      for(i = 0; i < 6; i++)
      {
         if(ROOM_EXIT(u, i) != nullptr)
         {
            ROOM_EXIT(u, i)->open_name.catnames(tmpbuf1);
            sprintbit(tmpbuf2, ROOM_EXIT(u, i)->exit_info, unit_open_flags);

            if(ROOM_EXIT(u, i)->to_room != nullptr)
            {
               sprintf(buf,
                       "EXIT %-5s to [%s@%s] (%s)\n\r"
                       "   Exit Name: [%s]\n\r"
                       "   Exit Bits: [%s]\n\r",
                       dirs[i],
                       UNIT_FI_NAME(ROOM_EXIT(u, i)->to_room),
                       UNIT_FI_ZONENAME(ROOM_EXIT(u, i)->to_room),
                       UNIT_TITLE_STRING(ROOM_EXIT(u, i)->to_room),
                       tmpbuf1,
                       tmpbuf2);
            }
            else
            {
               sprintf(buf,
                       "EXIT %-5s to [NOWHERE]\n\r"
                       "   Exit Name: [%s]\n\r"
                       "   Exit Bits: [%s]\n\r",
                       dirs[i],
                       tmpbuf1,
                       tmpbuf2);
            }

            send_to_char(buf, ch);
         }
      }
   }
}

static void stat_contents(const unit_data *ch, unit_data *u)
{
   char buf[MAX_INPUT_LENGTH];

   if(UNIT_CONTAINS(u) != nullptr)
   {
      for(u = UNIT_CONTAINS(u); u != nullptr; u = u->next)
      {
         if(CHAR_LEVEL(ch) >= UNIT_MINV(u))
         {
            sprintf(buf,
                    "[%s@%s] Name '%s', Title '%s'  %s\n\r",
                    UNIT_FI_NAME(u),
                    UNIT_FI_ZONENAME(u),
                    UNIT_NAME(u),
                    UNIT_TITLE_STRING(u),
                    IS_OBJ(u) && (OBJ_EQP_POS(u) != 0u) ? "Equipped" : "");
            send_to_char(buf, ch);
         }
      }
   }
   else
   {
      send_to_char("It is empty.\n\r", ch);
   }
}

static void stat_descriptor(const unit_data *ch, unit_data *u)
{
   send_to_char("Is yet to be programmed.\n\r", ch);
}

void do_wstat(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char               buf[256];
   unit_data         *u    = nullptr;
   struct zone_type  *zone = nullptr;
   int                argno;

   static const char *arguments[] = {"data",
                                     "contents",
                                     "affects",
                                     "descriptor",
                                     "functions",
                                     "spells",
                                     "skills",
                                     "weapons",
                                     "extras",
                                     "quests",
                                     "ability",
                                     "account",
                                     "bank",
                                     "combat",
                                     "info",
                                     "ip",
                                     nullptr};

#define FUNC_ELMS (sizeof functions / sizeof(void (*)(unit_data *, unit_data *)))

   static void (*functions[])(const unit_data *, unit_data *) = {stat_data,
                                                                 stat_contents,
                                                                 stat_affect,
                                                                 stat_descriptor,
                                                                 stat_func,
                                                                 stat_spell,
                                                                 stat_skill,
                                                                 stat_wskill,
                                                                 stat_extra_descr,
                                                                 stat_extra_quest,
                                                                 stat_ability,
                                                                 account_local_stat,
                                                                 stat_bank,
                                                                 stat_combat,
                                                                 stat_extra_info,
                                                                 stat_ip};

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Usage: See help wstat\n\r", ch);
      return;
   }

   if(str_nccmp("room", argument, 4) == 0)
   {
      u = UNIT_IN(ch);
      argument += 4;
   }
   else if(strncmp("zone", argument, 4) == 0)
   {
      zone = unit_zone(ch);
      argument += 4;
   }
   else if(strncmp("memory", argument, 6) == 0)
   {
      stat_memory(ch);
      return;
   }
   else if(strncmp("string", argument, 6) == 0)
   {
      stat_string(ch);
      return;
   }
   else if(strncmp("account", argument, 7) == 0)
   {
      account_global_stat(ch);
      return;
   }
   else if(strncmp("swap", argument, 4) == 0)
   {
      stat_swap(ch);
      return;
   }
   else if(strncmp("creators", argument, 8) == 0)
   {
      argument += 8;
      stat_creators(ch, argument);
      return;
   }
   else if(strncmp("world", argument, 5) == 0)
   {
      argument += 5;
      if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
      {
         stat_world(ch);
      }
      else
      {
         stat_world_extra(ch);
      }
      return;
   }
   else
   {
      file_index_type *fi;

      u = find_unit(ch, &argument, nullptr, FIND_UNIT_GLOBAL);

      if(u == nullptr)
      {
         fi = pc_str_to_file_index(ch, argument);

         if(fi != nullptr)
         {
            if(fi->room_ptr != nullptr)
            {
               u = fi->room_ptr;
            }
            else
            {
               if(fi->no_in_mem == 0)
               {
                  if(!IS_ADMINISTRATOR(ch))
                  {
                     send_to_char("No instances in memory.\n\r", ch);
                  }
                  else
                  {
                     do_load(ch, argument, cmd);
                  }
               }
               u = find_symbolic_instance(fi);
            }
         }
      }
   }

   if((u == nullptr) && (zone == nullptr))
   {
      send_to_char("No such thing anywhere.\n\r", ch);
      return;
   }

   if(zone != nullptr)
   {
      if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
      {
         stat_zone(ch, zone);
      }
      else
      {
         extra_stat_zone(ch, argument, zone);
      }

      return;
   }

   one_argument(argument, buf);
   argno = search_block(buf, arguments, 0);

   if(0 <= argno && argno <= (int)FUNC_ELMS)
   {
      (*(functions[argno]))(ch, u);
   }
   else
   {
      stat_normal(ch, u);
   }
}
