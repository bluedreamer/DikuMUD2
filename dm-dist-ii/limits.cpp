/* *********************************************************************** *
 * File   : limits.c                                  Part of Valhalla MUD *
 * Version: 1.04                                                           *
 * Author : bombman, seifert and quinn (@diku.dk)                          *
 *                                                                         *
 * Purpose: Xp, hit, mana, endurance, food and age gain/modifications.     *
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

/* 23/07/92 seifert: Gain: Mana gain modified, gain fighting=>0 gain       */
/* 01/09/92 seifert: Mana gain now depends on Charisma!                    */
/* 10/04/94 seifert: Mana gain relative to Charisma & level!               */
/* 6/12/94  bombman: Demigods neen no food or drink                        */

#include "comm.h"
#include "common.h"
#include "constants.h"
#include "fight.h"
#include "guild.h"
#include "handler.h"
#include "interpreter.h"
#include "skills.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern unit_data *unit_list;

/* External procedures */

/* Count the number of items a unit contains */
auto              char_carry_n(unit_data *unit) -> int
{
   int i;

   for(unit = UNIT_CONTAINS(unit), i = 0; unit != nullptr; unit = unit->next)
   {
      if(!IS_OBJ(unit) || ((OBJ_EQP_POS(unit) == 0) && (OBJ_TYPE(unit) != ITEM_MONEY)))
      {
         i++;
      }
   }

   return i;
}

auto char_carry_n_limit(unit_data *ch) -> int
{
   return 10 + (CHAR_DEX(ch) / 10);
}

auto char_can_carry_n(unit_data *ch, int n) -> int
{
   return static_cast<int>(char_carry_n_limit(ch) >= (char_carry_n(ch) + n));
}

auto char_carry_w_limit(unit_data *ch) -> int
{
   return 50 + std::max(50, UNIT_BASE_WEIGHT(ch) / 2) + CHAR_STR(ch) * 2;
}

auto char_can_carry_w(unit_data *ch, int weight) -> int
{
   return static_cast<int>(char_carry_w_limit(ch) >= (UNIT_CONTAINING_W(ch) + weight));
}

auto char_can_carry_unit(unit_data *ch, unit_data *unit) -> int
{
   return static_cast<int>((char_can_carry_w(ch, UNIT_WEIGHT(unit)) != 0) && (char_can_carry_n(ch, 1) != 0));
}

auto char_can_get_unit(unit_data *ch, unit_data *unit) -> int
{
   return UNIT_WEAR((unit), MANIPULATE_TAKE) && CHAR_CAN_SEE(ch, unit) && (char_can_carry_unit(ch, unit) != 0);
}

auto char_drag_w_limit(unit_data *ch) -> int
{
   return (3 * char_carry_w_limit(ch));
}

auto char_can_drag_w(unit_data *ch, int weight) -> int
{
   return static_cast<int>(char_drag_w_limit(ch) >= weight);
}

/* For lifespan 100 (Human), result is                     */
/* When age in [ 0..16] calculate the line between p0 & p1 */
/* When age in [17..32] calculate the line between p1 & p2 */
/* When age in [33..48] calculate the line between p2 & p3 */
/* When age in [49..64] calculate the line between p3 & p4 */
/* When age in [65..80] calculate the line between p4 & p5 */
/* When age in [81..mx] calculate the line between p5 & p6 */
/* When age > mx return p7                                 */

auto age_graph(int age, int lifespan, int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7) -> int
{
   int step = std::max(1, lifespan / 6);

   if(age <= step)
   {
      return (int)(p0 + (((age) * (p1 - p0)) / step));
   }
   if(age <= 2 * step)
   {
      return (int)(p1 + (((age - step) * (p2 - p1)) / step));
   }
   if(age <= 3 * step)
      return (int)(p2 + (((age - 2 * step) * (p3 - p2)) / step));
   else if(age <= 4 * step)
      return (int)(p3 + (((age - 3 * step) * (p4 - p3)) / step));
   else if(age <= 5 * step)
      return (int)(p4 + (((age - 4 * step) * (p5 - p4)) / step));
   else
   {
      if(age > lifespan)
         return p7;
      else if(age >= lifespan)
         return p6;
      else
         return (int)(p5 + (((age - 5 * step) * (p6 - p5)) / step));
   }
}

/* This function is copied into basis.zon - remember to update accordingly!! */

static auto hit_limit_number(unit_data *ch, int point) -> int
{
   if(IS_PC(ch))
   {
      if(IS_IMMORTAL(ch))
      { // MS2020
         return 3 * point + 20;
      }

      if(age(ch).year > PC_LIFESPAN(ch))
      {
         return -1;
      }

      return 3 * point + 20;
   }
   return 3 * point + 10;
}

