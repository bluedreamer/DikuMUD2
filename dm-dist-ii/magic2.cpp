#include "affect.h"
#include "comm.h"
#include "db.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "movement.h"
#include "skills.h"
#include "spell_args.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"
#include "zone_info_type.h"

#include <algorithm>
#include <climits>

#define RIFT_RISK 100

/* Extern structures */

/* When you attempt lesser summoning, greater summoning, random teleport,
   or controlled teleport - there is a risk of pretty bad failure.
   These are all the nasty failure things that can be done.

   caster is person to do nasty stuff to.
   target is optional intended target.
   spl is the originating spell. */

auto random_room() -> unit_data *
{
   unit_data        *room = nullptr;
   struct zone_type *zone;
   file_index_type  *fi;
   int               no;

   for(;;)
   {
      no = number(1, zone_info.no_of_zones) - 1;
      for(zone = zone_info.zone_list; (zone != nullptr) && no > 0; zone = zone->next, no--)
      {
         ;
      }

      no = number(1, zone->no_of_fi) - 1;
      for(fi = zone->fi; (fi != nullptr) && (no > 0 || (fi->room_ptr == nullptr)); fi = fi->next, no--)
      {
         ;
      }

      if(!((fi != nullptr) && ((room = fi->room_ptr) != nullptr)))
      {
         return nullptr; /* Give up */
      }

      if(static_cast<unsigned int>(may_teleport_away(room)) != 0U)
      { /* Can we go to that room? */
         return room;
      }
   }

   return nullptr;
}

auto random_npc() -> unit_data *
{
   unit_data *u;
   int        i;

   extern int world_nochars;

   i = number(1, world_nochars);

   for(u = unit_list; u != nullptr; u = u->gnext, i--)
   {
      if(IS_NPC(u) && i <= 0 && (static_cast<unsigned int>(may_teleport_away(u)) != 0U) && (is_destructed(DR_UNIT, u) == 0))
      {
         return u;
      }
   }

   return nullptr;
}

auto random_pc() -> unit_data *
{
   return nullptr;
}

auto random_char() -> unit_data *
{
   return nullptr;
}

void summon_attack_npc(unit_data *caster, int n)
{
   unit_data *u;

   for(; n > 0; n--)
   {
      if(((u = random_npc()) != nullptr) && (u != caster) && (static_cast<unsigned int>(may_teleport_away(u)) != 0U))
      {
         if(CHAR_FIGHTING(u))
         {
            stop_fighting(u);
         }
         act("$1n disappears into a rift!", A_ALWAYS, u, nullptr, nullptr, TO_ROOM);
         create_fptr(u, SFUN_RETURN_TO_ORIGIN, PULSE_SEC * 10, SFB_TICK, unit_room(u));
         unit_from_unit(u);
         unit_to_unit(u, UNIT_IN(caster));
         act("$1n appears though a rift!", A_ALWAYS, u, nullptr, nullptr, TO_ROOM);
         provoked_attack(u, caster);
      }
   }
}

void rift_failure(unit_data *caster, unit_data *target)
{
   spell_args        sa;

   int               i = number(1, 100);

   extern unit_data *void_room;

   act("You cause a great disturbance in the powers of magic.", A_ALWAYS, caster, nullptr, nullptr, TO_CHAR);
   act("$1n causes a great disturbance in the powers of magic.", A_ALWAYS, caster, nullptr, nullptr, TO_ROOM);

   if(IS_IMMORTAL(caster))
   {
      act("In the last second you successfully close the rift!", A_ALWAYS, caster, nullptr, nullptr, TO_CHAR);
      act("$1n battles the rift for a split second, and closes it again!", A_ALWAYS, caster, nullptr, nullptr, TO_ROOM);
      return;
   }

   CHAR_MANA(caster) = 0;

   if(i <= 35)
   {
      if((target == nullptr) || (number(0, 1) != 0))
      {
         char mbuf[MAX_INPUT_LENGTH] = {0};
         set_spellargs(&sa, caster, caster, caster, mbuf, 100);
         spell_random_teleport(&sa);
      }
      else
      {
         char mbuf[MAX_INPUT_LENGTH] = {0};
         set_spellargs(&sa, caster, caster, target, mbuf, 100);
         spell_control_teleport(&sa);
      }
   }
   else if(i <= 60)
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      set_spellargs(&sa, caster, caster, void_room, mbuf, 100);
      spell_control_teleport(&sa);
   }
   else if(i <= 80)
   {
      summon_attack_npc(caster, 2);
   }
   else if(i <= 85)
   {
      summon_attack_npc(caster, 3);
   }
   else if(i <= 90)
   {
      summon_attack_npc(caster, 4);
   }
   else if(i <= 95)
   {
      summon_attack_npc(caster, 5);
   }
   else
   {
      summon_attack_npc(caster, 6);
   }
}

