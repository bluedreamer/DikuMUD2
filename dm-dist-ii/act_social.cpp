/* *********************************************************************** *
 * File   : act_social.c                              Part of Valhalla MUD *
 * Version: 3.00                                                           *
 * Author : gnort@daimi.aau.dk                                             *
 *                                                                         *
 * Purpose: Handling the social commands                                   *
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
/* Tue  6-Jul-93   HHS: added exchangable lib dir
 * Sun 12-Mar-95 gnort: changed from binary searched array to trie
 *			because old method was too CPU intensive
 *			(as always, this meant I had to *think* to write
 *			sprint_social!!!1!)
 */

#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "movement.h"
#include "structs.h"
#include "textutil.h"
#include "trie.h"
#include "trie_type.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

/* externs */
extern char libdir[]; /* from dikumud.c */

struct social_msg
{
   char   *cmd_str;     /* "hug" "kiss" etc */
   uint8_t hide_flag;   /* Should we hideinvis? */
   uint8_t min_pos;     /* Minimum position of char */
   uint8_t vic_min_pos; /* Minimum position of victim */
   uint8_t level;       /* Is this a restricted social? (silly concept) */

   /* No argument was supplied */
   char   *char_no_arg;
   char   *others_no_arg;

   /* An argument was there, and a victim was found */
   char   *char_found; /* if NULL, read no further, ignore args */
   char   *others_found;
   char   *vict_found;

   /* An argument was there, but no victim was found */
   char   *not_found;

   /* The victim turned out to be the character */
   char   *char_auto;
   char   *others_auto;
};

static trie_type *soc_trie;

static auto       fread_action(FILE *fl) -> char *
{
   char  buf[512];

   char *m_tmp = fgets(buf, sizeof buf, fl);

   if(feof(fl) != 0)
   {
      perror("fread_action");
      exit(1);
   }

   if(*buf == '#')
   {
      return nullptr;
   }

   buf[strlen(buf) - 1] = '\0';

   return str_dup(buf);
}

static auto str_to_min_pos(char *str) -> int
{
   int pos = 0;

   if(str_ccmp(str, "dead") == 0)
   {
      pos = POSITION_DEAD;
   }
   else if(str_ccmp(str, "sleep") == 0)
   {
      pos = POSITION_SLEEPING;
   }
   else if(str_ccmp(str, "rest") == 0)
   {
      pos = POSITION_RESTING;
   }
   else if(str_ccmp(str, "sit") == 0)
   {
      pos = POSITION_SITTING;
   }
   else if(str_ccmp(str, "fight") == 0)
   {
      pos = POSITION_FIGHTING;
   }
   else if(str_ccmp(str, "stand") == 0)
   {
      pos = POSITION_STANDING;
   }
   else
   {
      error(HERE, "Illegal position in actions-file: %s", str);
   }

   return pos;
}

static auto str_to_hide_flag(char *str) -> int
{
   int flag = 0;

   if(str_ccmp(str, "someone") == 0)
   {
      flag = A_SOMEONE;
   }
   else if(str_ccmp(str, "hideinv") == 0)
   {
      flag = A_HIDEINV;
   }
   else if(str_ccmp(str, "always") == 0)
   {
      flag = A_ALWAYS;
   }
   else
   {
      error(HERE, "Illegal hide-flag in actions-file: %s", str);
   }

   return flag;
}

static auto soc_sort_cmp(struct social_msg *dat1, struct social_msg *dat2) -> int
{
   return strcmp(dat1->cmd_str, dat2->cmd_str);
}

/*  Boot builds a array of the socials first, so we can sort them before
 *  putting them into the trie.
 */