auto hit_limit(unit_data *ch) -> int
{
   return hit_limit_number(ch, CHAR_HPP(ch));
}

/* Hitpoint gain pr. game hour */
auto hit_gain(unit_data *ch) -> int
{
   int gain;

   assert(IS_CHAR(ch));

   /* 10 turns to regenerate */
   if(CHAR_POS(ch) != POSITION_FIGHTING)
   {
      gain = 1 + hit_limit_number(ch, CHAR_CON(ch)) / 10;
   }
   else
   {
      gain = 0;
   }

   switch(CHAR_POS(ch))
   {
      case POSITION_SLEEPING:
      case POSITION_RESTING:
         gain += gain / 2; /* gain *= 1.5  */
         break;

      case POSITION_SITTING:
         gain += gain / 4; /* gain *= 1.25 */
         break;
   }

   unit_data *u = ch;

   while(u != nullptr)
   {
      if(IS_SET(UNIT_FLAGS(u), UNIT_FL_SACRED))
      {
         gain *= 2;
         break;
      }
      u = UNIT_IN(u);
   }

   if(IS_PC(ch))
   {
      /* gain = graf(age(ch).year, 2,5,10,18,6,4,2); */
      if((PC_COND(ch, FULL) < 0) || (PC_COND(ch, THIRST) < 0))
      {
         gain += 3 * std::min(static_cast<int>(PC_COND(ch, FULL)), 3 * PC_COND(ch, THIRST));
      }
   }

   return gain;
}

auto move_limit(unit_data *ch) -> int
{
   int ml = CHAR_CON(ch) * 2 + 150;

   if(IS_PC(ch))
   {
      int pct;

      if(IS_IMMORTAL(ch))
      { // MS2020
         return ml;
      }

      pct = age_graph(age(ch).year, PC_LIFESPAN(ch), 80, 130, 110, 90, 70, 50, 20, 0);

      return (pct * ml) / 100; /* actually it is (100 * pct) / 100 */
   }
   return ml;
}

/* move gain pr. game hour */
auto move_gain(unit_data *ch) -> int
{
   int gain;

   assert(IS_CHAR(ch));

   if(CHAR_POS(ch) != POSITION_FIGHTING)
   {
      gain = 1 + move_limit(ch) / 10; /* 10 turns to regenerate */
   }
   else
   {
      gain = 0;
   }

   /* Position calculations    */
   switch(CHAR_POS(ch))
   {
      case POSITION_SLEEPING:
      case POSITION_RESTING:
         gain += gain / 2; /* Divide by 4 */
         break;

      case POSITION_SITTING:
         gain += gain / 4; /* Divide by 8 */
         break;
   }

   unit_data *u = ch;

   while(u != nullptr)
   {
      if(IS_SET(UNIT_FLAGS(u), UNIT_FL_SACRED))
      {
         gain *= 2;
         break;
      }
      u = UNIT_IN(u);
   }

   if(IS_PC(ch))
   {
      /* gain = graf(age(ch).year, ... Age calcs? */
      if((PC_COND(ch, FULL) < 0) || (PC_COND(ch, THIRST) < 0))
      {
         gain += 3 * std::min(static_cast<int>(PC_COND(ch, FULL)), 3 * PC_COND(ch, THIRST));
      }
   }

   return gain;
}

auto mana_limit(unit_data *ch) -> int
{
   assert(IS_CHAR(ch));

   int ml = std::min(200, 100 + (CHAR_BRA(ch) + CHAR_CHA(ch)) / 2);

   if(IS_PC(ch))
   {
      int pct;

      if(IS_IMMORTAL(ch))
      { // MS2020
         return ml;
      }

      pct = age_graph(age(ch).year, PC_LIFESPAN(ch), 0, 100, 105, 110, 120, 130, 140, 0);

      return (ml * pct) / 100;
   }
   return ml;
}

/* manapoint gain pr. game hour */
auto mana_gain(unit_data *ch) -> int
{
   int gain;

   assert(IS_CHAR(ch));

   /* 10 turns to regenerate is default when CHA == LEVEL */
   if(CHAR_POS(ch) != POSITION_FIGHTING)
   {
      gain = 1 + mana_limit(ch) / 10;
      gain += (CHAR_CHA(ch) - std::max(CHAR_MAG(ch), CHAR_DIV(ch))) / 3;
      gain = std::max(1, gain);
   }
   else
   {
      gain = 0;
   }

   switch(CHAR_POS(ch))
   {
      case POSITION_SLEEPING:
      case POSITION_RESTING:
         gain += gain / 2; /* Divide by 2 */
         break;
      case POSITION_SITTING:
         gain += gain / 4; /* Divide by 4 */
         break;
   }

   unit_data *u = ch;

   while(u != nullptr)
   {
      if(IS_SET(UNIT_FLAGS(u), UNIT_FL_SACRED))
      {
         gain *= 2;
         break;
      }
      u = UNIT_IN(u);
   }

   if(IS_PC(ch))
   {
      if((PC_COND(ch, FULL) < 0) || (PC_COND(ch, THIRST) < 0))
      {
         gain += 3 * std::min(static_cast<int>(PC_COND(ch, FULL)), 3 * PC_COND(ch, THIRST));
      }
   }

   return gain;
}

