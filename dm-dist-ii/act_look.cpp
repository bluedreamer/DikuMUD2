#include "act_movement.h"
#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
#include "CServerConfiguration.h"
#include "db.h"
#include "dilrun.h"
#include "files.h"
#include "guild.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "movement.h"
#include "protocol.h"
#include "skills.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "trie.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

/* extern variables */

extern descriptor_data *descriptor_list;

extern char            *credits;
extern char            *news;
extern char            *info;
extern char            *wizlist;

struct looklist_type
{
   struct look_msg *msgs;
   int              size;
   int              index;
};

struct look_msg
{
   char *msg;
   int   cnt;
};

void show_char_to_char_trans_cont(char *b, unit_data *i, unit_data *ch);

/* To free, call with NULL msg. */
auto add_to_look_list(struct looklist_type *list, char *msg) -> struct looklist_type *
{
   int i;

   if(msg == nullptr)
   {
      assert(list);

      for(i = 0; i < list->index; i++)
      {
         free(list->msgs[i].msg);
      }
      free(list->msgs);
      free(list);

      return nullptr;
   }

   if(*msg == 0)
   {
      return list;
   }

   if(list == nullptr)
   {
      assert(msg);
      CREATE(list, struct looklist_type, 1);
      CREATE(list->msgs, struct look_msg, 20);
      list->index = 0;
      list->size  = 20;
      for(i = 0; i < list->size; i++)
      {
         list->msgs[i].msg = nullptr;
         list->msgs[i].cnt = 0;
      }
   }

   for(i = 0; i < list->index; i++)
   {
      if(strcmp(list->msgs[i].msg, msg) == 0)
      {
         list->msgs[i].cnt++;
         return list;
      }
   }

   if(list->index >= list->size)
   {
      RECREATE(list->msgs, struct look_msg, list->size + 20);
      for(i = list->size; i < list->size + 20; i++)
      {
         list->msgs[i].msg = nullptr;
         list->msgs[i].cnt = 0;
      }
      list->size += 20;
   }

   list->msgs[list->index].msg = str_dup(msg);
   list->msgs[list->index].cnt++;
   list->index++;

   return list;
}

void send_looklist(struct looklist_type *list, unit_data *ch)
{
   int  i;
   char buf[20];

   for(i = 0; i < list->index; i++)
   {
      if(list->msgs[i].cnt > 1)
      {
         sprintf(buf, "[x%d] ", list->msgs[i].cnt);
         send_to_char(buf, ch);
      }
      send_to_char(list->msgs[i].msg, ch);
   }
}

auto single_unit_messg(unit_data *unit, const char *type, const char *pSubStr, const char *mesg) -> const char *
{
   extra_descr_data *exd = UNIT_EXTRA_DESCR(unit);

   while((exd != nullptr) && ((exd = exd->find_raw(type)) != nullptr))
   {
      if(exd->descr.StringPtr() != nullptr)
      {
         if((static_cast<unsigned int>(str_is_empty(pSubStr)) != 0U) || (exd->names.Name(1) == nullptr))
         {
            return exd->descr.String();
         }

         if(str_cstr(exd->names.Name(1), pSubStr) != nullptr)
         {
            return exd->descr.String();
         }
      }

      exd = exd->next;
   }

   if(static_cast<unsigned int>(str_is_empty(mesg)) != 0U)
   {
      return nullptr;
   }
   return mesg;
}

void unit_messg(unit_data *ch, unit_data *unit, const char *type, const char *mesg_s, const char *mesg_o)
{
   extra_descr_data *exd = nullptr;
   const char       *c;

   c = single_unit_messg(unit, str_cc(type, "_s"), "", mesg_s);

   if(static_cast<unsigned int>(str_is_empty(c)) == 0U)
   {
      act(c, A_SOMEONE, ch, unit, nullptr, TO_CHAR);
   }

   c = single_unit_messg(unit, str_cc(type, "_o"), "", mesg_o);

   if(static_cast<unsigned int>(str_is_empty(c)) == 0U)
   {
      act(c, A_SOMEONE, ch, unit, nullptr, TO_ROOM);
   }
}

/* Assume unit is a char */
void cat_char_effects(unit_data *ch, unit_data *unit, char *buf)
{
   assert(IS_CHAR(unit));

   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_POISON) && (dil_find("spl_poison@basis", unit) != nullptr))
   {
      strcat(buf, " (black)");
      TAIL(buf);
   }

   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_UNDEAD) && CHAR_IS_UNDEAD(unit))
   {
      strcat(buf, " (violet)");
      TAIL(buf);
   }

   /* Detect Life is elsewhere */
}

void cat_obj_effects(unit_data *ch, unit_data *unit, char *buf)
{
   assert(IS_OBJ(unit));

   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_POISON) && OBJ_VALUE(unit, 3) > 0)
   {
      strcat(buf, " (black)");
      TAIL(buf);
   }
}

void cat_unit_effects(unit_data *ch, unit_data *unit, char *buf)
{
   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_ALIGN))
   {
      /* NB! Saving throws/compare can be inserted later :) */
      if(UNIT_IS_GOOD(unit))
      {
         strcat(buf, " (blue)");
      }
      else if(UNIT_IS_EVIL(unit))
      {
         strcat(buf, " (red)");
      }
      else
      {
         strcat(buf, " (green)");
      }
      TAIL(buf);
   }

   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_INVISIBLE) && IS_SET(UNIT_FLAGS(unit), UNIT_FL_INVISIBLE))
   {
      strcat(buf, " (clear)");
      TAIL(buf);
   }

   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_MAGIC) && IS_SET(UNIT_FLAGS(unit), UNIT_FL_MAGIC))
   {
      strcat(buf, " (yellow)");
      TAIL(buf);
   }

   if(IS_SET(CHAR_FLAGS(ch), CHAR_DETECT_CURSE))
   {
      /* What identifies an object as cursed? A bit? The Spell? (orange) */
   }

   if(IS_CHAR(unit))
   {
      cat_char_effects(ch, unit, buf);
   }
   else if(IS_OBJ(unit))
   {
      cat_obj_effects(ch, unit, buf);
   }
}

