/* *********************************************************************** *
 * File   : convert.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Conversion of playerfiles.                                     *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

/* 09/09/93 seifert : Changed to accomodate blkfile V2                     */
/*          seifert : Changed to accomodate single player files.           */

#include <cctype>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "affect.h"
#include "blkfile.h"
#include "common.h"
#include "db.h"
#include "db_file.h"
#include "files.h"
#include "handler.h"
#include "money.h"
#include "pcsave.h"
#include "postoffice.h"
#include "skills.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include "zon_basis.h"

auto save_contents(const char *pFileName, unit_data *unit, int fast, int bContainer) -> int;
auto player_exists(const char *pName) -> int;
auto delete_player(const char *pName) -> int;
auto delete_inventory(const char *pName) -> int;
void save_player_file(unit_data *pc);

char   **player_name_list = nullptr;
int      max_id           = -1;
int      top_id           = -1;
uint8_t *ids              = nullptr; /* For checking duplicate players... */

#define OUTPUT_DIR "lib/"

void convert_free_unit(unit_data *u)
{
   while(UNIT_CONTAINS(u) != nullptr)
   {
      convert_free_unit(UNIT_CONTAINS(u));
   }

   UNIT_AFFECTED(u) = nullptr;
   UNIT_FUNC(u)     = nullptr;

   unit_from_unit(u);
   remove_from_unit_list(u);

   delete u;
}

void free_inventory(unit_data *u)
{
   unit_data *tmp;
   unit_data *nxt;

   for(tmp = u; tmp != nullptr; tmp = nxt)
   {
      nxt = tmp->next;
      convert_free_unit(tmp);
   }
}

auto days_old(time_t last_logon) -> int
{
   return (int)(difftime(time(nullptr), last_logon) / SECS_PER_REAL_DAY);
}

auto convert_item(unit_data *u, unit_data *pc, int bList) -> unit_data *
{
   unit_data *nu = u;

   if(bList != 0)
   {
      if(strcmp(UNIT_FI_ZONE(u)->name, "treasure") == 0)
      {
         printf("\n%s@treasure", UNIT_FI_NAME(u));
      }
      return u;
   }

   if(IS_OBJ(u))
   {
      UNIT_SIZE(u) = UNIT_SIZE(pc);
   }

   return u;

#ifdef SUSPEKT
   if(!IS_MONEY(u))
   {
      if(UNIT_FILE_INDEX(u))
      {
         if(!find_fptr(u, SFUN_DICTIONARY))
            nu = read_unit(UNIT_FILE_INDEX(u));
      }
      else
      {
         extern file_index_type *slime_fi;

         nu = read_unit(slime_fi);
      }
   }

   return nu;

#endif

#ifdef DILREPLACE
   /* This code replaces all DIL items... */
   if(find_fptr(u, SFUN_DIL_INTERNAL))
   {
      unit_data *nu;

      if(UNIT_FILE_INDEX(u))
      {
         nu = read_unit(UNIT_FILE_INDEX(u));
         slog(LOG_ALL, 0, "Replaced DIL item.");
      }
      else
      {
         extern file_index_type *slime_fi;

         nu = read_unit(slime_fi);
         slog(LOG_ALL, 0, "Slimed DIL item.");
      }

      unit_to_unit(nu, UNIT_IN(u));

      convert_free_unit(u);

      u = nu;
   }

   return u;
#endif
}

void convert_inventory(unit_data *u, unit_data *pc, int bList = static_cast<int>(FALSE))
{
   unit_data *bla;

   if(u == nullptr)
   {
      return;
   }

   convert_inventory(UNIT_CONTAINS(u), pc, bList);

   convert_inventory(u->next, pc, bList);

   bla = convert_item(u, pc, bList);

   assert(!is_destructed(DR_UNIT, u));

   if(bla != u)
   {
      unit_data *tmpu;

      assert(UNIT_IN(u));

      unit_to_unit(bla, UNIT_IN(u));

      while(UNIT_CONTAINS(u) != nullptr)
      {
         tmpu = UNIT_CONTAINS(u);
         unit_from_unit(tmpu);
         unit_to_unit(tmpu, bla);
      }

      unit_from_unit(u);
      convert_free_unit(u);
   }

#ifdef SUSPEKT
   unit_data *tmp, *nxt, *bla;

   for(tmp = u; tmp; tmp = nxt)
   {
      convert_inventory(UNIT_CONTAINS(tmp));

      bla = convert_item(tmp);

      nxt = tmp->next;

      if(bla != tmp)
      {
         unit_to_unit(bla, UNIT_IN(tmp));
         convert_free_unit(tmp);
      }
   }
#endif
}

