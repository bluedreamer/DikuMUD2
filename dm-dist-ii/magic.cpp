#include "magic.h"

#include "common.h"
#include "damage_chart_type.h"
#include "dbfind.h"
#include "dil.h"
#include "dilprg.h"
#include "dilrun.h"
#include "dilvar.h"
#include "experience.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "skills.h"
#include "spell_args.h"
#include "spell_info_type.h"
#include "textutil.h"
#include "tree_type.h"
#include "utility.h"
#include "utils.h"

#include <algorithm>

/* Extern structures */
extern unit_data *unit_list;

/* Returns TRUE when effect is shown by DIL */

auto dil_effect(const char *pStr, spell_args *sa) -> int
{
   if(static_cast<unsigned int>(str_is_empty(pStr)) != 0U)
   {
      return static_cast<int>(FALSE);
   }

   diltemplate *tmpl;

   tmpl = find_dil_template(pStr);

   if(tmpl == nullptr)
   {
      return static_cast<int>(FALSE);
   }

   if(tmpl->argc != 3)
   {
      slog(LOG_ALL, 0, "Spell DIL effect wrong arg count.");
      return static_cast<int>(FALSE);
   }

   if(tmpl->argt[0] != DILV_UP)
   {
      slog(LOG_ALL, 0, "Spell DIL effect arg 1 mismatch.");
      return static_cast<int>(FALSE);
   }

   if(tmpl->argt[1] != DILV_UP)
   {
      slog(LOG_ALL, 0, "Spell DIL effect arg 2 mismatch.");
      return static_cast<int>(FALSE);
   }

   if(tmpl->argt[2] != DILV_INT)
   {
      slog(LOG_ALL, 0, "Spell DIL effect arg 3 mismatch.");
      return static_cast<int>(FALSE);
   }

   dilprg    *prg;
   unit_fptr *fptr;

   prg          = dil_copy_template(tmpl, sa->caster, &fptr);
   prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

   prg->sp->vars[0].val.unitptr = sa->medium;
   prg->sp->vars[1].val.unitptr = sa->target;
   prg->sp->vars[2].val.integer = sa->hm;

   dil_add_secure(prg, sa->medium, prg->sp->tmpl->core);
   dil_add_secure(prg, sa->target, prg->sp->tmpl->core);

   assert(fptr);

   struct spec_arg sarg;

   sarg.owner     = prg->owner;
   sarg.activator = sa->caster;
   sarg.medium    = nullptr;
   sarg.target    = nullptr;
   sarg.pInt      = nullptr;
   sarg.fptr      = fptr;
   sarg.cmd       = &cmd_auto_tick;
   sarg.arg       = nullptr;
   sarg.mflags    = SFB_TICK | SFB_AWARE;

   run_dil(&sarg);

   return static_cast<int>(TRUE);
}

/* This procedure uses mana from a medium */
/* returns TRUE if ok, and FALSE if there was not enough mana.  */
/* wands and staffs uses one charge, no matter what 'mana' is. --HHS */
auto use_mana(unit_data *medium, int mana) -> bool
{
   if(IS_CHAR(medium))
   {
      if(CHAR_MANA(medium) >= mana)
      {
         CHAR_MANA(medium) -= mana;
         return TRUE;
      }
      return FALSE;
   }
   if(IS_OBJ(medium))
   {
      switch(OBJ_TYPE(medium))
      {
         case ITEM_STAFF:
         case ITEM_WAND:
            if(OBJ_VALUE(medium, 1))
            {
               --OBJ_VALUE(medium, 1);
               return TRUE;
            }
            else
            {
               return FALSE;
            }
            break;
         default:
            return FALSE; /* no mana in other objects */
      }
   }
   return FALSE; /* no mana/charge in this type of unit */
}

/* Determines if healing combat mana should be cast?? */
auto cast_magic_now(unit_data *ch, int mana) -> bool
{
   int hleft;
   int sleft;

   if(CHAR_MANA(ch) > mana)
   {
      if(UNIT_MAX_HIT(ch) <= 0)
      {
         hleft = 0;
      }
      else
      {
         hleft = (100 * UNIT_HIT(ch)) / UNIT_MAX_HIT(ch);
      }

      sleft = mana != 0 ? CHAR_MANA(ch) / mana : 20;

      if(sleft >= 5)
      {
         return TRUE;
      }

      if(hleft >= 95)
      {
         return FALSE;
      }
      if(hleft > 80)
      { /* Small chance, allow heal to be possible */
         return (number(1, std::max(1, 16 - 2 * sleft)) == 1);
      }
      if(hleft > 50)
         return (number(1, std::max(1, 6 - sleft)) == 1);
      else if(hleft > 40)
         return (number(1, std::max(1, 4 - sleft)) == 1);
      else
         return TRUE;
   }
   return FALSE;
}

