#include "handler.h"

#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "db.h"
#include "destruct.h"
#include "dilrun.h"
#include "files.h"
#include "interpreter.h"
#include "main.h"
#include "money.h"
#include "nanny.h"
#include "skills.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "unit_function_array_type.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

extern unit_data *combat_list;

/* External procedures */

void stop_special(unit_data *u, unit_fptr *fptr);

auto unit_is_edited(unit_data *u) -> descriptor_data *
{
   descriptor_data *d;

   for(d = descriptor_list; d != nullptr; d = d->next)
   {
      if(d->editing == u)
      {
         return d;
      }
   }

   return nullptr;
}

/* By using this, we can easily sort the list if ever needed */
void insert_in_unit_list(unit_data *u)
{
   unit_data *tmp_u;

   tmp_u = unit_list;

   if(!IS_PC(u))
   {
      for(; (tmp_u != nullptr) && IS_PC(tmp_u); tmp_u = tmp_u->gnext)
      {
         ;
      }
   }

   if((tmp_u == nullptr) || (tmp_u == unit_list)) /* Head of list */
   {
      u->gnext     = unit_list;
      u->gprevious = nullptr;
      if(unit_list != nullptr)
      {
         unit_list->gprevious = u;
      }
      unit_list = u;
   }
   else /* Middle of list  */
   {
      u->gnext                = tmp_u;
      u->gprevious            = tmp_u->gprevious;
      tmp_u->gprevious->gnext = u;
      tmp_u->gprevious        = u;
   }
}

/* Remove a unit from the unit_list */
void remove_from_unit_list(unit_data *unit)
{
   assert(unit->gprevious || unit->gnext || (unit_list == unit));

   if(unit_list == unit)
   {
      unit_list = unit->gnext;
   }
   else
   { /* Then this is always true 'if (unit->gprevious)'  */
      unit->gprevious->gnext = unit->gnext;
   }

   if(unit->gnext != nullptr)
   {
      unit->gnext->gprevious = unit->gprevious;
   }

   unit->gnext = unit->gprevious = nullptr;
}

auto find_fptr(unit_data *u, uint16_t idx) -> unit_fptr *
{
   unit_fptr *tf;

   for(tf = UNIT_FUNC(u); tf != nullptr; tf = tf->next)
   {
      if(tf->index == idx)
      {
         return tf;
      }
   }

   return nullptr;
}

auto create_fptr(unit_data *u, uint16_t index, uint16_t beat, uint16_t flags, void *data) -> unit_fptr *
{
   unit_fptr *f;

   void start_special(unit_data * u, unit_fptr * fptr);

   CREATE(f, unit_fptr, 1);
   assert(f);
   assert(!is_destructed(DR_FUNC, f));

   f->index      = index;
   f->heart_beat = beat;
   f->flags      = flags;
   f->data       = data;

   f->next      = UNIT_FUNC(u);
   UNIT_FUNC(u) = f;

   start_special(u, f);

   return f;
}

/* Does not free 'f' - it is done by clear_destruct by comm.c */
void destroy_fptr(unit_data *u, unit_fptr *f)
{
   unit_fptr *tf;
   spec_arg   sarg;

   assert(f);
   assert(!is_destructed(DR_FUNC, f));

   register_destruct(DR_FUNC, f);

#ifdef DEBUG_HISTORY
   add_func_history(u, f->index, 0);
#endif

   sarg.owner     = (unit_data *)u;
   sarg.activator = nullptr;
   sarg.medium    = nullptr;
   sarg.target    = nullptr;
   sarg.pInt      = nullptr;
   sarg.fptr      = f;
   sarg.cmd       = &cmd_auto_extract;
   sarg.arg       = "";
   sarg.mflags    = ((uint16_t)0);

   (*(unit_function_array[f->index].func))(&sarg);

   /* Data is free'ed in destruct() if it is not NULL now */

   stop_special((unit_data *)u, f);

   /* Only unlink function, do not free it! */
   if(UNIT_FUNC(u) == f)
   {
      UNIT_FUNC(u) = f->next;
   }
   else
   {
      for(tf = UNIT_FUNC(u); (tf != nullptr) && (tf->next != f); tf = tf->next)
      {
         ;
      }
      if(tf != nullptr)
      {
         assert(tf->next == f);
         tf->next = f->next;
      }
   }
}