void boot_social_messages()
{
   FILE              *fl;
   char               cmd[80];
   char               hide[80];
   char               min_pos[80];
   char               vic_min_pos[80];
   char               buf[256];
   int                level;

   struct social_msg *list      = nullptr;
   int                list_elms = 0;
   int                list_size = 0;

   if((fl = fopen(str_cc(libdir, SOCMESS_FILE), "r")) == nullptr)
   {
      perror("boot_social_messages");
      exit(1);
   }

   for(;;)
   {
      do
      {
         if(fgets(buf, sizeof buf, fl) == nullptr)
         {
            if(feof(fl) != 0)
            {
               int i;

               fclose(fl);

               /* Release mem not used. */
               RECREATE(list, struct social_msg, list_elms);

               qsort(list, list_elms, sizeof(struct social_msg), (int (*)(const void *, const void *))soc_sort_cmp);

               soc_trie = nullptr;

               for(i = 0; i < list_elms; i++)
               {
                  soc_trie = add_trienode(list[i].cmd_str, soc_trie);
               }

               qsort_triedata(soc_trie);

               for(i = 0; i < list_elms; i++)
               {
                  set_triedata(list[i].cmd_str, soc_trie, &list[i], FALSE);
               }

               return; /* list is not freed, as it it used for the trie */
            }

            perror("boot_social_messages");
            exit(1);
         }
      }
      /* Skip empty lines & comments */
      while(buf[0] == '\n' || buf[0] == '#');

      sscanf(buf, "%s %s %s %s %d", cmd, hide, min_pos, vic_min_pos, &level);

      /* alloc new cells */
      if(list == nullptr)
      {
         list_size = 10;
         CREATE(list, struct social_msg, list_size);
      }
      else if(list_size == list_elms)
      {
         list_size *= 2;
         RECREATE(list, struct social_msg, list_size);
      }

      list[list_elms].cmd_str       = str_dup(cmd);
      list[list_elms].hide_flag     = str_to_hide_flag(hide);
      list[list_elms].min_pos       = str_to_min_pos(min_pos);
      list[list_elms].vic_min_pos   = str_to_min_pos(vic_min_pos);
      list[list_elms].level         = MIN(255, level);

      list[list_elms].char_no_arg   = fread_action(fl);
      list[list_elms].others_no_arg = fread_action(fl);

      list[list_elms].char_found    = fread_action(fl);

      /* if no char_found, the rest is to be ignored */
      if(list[list_elms].char_found != nullptr)
      {
         list[list_elms].others_found = fread_action(fl);
         list[list_elms].vict_found   = fread_action(fl);
         list[list_elms].not_found    = fread_action(fl);
         list[list_elms].char_auto    = fread_action(fl);
         list[list_elms].others_auto  = fread_action(fl);
      }

      list_elms++;
   }
}

/* is cmd an nonabbreviated social-string? */
auto cmd_is_a_social(char *cmd, int complete) -> bool
{
   struct social_msg *action;

   if(complete != 0)
   {
      return (((action = (struct social_msg *)search_trie(cmd, soc_trie)) != nullptr) && str_ccmp(action->cmd_str, cmd) == 0);
   }
   return search_trie(cmd, soc_trie) != nullptr;
}

