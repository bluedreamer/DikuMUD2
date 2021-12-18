/* *********************************************************************** *
 * File   : guild.c                                   Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Basic special routines handling guilds.                        *
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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "affect.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "magic.h"
#include "money.h"
#include "movement.h"
#include "skills.h"
#include "spells.h"
#include "str_parse.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include <climits>

struct guild_type
{
   char *pGuildName;

   char **ppLeaveQuest;
   int    nLeaveCost;

   char **ppEnterQuest;
   int    nEnterCost;

   char **ppExcludeQuest;
};

auto find_quest(char *word, unit_data *unit) -> struct extra_descr_data *
{
   if(!IS_PC(unit) || (word == nullptr))
   {
      return nullptr;
   }

   return PC_QUEST(unit)->find_raw(word);
}

auto char_guild_level(unit_data *ch) -> int
{
   uint32_t                 i;
   struct extra_descr_data *exd;

   assert(IS_CHAR(ch));

   if(IS_NPC(ch))
   {
      return CHAR_LEVEL(ch);
   }

   if(PC_GUILD(ch) && (*PC_GUILD(ch) != 0))
   {
      exd = find_quest(str_cc("$", PC_GUILD(ch)), ch);

      if(exd != nullptr)
      {
         for(i = 0; i < exd->names.Length(); i++)
         {
            if(isdigit(*exd->names.Name(i)) != 0)
            {
               return atoi(exd->names.Name(i));
            }
         }
      }
   }

   return CHAR_LEVEL(ch);
}

void advance_guild_level(unit_data *ch)
{
   struct extra_descr_data *exd;

   int      lvl = char_guild_level(ch);
   uint32_t i;

   if(!IS_PC(ch))
   {
      return;
   }

   if(PC_GUILD(ch) && (*PC_GUILD(ch) != 0))
   {
      exd = find_quest(str_cc("$", PC_GUILD(ch)), ch);

      if(exd != nullptr)
      {
         for(i = 0; i < exd->names.Length(); i++)
         {
            if(isdigit(*exd->names.Name(i)) != 0)
            {
               lvl++;
               exd->names.Substitute(i, itoa(lvl));
               return;
            }
         }
      }
   }
}

static void free_guild_data(struct guild_type *pGt)
{
   if(pGt->pGuildName != nullptr)
   {
      free(pGt->pGuildName);
   }

   if(pGt->ppLeaveQuest != nullptr)
   {
      free_namelist(pGt->ppLeaveQuest);
   }

   if(pGt->ppEnterQuest != nullptr)
   {
      free_namelist(pGt->ppEnterQuest);
   }

   if(pGt->ppExcludeQuest != nullptr)
   {
      free_namelist(pGt->ppExcludeQuest);
   }

   free(pGt);
}

static auto parse_guild_data(unit_data *npc, char *pStr) -> struct guild_type *
{
   char              *pTmp1;
   struct guild_type *pG;
   int                ok = 0;

   CREATE(pG, struct guild_type, 1);

   pTmp1 = pStr;

   pG->pGuildName   = parse_match_name(&pTmp1, "Guild");
   pG->ppEnterQuest = parse_match_namelist(&pTmp1, "Guild Enter Quest");
   ok += parse_match_num(&pTmp1, "Guild Enter Cost", &pG->nEnterCost);
   pG->ppLeaveQuest = parse_match_namelist(&pTmp1, "Guild Leave Quest");
   ok += parse_match_num(&pTmp1, "Guild Leave Cost", &pG->nLeaveCost);
   pG->ppExcludeQuest = parse_match_namelist(&pTmp1, "Guild Exclude Quest");

   if(ok != 2 || (pG->pGuildName == nullptr) || (pG->ppLeaveQuest == nullptr) || (pG->ppEnterQuest == nullptr) ||
      (pG->ppExcludeQuest == nullptr))
   {
      szonelog(UNIT_FI_ZONE(npc), "GUILD-ERROR (%s@%s): Illegal initialization element.", UNIT_FI_NAME(npc), UNIT_FI_ZONENAME(npc));
      free_guild_data(pG);
      return nullptr;
   }

   return pG;
}

auto guild_master_init(struct spec_arg *sarg) -> int
{
   struct guild_type *pG;

   if(sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      pG = (struct guild_type *)parse_guild_data(sarg->owner, (char *)sarg->fptr->data);
      if(pG == nullptr)
      {
         destroy_fptr(sarg->owner, sarg->fptr);
      }
      else
      {
         free(sarg->fptr->data);
         sarg->fptr->data  = pG;
         sarg->fptr->index = SFUN_GUILD_INTERNAL;
      }
   }
   return SFR_SHARE;
}

/* Send the string 'msg' to all players that are members of the guild  */
/* 'guild'. Uses the act() function to send string with $1 as 'member' */
/* $2 as 'nonmember' and $3 as character                               */
/* Message will never be sent to 'nonmember'                           */
void act_to_guild(const char *msg, char *guild, unit_data *member, unit_data *nonmember)
{
   struct descriptor_data *d;

   extern struct descriptor_data *descriptor_list;

   if(guild == nullptr || *guild == '\0')
   {
      slog(LOG_ALL, 0, "No guild name in send_to_guild");
      return;
   }

   for(d = descriptor_list; d != nullptr; d = d->next)
   {
      if((descriptor_is_playing(d) != 0) && (d->character != nonmember) && IS_PC(d->character) && PC_GUILD(d->character) &&
         strcmp(PC_GUILD(d->character), guild) == 0)
      {
         act(msg, A_ALWAYS, member, nonmember, d->character, TO_VICT);
      }
   }
}