/* Gain maximum in various points */
void advance_level(unit_data *ch)
{
   void clear_training_level(unit_data * ch);

   assert(IS_PC(ch));

   if(IS_IMMORTAL(ch))
   {
      return;
   }

#ifdef NOBLE
   if(IS_NOBLE(ch))
      return;
#endif

   send_to_char("You raise a level!\n\r", ch);
   clear_training_level(ch);

   advance_guild_level(ch);
   PC_VIRTUAL_LEVEL(ch)++;
   PC_SKILL_POINTS(ch) += skill_point_gain();

   if(CHAR_LEVEL(ch) < MORTAL_MAX_LEVEL)
   {
      PC_ABILITY_POINTS(ch) += ability_point_gain();
      CHAR_LEVEL(ch)++;
   }

#ifdef NOBLE
   if(IS_NOBLE(ch))
      send_to_char("You become a noble, seek audience "
                   "with King Welmar.\n\r",
                   ch);

#endif
}

void gain_condition(unit_data *ch, int condition, int value)
{
   bool intoxicated;

   if(!IS_PC(ch) || (PC_COND(ch, condition) >= 48))
   {
      return;
   }

   /* No change in sacred rooms */

   intoxicated = (PC_COND(ch, DRUNK) > 0);

   PC_COND(ch, condition) += value;

   PC_COND(ch, condition) = std::min(24, static_cast<int>(PC_COND(ch, condition)));

   if(condition == DRUNK)
   { /* How can one be less sober than 0? */
      PC_COND(ch, condition) = std::max(0, static_cast<int>(PC_COND(ch, condition)));
   }
   else
   {
      PC_COND(ch, condition) = std::max(-96, static_cast<int>(PC_COND(ch, condition)));
   }

   if(PC_COND(ch, condition) > 3)
   {
      return;
   }

   switch(condition)
   {
      case FULL:
         if(condition > -4)
         {
            send_to_char("You are hungry.\n\r", ch);
         }
         else if(condition > -8)
         {
            send_to_char("You are very hungry.\n\r", ch);
         }
         else if(condition > -12)
         {
            send_to_char("You are starving.\n\r", ch);
         }
         break;

      case THIRST:
         if(condition > -4)
         {
            send_to_char("You are thirsty.\n\r", ch);
         }
         else if(condition > -8)
         {
            send_to_char("You are very thirsty.\n\r", ch);
         }
         else if(condition > -12)
         {
            send_to_char("You are dehydrated.\n\r", ch);
         }
         break;

      case DRUNK:
         if(intoxicated && (PC_COND(ch, DRUNK) == 0))
         {
            send_to_char("You are now sober.\n\r", ch);
         }
         break;

      default:
         break;
   }
}

/* Update both PC's & NPC's */
void point_update()
{
   unit_data *u;
   unit_data *next_dude;
   int        hgain;

   /* characters */
   for(u = unit_list; u != nullptr; u = next_dude)
   {
      next_dude = u->gnext;
      if(IS_NPC(u) || (IS_PC(u) && !IS_SET(PC_FLAGS(u), PC_SPIRIT)))
      {
         if(CHAR_POS(u) >= POSITION_STUNNED)
         {
            CHAR_MANA(u)      = std::min(CHAR_MANA(u) + mana_gain(u), mana_limit(u));

            CHAR_ENDURANCE(u) = std::min(CHAR_ENDURANCE(u) + move_gain(u), move_limit(u));

            hgain             = hit_gain(u);
            if(hgain >= 0)
            {
               UNIT_HIT(u) = std::min(UNIT_HIT(u) + hgain, hit_limit(u));
            }
            else
            {
               damage(u, u, nullptr, -hgain, MSG_TYPE_OTHER, MSG_OTHER_STARVATION, COM_MSG_EBODY);
            }

            CHAR_MANA(u)      = std::max(static_cast<int16_t>(0), CHAR_MANA(u));
            CHAR_ENDURANCE(u) = std::max(static_cast<int16_t>(0), CHAR_ENDURANCE(u));

            if(CHAR_POS(u) == POSITION_STUNNED)
            {
               update_pos(u);
            }
         }
         else if(CHAR_POS(u) == POSITION_INCAP)
         {
            damage(u, u, nullptr, 1, MSG_TYPE_OTHER, MSG_OTHER_BLEEDING, COM_MSG_EBODY);
         }
         else if(IS_PC(u) && (CHAR_POS(u) == POSITION_MORTALLYW))
         {
            damage(u, u, nullptr, 2, MSG_TYPE_OTHER, MSG_OTHER_BLEEDING, COM_MSG_EBODY);
         }
      }
   }
}