void spell_clear_skies(spell_args *sa)
{
   unit_data *room = unit_room(sa->caster);

   if((sa->hm / 20 <= 0) || ((IS_SET(UNIT_FLAGS(room), UNIT_FL_NO_WEATHER | UNIT_FL_INDOORS)) != 0))
   {
      act("Nothing happens.", A_ALWAYS, sa->caster, nullptr, nullptr, TO_CHAR);
      return;
   }

   unit_zone(sa->caster)->weather.change += sa->hm / 20;
   unit_zone(sa->caster)->weather.change = std::min(unit_zone(sa->caster)->weather.change, 12);

   act("You feel a warm breeze.", A_ALWAYS, sa->caster, nullptr, nullptr, TO_ALL);
}

void spell_storm_call(spell_args *sa)
{
   unit_data *room = unit_room(sa->caster);

   if((sa->hm / 20 <= 0) || ((IS_SET(UNIT_FLAGS(room), UNIT_FL_NO_WEATHER | UNIT_FL_INDOORS)) != 0))
   {
      act("Nothing happens.", A_ALWAYS, sa->caster, nullptr, nullptr, TO_CHAR);
      return;
   }

   unit_zone(sa->caster)->weather.change -= sa->hm / 20;
   unit_zone(sa->caster)->weather.change = std::max(unit_zone(sa->caster)->weather.change, -12);

   act("A cold wind chills you to the bone.", A_ALWAYS, sa->caster, nullptr, nullptr, TO_ALL);
}

void spell_random_teleport(spell_args *sa)
{
   unit_data *room;

   if((room = random_room()) == nullptr)
   {
      return;
   }

   sa->hm = spell_resistance(sa->medium, unit_room(sa->caster), SPL_RANDOM_TELEPORT);

   if(sa->hm < 0)
   {
      return;
   }

   if(number(1, RIFT_RISK) <= 5)
   {
      rift_failure(sa->caster, sa->target);
      return;
   }

   if(static_cast<unsigned int>(may_teleport(sa->caster, room)) == 0U)
   {
      send_to_char("It seems that you can't teleport there.\n\r", sa->caster);
      return;
   }

#ifdef SUSPEKT
   /* If this code is in effect, then you cannot tele between
      non connected zones. */

   if(move_to(unit_room(sa->target), room) == DIR_IMPOSSIBLE)
   {
      /* No non-intercontinental teleports */
      act("The magic disperses.", A_ALWAYS, sa->target, 0, 0, TO_CHAR);
      return;
   }
#endif

   if(IS_CHAR(sa->medium))
   {
      act("$1n disappears in a puff of green smoke.", A_SOMEONE, sa->target, nullptr, nullptr, TO_ROOM);
   }

   unit_from_unit(sa->target);
   unit_to_unit(sa->target, room);

   if(IS_CHAR(sa->medium))
   {
      act("$1n appears in a puff of green smoke.", A_SOMEONE, sa->target, nullptr, nullptr, TO_ROOM);
      act("You feel confused.", A_SOMEONE, sa->target, nullptr, nullptr, TO_CHAR);
   }

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->target, mbuf, &cmd_auto_unknown);
}