/* Stop the 'ch' from following his master    */
/* Call die_follower if a person dies         */
void stop_following(unit_data *ch)
{
   char_follow_type *j;
   char_follow_type *k;

   assert(CHAR_MASTER(ch));

   if(CHAR_FOLLOWERS(CHAR_MASTER(ch))->follower == ch) /* Head of list? */
   {
      k                               = CHAR_FOLLOWERS(CHAR_MASTER(ch));
      CHAR_FOLLOWERS(CHAR_MASTER(ch)) = k->next;
      free(k);
   }
   else
   { /* locate follower who is not head of list */
      for(k = CHAR_FOLLOWERS(CHAR_MASTER(ch)); k->next->follower != ch; k = k->next)
      {
         ;
      }
      j       = k->next;
      k->next = j->next;
      free(j);
   }

   CHAR_MASTER(ch) = nullptr;

   send_done(ch, nullptr, nullptr, 0, cmd_follow, "");
}

/* Set 'ch' to follow leader. Circles allowed. */
void start_following(unit_data *ch, unit_data *leader)
{
   char_follow_type *k;

   assert(!is_destructed(DR_UNIT, leader));
   assert(!is_destructed(DR_UNIT, ch));

   REMOVE_BIT(CHAR_FLAGS(ch), CHAR_GROUP);
   if(CHAR_MASTER(ch))
   {
      stop_following(ch);
   }
   CHAR_MASTER(ch) = leader;
   CREATE(k, char_follow_type, 1);
   k->follower            = ch;
   k->next                = CHAR_FOLLOWERS(leader);
   CHAR_FOLLOWERS(leader) = k;

   send_done(ch, nullptr, leader, 0, cmd_follow, "");
}

/* Called by extract_unit when a character that follows/is followed dies */
void die_follower(unit_data *ch)
{
   char_follow_type *j;
   char_follow_type *k;

   if(CHAR_MASTER(ch))
   {
      stop_following(ch);
   }

   for(k = CHAR_FOLLOWERS(ch); k != nullptr; k = j)
   {
      j = k->next;
      stop_following(k->follower);
   }
}

/* Call this routine if you modify the brightness of a unit */
/* in order to correctly update the environment of the unit */
void modify_bright(unit_data *unit, int bright)
{
   unit_data *ext;
   unit_data *in;

   UNIT_BRIGHT(unit) += bright;

   if((in = UNIT_IN(unit)) != nullptr)
   { /* Light up what the unit is inside */
      UNIT_LIGHTS(in) += bright;
   }

   if(IS_OBJ(unit) && (OBJ_EQP_POS(unit) != 0u))
   {
      /* The char holding the torch light up the SAME way the torch does! */
      /* this works with the equib/unequib functions. This is NOT a case  */
      /* of transparancy.  */
      modify_bright(in, bright);
   }
   else if((in != nullptr) && UNIT_IS_TRANSPARENT(in))
   {
      /* the unit is inside a transperant unit, so it lights up too */
      /* this works with actions in unit-up/down                    */
      UNIT_BRIGHT(in) += bright;
      if((ext = UNIT_IN(in)) != nullptr)
      {
         UNIT_LIGHTS(ext) += bright;
         UNIT_ILLUM(in) += bright;
      }
   }
}

void trans_set(unit_data *u)
{
   unit_data *u2;
   int        sum = 0;

   for(u2 = UNIT_CONTAINS(u); u2 != nullptr; u2 = u2->next)
   {
      sum += UNIT_BRIGHT(u2);
   }

   UNIT_ILLUM(u) = sum;
   UNIT_BRIGHT(u) += sum;

   if(UNIT_IN(u) != nullptr)
   {
      UNIT_LIGHTS(UNIT_IN(u)) += sum;
   }
}