/* This is called from locations when doing inventory, looking, walking */
/* etc. to see obvious effects (a torch is lit, etc.)                   */
void cat_passing_effects(unit_data *ch, unit_data *unit, char *buf)
{
   if(UNIT_MINV(unit) != 0u)
   {
      strcat(buf, " (wizinv)");
      TAIL(buf);
   }

   /* This ought to be visible to everyone :-) */
   if(IS_OBJ(unit) && OBJ_TYPE(unit) == ITEM_LIGHT && (affected_by_spell(unit, ID_LIGHT_EXTINGUISH) != nullptr))
   {
      strcat(buf, " (lit)");
      TAIL(buf);
   }
}

void show_examine_aura(unit_data *ch, unit_data *unit)
{
   char buf[1024];

   *buf = '\0';
   cat_unit_effects(ch, unit, buf);

   if(UNIT_BRIGHT(unit) > 0)
   {
      act("Light comes from the $3N.", A_ALWAYS, ch, nullptr, unit, TO_CHAR);
   }

   if(UNIT_BRIGHT(unit) < 0)
   {
      act("Darkness comes from the $3N.", A_ALWAYS, ch, nullptr, unit, TO_CHAR);
   }

   if(*buf != 0)
   {
      act("The $3N reveals an aura of$2t", A_ALWAYS, ch, buf, unit, TO_CHAR);
   }
}