void spell_transport(spell_args *sa)
{
   /* This spell is alot harder to get off successfull */
   unit_data *room;

   room = unit_room(sa->caster);
   assert(room);

   sa->hm = spell_resistance(sa->medium, room, SPL_TRANSPORT);
   if(sa->hm < 0)
   {
      send_to_char("You fail.\n\r", sa->caster);
      return;
   }

   /* find destination */
   room = unit_room(sa->target);
   assert(room);

   if(static_cast<unsigned int>(may_teleport(sa->caster, room)) == 0U)
   {
      send_to_char("It seems that you can't teleport there.\n\r", sa->caster);
      return;
   }

   sa->hm = spell_resistance(sa->medium, sa->target, SPL_TRANSPORT);
   if(sa->hm < 0)
   {
      send_to_char("You can't seem to clearly locate the target.\n\r", sa->caster);
      return;
   }

   if(number(1, RIFT_RISK) <= 5)
   {
      rift_failure(sa->caster, sa->target);
      return;
   }

   /* If this code is in effect, then you cannot tele between
      non connected zones. */

   if(move_to(unit_room(sa->caster), room) == DIR_IMPOSSIBLE)
   {
      /* No non-intercontinental teleports */
      act("The magic disperses.", A_ALWAYS, sa->caster, nullptr, nullptr, TO_CHAR);
      return;
   }

   if(IS_CHAR(sa->medium))
   {
      act("$1n disappears in a puff of green smoke.", A_SOMEONE, sa->caster, nullptr, nullptr, TO_ROOM);
   }

   unit_from_unit(sa->caster);
   unit_to_unit(sa->caster, room);

   if(IS_CHAR(sa->medium))
   {
      act("$1n appears in a puff of green smoke.", A_SOMEONE, sa->caster, nullptr, nullptr, TO_ROOM);
   }

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->caster, mbuf, &cmd_auto_unknown);
}

void spell_control_teleport(spell_args *sa)
{
   /* This spell is alot harder to get off successfull */
   unit_data *room;

   room = unit_room(sa->caster);
   assert(room);

   sa->hm = spell_resistance(sa->medium, room, SPL_CONTROL_TELEPORT);
   if(sa->hm < 0)
   {
      send_to_char("You fail.\n\r", sa->caster);
      return;
   }

   /* find destination */
   room = unit_room(sa->target);
   assert(room);

   if(static_cast<unsigned int>(may_teleport(sa->caster, room)) == 0U)
   {
      send_to_char("It seems that you can't teleport there.\n\r", sa->caster);
      return;
   }

   sa->hm = spell_resistance(sa->medium, sa->target, SPL_CONTROL_TELEPORT);
   if(sa->hm < 0)
   {
      send_to_char("You can't seem to clearly locate the target.\n\r", sa->caster);
      return;
   }

   if(number(1, RIFT_RISK) <= 5)
   {
      rift_failure(sa->caster, sa->target);
      return;
   }

   if(IS_CHAR(sa->medium))
   {
      act("$1n disappears in a puff of green smoke.", A_SOMEONE, sa->caster, nullptr, nullptr, TO_ROOM);
   }

   unit_from_unit(sa->caster);
   unit_to_unit(sa->caster, room);

   if(IS_CHAR(sa->medium))
   {
      act("$1n appears in a puff of green smoke.", A_SOMEONE, sa->caster, nullptr, nullptr, TO_ROOM);
   }

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->caster, mbuf, &cmd_auto_unknown);
}