void trans_unset(unit_data *u)
{
   UNIT_BRIGHT(u) -= UNIT_ILLUM(u);

   if(UNIT_IN(u) != nullptr)
   {
      UNIT_LIGHTS(UNIT_IN(u)) -= UNIT_ILLUM(u);
   }

   UNIT_ILLUM(u) = 0;
}

/*
void recalc_dex_red(unit_data *ch)
{
   unit_data *eq;
   int reduction;

   reduction = 0;

   for (eq = UNIT_CONTAINS(ch); eq; eq = eq->next)
     if (IS_OBJ(eq) && OBJ_EQP_POS(eq))
     {
       if (OBJ_TYPE(eq) == ITEM_ARMOR && CHAR_STR(ch) < OBJ_VALUE(eq, 0))
         reduction += OBJ_VALUE(eq, 0) - CHAR_STR(ch);
       else if (OBJ_TYPE(eq) == ITEM_SHIELD && CHAR_STR(ch) < OBJ_VALUE(eq, 3))
         reduction += OBJ_VALUE(eq, 3) - CHAR_STR(ch);
     }

   CHAR_DEX_RED(ch) = std::min(95, reduction);
}
*/

auto equipment(unit_data *ch, uint8_t pos) -> unit_data *
{
   unit_data *u;

   assert(IS_CHAR(ch));

   for(u = UNIT_CONTAINS(ch); u != nullptr; u = u->next)
   {
      if(IS_OBJ(u) && pos == OBJ_EQP_POS(u))
      {
         return u;
      }
   }

   return nullptr;
}

/* The following functions find armor / weapons on a person with     */
/* type checks (i.e. trash does not protect!).                       */
auto equipment_type(unit_data *ch, int pos, uint8_t type) -> unit_data *
{
   unit_data *obj;

   obj = equipment(ch, pos);

   if((obj != nullptr) && OBJ_TYPE(obj) == type)
   {
      return obj;
   }
   return nullptr;
}

void equip_char(unit_data *ch, unit_data *obj, uint8_t pos)
{
   unit_affected_type *af;
   unit_affected_type  newaf;

   assert(pos > 0 && IS_OBJ(obj) && IS_CHAR(ch));
   assert(!equipment(ch, pos));
   assert(UNIT_IN(obj) == ch); /* Must carry object in inventory */

   OBJ_EQP_POS(obj) = pos;

   /* recalc_dex_red(ch); */

   modify_bright(ch, UNIT_BRIGHT(obj)); /* Update light sources */

   for(af = UNIT_AFFECTED(obj); af != nullptr; af = af->next)
   {
      if(af->id < 0) /* It is a transfer affect! */
      {
         newaf          = *af;
         newaf.id       = -newaf.id; /* No longer a transfer    */
         newaf.duration = -1;        /* Permanent until unequip */
         create_affect(ch, &newaf);
      }
   }
}

auto unequip_object(unit_data *obj) -> unit_data *
{
   unit_data          *ch;
   unit_affected_type *af;
   unit_affected_type *caf;

   ch = UNIT_IN(obj);

   assert(IS_OBJ(obj) && OBJ_EQP_POS(obj));
   assert(IS_CHAR(ch));

   OBJ_EQP_POS(obj) = 0;

   /* recalc_dex_red(ch); */

   modify_bright(ch, -UNIT_BRIGHT(obj)); /* Update light sources */

   for(af = UNIT_AFFECTED(obj); af != nullptr; af = af->next)
   {
      if(af->id < 0) /* It is a transfer affect! */
      {
         for(caf = UNIT_AFFECTED(ch); caf != nullptr; caf = caf->next)
         {
            if((-caf->id == af->id) && (caf->duration == -1) && (caf->data[0] == af->data[0]) && (caf->data[1] == af->data[1]) &&
               // THIS IS NOT TESTED! (caf->data[2] == af->data[2]) &&
               (caf->applyf_i == af->applyf_i) && (caf->firstf_i == af->firstf_i) && (caf->lastf_i == af->lastf_i) &&
               (caf->tickf_i == af->tickf_i))
            {
               destroy_affect(caf);
               break; /* Skip inner for loop since we found the affect */
            }
         }
      }
   }

   return obj;
}