/* This procedure makes any conversion you might want on every player in   *
 * the playerfile, including affects and inventory                         */
void convert_player(unit_data *pc)
{
   extra_descr_data *exd;
   extra_descr_data *nextexd;

   int lvl;

   void race_cost(unit_data * ch);
   void reroll(unit_data * victim);

   assert(IS_PC(pc));

   /* for (exd = PC_QUEST(pc); exd; exd = nextexd)
      {
         nextexd = exd->next;

         if (exd->names.Name())
         {
            if (*exd->names.Name() == '$')
              PC_QUEST(pc) = PC_QUEST(pc)->remove(PC_QUEST(pc)->names.Name());
            else if (strstr(exd->names.Name(), "Quitter"))
              PC_QUEST(pc) = PC_QUEST(pc)->remove(PC_QUEST(pc)->names.Name());
         }
      }

      lvl = MAX(1,CHAR_LEVEL(pc) / 4);
      reroll(pc);
      CHAR_EXP(pc) = required_xp(lvl);
   */
}

/* Return TRUE if Ok. */
auto sanity_check(unit_data *u) -> int
{
   void race_adjust(unit_data * ch);

   if(g_nCorrupt == static_cast<int>(TRUE))
   {
      printf("Corrupted unit in READ.");
      return static_cast<int>(FALSE);
   }

   if(PC_ID(u) >= top_id)
   {
      printf("Maximum ID exceeded!");
      return static_cast<int>(FALSE);
   }

   if(PC_ID(u) < 0)
   {
      printf("Illegal ID < 0!");
      return static_cast<int>(FALSE);
   }

   if((UNIT_HIT(u) > 10000) || (UNIT_MAX_HIT(u) > 10000))
   {
      printf("Corrupted UNIT HITPOINTS");
      return static_cast<int>(FALSE);
   }

   if(!IS_PC(u))
   {
      printf("Not a player!");
      return static_cast<int>(FALSE);
   }

   if(PC_TIME(u).creation > time(nullptr))
   {
      printf("Corrupted creation time.");
      return static_cast<int>(FALSE);
   }

   if(PC_TIME(u).connect > time(nullptr))
   {
      printf("Corrupted connect time.");
      return static_cast<int>(FALSE);
   }

   if(CHAR_RACE(u) >= PC_RACE_MAX)
   {
      printf("Corrupted RACE");
      return static_cast<int>(FALSE);
   }

   if((UNIT_CONTAINS(u) == nullptr) && (UNIT_WEIGHT(u) != UNIT_BASE_WEIGHT(u)))
   {
      printf("Fixed illegal weight.");
      UNIT_WEIGHT(u) = UNIT_BASE_WEIGHT(u);
   }

   return static_cast<int>(TRUE);
}

auto shall_delete(unit_data *pc) -> int
{
   int days;

   days = days_old(PC_TIME(pc).connect);

   /* Player which have paid at some point in time remain almost permanent. */
   if(PC_ACCOUNT(pc).total_credit > 0)
   {
      return static_cast<int>(FALSE);
   }

   if(days > 360)
   {
      printf("360 Days DEL");
      return static_cast<int>(TRUE);
   }

   if(CHAR_LEVEL(pc) < START_LEVEL)
   {
      printf(" LVL 1/2 DEL");
      return static_cast<int>(TRUE);
   }

   if((days > 14) && (CHAR_EXP(pc) <= required_xp(START_LEVEL)) && IS_MORTAL(pc))
   {
      printf(" 14 Days DEL (unplayed newbie)");
      return static_cast<int>(TRUE);
   }

   if((days > 30) && (CHAR_LEVEL(pc) <= START_LEVEL + 1))
   {
      printf(" 30 Days DEL (+1 level)");
      return static_cast<int>(TRUE);
   }

   if((days > 80) && (CHAR_LEVEL(pc) <= START_LEVEL + 3))
   {
      printf("80 Days DEL (+2 .. +3 levels)");
      return static_cast<int>(TRUE);
   }

   if((days > 180) && (CHAR_LEVEL(pc) <= START_LEVEL + 8))
   {
      printf("180 Days DEL (+4 .. +8 levels)");
      return static_cast<int>(TRUE);
   }

   return static_cast<int>(FALSE);
}