void spell_undead_door(spell_args *sa)
{
   /* This spell is alot harder to get off successfull */
   unit_data *roomf;
   unit_data *roomt;

   roomf  = unit_room(sa->caster);
   roomt  = unit_room(sa->target);

   sa->hm = spell_resistance(sa->medium, roomf, SPL_UNDEAD_DOOR);

   if((sa->hm < 0) || !CHAR_IS_UNDEAD(sa->target))
   {
      send_to_char("You fail.\n\r", sa->caster);
      return;
   }

   /* find destination */

   if((static_cast<unsigned int>(may_teleport(sa->caster, roomt)) == 0U) ||
      (static_cast<unsigned int>(may_teleport(sa->target, roomf)) == 0U))
   {
      send_to_char("It seems that you can't switch with this undead.\n\r", sa->caster);
      return;
   }

   sa->hm = spell_resistance(sa->medium, sa->target, SPL_UNDEAD_DOOR);
   if(sa->hm < 0)
   {
      send_to_char("You can't seem to clearly locate the target.\n\r", sa->caster);
      return;
   }

   if(number(1, RIFT_RISK) <= 5)
   {
      rift_failure(sa->caster, sa->target);
      return;
   }

   if(IS_CHAR(sa->medium))
   {
      act("$1n is replaced with $3n!", A_SOMEONE, sa->caster, nullptr, sa->target, TO_ROOM);

      act("$1n is replaced with $3n!", A_SOMEONE, sa->target, nullptr, sa->caster, TO_ROOM);
   }

   unit_from_unit(sa->caster);
   unit_to_unit(sa->caster, roomt);

   unit_from_unit(sa->target);
   unit_to_unit(sa->target, roomf);

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->caster, mbuf, &cmd_auto_unknown);
   do_look(sa->target, mbuf, &cmd_auto_unknown);
}

void spell_summon_char_1(spell_args *sa)
{
   unit_data *room;

   if(CHAR_LEVEL(sa->target) > CHAR_LEVEL(sa->caster))
   {
      return;
   }

   room = unit_room(sa->caster);

   if(static_cast<unsigned int>(same_surroundings(room, sa->caster)) == 0U)
   {
      send_to_char("The magic fizzles as there is nowhere for the creature to "
                   "arrive.\n\r",
                   sa->caster);
      return;
   }

   if(static_cast<unsigned int>(may_teleport(sa->target, room)) == 0U)
   {
      send_to_char("Powers beyond your control prevent the summoning.\n\r", sa->caster);
      return;
   }

   if(sa->hm < 0)
   {
      act("$2n tried to summon you.", A_ALWAYS, sa->target, sa->caster, nullptr, TO_CHAR);
      return;
   }

   /* 30% fail if fighting */
   if(CHAR_FIGHTING(sa->target) && number(0, 100) < 30)
   {
      send_to_char("There is a disturbance in the powers, you fail.\n\r", sa->caster);
      send_to_char("The air is loaded with energy for a short moment.\n\r", sa->target);
      return;
   }

   if(number(1, RIFT_RISK) <= 5)
   {
      rift_failure(sa->caster, sa->target);
      return;
   }

   if(IS_PC(sa->caster) && IS_PC(sa->target))
   {
      slog(LOG_ALL,
           0,
           "%s was summoned by %s to %s@%s.",
           UNIT_NAME(sa->target),
           UNIT_NAME(sa->caster),
           UNIT_FI_NAME(room),
           UNIT_FI_ZONENAME(room));
   }

   act("You are summoned!", A_SOMEONE, sa->target, nullptr, nullptr, TO_CHAR);
   act("$1n disappears in a puff of smoke!", A_SOMEONE, sa->target, nullptr, nullptr, TO_ROOM);

   unit_from_unit(sa->target);
   unit_to_unit(sa->target, room);

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->target, mbuf, &cmd_auto_unknown);
   act("$1n appears in a puff of smoke!", A_SOMEONE, sa->target, nullptr, nullptr, TO_ROOM);

   if(CHAR_FIGHTING(sa->target))
   {
      stop_fighting(sa->target);
   }

   REMOVE_BIT(CHAR_FLAGS(sa->target), CHAR_LEGAL_TARGET);

   provoked_attack(sa->target, sa->caster);

   if(CHAR_COMBAT(sa->caster) != nullptr)
   {
      CHAR_COMBAT(sa->caster)->changeSpeed(2 * 12);
   }
}