auto unequip_char(unit_data *ch, uint8_t pos) -> unit_data *
{
   unit_data *obj;

   obj = equipment(ch, pos);
   assert(obj);

   unequip_object(obj);

   return obj;
}

/* Checks if a unit which is to be put inside another unit will pass */
/* through itself. For example if Papi carries a coffin and God is   */
/* inside the coffin, when God types trans Papi this function will   */
/* return TRUE                                                       */
auto unit_recursive(unit_data *from, unit_data *to) -> int
{
   unit_data *u;

   for(u = to; u != nullptr; u = UNIT_IN(u))
   {
      if(u == from)
      {
         return static_cast<int>(TRUE);
      }
   }

   return static_cast<int>(FALSE);
}

auto unit_zone(const unit_data *unit) -> zone_type *
{
   auto *org = (unit_data *)unit;

   for(; unit != nullptr; unit = UNIT_IN(unit))
   {
      if(UNIT_IN(unit) == nullptr)
      {
         assert(IS_ROOM(unit));
         return UNIT_FILE_INDEX(unit)->zone;
      }
   }

   slog(LOG_ALL, 0, "ZONE: FATAL: %s@%s IN NO ROOMS WHILE NOT A ROOM!!", UNIT_FI_NAME(org), UNIT_FI_ZONENAME(org));
   return nullptr;
}

auto unit_room(unit_data *unit) -> unit_data *
{
   unit_data *org = unit;

   for(; unit != nullptr; unit = UNIT_IN(unit))
   {
      if(IS_ROOM(unit))
      {
         return unit;
      }
   }

   slog(LOG_ALL, 0, "ROOM: FATAL: %s@%s IN NO ROOMS WHILE NOT A ROOM!!", UNIT_FI_NAME(org), UNIT_FI_ZONENAME(org));
   return nullptr;
}

void intern_unit_up(unit_data *unit, bool pile)
{
   unit_data *u;
   unit_data *in;
   unit_data *toin;
   unit_data *extin;
   int8_t     bright;
   int8_t     selfb;

   assert(UNIT_IN(unit));

   /* resolve *ALL* light!!! */
   in     = UNIT_IN(unit);                             /* where to move unit up to */
   toin   = UNIT_IN(in);                               /* unit around in           */
   extin  = toin != nullptr ? UNIT_IN(toin) : nullptr; /* unit around toin         */
   bright = UNIT_BRIGHT(unit);                         /* brightness inc. trans    */
   selfb  = bright - UNIT_ILLUM(unit);                 /* brightness excl. trans   */

   UNIT_LIGHTS(in) -= bright; /* Subtract Light */
   if(UNIT_IS_TRANSPARENT(in))
   {
      UNIT_ILLUM(in) -= selfb;
      UNIT_BRIGHT(in) -= selfb;
   }
   else if(toin != nullptr)
   {
      UNIT_LIGHTS(toin) += bright;
   }

   if((toin != nullptr) && UNIT_IS_TRANSPARENT(toin))
   {
      UNIT_BRIGHT(toin) += selfb;
      UNIT_ILLUM(toin) += selfb;
      if(extin != nullptr)
      {
         UNIT_LIGHTS(extin) += selfb;
      }
   }

   if(IS_CHAR(unit))
   {
      --UNIT_CHARS(UNIT_IN(unit));
   }

   UNIT_WEIGHT(UNIT_IN(unit)) -= UNIT_WEIGHT(unit);

   if(unit == UNIT_CONTAINS(UNIT_IN(unit)))
   {
      UNIT_CONTAINS(UNIT_IN(unit)) = unit->next;
   }
   else
   {
      for(u = UNIT_CONTAINS(UNIT_IN(unit)); u->next != unit; u = u->next)
      {
         ;
      }
      u->next = unit->next;
   }

   unit->next = nullptr;

   if((UNIT_IN(unit) = UNIT_IN(UNIT_IN(unit))) != nullptr)
   {
      unit->next                   = UNIT_CONTAINS(UNIT_IN(unit));
      UNIT_CONTAINS(UNIT_IN(unit)) = unit;
      if(IS_CHAR(unit))
      {
         ++UNIT_CHARS(UNIT_IN(unit));
      }
   }

   if((static_cast<unsigned int>(pile) != 0U) && IS_MONEY(unit) && (UNIT_IN(unit) != nullptr))
   {
      pile_money(unit);
   }
}