void show_obj_to_char_lookat(unit_data *obj, unit_data *ch, int extra)
{
   if(affected_by_spell(obj, ID_REWARD) != nullptr)
   {
      act("There is a reward on $2n.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);
   }

   if(OBJ_TYPE(obj) == ITEM_WEAPON)
   {
      act("The $2N clearly requires a $3t fighting style.", A_SOMEONE, ch, obj, wpn_text[OBJ_VALUE(obj, 0)], TO_CHAR);
   }
   else if(extra == 0)
   {
      send_to_char("You see nothing special.\n\r", ch);
   }
}

void show_obj_to_char_trans_cont(char *b, unit_data *obj, unit_data *ch)
{
   int        anything = static_cast<int>(FALSE);
   unit_data *c;

   assert(IS_OBJ(obj));

   if(CHAR_CAN_SEE(ch, obj) && (UNIT_CHARS(obj) != 0u) && UNIT_IS_TRANSPARENT(obj) &&
      (UNIT_IN(obj) == UNIT_IN(ch) ||         /* same level */
       UNIT_IN(UNIT_IN(ch)) == UNIT_IN(obj))) /* obj is level above */
   {
      /* the object about to be showed is transparent, */
      /* and contains chars that is to be listed. */

      for(c = UNIT_CONTAINS(obj); c != nullptr; c = c->next)
      {
         if(IS_CHAR(c) && CHAR_CAN_SEE(ch, c))
         {
            if(anything == static_cast<int>(FALSE))
            {
               act_generate(b, "The $1N contains:", A_SOMEONE, obj, nullptr, ch, TO_VICT, ch);
               TAIL(b);
               anything = static_cast<int>(TRUE);
            }

            if(IS_PC(c))
            {
               sprintf(b, "   %s%s%s", UNIT_NAME(c), *UNIT_TITLE_STRING(c) == ',' ? "" : " ", UNIT_TITLE_STRING(c));
            }
            else
            {
               sprintf(b, "   %s", UNIT_TITLE_STRING(c));
            }
            TAIL(b);
            cat_passing_effects(ch, c, b);
            strcat(b, "\n\r");
            TAIL(b);
         }
      }
   }
}

void show_obj_to_char_blank(char *buffer, unit_data *obj, unit_data *ch)
{
   *buffer = 0;

   if((UNIT_OUT_DESCR_STRING(obj) != nullptr) && (*UNIT_OUT_DESCR_STRING(obj) != 0))
   {
      strcpy(buffer, UNIT_OUT_DESCR_STRING(obj));
      cat_passing_effects(ch, obj, buffer);
      strcat(buffer, "\n\r");
      CAPC(buffer);
      TAIL(buffer);
   }

   show_obj_to_char_trans_cont(buffer, obj, ch);
}

void show_obj_to_char_inv(char *buffer, unit_data *obj, unit_data *ch)
{
   assert(IS_OBJ(obj));

   const char *c;

   c = single_unit_messg(UNIT_IN(obj), "$in_it_o", "", "$1n");

   act_generate(buffer, c, A_HIDEINV, obj, UNIT_IN(obj), ch, TO_VICT, ch);

   /* MS2020 BUG - buffer can be ""
   TAIL(buffer);
   buffer -= 2; // Overwrite \n\r
   *buffer = 0; */

   if(strlen(buffer) >= 2)
   {
      buffer[strlen(buffer) - 2] = 0;
   }

   cat_passing_effects(ch, obj, buffer);
   strcat(buffer, "\n\r");
}

/* When passing <char> in a room      */
void show_char_to_char_blank(char *buffer, unit_data *i, unit_data *ch)
{
   *buffer = 0;

   if(ch == i || (!CHAR_HAS_FLAG(ch, CHAR_DETECT_LIFE) && !CHAR_CAN_SEE(ch, i)))
   {
      return;
   }

   if((CHAR_HAS_FLAG(i, CHAR_HIDE) && IS_MORTAL(ch)) || !CHAR_CAN_SEE(ch, i))
   {
      if(IS_MORTAL(i) && CHAR_HAS_FLAG(ch, CHAR_DETECT_LIFE))
      {
         strcpy(buffer, "You sense a hidden life form in the room.\n\r");
      }
      return;
   }

   if(IS_NPC(i) && (CHAR_POS(i) == NPC_DEFAULT(i)) && (UNIT_OUT_DESCR(i).StringPtr() != nullptr))
   {
      /* Show the NPC's description, since in default position */

      strcpy(buffer, UNIT_OUT_DESCR_STRING(i));
      CAPC(buffer);
      TAIL(buffer);
      cat_passing_effects(ch, i, buffer);
      strcat(buffer, "\n\r");
      TAIL(buffer);
      show_char_to_char_trans_cont(buffer, i, ch);
   }
   else
   {
      /* A player or a mobile without long descr, or not in default pos. */
      if(IS_PC(i))
      {
         TAIL(buffer);
         sprintf(buffer, "%s%s%s", UNIT_NAME(i), *UNIT_TITLE_STRING(i) == ',' ? "" : " ", UNIT_TITLE_STRING(i));
         CAPC(buffer);
      }
      else
      {
         TAIL(buffer);
         strcpy(buffer, UNIT_TITLE_STRING(i));
         CAPC(buffer);
      }

      switch(CHAR_POS(i))
      {
         case POSITION_STUNNED:
            strcat(buffer, " is lying here, stunned.");
            break;
         case POSITION_INCAP:
            strcat(buffer, " is lying here, incapacitated.");
            break;
         case POSITION_MORTALLYW:
            strcat(buffer, " is lying here, mortally wounded.");
            break;
         case POSITION_DEAD:
            strcat(buffer, " is lying here, dead ... creepy.");
            break;
         case POSITION_STANDING:
            strcat(buffer, " is standing here.");
            break;
         case POSITION_SITTING:
            strcat(buffer, " is sitting here.");
            break;
         case POSITION_RESTING:
            strcat(buffer, " is resting here.");
            break;
         case POSITION_SLEEPING:
            strcat(buffer, " is sleeping here.");
            break;
         case POSITION_FIGHTING:
            if(CHAR_FIGHTING(i))
            {
               strcat(buffer, " is here, fighting ");

               if(static_cast<unsigned int>(same_surroundings(i, CHAR_FIGHTING(i))) == 0U)
               {
                  strcat(buffer, "someone who has already left.");
               }
               else if(CHAR_FIGHTING(i) == ch)
               {
                  strcat(buffer, "YOU!");
               }
               else if(IS_NPC(CHAR_FIGHTING(i)))
               {
                  strcat(buffer, UNIT_TITLE_STRING(CHAR_FIGHTING(i)));
               }
               else
               {
                  strcat(buffer, UNIT_NAME(CHAR_FIGHTING(i)));
               }
            }
            else /* NIL fighting pointer */
            {
               if(CHAR_COMBAT(i) == nullptr)
               {
                  strcat(buffer, " is here struggling with thin air.");
               }
               else
               {
                  strcat(buffer, " is here, fighting ");

                  if(static_cast<unsigned int>(same_surroundings(i, CHAR_COMBAT(i)->Opponent())) == 0U)
                  {
                     strcat(buffer, "someone who has already left.");
                  }
                  else
                  {
                     if(CHAR_COMBAT(i)->Opponent() == ch)
                     {
                        strcat(buffer, "you");
                     }
                     else if(IS_NPC(CHAR_COMBAT(i)->Opponent()))
                     {
                        strcat(buffer, UNIT_TITLE_STRING(CHAR_COMBAT(i)->Opponent()));
                     }
                     else
                     {
                        strcat(buffer, UNIT_NAME(CHAR_COMBAT(i)->Opponent()));
                     }
                     strcat(buffer, " at a distance.");
                  }
               }
            }
            break;

         default:
            strcat(buffer, " is floating here.");
            break;
      } /* end switch */

      cat_passing_effects(ch, i, buffer);
      if(CHAR_HAS_FLAG(i, CHAR_HIDE))
      {
         strcat(buffer, " (hidden)");
      }
      strcat(buffer, "\n\r");

      TAIL(buffer);
      show_char_to_char_trans_cont(buffer, i, ch);
   }
}

void show_char_to_char_trans_cont(char *b, unit_data *i, unit_data *ch)
{
   unit_data *c;
   int        anything = static_cast<int>(FALSE);

   assert(IS_CHAR(i));

   if(CHAR_CAN_SEE(ch, i) && (UNIT_CHARS(i) != 0u) && UNIT_IS_TRANSPARENT(i) &&
      (UNIT_IN(i) == UNIT_IN(ch) ||         /* same level */
       UNIT_IN(UNIT_IN(ch)) == UNIT_IN(i))) /* obj is level above */
   {
      /* the object about to be showed is transparent, */
      /* and contains chars that is to be listed. */

      for(c = UNIT_CONTAINS(i); c != nullptr; c = c->next)
      {
         if(IS_CHAR(c) && CHAR_CAN_SEE(ch, c))
         {
            if(anything == static_cast<int>(FALSE))
            {
               act_generate(b, "$1n carries:", A_SOMEONE, i, nullptr, ch, TO_VICT, ch);
               TAIL(b);
               anything = static_cast<int>(TRUE);
            }

            if(IS_PC(c))
            {
               sprintf(b, "   %s%s%s", UNIT_NAME(c), *UNIT_TITLE_STRING(c) == ',' ? "" : " ", UNIT_TITLE_STRING(c));
               CAPC(b + 3);
            }
            else
            {
               sprintf(b, "   %s", UNIT_TITLE_STRING(c));
               CAPC(b + 3);
            }
            TAIL(b);
            cat_passing_effects(ch, c, b);
            strcat(b, "\n\r");
            TAIL(b);
         }
      }
   }
}

/* When with a char in something not a room                */
/* ch looks at "i" */
void show_char_to_char_trans(char *buffer, unit_data *i, unit_data *ch)
{
   if(ch == i || (!CHAR_HAS_FLAG(ch, CHAR_DETECT_LIFE) && !CHAR_CAN_SEE(ch, i)))
   {
      return;
   }

   *buffer = 0;
   const char *c;

   c = single_unit_messg(UNIT_IN(i), "$in_mob_o", "", "$1n");

   act_generate(buffer, c, A_HIDEINV, i, UNIT_IN(i), ch, TO_VICT, ch);
   TAIL(buffer);
   show_char_to_char_trans_cont(buffer, i, ch);
}

/* When in inventory                                       */
void show_char_to_char_inv(char *buffer, unit_data *i, unit_data *ch)
{
   if(ch == i || (!CHAR_HAS_FLAG(ch, CHAR_DETECT_LIFE) && !CHAR_CAN_SEE(ch, i)))
   {
      return;
   }

   *buffer = 0;
   act_generate(buffer, "$3n", A_HIDEINV, ch, nullptr, i, TO_CHAR, ch);
   TAIL(buffer);
   show_char_to_char_trans_cont(buffer, i, ch);
}

/* When 'look at <char>'                                   */
void show_char_to_char_lookat(unit_data *i, unit_data *ch)
{
   char       buffer[MAX_STRING_LENGTH];
   unit_data *unit;

   if(ch == i || (!CHAR_HAS_FLAG(ch, CHAR_DETECT_LIFE) && !CHAR_CAN_SEE(ch, i)))
   {
      return;
   }

   /* Descriptions are shown as 'extra' descriptions in do_look */

   for(unit = UNIT_CONTAINS(i); unit != nullptr; unit = unit->next)
   {
      if(IS_OBJ(unit) && (OBJ_EQP_POS(unit) != 0u))
      {
         if(CHAR_CAN_SEE(ch, unit))
         {
            if(OBJ_EQP_POS(unit) == WEAR_WIELD && (object_two_handed(unit) != 0))
            {
               send_to_char("<two hand wielded>   ", ch);
            }
            else
            {
               send_to_char(where[OBJ_EQP_POS(unit)], ch);
            }

            show_obj_to_char_inv(buffer, unit, ch);
            send_to_char(buffer, ch);
         }
      }
   }
}

auto list_obj_to_char(unit_data *i, unit_data *ch, bool show) -> int
{
   char                  buf[MAX_STRING_LENGTH];
   struct looklist_type *list = nullptr;

   for(; i != nullptr; i = i->next)
   {
      if(IS_OBJ(i) && CHAR_CAN_SEE(ch, i) && (i != UNIT_IN(ch)))
      {
         show_obj_to_char_blank(buf, i, ch);
         list = add_to_look_list(list, buf);
      }
   }

   if(list != nullptr)
   {
      send_looklist(list, ch);
      add_to_look_list(list, nullptr);
   }
   else if(show)
   {
      send_to_char("  Nothing.\n\r", ch);
   }

   return static_cast<int>(list != nullptr);
}

auto list_char_to_char(unit_data *i, unit_data *ch) -> int
{
   char                  buffer[MAX_STRING_LENGTH];
   struct looklist_type *list = nullptr;

   for(; i != nullptr; i = i->next)
   {
      if(IS_CHAR(i) && (i != ch) && (i != UNIT_IN(ch)))
      {
         show_char_to_char_blank(buffer, i, ch);
         list = add_to_look_list(list, buffer);
      }
   }

   if(list != nullptr)
   {
      send_to_char(COLOUR_MOB, ch);
      send_looklist(list, ch);
      send_to_char(COLOUR_NORMAL, ch);

      add_to_look_list(list, nullptr);
   }

   return static_cast<int>(list != nullptr);
}

auto list_room_to_char(unit_data *list, unit_data *ch) -> int
{
   unit_data *i;
   int        found = static_cast<int>(FALSE);

   for(i = list; i != nullptr; i = i->next)
   {
      if(IS_ROOM(i) && i != UNIT_IN(ch) && (UNIT_OUT_DESCR_STRING(i) != nullptr) &&
         (static_cast<unsigned int>(str_is_empty(UNIT_OUT_DESCR_STRING(i))) == 0U))
      {
         send_to_char(UNIT_OUT_DESCR_STRING(i), ch);
         send_to_char("\n\r", ch);
         found = static_cast<int>(TRUE);
      }
   }

   return found;
}

void list_contents(unit_data *ch, unit_data *unit, int show)
{
   char                  buffer[MAX_STRING_LENGTH];
   unit_data            *i;
   struct looklist_type *list = nullptr;

   for(i = unit; i != nullptr; i = i->next)
   {
      if((i != ch) && CHAR_CAN_SEE(ch, i) && (i != UNIT_IN(ch)))
      {
         if(IS_CHAR(i))
         {
            show_char_to_char_trans(buffer, i, ch);
         }
         else if(IS_OBJ(i))
         {
            show_obj_to_char_inv(buffer, i, ch);
         }

         list = add_to_look_list(list, buffer);
      }
   }

   if(list != nullptr)
   {
      send_looklist(list, ch);
      add_to_look_list(list, nullptr);
   }
   else if(show != 0)
   {
      send_to_char("Nothing.\n\r", ch);
   }
}

static void look_dir(unit_data *ch, int keyword_no)
{
   unit_data        *room = UNIT_IN(ch);
   extra_descr_data *pExd;

   if(!IS_ROOM(room))
   {
      room = UNIT_IS_TRANSPARENT(room) ? UNIT_IN(room) : nullptr;
   }

   if((room == nullptr) || !IS_ROOM(room))
   {
      send_to_char("No such direction here?\n\r", ch);
      return;
   }

   if((pExd = unit_find_extra((char *)dirs[keyword_no], room)) != nullptr)
   {
      send_to_char(pExd->descr.String(), ch);
      send_to_char("\n\r", ch);
   }

   if((ROOM_EXIT(room, keyword_no) != nullptr) && (ROOM_EXIT(room, keyword_no)->open_name.Name() != nullptr))
   {
      if(IS_SET(ROOM_EXIT(room, keyword_no)->exit_info, EX_CLOSED))
      {
         if(has_found_door(ch, keyword_no) != 0)
         {
            act("The $2t is closed.", A_SOMEONE, ch, ROOM_EXIT(room, keyword_no)->open_name.Name(), nullptr, TO_CHAR);
            return;
         }
      }
      else if(IS_SET(ROOM_EXIT(room, keyword_no)->exit_info, EX_OPEN_CLOSE))
      {
         act("The $2t is open.", A_SOMEONE, ch, ROOM_EXIT(room, keyword_no)->open_name.Name(), nullptr, TO_CHAR);
         return;
      }
   }

   if(pExd == nullptr)
   {
      send_to_char("You see nothing special.\n\r", ch);
   }
}

static void look_in(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *unit;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Look in what?\n\r", ch);
   }
   else if((unit = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) == nullptr)
   {
      send_to_char("No such thing here.\n\r", ch);
   }
   else if(IS_CHAR(unit))
   {
      act("Well, if you kill $3m then you could look inside.", A_SOMEONE, ch, nullptr, unit, TO_CHAR);
   }
   else if(IS_ROOM(unit))
   {
      if(IS_SET(UNIT_OPEN_FLAGS(unit), EX_CLOSED))
      {
         send_to_char("It is closed.\n\r", ch);
      }
      else
      {
         const char *c;
         c = single_unit_messg(unit, "$scope", "", "$1N ($2t):");

         act(c, A_SOMEONE, unit, "here", ch, TO_VICT);
         list_contents(ch, UNIT_CONTAINS(unit), static_cast<int>(TRUE));
         send_done(ch, nullptr, unit, 3, cmd, arg);
      }
   }
   else
   {
      assert(IS_OBJ(unit));
      switch(OBJ_TYPE(unit))
      {
         case ITEM_DRINKCON:
            if(OBJ_VALUE(unit, 1) == 0)
            {
               act("It is empty.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
            }
            else
            {
               int nFull = OBJ_VALUE(unit, 1) * 3;
               int nCol  = OBJ_VALUE(unit, 2);

               if(OBJ_VALUE(unit, 0) > 0)
               {
                  nFull /= OBJ_VALUE(unit, 0);
               }
               else
               {
                  nFull = 3;
               }

               if(nFull < 0)
               {
                  nFull = 3;
               }

               if(is_in(nFull, 0, 3) == 0)
               {
                  slog(LOG_ALL, 0, "%s@%s: Illegal drink container full value.", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
                  nFull = 3;
               }

               if(is_in(nCol, 0, LIQ_MAX) == 0)
               {
                  slog(LOG_ALL, 0, "%s@%s: Illegal drink container type value.", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
                  nCol = 0;
               }

               act("It is $2tfull of a $3t liquid.", A_SOMEONE, ch, fullness[nFull], color_liquid[nCol], TO_CHAR);
               send_done(ch, nullptr, unit, 3, cmd, arg);
            }
            break;

         case ITEM_BOAT:
         case ITEM_CONTAINER:
            if(IS_SET(UNIT_OPEN_FLAGS(unit), EX_CLOSED))
            {
               send_to_char("It is closed.\n\r", ch);
            }
            else
            {
               const char *c;
               c = single_unit_messg(unit, "$scope", "", "$1N ($2t) : ");

               act(c, A_SOMEONE, unit, UNIT_IN(unit) == ch ? (OBJ_EQP_POS(unit) != 0u ? "worn" : "carried") : "here", ch, TO_VICT);

               list_contents(ch, UNIT_CONTAINS(unit), static_cast<int>(TRUE));
               send_done(ch, nullptr, unit, 3, cmd, arg);
            }
            break;

         default:
            send_to_char("It does not appear to be able to contain "
                         "anything.\n\r",
                         ch);
            break;
      }
   }
}

static void look_at(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data        *unit;
   extra_descr_data *ed;
   char             *b = arg;
   char             *c;
   int               i;
   int               j;
   int               fnd;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Look at what?\n\r", ch);
      return;
   }

   c = strchr(b, '\'');
   if((c != nullptr) && c[1] == 's' && ((c[2] == 0) || (isspace(c[2]) != 0)))
   {
      /* examine something specific */
      c[0] = ' ';
      c[1] = ' ';
      unit = find_unit(ch, &b, nullptr, FIND_UNIT_HERE);

      if(unit != nullptr)
      {
         if(IS_CHAR(unit))
         {
            unit_data *target = nullptr;
            target            = find_unit_general(ch, unit, &b, nullptr, FIND_UNIT_EQUIP);
            if(target != nullptr)
            {
               /* describe the object in the unit */
               act("You look at $3n's $2N.", A_HIDEINV, ch, target, unit, TO_CHAR);

               if((ed = unit_find_extra(UNIT_NAME(target), target)) != nullptr)
               {
                  page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
                  page_string(CHAR_DESCRIPTOR(ch), "\n\r");
               }

               show_obj_to_char_lookat(target, ch, static_cast<int>(ed != nullptr ? TRUE : FALSE));

               if(unit != ch)
               {
                  act("$1n looks at $3n's $2N.", A_HIDEINV, ch, target, unit, TO_NOTVICT);
                  act("$1n looks at your $2N.", A_HIDEINV, ch, target, unit, TO_VICT);
               }

               send_done(ch, unit, target, 1, cmd, arg);
               return;
            }
         } /* IS_CHAR */

         /* locate extra descr. */
         if((ed = unit_find_extra(b, unit)) != nullptr)
         {
            unit_data *eq = nullptr;

            /* now generate relevant equip descr. */
            fnd           = 0;
            if(IS_CHAR(unit))
            {
               /* Is this a body part at all ? */
               if(ed->names.Name() != nullptr)
               {
                  char buf[256];
                  strcpy(buf, ed->names.Name());

                  i = search_block(buf, bodyparts, 1);
               }
               else
               {
                  i = -1;
               }

               if(i != -1)
               {
                  /* search for body parts */
                  for(j = 0; bodyparts_cover[i][j] != WEAR_UNUSED; j++)
                  {
                     eq = equipment(unit, bodyparts_cover[i][j]);
                     if((eq != nullptr) && (!IS_SET(OBJ_FLAGS(eq), OBJ_NOCOVER)) && CHAR_CAN_SEE(ch, eq))
                     {
                        fnd = 1;
                        break;
                     }
                  }

                  if(fnd == 1)
                  {
                     char buf[200];

                     sprintf(buf, "$3n has $3s %s covered by $2n.", bodyparts[i]);
                     act(buf, A_ALWAYS, ch, eq, unit, TO_CHAR);
                     return;
                  }
               }
            } /* else, no body parts are searched */

            if(fnd == 0)
            {
               /* uncovered or not a char */
               if(ed->names.Name() != nullptr)
               {
                  act("You look at $2n's $3t.", A_HIDEINV, ch, unit, ed->names.Name(), TO_CHAR);
               }
               else
               {
                  act("You look at $2n.", A_HIDEINV, ch, unit, nullptr, TO_CHAR);
               }

               page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
               page_string(CHAR_DESCRIPTOR(ch), "\n\r");

               if(unit != ch)
               {
                  if(ed->names.Name() != nullptr)
                  {
                     act("$1n looks at $3n's $2t.", A_HIDEINV, ch, ed->names.Name(), unit, TO_NOTVICT);
                     act("$1n looks at your $2t.", A_HIDEINV, ch, ed->names.Name(), unit, TO_VICT);
                  }
                  else
                  {
                     act("$1n looks at $3n.", A_HIDEINV, ch, nullptr, unit, TO_NOTVICT);
                     act("$1n looks at you.", A_HIDEINV, ch, nullptr, unit, TO_VICT);
                  }
               }
               send_done(ch, nullptr, unit, 1, cmd, arg);
            }
         }
         else /* if (ed = ... ) */
         {
            /* unit does not seem to have such a thing */
            if(is_name(b, bodyparts) != nullptr)
            {
               send_to_char("You see nothing special.\n\r", ch);
            }
            else
            {
               if(static_cast<unsigned int>(str_is_empty(b)) != 0U)
               {
                  act("Look at $1n's what?", A_ALWAYS, unit, nullptr, ch, TO_VICT);
               }
               else
               {
                  act("$1n does not seem to have a $2t.", A_ALWAYS, unit, skip_blanks(b), ch, TO_VICT);
               }
            }
         }
         return;
      }
   }
   else
   {
      unit = find_unit(ch, &b, nullptr, FIND_UNIT_HERE);
   }

   unit_data *target;

   /* If there are no units with the given name, we must */
   /* assume that the look was upon an extra description */
   /* If we looked at a room, it is not handeled like    */
   /* ordinary objects in the room, since room extra's   */
   /* are always shown when INSIDE the rooms (not out)   */
   if((unit == nullptr) || IS_ROOM(unit))
   {
      if((ed = char_unit_find_extra(ch, &target, arg, UNIT_IN(ch))) != nullptr)
      {
         act("You look at the $3t.", A_HIDEINV, ch, nullptr, ed->names.Name() != nullptr ? ed->names.Name() : UNIT_NAME(target), TO_CHAR);
         page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
         page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         send_done(ch, nullptr, nullptr, 2, cmd, arg);
      }
      else if((UNIT_IN(ch) != nullptr) && UNIT_IS_TRANSPARENT(UNIT_IN(ch)) && (UNIT_IN(UNIT_IN(ch)) != nullptr) &&
              ((ed = char_unit_find_extra(ch, &target, arg, UNIT_IN(UNIT_IN(ch)))) != nullptr))
      {
         act("You look at the $3t.", A_HIDEINV, ch, nullptr, ed->names.Name() != nullptr ? ed->names.Name() : UNIT_NAME(target), TO_CHAR);
         page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
         page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         send_done(ch, nullptr, nullptr, 2, cmd, arg);
      }
      else if((ed = char_unit_find_extra(ch, &target, arg, UNIT_IN(ch))) != nullptr)
      {
         act("You look at the $3t.", A_HIDEINV, ch, nullptr, ed->names.Name() != nullptr ? ed->names.Name() : UNIT_NAME(target), TO_CHAR);
         page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
         page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         send_done(ch, nullptr, nullptr, 2, cmd, arg);
      }
      else if((ed = char_unit_find_extra(ch, &target, arg, UNIT_CONTAINS(ch))) != nullptr)
      {
         act("You look at your $3t.", A_HIDEINV, ch, nullptr, ed->names.Name() != nullptr ? ed->names.Name() : UNIT_NAME(target), TO_CHAR);
         page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
         page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         send_done(ch, nullptr, nullptr, 2, cmd, arg);
      }
      else if((unit == nullptr) && ((ed = char_unit_find_extra(ch, &target, arg, UNIT_CONTAINS(UNIT_IN(ch)))) != nullptr))
      {
         act("You look at $2n's $3t.", A_HIDEINV, ch, target, ed->names.Name() != nullptr ? ed->names.Name() : UNIT_NAME(target), TO_CHAR);
         page_string(CHAR_DESCRIPTOR(ch), ed->descr.String());
         page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         send_done(ch, nullptr, nullptr, 2, cmd, arg);
      }
      else if(unit != nullptr)
      {
         send_to_char("You see nothing special about it.\n\r", ch);
      }
      else
      {
         send_to_char("You do not see that here.\n\r", ch);
      }
   }
   else
   {
      extra_descr_data *exd;

      if(unit != nullptr)
      {
         exd = unit_find_extra(UNIT_NAME(unit), unit);
      }
      else
      {
         exd = unit_find_extra(arg, unit);
      }

      if(IS_CHAR(unit))
      {
         if(exd == nullptr)
         {
            /* I'll force people to make their descriptions - he he */
            if(IS_PC(unit))
            {
               act("$3e has big green ears, and long greasy hair.", A_SOMEONE, ch, nullptr, unit, TO_CHAR);
            }
            else
            {
               act("You see nothing special about $3m.", A_SOMEONE, ch, nullptr, unit, TO_CHAR);
            }
         }
         else
         {
            page_string(CHAR_DESCRIPTOR(ch), exd->descr.String());
            page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         }

         if(ch != unit)
         {
            act("$1n looks at you.", A_HIDEINV, ch, nullptr, unit, TO_VICT);
            act("$1n looks at $3n.", A_HIDEINV, ch, nullptr, unit, TO_NOTVICT);
         }
         else
         {
            act("$1n looks at $1mself.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
         }

         if(affected_by_spell(unit, ID_REWARD) != nullptr)
         {
            act("There is a reward on $2s head.", A_SOMEONE, ch, unit, nullptr, TO_CHAR);
         }

         if(IS_SET(CHAR_FLAGS(unit), CHAR_PROTECTED))
         {
            if(IS_SET(CHAR_FLAGS(unit), CHAR_OUTLAW))
            {
               act("$1n an outlaw wanted alive.", A_SOMEONE, unit, nullptr, ch, TO_VICT);
            }
            else
            {
               act("$1n is a protected citizen.", A_SOMEONE, unit, nullptr, ch, TO_VICT);
            }
         }
         else if(IS_SET(CHAR_FLAGS(unit), CHAR_OUTLAW))
         {
            act("$1n is an outlaw, wanted dead or alive.", A_SOMEONE, unit, nullptr, ch, TO_VICT);
         }

         if(IS_SET(CHAR_FLAGS(unit), CHAR_LEGAL_TARGET))
         {
            act("You may kill $1m now.", A_SOMEONE, unit, nullptr, ch, TO_VICT);
         }

         if((g_cServerConfig.m_bBOB != 0) && IS_PC(unit) && IS_SET(PC_FLAGS(unit), PC_PK_RELAXED))
         {
            act("You notice a scar on $3s wrist from the Book of Blood.", A_SOMEONE, ch, nullptr, unit, TO_CHAR);
         }

         show_char_to_char_lookat(unit, ch); /* Show equipment etc. */
         send_done(ch, nullptr, unit, 2, cmd, arg);
      }
      else if(IS_OBJ(unit))
      {
         if(exd != nullptr)
         {
            page_string(CHAR_DESCRIPTOR(ch), exd->descr.String());
            page_string(CHAR_DESCRIPTOR(ch), "\n\r");
         }
         else
         {
            send_to_char("You see nothing special.\n\r", ch);
         }
         show_obj_to_char_lookat(unit, ch, static_cast<int>(TRUE));
         send_done(ch, nullptr, unit, 2, cmd, arg);
      }
      else
      {
         assert(FALSE); /* Illegal type */
      }
   }
}

static void look_exits(unit_data *ch)
{
   if(!IS_PC(ch))
   {
      return;
   }

   if(!IS_SET(PC_FLAGS(ch), PC_EXITS))
   {
      return;
   }

   int         door;
   int         found                  = static_cast<int>(FALSE);
   char        buf[MAX_STRING_LENGTH] = "";
   char       *b                      = buf;
   unit_data  *room;

   const char *exits[] = {"North", "East", "South", "West", "Up", "Down"};

   strcat(b, g_cServerConfig.m_sColor.pExitsBegin);
   TAIL(b);

   room = UNIT_IN(ch);
   if(!IS_ROOM(room) && UNIT_IS_TRANSPARENT(room))
   {
      room = UNIT_IN(room);
   }

   if(room == nullptr || !IS_ROOM(room))
   {
      strcat(b, "none");
      strcat(b, g_cServerConfig.m_sColor.pExitsEnd);
      strcat(b, "\n\r");
      send_to_char(buf, ch);
      return;
   }

   for(door = 0; door <= 5; door++)
   {
      if((ROOM_EXIT(room, door) != nullptr) && (ROOM_EXIT(room, door)->to_room != nullptr))
      {
         if(!IS_SET(ROOM_EXIT(room, door)->exit_info, EX_CLOSED))
         {
            sprintf(b, "%s, ", exits[door]);
            TAIL(b);
            found = static_cast<int>(TRUE);
         }
         else if(has_found_door(ch, door) != 0)
         {
            sprintf(b, "%s, ", exits[door]);
            TAIL(b);
            found = static_cast<int>(TRUE);
         }
      }
   }

   if(found != 0)
   {
      b -= 2;
      *b = 0;
   }
   else
   {
      strcat(b, "none");
   }

   strcat(b, g_cServerConfig.m_sColor.pExitsEnd);
   strcat(b, "\n\r");

   send_to_char(buf, ch);
}

static void look_blank(unit_data *ch, const struct command_info *cmd)
{
   char buffer[MAX_STRING_LENGTH];

   if(IS_ROOM(UNIT_IN(ch)))
   { /* inside a room, no upward recursion */
      if(IS_IMMORTAL(ch))
      {
         sprintf(buffer,
                 "%s%s%s [%s@%s]\n\r",
                 g_cServerConfig.m_sColor.pRoomTitle,
                 UNIT_TITLE_STRING(UNIT_IN(ch)),
                 g_cServerConfig.m_sColor.pDefault,
                 UNIT_FI_NAME(UNIT_IN(ch)),
                 UNIT_FI_ZONENAME(UNIT_IN(ch)));
      }
      else
      {
         sprintf(
            buffer, "%s%s%s\n\r", g_cServerConfig.m_sColor.pRoomTitle, UNIT_TITLE_STRING(UNIT_IN(ch)), g_cServerConfig.m_sColor.pDefault);
      }

      send_to_char(buffer, ch);

      if(!(IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_BRIEF) && cmd->no >= CMD_NORTH && cmd->no <= CMD_DOWN))
      {
         sprintf(
            buffer, "%s%s%s\n\r", g_cServerConfig.m_sColor.pRoomText, UNIT_IN_DESCR_STRING(UNIT_IN(ch)), g_cServerConfig.m_sColor.pDefault);
         send_to_char(buffer, ch);
      }

      look_exits(ch);

      list_room_to_char(UNIT_CONTAINS(UNIT_IN(ch)), ch);
      list_obj_to_char(UNIT_CONTAINS(UNIT_IN(ch)), ch, FALSE);
      list_char_to_char(UNIT_CONTAINS(UNIT_IN(ch)), ch);
   }
   else
   {
      /* Inside an object/char */

      if(UNIT_IN_DESCR(UNIT_IN(ch)).StringPtr())
      {
         act(UNIT_IN_DESCR_STRING(UNIT_IN(ch)), A_ALWAYS, ch, UNIT_IN(ch), nullptr, TO_CHAR);
      }
      else
      {
         act("You are $2t $3n", A_SOMEONE, ch, IS_OBJ(UNIT_IN(ch)) ? "inside" : "carried by", UNIT_IN(ch), TO_CHAR);
      }

      sprintf(buffer, "%s %s :", IS_OBJ(UNIT_IN(ch)) ? "The $1N" : "$1n", IS_OBJ(UNIT_IN(ch)) ? "contains" : "carries");

      const char *c;
      c = single_unit_messg(UNIT_IN(ch), "$scope", "", buffer);

      act(c, A_SOMEONE, UNIT_IN(ch), IS_OBJ(UNIT_IN(ch)) ? "contains" : "carries", ch, TO_VICT);

      /* list units present */
      list_contents(ch, UNIT_CONTAINS(UNIT_IN(ch)), static_cast<int>(TRUE));

      /* show unit, the surrounding unit is in */
      if(UNIT_IS_TRANSPARENT(UNIT_IN(ch)))
      {
         unit_data *room = UNIT_IN(UNIT_IN(ch));

         send_to_char("\n\r", ch);

         if(UNIT_IS_DARK(room))
         {
            send_to_char("It is pitch black.. \n\r", ch);
         }
         else
         {
            if(IS_ROOM(room))
            {
               /* inside a room, no upward recursion */
               sprintf(buffer, "%s\n\r", UNIT_TITLE_STRING(room));
               send_to_char(buffer, ch);
               if(!(IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_BRIEF) && cmd->no >= CMD_NORTH && cmd->no <= CMD_DOWN))
               {
                  sprintf(buffer, "%s\n\r", UNIT_IN_DESCR_STRING(room));
                  send_to_char(buffer, ch);
               }

               look_exits(ch);

               /* now list the unit, the surrounding unit is in */
               list_room_to_char(UNIT_CONTAINS(room), ch);
               list_obj_to_char(UNIT_CONTAINS(room), ch, FALSE);
               list_char_to_char(UNIT_CONTAINS(room), ch);
            }
            else
            {
               /* Inside an object/char */
               sprintf(buffer,
                       "%s is %s $2n, which %s",
                       IS_OBJ(UNIT_IN(ch)) ? "The $1N" : "$1n",
                       IS_OBJ(room) ? "inside" : "carried by",
                       IS_OBJ(room) ? "contains" : "carries");

               act(buffer, A_SOMEONE, UNIT_IN(ch), UNIT_IN(UNIT_IN(ch)), ch, TO_VICT);

               /* now list the unit, the surrounding unit is in */
               list_contents(ch, UNIT_CONTAINS(room), static_cast<int>(TRUE));
            }
         }
      }
   }

   send_done(ch, nullptr, nullptr, 0, cmd, "");
}

void do_look(unit_data *ch, char *aaa, const struct command_info *cmd)
{
   char *arg = (char *)aaa;
   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   if(CHAR_POS(ch) < POSITION_SLEEPING)
   {
      send_to_char("You can't see anything but stars!\n\r", ch);
   }
   else if(CHAR_POS(ch) == POSITION_SLEEPING)
   {
      send_to_char("You can not see anything when you're sleeping.\n\r", ch);
   }
   else if(CHAR_HAS_FLAG(ch, CHAR_BLIND))
   {
      send_to_char("You can't see a damn thing, you are blinded!\n\r", ch);
   }
   else if(IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried!\n\r", ch);
   }
   else if(UNIT_IS_DARK(UNIT_IN(ch)))
   {
      send_to_char("It is pitch black.\n\r", ch);
   }
   else
   {
      static const char *keywords[] = {"north",
                                       "east",
                                       "south",
                                       "west",
                                       "up",
                                       "down",
                                       "in",
                                       "at",
                                       "", /* Look at '' case (8) */
                                       nullptr};

      char               arg1[MAX_INPUT_LENGTH];
      int                keyword_no;

      str_next_word(arg, arg1);
      keyword_no = search_block(arg1, keywords, FALSE); /* Partiel Match */

      if(keyword_no == -1)
      {
         if(static_cast<unsigned int>(str_is_empty(arg1)) != 0U) /* wrong arg  */
         {
            send_to_char("Sorry, I didn't understand that.\n\r", ch);
            return;
         }
         keyword_no = 7;
      }
      else
      {
         char *pOrgArg = arg;

         arg           = str_next_word(arg, arg1);
         if((keyword_no >= 0) && (keyword_no <= 5))
         {
            if(static_cast<unsigned int>(str_is_empty(arg)) == 0U)
            {
               arg        = pOrgArg;
               keyword_no = 7;
            }
         }
      }

      switch(keyword_no)
      {
         case 0: /* look <dir> */
         case 1:
         case 2:
         case 3:
         case 4:
         case 5:
            if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
            {
               look_dir(ch, keyword_no);
            }

            break;

         case 6: /* look 'in'  */
            look_in(ch, arg, cmd);
            break;

         case 7: /* look 'at'  */
            look_at(ch, arg, cmd);
            break;

         case 8: /* look ''    */
            look_blank(ch, cmd);
            break;
      }
   }
}
/* end of look */

void do_read(unit_data *ch, char *arg, const struct command_info *cmd)
{
   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Read what?\n\r", ch);
      return;
   }

   do_look(ch, arg, cmd);

#ifdef SUSPEKT
   /* I dont like the idea of $read anyway... :-( --Michael
      Sorry about all the hype. */

   obj = find_unit(ch, &arg, 0, FIND_UNIT_HERE);

   if(obj == NULL)
   {
      send_to_char("There is no such thing to read from.\n\r", ch);
      return;
   }

   if(!IS_OBJ(obj))
   {
      act("You can not read from $2n, try to look at it instead.", A_ALWAYS, ch, obj, 0, TO_CHAR);
      return;
   }

   exd = find_raw_ex_descr("$read", UNIT_EXTRA_DESCR(obj));
   if(exd)
   {
      act("There is something written upon the $2N:\n\r", A_SOMEONE, ch, obj, 0, TO_CHAR);
      page_string(CHAR_DESCRIPTOR(ch), exd->descr, 1);
   }
   else
      act("The $2N is blank.", A_SOMEONE, ch, obj, 0, TO_CHAR);
#endif
}