void spell_summon_char_2(spell_args *sa)
{
   unit_data *room;

   if(sa->hm < 0)
   {
      act("$2n tried to summon you.", A_ALWAYS, sa->target, sa->caster, nullptr, TO_CHAR);
      return;
   }

   room = unit_room(sa->caster);

   if(static_cast<unsigned int>(same_surroundings(room, sa->caster)) == 0U)
   {
      send_to_char("The magic fizzles as there is nowhere for the creature to "
                   "arrive.\n\r",
                   sa->caster);
      return;
   }

   if(static_cast<unsigned int>(may_teleport(sa->target, room)) == 0U)
   {
      send_to_char("Powers beyond your control prevent the summoning.\n\r", sa->caster);
      return;
   }

   /* 30% fail if fighting */
   if(CHAR_FIGHTING(sa->target) && number(0, 100) < 30)
   {
      send_to_char("There is a disturbance in the powers, you fail.\n\r", sa->caster);
      send_to_char("The air is loaded with energy for a short moment.\n\r", sa->target);
      return;
   }

   if(number(1, RIFT_RISK - std::max(0, CHAR_LEVEL(sa->target) - CHAR_LEVEL(sa->caster))) <= 5)
   {
      rift_failure(sa->caster, sa->target);
      return;
   }

   if(IS_PC(sa->caster) && IS_PC(sa->target))
   {
      slog(LOG_ALL,
           0,
           "%s was summoned by %s to %s@%s.",
           UNIT_NAME(sa->target),
           UNIT_NAME(sa->caster),
           UNIT_FI_NAME(room),
           UNIT_FI_ZONENAME(room));
   }

   act("You are summoned!", A_SOMEONE, sa->target, nullptr, nullptr, TO_CHAR);
   act("$1n disappears in a flash of light!", A_HIDEINV, sa->target, nullptr, nullptr, TO_ROOM);

   unit_from_unit(sa->target);
   unit_to_unit(sa->target, room);

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->target, mbuf, &cmd_auto_unknown);
   act("$1n appears in a flash of light!", A_HIDEINV, sa->target, nullptr, nullptr, TO_ROOM);

   if(CHAR_FIGHTING(sa->target))
   {
      stop_fighting(sa->target);
   }

   REMOVE_BIT(CHAR_FLAGS(sa->target), CHAR_LEGAL_TARGET);

   provoked_attack(sa->target, sa->caster);

   if(CHAR_COMBAT(sa->caster) != nullptr)
   {
      CHAR_COMBAT(sa->caster)->changeSpeed(2 * 12);
   }
}

void spell_animate_dead(spell_args *sa)
{
   extern file_index_type *zombie_fi;
   unit_data              *u;
   unit_data              *zombie;
   char                    buf[1024];

   if(OBJ_TYPE(sa->target) != ITEM_CONTAINER || (affected_by_spell(sa->target, ID_CORPSE) == nullptr))
   {
      return;
   }

   if(!RACE_IS_HUMANOID(OBJ_VALUE(sa->target, 4)))
   {
      return;
   }

   zombie = read_unit(zombie_fi);
   unit_to_unit(zombie, UNIT_IN(sa->target));

   act("$1n animates $2n into a zombie.", A_ALWAYS, sa->caster, sa->target, nullptr, TO_ROOM);
   act("You animate $2n into a zombie.", A_ALWAYS, sa->caster, sa->target, nullptr, TO_CHAR);

   strcpy(buf, UNIT_NAME(sa->caster));
   do_follow(zombie, buf, &cmd_auto_unknown);

   UNIT_ALIGNMENT(sa->caster) -= 50;

   while((u = UNIT_CONTAINS(sa->target)) != nullptr)
   {
      unit_from_unit(u);
      unit_to_unit(u, zombie);
   }
   extract_unit(sa->target);
}