void unit_up(unit_data *unit)
{
   intern_unit_up(unit, TRUE);
}

void unit_from_unit(unit_data *unit)
{
   while(UNIT_IN(unit) != nullptr)
   {
      intern_unit_up(unit, FALSE);
   }
}

void intern_unit_down(unit_data *unit, unit_data *to, bool pile)
{
   unit_data *u;
   unit_data *in;
   unit_data *extin;
   int8_t     bright;
   int8_t     selfb;

   assert(UNIT_IN(unit) == UNIT_IN(to));
   assert(unit != to);

   /* do *ALL* light here!!!! */
   in     = UNIT_IN(unit);
   extin  = in != nullptr ? UNIT_IN(in) : nullptr;
   bright = UNIT_BRIGHT(unit);
   selfb  = bright - UNIT_ILLUM(unit);

   UNIT_LIGHTS(to) += bright;
   if(UNIT_IS_TRANSPARENT(to))
   {
      UNIT_BRIGHT(to) += selfb;
      UNIT_ILLUM(to) += selfb;
   }
   else if(in != nullptr)
   {
      UNIT_LIGHTS(in) -= bright;
   }

   if((in != nullptr) && UNIT_IS_TRANSPARENT(in))
   {
      UNIT_BRIGHT(in) -= selfb;
      UNIT_ILLUM(in) -= selfb;
      if(extin != nullptr)
      {
         UNIT_LIGHTS(extin) -= selfb;
      }
   }

   if(UNIT_IN(unit) != nullptr)
   {
      if(IS_CHAR(unit))
      {
         --UNIT_CHARS(UNIT_IN(unit));
      }
      if(unit == UNIT_CONTAINS(UNIT_IN(unit)))
      {
         UNIT_CONTAINS(UNIT_IN(unit)) = unit->next;
      }
      else
      {
         for(u = UNIT_CONTAINS(UNIT_IN(unit)); u->next != unit; u = u->next)
         {
            ;
         }
         u->next = unit->next;
      }
   }

   UNIT_IN(unit)     = to;
   unit->next        = UNIT_CONTAINS(to);
   UNIT_CONTAINS(to) = unit;

   if(IS_CHAR(unit))
   {
      ++UNIT_CHARS(UNIT_IN(unit));
   }
   UNIT_WEIGHT(to) += UNIT_WEIGHT(unit);

   if((static_cast<unsigned int>(pile) != 0U) && IS_MONEY(unit))
   {
      pile_money(unit);
   }
}

void unit_down(unit_data *unit, unit_data *to)
{
   intern_unit_down(unit, to, TRUE);
}

void intern_unit_to_unit(unit_data *unit, unit_data *to, bool pile)
{
   assert(to);

   if(UNIT_IN(to) != nullptr)
   {
      intern_unit_to_unit(unit, UNIT_IN(to), FALSE);
   }

   intern_unit_down(unit, to, FALSE);

   if((static_cast<unsigned int>(pile) != 0U) && IS_MONEY(unit))
   {
      pile_money(unit);
   }
}

void unit_to_unit(unit_data *unit, unit_data *to)
{
   intern_unit_to_unit(unit, to, TRUE);
}

void snoop(unit_data *ch, unit_data *victim)
{
   assert(!is_destructed(DR_UNIT, victim));
   assert(!is_destructed(DR_UNIT, ch));

   assert(ch != victim);
   /*   assert(IS_PC(ch) && IS_PC(victim)); */
   assert(CHAR_DESCRIPTOR(ch) && CHAR_DESCRIPTOR(victim));
   assert(!CHAR_IS_SNOOPING(ch) && !CHAR_IS_SNOOPED(victim));
   assert(CHAR_LEVEL(CHAR_ORIGINAL(victim)) < CHAR_LEVEL(CHAR_ORIGINAL(ch)));
   /*   assert(!CHAR_IS_SWITCHED(victim)); */

   CHAR_DESCRIPTOR(ch)->snoop.snooping     = victim;
   CHAR_DESCRIPTOR(victim)->snoop.snoop_by = ch;
}