void do_examine(unit_data *ch, char *aaa, const struct command_info *cmd)
{
   char      *arg = (char *)aaa;
   char      *b;
   unit_data *unit;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Examine what?\n\r", ch);
      return;
   }

   b    = arg;

   unit = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE);

   if(unit == nullptr)
   {
      look_at(ch, b, cmd);
      return;
   }

   show_examine_aura(ch, unit);

   look_at(ch, b, cmd);

   if((unit != nullptr) && IS_OBJ(unit) && (OBJ_TYPE(unit) == ITEM_DRINKCON || OBJ_TYPE(unit) == ITEM_CONTAINER))
   {
      send_to_char("When you look inside, you see:\n\r", ch);
      look_in(ch, b, cmd);
   }
}

void do_inventory(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char                  buffer[MAX_STRING_LENGTH];
   unit_data            *thing;
   struct looklist_type *list = nullptr;

   send_to_char("You are carrying:\n\r", ch);

   for(thing = UNIT_CONTAINS(ch); thing != nullptr; thing = thing->next)
   {
      if(CHAR_CAN_SEE(ch, thing))
      {
         if(IS_OBJ(thing))
         {
            if((OBJ_EQP_POS(thing) == 0u) && !IS_MONEY(thing))
            {
               show_obj_to_char_inv(buffer, thing, ch);
               list = add_to_look_list(list, buffer);
            }
         }
         else if(IS_CHAR(thing))
         {
            show_char_to_char_inv(buffer, thing, ch);
            list = add_to_look_list(list, buffer);
         }
         else
         {
            slog(LOG_ALL, 0, "NEAR FATAL: Illegal item in inventory.");
         }
      }
   }

   if(list != nullptr)
   {
      send_looklist(list, ch);
      add_to_look_list(list, nullptr);
   }
   else
   {
      send_to_char("  Nothing.\n\r", ch);
   }
}

void do_equipment(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char       buf[MAX_STRING_LENGTH];
   unit_data *thing;
   bool       found;

   send_to_char("You are using:\n\r", ch);
   found = FALSE;

   for(thing = UNIT_CONTAINS(ch); thing != nullptr; thing = thing->next)
   {
      if(IS_OBJ(thing) && (OBJ_EQP_POS(thing) != 0u) && CHAR_CAN_SEE(ch, thing))
      {
         found = TRUE;

         if(OBJ_EQP_POS(thing) == WEAR_WIELD && (object_two_handed(thing) != 0))
         {
            send_to_char("<two hand wielded>   ", ch);
         }
         else
         {
            send_to_char(where[OBJ_EQP_POS(thing)], ch);
         }

         show_obj_to_char_inv(buf, thing, ch);
         send_to_char(buf, ch);
      }
   }

   if(!found)
   {
      send_to_char("  Nothing.\n\r", ch);
   }
}