void food_update()
{
   unit_data *u;
   unit_data *next_dude;

   /* characters */
   for(u = unit_list; u != nullptr; u = next_dude)
   {
      next_dude = u->gnext;

      if(IS_PC(u) && (CHAR_DESCRIPTOR(u) != nullptr))
      {
         unit_data *tu = u;

         while(tu != nullptr)
         {
            if(IS_SET(UNIT_FLAGS(tu), UNIT_FL_SACRED))
            {
               break;
            }
            tu = UNIT_IN(tu);
         }

         if(tu != nullptr)
         {
            continue;
         }

         if(PC_COND(u, FULL) < 48)
         {
            gain_condition(u, FULL, -1);
         }
         if(PC_COND(u, THIRST) < 48)
         {
            gain_condition(u, THIRST, -1);
         }
         if(PC_COND(u, DRUNK) > 0)
         {
            gain_condition(u, DRUNK, -1);
         }
      }
   }
}

void set_title(unit_data *ch)
{
   char buf[256];

   assert(IS_PC(ch));

   if(CHAR_LEVEL(ch) == 0)
   {
      UNIT_TITLE(ch).Reassign("the guest");
   }
   else if(CHAR_LEVEL(ch) <= START_LEVEL)
   {
      assert(CHAR_RACE(ch) < PC_RACE_MAX);
      sprintf(buf, "the %s", pc_races[CHAR_RACE(ch)]);
      UNIT_TITLE(ch).Reassign(buf);
   }
   else if(IS_IMMORTAL(ch))
   {
      UNIT_TITLE(ch).Reassign("the God");
   }
}

void gain_exp_regardless(unit_data *ch, int gain)
{
   int j;

   if(!IS_PC(ch))
   {
      return;
   }

   if(gain > 0)
   {
      j = required_xp(PC_VIRTUAL_LEVEL(ch) + 1);

      if((CHAR_EXP(ch) + gain >= j) && (CHAR_EXP(ch) < j))
      {
         send_to_char("You can now raise a level by using the 'level' "
                      "command!\n\r",
                      ch);
      }

      CHAR_EXP(ch) += gain;
   }
   else /* gain <= 0 */
   {
      if((((int32_t)CHAR_EXP(ch)) + gain) < required_xp(START_LEVEL))
      {
         CHAR_EXP(ch) = required_xp(START_LEVEL);
      }
      else
      {
         CHAR_EXP(ch) += gain;
      }
   }
}

void gain_exp(unit_data *ch, int gain)
{
   if(IS_MORTAL(ch))
   {
      gain_exp_regardless(ch, gain);
   }
}

void do_level(unit_data *ch, char *arg, const struct command_info *cmd)
{
   int now;

   if(!IS_PC(ch))
   {
      send_to_char("You ain't nothin' but a hound-dog.\n\r", ch);
      return;
   }

#ifdef NOBLE
   if(CHAR_LEVEL(ch) >= NOBLE_OCCURANCE)
   {
      send_to_char("You can't raise levels this way.\n\r", ch);
      return;
   }
#endif

   if(CHAR_LEVEL(ch) > MORTAL_MAX_LEVEL)
   {
      send_to_char("You can't raise levels this way.\n\r", ch);
      return;
   }

   if(required_xp(1 + PC_VIRTUAL_LEVEL(ch)) > CHAR_EXP(ch))
   {
      send_to_char("You are not experienced enough.\n\r", ch);
      return;
   }

   now = static_cast<int>(str_ccmp_next_word(arg, "now") != nullptr);

   if((now == 0) && PC_SKILL_POINTS(ch) >= skill_point_gain())
   {
      send_to_char("You havn't used your skill points at all, if you "
                   "really want to level now, type 'level now'\n\r",
                   ch);
      return;
   }

   if((now == 0) && PC_ABILITY_POINTS(ch) >= ability_point_gain())
   {
      send_to_char("You havn't used your ability points at all, if you "
                   "really want to level now, type 'level now'\n\r",
                   ch);
      return;
   }

   advance_level(ch);
}