/* Mode 0: Stop ch from snooping a person       */
/* Mode 1: Mode 0 + stop any person snooping ch */
void unsnoop(unit_data *ch, int mode)
{
   assert(CHAR_DESCRIPTOR(ch));
   assert(CHAR_IS_SNOOPING(ch) || CHAR_IS_SNOOPED(ch));

   if(CHAR_IS_SNOOPING(ch))
   {
      act("You no longer snoop $3n.", A_SOMEONE, ch, nullptr, CHAR_DESCRIPTOR(ch)->snoop.snooping, TO_CHAR);
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snooping)->snoop.snoop_by = nullptr;
      CHAR_DESCRIPTOR(ch)->snoop.snooping                                  = nullptr;
   }

   if(CHAR_IS_SNOOPED(ch) && (mode != 0))
   {
      act("You no longer snoop $3n, $3e was extracted.", A_SOMEONE, CHAR_DESCRIPTOR(ch)->snoop.snoop_by, nullptr, ch, TO_CHAR);
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snoop_by)->snoop.snooping = nullptr;
      CHAR_DESCRIPTOR(ch)->snoop.snoop_by                                  = nullptr;
   }
}

void switchbody(unit_data *ch, unit_data *vict)
{
   assert(CHAR_DESCRIPTOR(ch) && IS_NPC(vict));
   assert(!CHAR_DESCRIPTOR(vict));
   assert(!CHAR_IS_SNOOPING(ch) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPING(ch)));
   assert(!CHAR_IS_SNOOPED(ch) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPED(ch)));
   assert(!is_destructed(DR_UNIT, vict));
   assert(!is_destructed(DR_UNIT, ch));

   CHAR_DESCRIPTOR(ch)->character = vict;

   if(IS_PC(ch))
   {
      CHAR_DESCRIPTOR(ch)->original = ch;
   }
   if(CHAR_IS_SNOOPING(ch))
   {
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snooping)->snoop.snoop_by = vict;
   }
   if(CHAR_IS_SNOOPED(ch))
   {
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snoop_by)->snoop.snooping = vict;
   }

   CHAR_DESCRIPTOR(vict) = CHAR_DESCRIPTOR(ch);
   CHAR_DESCRIPTOR(ch)   = nullptr;
   CHAR_LAST_ROOM(vict)  = nullptr;
}

void unswitchbody(unit_data *npc)
{
   assert(IS_NPC(npc) && CHAR_DESCRIPTOR(npc));
   assert(CHAR_IS_SWITCHED(npc));
   assert(!CHAR_IS_SNOOPING(npc) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPING(npc)));
   assert(!CHAR_IS_SNOOPED(npc) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPED(npc)));

   send_to_char("You return to your original body.\n\r", npc);

   if(CHAR_IS_SNOOPING(npc))
   {
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->snoop.snooping)->snoop.snoop_by = CHAR_ORIGINAL(npc);
   }

   if(CHAR_IS_SNOOPED(npc))
   {
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->snoop.snoop_by)->snoop.snooping = CHAR_ORIGINAL(npc);
   }

   CHAR_DESCRIPTOR(npc)->character = CHAR_ORIGINAL(npc);
   CHAR_DESCRIPTOR(npc)->original  = nullptr;

   CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->character) = CHAR_DESCRIPTOR(npc);
   CHAR_DESCRIPTOR(npc)                             = nullptr;
}