auto shall_exclude(const char *name) -> int
{
   char buf[256];

   auto _parse_name(const char *arg, char *name)->int;

   if(static_cast<int>(static_cast<int>(_parse_name(name, buf)) == 0) == 0)
   {
      return static_cast<int>(TRUE);
   }

   return static_cast<int>(FALSE);
}

auto convert_load_player(char *name) -> unit_data *
{
   unit_data *ch;

   if(player_exists(name) == 0)
   {
      /* printf("No such player.\n"); */
      return nullptr;
   }

   ch = load_player(name);

   if(ch == nullptr)
   {
      return nullptr;
   }

   insert_in_unit_list(ch);
   unit_to_unit(ch, destroy_room);

   if(shall_exclude(name) != 0)
   {
      printf("EXCLUDED.\n");
      convert_free_unit(ch);
      return nullptr;
   }

   if(sanity_check(ch) == 0)
   {
      printf("SANITY ERROR.\n");
      convert_free_unit(ch);
      return nullptr;
   }

   if(PC_ID(ch) > top_id)
   {
      printf("TOP ID ERROR %d vs %d.\n", PC_ID(ch), top_id);
      convert_free_unit(ch);
      return nullptr;
   }

   if(PC_TIME(ch).played > SECS_PER_REAL_DAY * 180)
   {
      printf("Had played over 180 days! Wierd!");
   }

   if(PC_ID(ch) > max_id)
   {
      max_id = PC_ID(ch);
   }

   return ch;
}

void list()
{
   long       total_time = 0;
   int        i;
   int        years;
   int        days;
   unit_data *pc;
   auto      *void_char = new(unit_data)(UNIT_ST_NPC);

   for(i = 0; player_name_list[i] != nullptr; i++)
   {
      printf("%-15s: ", player_name_list[i]);

      pc = convert_load_player(player_name_list[i]);
      if(pc == nullptr)
      {
         printf("ERROR: Corrupt\n");
         continue;
      }

      if(str_ccmp(player_name_list[i], UNIT_NAME(pc)) != 0)
      {
         printf("ERROR: Name Mismatch");
         continue;
      }

      printf("Id [%4d]  Lvl [%3d]  %-3s (%3d days)", PC_ID(pc), CHAR_LEVEL(pc), IS_MORTAL(pc) ? "   " : (IS_GOD(pc) ? "GOD" : "DEM"),
             days_old(PC_TIME(pc).connect));

      if(ids[PC_ID(pc)] != 0U)
      {
         printf("Duplicate ID! (%ld)", (signed long)PC_ID(pc));
      }
      else
      {
         ids[PC_ID(pc)] = 1;
      }

      total_time += PC_TIME(pc).played;

      if(shall_exclude(UNIT_NAME(pc)) != 0)
      {
         printf(" WILL DELETE (EXCLUDED)");
      }

      if(shall_delete(pc) != 0)
      {
         printf(" WILL DELETE (AGE)");
      }

      UNIT_CONTAINS(void_char) = nullptr;
      load_contents(player_name_list[i], void_char);

      if(UNIT_CONTAINS(void_char) != nullptr)
      {
         printf("  INV");
         convert_inventory(UNIT_CONTAINS(void_char), pc, static_cast<int>(TRUE));
         free_inventory(UNIT_CONTAINS(void_char));
         if(days_old(PC_TIME(pc).connect) > 60)
         {
            printf("-DEL");
         }
      }

      printf("\n");

      convert_free_unit(pc);
   }

   years = total_time / SECS_PER_REAL_YEAR;
   days  = (total_time / SECS_PER_REAL_DAY) - years * 365;

   printf("Complete Playing Time is %d years, %d days\n", years, days);
   printf("Maximum ID was %d / Top %d\n", max_id, top_id);
}