/* ===================================================================== */

/* down and up is how much percentage that num will be adjusted randomly */
/* up or down                                                            */
auto variation(int num, int d, int u) -> int
{
   return number(num - (num * d) / 100, num + (num * u) / 100);
}

/* See if unit is allowed to be transferred away from its surroundings */
/* i.e. if a player is allowed to transfer out of jail, etc.           */
auto may_teleport_away(unit_data *unit) -> bool
{
   if(IS_SET(UNIT_FLAGS(unit), UNIT_FL_NO_TELEPORT))
   {
      return FALSE;
   }

   while((unit = UNIT_IN(unit)) != nullptr)
   {
      if(IS_SET(UNIT_FLAGS(unit), UNIT_FL_NO_TELEPORT))
      {
         return FALSE;
      }
   }

   return TRUE;
}

/* See if unit is allowed to be transferred to 'dest' */
auto may_teleport_to(unit_data *unit, unit_data *dest) -> bool
{
   if(unit == dest || IS_SET(UNIT_FLAGS(dest), UNIT_FL_NO_TELEPORT) || (unit_recursive(unit, dest) != 0) ||
      UNIT_WEIGHT(unit) + UNIT_WEIGHT(dest) > UNIT_CAPACITY(dest))
   {
      return FALSE;
   }

   do
   {
      if(IS_SET(UNIT_FLAGS(dest), UNIT_FL_NO_TELEPORT))
      {
         return FALSE;
      }
   } while((dest = UNIT_IN(dest)) != nullptr);

   return TRUE;
}

/* See if unit is allowed to be transferred to 'dest' */
auto may_teleport(unit_data *unit, unit_data *dest) -> bool
{
   return (static_cast<bool>(static_cast<unsigned int>(may_teleport_away(unit)) != 0U) &&
           static_cast<unsigned int>(may_teleport_to(unit, dest)) != 0U);
}

/* ===================================================================== */

auto object_power(unit_data *unit) -> int
{
   if(IS_OBJ(unit))
   {
      if(OBJ_TYPE(unit) == ITEM_POTION || OBJ_TYPE(unit) == ITEM_SCROLL || OBJ_TYPE(unit) == ITEM_WAND || OBJ_TYPE(unit) == ITEM_STAFF)
      {
         return OBJ_VALUE(unit, 0);
      }
      return OBJ_RESISTANCE(unit);
   }

   return 0;
}

auto room_power(unit_data *unit) -> int
{
   if(IS_ROOM(unit))
   {
      return ROOM_RESISTANCE(unit);
   }
   return 0;
}

/* Return how well a unit defends itself against a spell, by using */
/* its skill (not its power) - That is the group (or attack).      */
/*                                                                 */
auto spell_defense_skill(unit_data *unit, int spell) -> int
{
   int max;

   if(IS_PC(unit))
   {
      if(TREE_ISLEAF(spl_tree, spell))
      {
         max = PC_SPL_SKILL(unit, spell) / 2;
      }
      else
      {
         max = PC_SPL_SKILL(unit, spell);
      }

      while(!TREE_ISROOT(spl_tree, spell))
      {
         spell = TREE_PARENT(spl_tree, spell);

         if(PC_SPL_SKILL(unit, spell) > max)
         {
            max = PC_SPL_SKILL(unit, spell);
         }
      }

      return max;
   }

   if(IS_OBJ(unit))
   {
      return object_power(unit); //  Philosophical... / 2 ?
   }

   if(IS_NPC(unit))
   {
      if(TREE_ISLEAF(spl_tree, spell))
      {
         spell = TREE_PARENT(spl_tree, spell);
      }

      if(TREE_ISROOT(spl_tree, spell))
      {
         max = NPC_SPL_SKILL(unit, spell);
      }
      else
      {
         max = NPC_SPL_SKILL(unit, spell) / 2;
      }

      while(!TREE_ISROOT(spl_tree, spell))
      {
         spell = TREE_PARENT(spl_tree, spell);

         if(NPC_SPL_SKILL(unit, spell) > max)
         {
            max = NPC_SPL_SKILL(unit, spell);
         }
      }

      return max;
   }
   return room_power(unit);
}