void spell_control_undead(spell_args *sa)
{
   char buf[1024];
   if(!CHAR_IS_UNDEAD(sa->target))
   {
      return;
   }

   UNIT_ALIGNMENT(sa->caster) -= 50;

   if(sa->hm >= 30)
   {
      act("$3n is controlled by $1n.", A_ALWAYS, sa->caster, nullptr, sa->target, TO_ROOM);
      strcpy(buf, UNIT_NAME(sa->caster));
      do_follow(sa->target, buf, &cmd_auto_unknown);
      create_fptr(sa->target, SFUN_OBEY, WAIT_SEC * 60 * 5, SFB_CMD | SFB_PRIORITY | SFB_TICK, nullptr);
   }
   else
   {
      provoked_attack(sa->target, sa->caster);
   }
}

void spell_colourspray_1(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_COLOURSPRAY_1);
}

void spell_colourspray_2(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_COLOURSPRAY_2);
}

void spell_colourspray_3(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_COLOURSPRAY_3);
}

void spell_invisibility(spell_args *sa)
{
   unit_affected_type af;

   if(IS_PC(sa->target))
   { /* Assume willing target */
      sa->hm = spell_cast_check(sa->medium, SPL_INVISIBILITY);
   }
   else
   {
      sa->hm = spell_resistance(sa->medium, sa->target, SPL_INVISIBILITY);
   }

   if(sa->hm < 0)
   {
      return;
   }

   act("$1n fades away.", A_HIDEINV, sa->target, nullptr, nullptr, TO_ROOM);
   act("You become invisible.", A_HIDEINV, sa->target, nullptr, nullptr, TO_CHAR);

   af.id       = ID_INVISIBILITY;
   af.duration = skill_duration(sa->hm);
   af.beat     = WAIT_SEC * 30;
   af.data[0]  = UNIT_FL_INVISIBLE;
   af.data[1] = af.data[2] = 0;

   af.firstf_i             = TIF_INVISIBILITY_ON;
   af.tickf_i              = TIF_NONE;
   af.lastf_i              = TIF_INVISIBILITY_OFF;

   af.applyf_i             = APF_MOD_UNIT_FLAGS;

   create_affect(sa->target, &af);
}

void spell_wizard_eye(spell_args *sa)
{
   unit_data *pOrgUnit;
   unit_data *pTo;

   if(UNIT_IN(sa->target) == nullptr)
   {
      pTo = sa->target;
   }
   else
   {
      pTo = UNIT_IN(sa->target);
   }

   if(sa->hm < 0 || (unit_recursive(sa->caster, pTo) != 0))
   {
      return;
   }

   pOrgUnit = UNIT_IN(sa->caster);
   unit_from_unit(sa->caster);
   unit_to_unit(sa->caster, pTo);
   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(sa->caster, mbuf, &cmd_auto_unknown);
   unit_from_unit(sa->caster);
   unit_to_unit(sa->caster, pOrgUnit);
}

void spell_fear(spell_args *sa)
{
   unit_affected_type af;

   if(!CHAR_IS_MAMMAL(sa->target) && !CHAR_IS_HUMANOID(sa->target))
   {
      return;
   }

   provoked_attack(sa->target, sa->caster);

   if(sa->hm >= 0)
   {
      af.id       = ID_FEAR;
      af.duration = skill_duration(sa->hm);
      af.beat     = WAIT_SEC * 30;
      af.data[0] = af.data[1] = af.data[2] = 0;

      af.firstf_i                          = TIF_FEAR_CHECK;
      af.tickf_i                           = TIF_FEAR_CHECK;
      af.lastf_i                           = TIF_NONE;
      af.applyf_i                          = APF_NONE;

      create_affect(sa->target, &af);
   }
}