void convert_file()
{
   int        i;
   unit_data *pc;
   auto      *void_char = new(unit_data)(UNIT_ST_NPC);

   for(i = 0; player_name_list[i] != nullptr; i++)
   {
      printf("\n%-15s: ", player_name_list[i]);

      pc = convert_load_player(player_name_list[i]);

      if(pc == nullptr)
      {
         printf("Corrupt Player ERASED.");
         delete_player(player_name_list[i]);
         continue;
      }

      if(str_ccmp(player_name_list[i], UNIT_NAME(pc)) != 0)
      {
         printf("NAME MISMATCH - ERASED.");
         convert_free_unit(pc);
         delete_player(player_name_list[i]);
         continue;
      }

      if(ids[PC_ID(pc)] != 0U)
      {
         printf("Duplicate ID! (%ld)", (signed long)PC_ID(pc));
      }
      else
      {
         ids[PC_ID(pc)] = 1;
      }

      printf("%-15s Lvl [%3d] %-3s", UNIT_NAME(pc), CHAR_LEVEL(pc), IS_MORTAL(pc) ? "   " : (IS_GOD(pc) ? "GOD" : "DEM"));

      if(shall_delete(pc) != 0)
      {
         convert_free_unit(pc);
         delete_player(player_name_list[i]);
         continue;
      }
      fflush(stdout);

      convert_player(pc);
      save_player_file(pc);

      UNIT_CONTAINS(void_char) = nullptr;
      load_contents(player_name_list[i], void_char);
      if(UNIT_CONTAINS(void_char) != nullptr)
      {
         printf("  INV");

         if(days_old(PC_TIME(pc).connect) <= 60)
         {
            convert_inventory(UNIT_CONTAINS(void_char), pc);
            assert(!is_destructed(DR_UNIT, void_char));
            save_contents(player_name_list[i], void_char, static_cast<int>(FALSE), static_cast<int>(FALSE));
         }
         else
         {
            printf("-DELETE");
            delete_inventory(player_name_list[i]);
         }
         free_inventory(UNIT_CONTAINS(void_char));
         assert(!is_destructed(DR_UNIT, void_char));
      }
      convert_free_unit(pc);
   }
}

void cleanup()
{
   int        i;
   unit_data *pc;
   auto      *void_char = new(unit_data)(UNIT_ST_NPC);

   for(i = 0; player_name_list[i] != nullptr; i++)
   {
      printf("\n%-15s: ", player_name_list[i]);

      pc = convert_load_player(player_name_list[i]);

      if(pc == nullptr)
      {
         printf("Corrupt Player ERASED.");
         delete_player(player_name_list[i]);
         continue;
      }

      if(str_ccmp(player_name_list[i], UNIT_NAME(pc)) != 0)
      {
         printf("NAME MISMATCH - ERASED.");
         convert_free_unit(pc);
         delete_player(player_name_list[i]);
         continue;
      }

      printf("%-15s Lvl [%3d] %-3s", UNIT_NAME(pc), CHAR_LEVEL(pc), IS_MORTAL(pc) ? "   " : (IS_GOD(pc) ? "GOD" : "DEM"));

      if(shall_delete(pc) != 0)
      {
         convert_free_unit(pc);
         delete_player(player_name_list[i]);
         continue;
      }

      fflush(stdout);

      UNIT_CONTAINS(void_char) = nullptr;
      load_contents(player_name_list[i], void_char);
      if(UNIT_CONTAINS(void_char) != nullptr)
      {
         printf("  INV");
         if(days_old(PC_TIME(pc).connect) > 60)
         {
            printf("-DELETE");
            delete_inventory(player_name_list[i]);
         }
         free_inventory(UNIT_CONTAINS(void_char));
         assert(!is_destructed(DR_UNIT, void_char));
      }

      convert_free_unit(pc);
   }
}

void cleanup_playerfile(int argc, char *argv[])
{
   char c;

   extern unit_data *entry_room;

   top_id = read_player_id();
   CREATE(ids, uint8_t, top_id + 1);

   memset(ids, 0, top_id);
   printf("\n\n\n");

   entry_room   = new(unit_data)(UNIT_ST_ROOM);
   destroy_room = new(unit_data)(UNIT_ST_ROOM);

   printf("   Z) Convert players (do not use unless you are told to)\n"
          "   L)ist (integrity check & preview cleanup)\n"
          "   C)leanup players (erase old ones)\n\n"
          "   Select: ");

   int ms_tmp = fscanf(stdin, "%c", &c);
   printf("\n\n");

   if(c == 'Z')
   {
      convert_file();
   }
   else if(toupper(c) == 'C')
   {
      cleanup();
   }
   else
   {
      list();
   }

   printf("\n\nFinished.\n");
}