/* Return how well a unit attacks with a spell, by using its skill */
/* (not its power).                                                */
/*                                                                 */
auto spell_attack_skill(unit_data *unit, int spell) -> int
{
   if(IS_PC(unit))
   {
      return PC_SPL_SKILL(unit, spell);
   }

   if(IS_OBJ(unit))
   {
      return object_power(unit);
   }

   if(IS_NPC(unit))
   {
      if(TREE_ISLEAF(spl_tree, spell))
      {
         spell = TREE_PARENT(spl_tree, spell);
      }

      return NPC_SPL_SKILL(unit, spell);
   }
   return room_power(unit);
}

/* Return the power in a unit for a given spell type     */
/* For CHAR's determine if Divine or Magic power is used */
auto spell_attack_ability(unit_data *medium, int spell) -> int
{
   if(IS_CHAR(medium))
   {
      /* Figure out if char will use Divine or Magic powers */
      assert(spell_info[spell].realm == ABIL_MAG || spell_info[spell].realm == ABIL_DIV);

      return CHAR_ABILITY(medium, spell_info[spell].realm);
   }

   return spell_attack_skill(medium, spell);
}

auto spell_ability(unit_data *u, int ability, int spell) -> int
{
   if(IS_CHAR(u))
   {
      return CHAR_ABILITY(u, ability);
   }
   return spell_defense_skill(u, spell);
}

/* ===================================================================== */

/* Use this function when a spell caster 'competes' against something  */
/* else. These are typically aggressive spells like 'sleep' etc. where */
/* the defender is entiteled a saving throw.                           */
/*                                                                     */
auto spell_resistance(unit_data *att, unit_data *def, int spell) -> int
{
   if(IS_CHAR(att) && IS_CHAR(def))
   {
      return resistance_skill_check(spell_attack_ability(att, spell),
                                    spell_ability(def, ABIL_BRA, spell),
                                    spell_attack_skill(att, spell),
                                    spell_defense_skill(def, spell));
   }
   return resistance_skill_check(spell_attack_ability(att, spell),
                                 spell_ability(def, ABIL_BRA, spell),
                                 spell_attack_skill(att, spell),
                                 spell_defense_skill(def, spell));
}

/* Use this function when a spell caster competes against his own */
/* skill/ability when casting a spell. For example healing spells */
/* create food etc.                                               */
/* Returns how well it went "100" is perfect > better.            */
auto spell_cast_check(unit_data *att, int spell) -> int
{
   return resistance_skill_check(spell_attack_ability(att, spell), 0, spell_attack_skill(att, spell), 0);
}

/* ===================================================================== */

/* Use this function from attack spells, it will do all the  */
/* nessecary work for you                                    */
/* Qty is 1 for small, 2 for medium and 3 for large versions */
/* of each spell                                             */
auto spell_offensive(spell_args *sa, int spell_number, int bonus) -> int
{
   int        def_shield_bonus;
   int        armour_type;
   int        hit_loc;
   int        roll;
   int        bEffect;
   unit_data *def_shield;

   /* Does the spell perhaps only hit head / body? All?? Right now I
      do it randomly */
   hit_loc = hit_location(sa->caster, sa->target);

   bonus += spell_bonus(sa->caster, sa->medium, sa->target, hit_loc, spell_number, &armour_type, nullptr);

   roll = open100();
   roll_description(sa->caster, "spell", roll);
   bonus += roll;

   sa->hm = chart_damage(bonus, &(spell_chart[spell_number].element[armour_type]));

   def_shield_bonus = shield_bonus(sa->caster, sa->target, &def_shield);

   if((def_shield != nullptr) && spell_info[spell_number].shield != SHIELD_M_USELESS)
   {
      if((spell_info[spell_number].shield == SHIELD_M_BLOCK) && (number(1, 100) <= def_shield_bonus))
      {
         damage_object(sa->target, def_shield, sa->hm);
         damage(sa->caster, sa->target, def_shield, 0, MSG_TYPE_SPELL, spell_number, WEAR_SHIELD);
         return 0;
      }
      if((spell_info[spell_number].shield == SHIELD_M_REDUCE) && (number(1, 100) <= def_shield_bonus))
      {
         sa->hm -= (sa->hm * def_shield_bonus) / 100;
      }
   }

   bEffect = dil_effect(sa->pEffect, sa);

   if(sa->hm > 0)
   {
      damage(sa->caster,
             sa->target,
             nullptr,
             sa->hm,
             MSG_TYPE_SPELL,
             spell_number,
             COM_MSG_EBODY,
             static_cast<int>(static_cast<int>(bEffect) == 0));
   }
   else
   {
      damage(
         sa->caster, sa->target, nullptr, 0, MSG_TYPE_SPELL, spell_number, COM_MSG_MISS, static_cast<int>(static_cast<int>(bEffect) == 0));
   }

   return sa->hm;
}