/* Purpose: To be used as a guild 'block' routine before special commands: */
/*          CMD_PRACTICE, CMD_INFO                                         */
/* Format:  <Guild Name><#><Text String>                                   */
/* Example: "wizard#This is a members only club, $3n."                     */
/*          $1 and $3 can be used in text string.                          */
/*                                                                         */
auto teach_members_only(struct spec_arg *sarg) -> int
{
   char *str;
   int   guild;

   if((sarg->cmd->no == CMD_PRACTICE) && IS_PC(sarg->activator) && CHAR_AWAKE(sarg->owner))
   {
      if((str = strchr((char *)sarg->fptr->data, '#')) == nullptr)
      {
         slog(LOG_ALL, 0, "Error in SFUN argument, teach_members_only");
         return SFR_SHARE;
      }

      if(PC_GUILD(sarg->activator))
      {
         *str  = 0;
         guild = strcmp((char *)sarg->fptr->data, PC_GUILD(sarg->activator));
         *str  = '#';
      }
      else
      {
         guild = -1;
      }

      if(guild != 0)
      {
         act(str + 1, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
         return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}

/* Purpose: Used to prevent non-members to enter a guild.                 */
/* Fptr->data is a string, containing                                     */
/*    "<exitname>#<guild name>#[location]#[exclude]#<msg1>#<msg2>"        */
/* Where <exitname> is 0,1,2,3,4,5 for n,e,s,w,u,d respectively           */
/* <location> is the symbolic name of the room/obj the mobile must be in  */
/* The message is sent to room, you can use $1 and $3n                    */
/* Example: "1#warrior#midgaard/cth_square3#guard#$1n stops you and says, */
/*          'Members only!'#$1n stops $3n, and says, 'Members only!'"     */

auto guard_guild_way(struct spec_arg *sarg) -> int
{
   char *str;
   char *location;
   char *excl = NULL;
   char *msg1 = NULL;
   char *msg2 = NULL;
   char *guild_no;
   int   guild_cmp;

   auto charname_in_list(unit_data * ch, char *arg)->int;

   if(((str = (char *)sarg->fptr->data) != nullptr) && (sarg->cmd->no == (*str - '0')) && CHAR_IS_READY(sarg->owner))
   {
      guild_no = str + 2;

      if(((location = strchr(guild_no, '#')) == nullptr) || ((excl = strchr(location + 1, '#')) == nullptr) ||
         ((msg1 = strchr(excl + 1, '#')) == nullptr) || ((msg2 = strchr(msg1 + 1, '#')) == nullptr))
      {
         slog(LOG_ALL, 0, "Illegal data string in guard_way: %s", str);
         return SFR_SHARE;
      }

      if(IS_PC(sarg->activator))
      {
         if((PC_GUILD(sarg->activator) != nullptr) && (*PC_GUILD(sarg->activator) != '\0'))
         {
            *location = '\0';
            guild_cmp = strcmp(PC_GUILD(sarg->activator), guild_no);
            *location = '#';

            if(guild_cmp == 0)
            {
               return SFR_SHARE;
            }
         }
         else
         {
            /* Uhm. Well, if you are not a member of *any* guild      */
            /* you should be able to enter... Well, maybe as a guest? */
            /* I mean, you need someone to assist you??? Hm? I'm too  */
            /* lazy to do so now. ANyway it is another fundtion       */
            return SFR_SHARE;
         }
      }

      *excl = '\0';
      if((*(location + 1) != 0) && (strcmp(location + 1, UNIT_FI_NAME(UNIT_IN(sarg->owner))) != 0))
      {
         *excl = '#';
         return SFR_SHARE;
      }
      *excl = '#';

      *msg1 = '\0';
      if(charname_in_list(sarg->activator, excl + 1) != 0)
      {
         *msg1 = '#';
         return SFR_SHARE;
      }
      *msg2 = '\0';
      act(msg1 + 1, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      act(msg2 + 1, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_NOTVICT);
      *msg1 = '#';
      *msg2 = '#';
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}

void leave_guild(unit_data *player)
{
   struct extra_descr_data *exd;

   assert(PC_GUILD(player));

   if(CHAR_LEVEL(player) > START_LEVEL)
   {
      exd = find_quest(str_cc("$", PC_GUILD(player)), player);

      if((exd != nullptr) && (exd->names.IsName("quitter") == nullptr))
      {
         exd->names.AppendName("quitter");
      }

      send_to_char("You are no longer a member of your guild.\n\r", player);
   }
   else
   {
      if((exd = find_quest(str_cc("$", PC_GUILD(player)), player)) != nullptr)
      {
         PC_QUEST(player) = PC_QUEST(player)->remove(exd);
      }

      send_to_char("You are no longer a member of your guild, but you are "
                   "welcome back at any time.\n\r",
                   player);
   }

   free(PC_GUILD(player));

   PC_GUILD(player)      = nullptr;
   PC_GUILD_TIME(player) = PC_TIME(player).played;
}

void guild_banish_player(unit_data *ch)
{
   char                    *c;
   struct extra_descr_data *pExd;

   if(!IS_PC(ch))
   {
      return;
   }

   if(PC_GUILD(ch))
   {
      c = str_dup(PC_GUILD(ch));
      leave_guild(ch);

      pExd = find_quest(str_cc("$", c), ch);
      if((pExd != nullptr) && (pExd->names.IsName("banished") == nullptr))
      {
         pExd->names.AppendName("banished");
         send_to_char("You have been banished from your own guild!\n\r", ch);
      }

      free(c);
   }
}

auto can_leave_guild(struct guild_type *pG, unit_data *master, unit_data *ch) -> int
{
   char     **p;
   currency_t currency = local_currency(master);

   if(!IS_PC(ch))
   {
      return FALSE;
   }

   if(!PC_GUILD(ch) || (strcmp(PC_GUILD(ch), pG->pGuildName) != 0))
   {
      act("$1n says, 'You are not a member here, $3n'", A_SOMEONE, master, nullptr, ch, TO_ROOM);
      return FALSE;
   }

   if(CHAR_LEVEL(ch) > START_LEVEL)
   {
      for(p = pG->ppLeaveQuest; *p != nullptr; p++)
      {
         if((*p != nullptr) && (**p != 0))
         {
            if(find_quest(*p, ch) == nullptr)
            {
               act("$1n says, 'You must first complete the $2t quest, $3n'", A_SOMEONE, master, *p, ch, TO_ROOM);
               return FALSE;
            }
         }
      }

      if(char_can_afford(ch, pG->nLeaveCost, currency) == 0u)
      {
         act("$1n says, 'You can't afford the cost of $2t, $3n.'", A_SOMEONE, master, money_string(pG->nLeaveCost, currency, TRUE), ch,
             TO_ROOM);
         return FALSE;
      }
   }

   return TRUE;
}

void join_guild(unit_data *ch, char *guild_name)
{
   struct extra_descr_data *exd;

   assert(IS_PC(ch));

   if(PC_GUILD(ch))
   {
      free(PC_GUILD(ch));
   }

   PC_GUILD(ch)      = str_dup(guild_name);
   PC_GUILD_TIME(ch) = PC_TIME(ch).played;

   exd = quest_add(ch, str_cc("$", PC_GUILD(ch)), itoa(time(nullptr)));
   exd->names.AppendName("0");
   exd->names.AppendName("$guild");
}

auto can_join_guild(struct guild_type *pG, unit_data *master, unit_data *ch) -> int
{
   currency_t currency = local_currency(master);
   char     **p;

   if(!IS_PC(ch))
   {
      return FALSE;
   }

   if(PC_GUILD(ch))
   {
      if(strcmp(pG->pGuildName, PC_GUILD(ch)) == 0)
      {
         act("$1n says, 'You are already a member, $3n'", A_SOMEONE, master, nullptr, ch, TO_ROOM);
      }
      else
      {
         act("$1n says, 'You must first break your ties with your current"
             " guild, $3n'",
             A_SOMEONE, master, nullptr, ch, TO_ROOM);
      }
      return FALSE;
   }

   if(CHAR_LEVEL(ch) > START_LEVEL)
   {
      for(p = pG->ppEnterQuest; *p != nullptr; p++)
      {
         if((**p != 0) && find_quest(*p, ch) == nullptr)
         {
            act("$1n says, 'You must first complete the $2t quest, $3n'", A_SOMEONE, master, *p, ch, TO_ROOM);
            return FALSE;
         }
      }

      if(find_quest(str_cc("$", pG->pGuildName), ch) != nullptr)
      {
         act("$1n says, 'Don't you think we remeber how you acted last "
             "time, $3n? ",
             A_SOMEONE, master, nullptr, ch, TO_ROOM);
         return FALSE;
      }

      for(p = pG->ppExcludeQuest; *p != nullptr; p++)
      {
         if(find_quest(*p, ch) != nullptr)
         {
            act("$1n says, 'We will never be able to accept you as a "
                "member due to the $2t quest, $3n'",
                A_SOMEONE, master, *p, ch, TO_ROOM);
            return FALSE;
         }
      }

      if(char_can_afford(ch, pG->nEnterCost, currency) == 0u)
      {
         act("$1n says, 'You can't afford the entry cost of $2t, $3n.'", A_SOMEONE, master, money_string(pG->nEnterCost, currency, TRUE),
             ch, TO_ROOM);
         return FALSE;
      }
   }

   return TRUE;
}

/* You must be able to be expelled, to resign, to enroll and to obtain */
/* a higher guild status. All these matters will be implemented after  */
/* I have got the experience system to work.                           */
auto guild_master(struct spec_arg *sarg) -> int
{
   char      *arg    = (char *)sarg->arg;
   static int pc_pos = -2;
   auto      *pG     = (struct guild_type *)sarg->fptr->data;

   if(pG == nullptr)
   {
      return SFR_SHARE;
   }

   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      free_guild_data(pG);
      sarg->fptr->data = nullptr;
      return SFR_SHARE;
   }

   if((sarg->activator == nullptr) || !IS_PC(sarg->activator))
   {
      return SFR_SHARE;
   }

   if(!CHAR_IS_READY(sarg->activator) || !CHAR_IS_READY(sarg->owner))
   {
      return SFR_SHARE;
   }

   if(sarg->cmd->no == CMD_JOIN)
   {
      if(PC_GUILD(sarg->activator) == nullptr || *PC_GUILD(sarg->activator) == '\0')
      {
         if(can_join_guild(pG, sarg->owner, sarg->activator) != 0)
         {
            act("$1n says, 'Welcome in our guild, $3n'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);

            if(CHAR_LEVEL(sarg->activator) > START_LEVEL)
            {
               money_transfer(sarg->activator, sarg->owner, pG->nEnterCost, local_currency(sarg->owner));
            }

            join_guild(sarg->activator, pG->pGuildName);
         }
      }
      else
      {
         act("$1n says, 'You must first break your ties with your current"
             " guild, $3n'",
             A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      }
      return SFR_BLOCK;
   }
   if(sarg->cmd->no == CMD_INSULT)
   {
      if(find_unit(sarg->activator, &arg, 0, FIND_UNIT_SURRO) != sarg->owner)
         return SFR_SHARE;

      if(pc_pos != PC_ID(sarg->activator))
      {
         act("$1n says, 'Do that again $3n and I will kick you out of this "
             "guild.'",
             A_SOMEONE, sarg->owner, 0, sarg->activator, TO_ROOM);
         pc_pos = PC_ID(sarg->activator);
      }
      else /* Match! */
      {
         if(can_leave_guild(pG, sarg->owner, sarg->activator))
         {
            act("$1n says, 'So be it. Buggar ye off, $3n'", A_SOMEONE, sarg->owner, 0, sarg->activator, TO_ROOM);

            if(CHAR_LEVEL(sarg->activator) > START_LEVEL)
               money_transfer(sarg->activator, sarg->owner, pG->nLeaveCost, local_currency(sarg->owner));

            leave_guild(sarg->activator);

            pc_pos = -2;
         }
      }
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}

/* Purpose: To be used as a guild general stuff routine.                   */
/* sarg->fptr->data contains guild name.                                         */
auto guild_basis(struct spec_arg *sarg) -> int
{
   int        i;
   unit_data *u;

   if(sarg->cmd->no == CMD_AUTO_DEATH && sarg->owner == sarg->activator)
   {
      scan4_unit(sarg->owner, UNIT_ST_PC);

      for(i = 0; i < unit_vector.top; i++)
      {
         u = unit_vector.units[i];
         if(IS_PC(u) && CHAR_FIGHTING(u) == sarg->owner && PC_GUILD(u) && CHAR_CAN_SEE(sarg->owner, u))
         {
            if(strcmp(PC_GUILD(u), (char *)sarg->fptr->data) == 0)
            {
               guild_banish_player(u);
            }
         }
      }

      return SFR_BLOCK;
   }

   if((sarg->cmd->no == CMD_AUTO_COMBAT) && (CHAR_FIGHTING(sarg->activator) == sarg->owner))
   {
      /* Alert guild members of attack */
      /* Uhm, how do we know it is the 'first' attack, so that */
      /* we wont send this message all the time???             */
      act_to_guild("$1n tells you, 'Help! Our guild is attacked by $2n'", (char *)sarg->fptr->data, sarg->owner, sarg->activator);
      return SFR_SHARE;
   }

   return SFR_SHARE;
}

auto guild_title(struct spec_arg *sarg) -> int
{
   char  buf[MAX_STRING_LENGTH];
   char  male[MAX_STRING_LENGTH];
   char  female[MAX_STRING_LENGTH];
   char *c;
   int   i;
   int   title_no;

   if((sarg->cmd->no != CMD_TITLE) || !IS_PC(sarg->activator))
   {
      return SFR_SHARE;
   }

   c = (char *)sarg->fptr->data;

   if((c = str_line(c, buf)) == nullptr)
   {
      return SFR_BLOCK;
   }

   if((PC_GUILD(sarg->activator) == nullptr) || (strcmp(PC_GUILD(sarg->activator), buf) != 0))
   {
      act("$1n says, 'Thou art not a member $3n'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      return SFR_BLOCK;
   }

   title_no = (PC_TIME(sarg->activator).played - PC_GUILD_TIME(sarg->activator)) / (SECS_PER_REAL_HOUR * 4);
   male[0] = female[0] = 0;

   for(i = 0;; i++)
   {
      if((c = str_line(c, male)) == nullptr)
      {
         break;
      }

      if((c = str_line(c, female)) == nullptr)
      {
         break;
      }

      if(i == title_no)
      {
         break;
      }
   }

   if(CHAR_SEX(sarg->activator) == SEX_FEMALE)
   {
      sprintf(buf, female, pc_race_adverbs[CHAR_RACE(sarg->activator)]);
   }
   else
   {
      sprintf(buf, male, pc_race_adverbs[CHAR_RACE(sarg->activator)]);
   }

   if(strcmp(buf, UNIT_TITLE_STRING(sarg->activator)) == 0)
   {
      if(c == nullptr)
      {
         act("$1n says, 'You have reached the ultimate title, $3n'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      }
      else
      {
         act("$1n says, 'You must first be an older member $3N'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      }
      return SFR_BLOCK;
   }

   UNIT_TITLE(sarg->activator).Reassign(buf);

   act("$1n says, 'Enjoy the new title, $3n!'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);

   return SFR_BLOCK;
}

void do_guild(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char                     buf[MAX_STRING_LENGTH];
   int                      found = FALSE;
   struct extra_descr_data *exd;

   if(!IS_PC(ch))
   {
      send_to_char("You ain't nothin' but a hound-dog...\n\r", ch);
      return;
   }

   for(exd = PC_QUEST(ch); exd != nullptr; exd = exd->next)
   {
      if(exd->names.IsName("$guild") != nullptr)
      {
         found = TRUE;
         sprintf(buf, "%-30s %-2s\n\r", exd->names.Name(0) + 1, exd->names.Name(1));
         send_to_char(buf, ch);
      }
   }

   if(found == 0)
   {
      send_to_char("None.\n\r", ch);
   }
}