void spell_confusion(spell_args *sa)
{
   unit_affected_type af;

   if(sa->hm < 0)
   {
      provoked_attack(sa->target, sa->caster);
      return;
   }

   af.id       = ID_CONFUSION;
   af.duration = skill_duration(sa->hm);
   af.beat     = WAIT_SEC * 15;
   af.data[0] = af.data[1] = af.data[2] = 0;

   af.firstf_i                          = TIF_CONFUSION_ON;
   af.tickf_i                           = TIF_CONFUSION_TICK;
   af.lastf_i                           = TIF_CONFUSION_OFF;
   af.applyf_i                          = APF_NONE;

   create_affect(sa->target, &af);
}

void spell_xray_vision(spell_args *sa)
{
   unit_data *u;

   if(sa->hm < 0)
   {
      return;
   }

   if(UNIT_CONTAINS(sa->target) == nullptr)
   {
      send_to_char("It is empty.\n\r", sa->caster);
      return;
   }

   act("$2n contains:", A_SOMEONE, sa->caster, sa->target, nullptr, TO_CHAR);
   for(u = UNIT_CONTAINS(sa->target); u != nullptr; u = u->next)
   {
      act("$2n", A_HIDEINV, sa->caster, u, nullptr, TO_CHAR);
   }
}

void spell_command(spell_args *sa)
{
   char buf[MAX_INPUT_LENGTH];

   str_next_word(sa->arg, buf);
   if(*buf != 0)
   {
      act("$1n commands $3n to $2t.", A_SOMEONE, sa->caster, buf, sa->target, TO_NOTVICT);
      act("$1n commands you to $2t.", A_SOMEONE, sa->caster, buf, sa->target, TO_VICT);
      act("You command $3n to $2t.", A_SOMEONE, sa->caster, buf, sa->target, TO_CHAR);
   }
   else
   {
      act("$1n commands $3n to do nothing...", A_SOMEONE, sa->caster, buf, sa->target, TO_NOTVICT);
      act("$1n commands you to do nothing?", A_SOMEONE, sa->caster, buf, sa->target, TO_VICT);
      act("You command $3n to do nothing?", A_SOMEONE, sa->caster, buf, sa->target, TO_CHAR);
      return;
   }

   if(sa->hm >= 0 && IS_MORTAL(sa->target))
   {
      command_interpreter(sa->target, buf);
   }
   else
   {
      provoked_attack(sa->target, sa->caster);
   }
}

void spell_fireball_1(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_FIREBALL_1);
}

void spell_fireball_2(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_FIREBALL_2);
}

void spell_fireball_3(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_FIREBALL_3);
}

void spell_frostball_1(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_FROSTBALL_1);
}

void spell_frostball_2(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_FROSTBALL_2);
}

void spell_frostball_3(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_FROSTBALL_3);
}

void spell_lightning_1(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_LIGHTNING_1);
}

void spell_lightning_2(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_LIGHTNING_2);
}

void spell_lightning_3(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_LIGHTNING_3);
}

void spell_stinking_cloud_1(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_STINKING_CLOUD_1);
}

void spell_stinking_cloud_2(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_STINKING_CLOUD_2);
}

void spell_stinking_cloud_3(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_STINKING_CLOUD_3);
}

void spell_acidball_1(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_ACIDBALL_1);
}

void spell_acidball_2(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_ACIDBALL_2);
}

void spell_acidball_3(spell_args *sa)
{
   sa->hm = spell_offensive(sa, SPL_ACIDBALL_3);
}

void spell_mana_boost(spell_args *sa)
{
   if(sa->hm > 0)
   {
      CHAR_MANA(sa->target) = std::min(mana_limit(sa->target), CHAR_MANA(sa->target) + CHAR_MANA(sa->target) * skill_duration(sa->hm) / 10);
      act("You are filled with essence.", A_ALWAYS, sa->target, nullptr, nullptr, TO_CHAR);
      act("$1n seems to be filled with energy.", A_HIDEINV, sa->target, nullptr, nullptr, TO_ROOM);
   }
}