/* Used when a unit is to be extracted from the game */
/* Extracts recursively                              */
void extract_unit(unit_data *unit)
{
   descriptor_data *d;

   extern unit_data *destroy_room;

   void nanny_menu(descriptor_data * d, char *arg);
   void stop_all_special(unit_data * u);

   /* Prevent recursive calling on extracted units. */
   /* This happens on for example corpses. When the */
   /* destruct_affect is called inside extract we   */
   /* got a recursive call.                         */

   if(is_destructed(DR_UNIT, unit) != 0)
   {
      return;
   }

   /* We can't extract rooms! Sanity, MS 300595, wierd bug... */
   assert(!IS_ROOM(unit));

   if(IS_PC(unit) && (UNIT_IN(unit) != nullptr) && (!PC_IS_UNSAVED(unit)))
   {
      save_player(unit);
      save_player_contents(unit, static_cast<int>(TRUE));
   }

   DeactivateDil(unit);

   register_destruct(DR_UNIT, unit);

   if(UNIT_IS_EQUIPPED(unit))
   {
      unequip_object(unit);
   }

   stop_all_special(unit);
   stop_affect(unit);

   while(UNIT_CONTAINS(unit) != nullptr)
   {
      extract_unit(UNIT_CONTAINS(unit));
   }

   if(!IS_PC(unit) || (UNIT_IN(unit) != nullptr))
   {
      while((d = unit_is_edited(unit)) != nullptr)
      {
         send_to_char("\n\rUnit was extracted, "
                      "sorry.\n\r",
                      d->character);

         set_descriptor_fptr(d, descriptor_interpreter, FALSE);
      }

      if(IS_CHAR(unit))
      {
         if(CHAR_IS_SWITCHED(unit))
         {
            unswitchbody(unit);
         }

         /* If the PC which is switched is extracted, then unswitch */
         if(IS_PC(unit) && (CHAR_DESCRIPTOR(unit) == nullptr))
         {
            for(d = descriptor_list; d != nullptr; d = d->next)
            {
               if(d->original == unit)
               {
                  unswitchbody(d->character);
                  break;
               }
            }
         }

         if(CHAR_DESCRIPTOR(unit) != nullptr)
         {
            void disconnect_game(unit_data * pc);

            disconnect_game(unit);
         }

         if(CHAR_FOLLOWERS(unit) || CHAR_MASTER(unit))
         {
            die_follower(unit);
         }

         stop_fighting(unit);

         if(CHAR_IS_SNOOPING(unit) || CHAR_IS_SNOOPED(unit))
         {
            unsnoop(unit, 1); /* Remove all snoopers */
         }
      }

      if(UNIT_IN(unit) != nullptr)
      {
         unit_from_unit(unit);
      }

      unit_to_unit(unit, destroy_room);

      /* Otherwise find_unit will find it AFTER it has been extracted!! */
      remove_from_unit_list(unit);
   }
}

/* ***********************************************************************
   Here follows high-level versions of some earlier routines, ie functions
   which incorporate the actual player-data.
   *********************************************************************** */

/* Add weight to the unit and change everything it is in */
/* (It will not change the -basic- weight of a player)   */
void weight_change_unit(unit_data *unit, int weight)
{
   for(; unit != nullptr; unit = UNIT_IN(unit))
   {
      UNIT_WEIGHT(unit) += weight;
   }
}

auto quest_add(unit_data *ch, const char *name, char *descr) -> extra_descr_data *
{
   const char *namelist[2];

   assert(name != nullptr);
   assert(name[0]);

   namelist[0] = name;
   namelist[1] = nullptr;

   return (PC_QUEST(ch) = PC_QUEST(ch)->add(namelist, descr));
}

/* void szonelog(char *zonename, const char *fmt, ...) */
void szonelog(zone_type *zone, const char *fmt, ...)
{
   char    name[256];
   char    buf[MAX_STRING_LENGTH];
   char    buf2[MAX_STRING_LENGTH];
   va_list args;
   FILE   *f;

   time_t now   = time(nullptr);
   char  *tmstr = ctime(&now);

   tmstr[strlen(tmstr) - 1] = '\0';

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   if(zone == nullptr)
   {
      slog(LOG_ALL, 0, buf);
      return;
   }

   sprintf(buf2, "%s/%s", zone->name, buf);
   slog(LOG_ALL, 0, buf2);

   sprintf(name, "%s%s.err", zondir, zone->filename);

   if((f = fopen_cache(name, "a")) == nullptr)
   {
      slog(LOG_ALL, 0, "Unable to append to zonelog '%s'", name);
   }
   else
   {
      fprintf(f, "%s :: %s\n", tmstr, buf);
   }
}