auto perform_social(unit_data *ch, char *arg, const command_info *cmd) -> bool
{
   struct social_msg *action;
   char              *oarg = arg;

   action                  = (struct social_msg *)search_trie(cmd->cmd_str, soc_trie);

   if((action == nullptr) || (action->level > CHAR_LEVEL(ch)))
   {
      return FALSE;
   }
   if(CHAR_POS(ch) < action->min_pos)
   {
      wrong_position(ch);
   }
   else if(str_is_empty(arg) || (action->char_found == nullptr))
   {
      act(action->char_no_arg, A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      act(action->others_no_arg, action->hide_flag, ch, nullptr, nullptr, TO_ROOM);
      send_done(ch, nullptr, nullptr, 0, cmd, oarg);
   }
   else
   {
      unit_data *vict = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO);

      if(vict == nullptr || !IS_CHAR(vict))
      {
         act(action->not_found, A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
      else if(vict == ch)
      {
         act(action->char_auto, A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
         act(action->others_auto, action->hide_flag, ch, nullptr, nullptr, TO_ROOM);
         send_done(ch, nullptr, nullptr, 0, cmd, oarg);
      }
      else if(CHAR_POS(vict) < action->vic_min_pos)
      {
         act("$2n is not in a proper position for that.", A_SOMEONE, ch, vict, nullptr, TO_CHAR);
      }
      else
      {
         act(action->char_found, A_SOMEONE, ch, nullptr, vict, TO_CHAR);
         act(action->others_found, action->hide_flag, ch, nullptr, vict, TO_NOTVICT);
         act(action->vict_found, action->hide_flag, ch, nullptr, vict, TO_VICT);
         send_done(ch, nullptr, vict, 0, cmd, oarg);
      }
   }
   return TRUE;
}

/* Eeek, not trivial!
 *
 * b:   The array to hold the generated string
 * t:   The trie to search
 * no:  Pointer to the integer to control newlines
 * cur: The currently examined string
 * idx: Index to the permutable char of cur
 */
static auto sprint_social(char *b, trie_type *t, int *no, char *cur, int idx) -> int
{
   struct social_msg *sm;
   trie_type         *t2;
   int                i;
   int                count = 0;

   if(t != nullptr)
   {
      cur[idx + 1] = '\0'; /* Make sure cur is correctly nil terminated */

      for(i = 0; i < t->size; i++)
      {
         t2       = t->nexts[i].t;
         cur[idx] = t->nexts[i].c; /* extend the current string */

         if(((sm = (struct social_msg *)t2->data) != nullptr)
            /* also make sure it is an unabbreviated social-string! */
            && strcmp(sm->cmd_str, cur) == 0)
         {
            sprintf(b, "%-15s", sm->cmd_str);
            if(++*no % 5 == 0)
            {
               strcat(b, "\n\r");
            }
            TAIL(b);

            count++;
         }
         count += sprint_social(b, t2, no, cur, idx + 1);
         TAIL(b);
      }
   }

   return count;
}

void do_socials(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char buf[MAX_STRING_LENGTH];
   char cur[50];
   int  no = 0;

   if(!IS_PC(ch))
   {
      return;
   }

   send_to_char("The following social commands are available:\n\r\n\r", ch);

   if(sprint_social(buf, soc_trie, &no, cur, 0) != 0)
   {
      strcat(buf, "\n\r");
      page_string(CHAR_DESCRIPTOR(ch), buf);
   }
   else
   {
      send_to_char("  None!\n\r", ch);
   }
}

void do_insult(unit_data *ch, char *arg, const struct command_info *cmd)
{
   const char *insult;
   unit_data  *victim;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Surely you don't want to insult everybody.\n\r", ch);
      return;
   }

   victim = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO);

   if(victim == nullptr || !IS_CHAR(victim))
   {
      send_to_char("No one by that name here.\n\r", ch);
      return;
   }

   if(victim == ch)
   {
      backdoor(ch, arg, cmd);

      send_to_char("You feel insulted.\n\r", ch);
   }
   else
   {
      /* Add new cases ad libitum/nauseum */
      switch(number(0, 2))
      {
         case 0:
            if(CHAR_SEX(ch) == SEX_MALE)
            {
               if(CHAR_SEX(victim) == SEX_MALE)
               {
                  insult = "$1n accuses you of fighting like a woman!";
               }
               else
               {
                  insult = "$1n says that women can't fight.";
               }
            }
            else /* female (or neutral!) */
            {
               if(CHAR_SEX(victim) == SEX_MALE)
               {
                  insult = "$1n accuses you of having the smallest.... (brain?)";
               }
               else
               {
                  insult = "$1n tells you that you'd loose a beauty contest "
                           "against a troll!";
               }
            }
            break;

         case 1:
            insult = "$1n calls your mother a bitch!";
            break;

         default:
            insult = "$1n tells you to get lost!";
            break;
      }

      act("You insult $3n.", A_SOMEONE, ch, nullptr, victim, TO_CHAR);
      act("$1n insults $3n.", A_SOMEONE, ch, nullptr, victim, TO_NOTVICT);
      act(insult, A_SOMEONE, ch, nullptr, victim, TO_VICT);
   }
}

struct pose_type
{
   int   level;        /* minimum level for poser */
   char *poser_msg[4]; /* message to poser        */
   char *room_msg[4];  /* message to room         */
};

#define MAX_POSES 60

static struct pose_type pose_messages[MAX_POSES];

void                    boot_pose_messages()
{
   FILE   *fl;
   int16_t counter;
   int16_t cls;

   return; /* SUSPEKT  no reason to boot these, eh? */

   if((fl = fopen(str_cc(libdir, POSEMESS_FILE), "r")) == nullptr)
   {
      perror("boot_pose_messages");
      exit(0);
   }

   for(counter = 0;; counter++)
   {
      int m_tmp = fscanf(fl, " %d ", &pose_messages[counter].level);

      if(pose_messages[counter].level < 0)
      {
         break;
      }
      for(cls = 0; cls < 4; cls++)
      {
         pose_messages[counter].poser_msg[cls] = fread_action(fl);
         pose_messages[counter].room_msg[cls]  = fread_action(fl);
      }
   }

   fclose(fl);
}

void do_pose(unit_data *ch, char *argument, const struct command_info *cmd)
{
   send_to_char("Sorry Buggy command.\n\r", ch);

#ifdef SUSPEKT
   struct pose_type *to_pose;
   int               counter = 0;

   if(CHAR_LEVEL(ch) < pose_messages[0].level || IS_NPC(ch))
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }

   while(pose_messages[counter].level < CHAR_LEVEL(ch) && 0 < pose_messages[counter].level)
      counter++;

   to_pose = &pose_messages[number(0, counter - 1)];

   act(to_pose->poser_msg[0], A_SOMEONE, ch, 0, 0, TO_CHAR);
   act(to_pose->room_msg[0], A_SOMEONE, ch, 0, 0, TO_ROOM);
#endif
}
