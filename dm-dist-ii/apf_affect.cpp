/* *********************************************************************** *
 * File   : apf_affect.c                              Part of Valhalla MUD *
 * Version: 2.03                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Affect system apply routines (APF_).                           *
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

/* 23/07/92 seifert: Fixed grave bug in reordering af ticks                */

#include "affect.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "skills.h"
#include "spells.h"
#include "structs.h"
#include "tree_type.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

auto raw_destruct_affect(unit_affected_type *af) -> bool
{
   unlink_affect(af->owner, af); /* registers and frees later */
   return FALSE;                 /* CANCEL */
}

auto skill_overflow(int skill, int change, bool set) -> bool
{
   if(static_cast<unsigned int>(set) != 0U)
   {
      if(skill == 0)
      {
         return TRUE;
      }

      if(((skill + change) > SKILL_MAX) || ((skill + change) < 0))
      {
         return TRUE;
      }
   }
   else
   {
      if(((skill - change) > SKILL_MAX) || ((skill - change) < 0))
      {
         return TRUE;
      }
   }

   return FALSE;
}

/*                                                      */
/* Data[0] Must contain bits to set in CHAR_FLAGS()     */
/*                                                      */
auto apf_mod_char_flags(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   unit_affected_type *taf;

   assert(IS_CHAR(unit));

   if(static_cast<unsigned int>(set) != 0U)
   {
      SET_BIT(CHAR_FLAGS(unit), (uint32_t)af->data[0]);
   }
   else
   {
      REMOVE_BIT(CHAR_FLAGS(unit), (uint32_t)af->data[0]);

      /* After bit has been removed, call all apply functions  */
      /* which will set bits [and one of these may be the bit  */
      /* which this function just removed. Notice that this    */
      /* implies that a character can not permanently have     */
      /* these bits set, since a call of this function will    */
      /* remove them                                           */
      for(taf = UNIT_AFFECTED(af->owner); taf != nullptr; taf = taf->next)
      {
         if((taf != af) && (taf->applyf_i == APF_MOD_CHAR_FLAGS))
         {
            SET_BIT(CHAR_FLAGS(unit), (uint32_t)taf->data[0]);
         }
      }
   }
   return TRUE;
}

/*                                                      */
/* Data[0] Must contain bits to set in OBJ_FLAGS() */
/*                                                      */
auto apf_mod_obj_flags(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   unit_affected_type *taf;

   assert(IS_OBJ(unit));

   if(static_cast<unsigned int>(set) != 0U)
   {
      SET_BIT(OBJ_FLAGS(unit), (uint32_t)af->data[0]);
   }
   else
   {
      REMOVE_BIT(OBJ_FLAGS(unit), (uint32_t)af->data[0]);

      /* After bit has been removed, call all apply functions  */
      /* which will set bits [and one of these may be the bit  */
      /* which this function just removed. Notice that this    */
      /* implies that a object can not permanently have these  */
      /* bits set, since a call of this function will remove   */
      /* them                                                  */
      for(taf = UNIT_AFFECTED(af->owner); taf != nullptr; taf = taf->next)
      {
         if((taf != af) && (taf->applyf_i == APF_MOD_OBJ_FLAGS))
         {
            SET_BIT(OBJ_FLAGS(unit), (uint32_t)taf->data[0]);
         }
      }
   }
   return TRUE;
}

/*                                                      */
/* Data[0] Must contain bits to set in UNIT_FLAGS()     */
/*                                                      */
auto apf_mod_unit_flags(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   unit_affected_type *taf;

   if(static_cast<unsigned int>(set) != 0U)
   {
      SET_BIT(UNIT_FLAGS(unit), (uint16_t)af->data[0]);
   }
   else
   {
      REMOVE_BIT(UNIT_FLAGS(unit), (uint16_t)af->data[0]);

      /* After bit has been removed, call all apply functions  */
      /* which will set bits [and one of these may be the bit  */
      /* which this function just removed. Notice that this    */
      /* implies that a character can not permanently have     */
      /* these bits set, since a call of this function will    */
      /* remove them                                           */
      for(taf = UNIT_AFFECTED(af->owner); taf != nullptr; taf = taf->next)
      {
         if((taf != af) && (taf->applyf_i == APF_MOD_UNIT_FLAGS))
         {
            SET_BIT(UNIT_FLAGS(unit), (uint16_t)taf->data[0]);
         }
      }
   }
   return TRUE;
}

auto apf_weapon_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   int modify;

   if(!IS_CHAR(unit))
   {
      slog(LOG_ALL, 0, "ERROR: Affect weapon groups on room/obj %s@%s", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
      return TRUE;
   }

   modify = af->data[0];
   if(IS_NPC(unit))
   {
      while(modify > WPN_GROUP_MAX)
      {
         modify = TREE_PARENT(wpn_tree, modify);
      }
   }

   if(static_cast<unsigned int>(set) != 0U)
   {
      if(IS_PC(unit))
      {
         if(static_cast<unsigned int>(skill_overflow(PC_WPN_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            return raw_destruct_affect(af);
         }
         PC_WPN_SKILL(unit, modify) += af->data[1];
      }
      else
      {
         if(static_cast<unsigned int>(skill_overflow(NPC_WPN_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            return raw_destruct_affect(af);
         }
         NPC_WPN_SKILL(unit, modify) += af->data[1];
      }
   }
   else
   {
      if(IS_PC(unit))
      {
         if(static_cast<unsigned int>(skill_overflow(PC_WPN_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            af->duration = 0; /* Stall the destruction until possible */
            return FALSE;     /* CANCEL the destruction               */
         }
         PC_WPN_SKILL(unit, modify) -= af->data[1];
      }
      else
      {
         if(static_cast<unsigned int>(skill_overflow(NPC_WPN_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            af->duration = 0; /* Stall the destruction until possible */
            return FALSE;     /* CANCEL the destruction               */
         }
         NPC_WPN_SKILL(unit, modify) -= af->data[1];
      }
   }

   return TRUE;
}

/* NPC's are ignored, they don't have skills. */
auto apf_skill_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   if(!IS_CHAR(unit))
   {
      slog(LOG_ALL, 0, "ERROR: Affect skill groups on room/obj %s@%s", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
      return TRUE;
   }

   if(static_cast<unsigned int>(set) != 0U)
   {
      if(IS_PC(unit))
      {
         if(static_cast<unsigned int>(skill_overflow(PC_SKI_SKILL(unit, af->data[0]), af->data[1], set)) != 0U)
         {
            return raw_destruct_affect(af);
         }
         PC_SKI_SKILL(unit, af->data[0]) += af->data[1];
      }
   }
   else
   {
      if(IS_PC(unit))
      {
         if(static_cast<unsigned int>(skill_overflow(PC_SKI_SKILL(unit, af->data[0]), af->data[1], set)) != 0U)
         {
            af->duration = 0; /* Stall the destruction until possible */
            return FALSE;     /* CANCEL the destruction               */
         }
         PC_SKI_SKILL(unit, af->data[0]) -= af->data[1];
      }
   }

   return TRUE;
}

/* Data[0] must contain the SPL_XXX to change             */
/* Data[1] must contain the amount to change              */
/* Data[1] is added when set, and subtracted when not set */
/* Unit can be CHAR                                       */
auto apf_spell_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   int modify;

   if(!IS_CHAR(unit))
   {
      slog(LOG_ALL, 0, "ERROR: Affect spell groups on room/obj %s@%s", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
      return TRUE;
   }

   modify = af->data[0];
   if(IS_NPC(unit))
   {
      while(modify > SPL_GROUP_MAX)
      {
         modify = TREE_PARENT(spl_tree, modify);
      }
   }

   if(static_cast<unsigned int>(set) != 0U)
   {
      if(IS_PC(unit))
      {
         if(static_cast<unsigned int>(skill_overflow(PC_SPL_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            return raw_destruct_affect(af);
         }
         PC_SPL_SKILL(unit, modify) += af->data[1];
      }
      else
      {
         if(static_cast<unsigned int>(skill_overflow(NPC_SPL_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            return raw_destruct_affect(af);
         }
         NPC_SPL_SKILL(unit, modify) += af->data[1];
      }
   }
   else
   {
      if(IS_PC(unit))
      {
         if(static_cast<unsigned int>(skill_overflow(PC_SPL_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            af->duration = 0; /* Stall the destruction until possible */
            return FALSE;     /* CANCEL the destruction               */
         }
         PC_SPL_SKILL(unit, modify) -= af->data[1];
      }
      else
      {
         if(static_cast<unsigned int>(skill_overflow(NPC_SPL_SKILL(unit, modify), af->data[1], set)) != 0U)
         {
            af->duration = 0; /* Stall the destruction until possible */
            return FALSE;     /* CANCEL the destruction               */
         }
         NPC_SPL_SKILL(unit, modify) -= af->data[1];
      }
   }

   return TRUE;
}

/* Data[0] must contain the ABIL_XXX to change            */
/* Data[1] must contain the amount to change              */
/* Data[1] is added when set, and subtracted when not set */
/* Unit must be a CHAR!                                   */
auto apf_ability_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   assert(IS_CHAR(unit));

   if(static_cast<unsigned int>(set) != 0U)
   {
      if(static_cast<unsigned int>(skill_overflow(CHAR_ABILITY(unit, af->data[0]), af->data[1], set)) != 0U)
      {
         return raw_destruct_affect(af);
      }

      CHAR_ABILITY(unit, af->data[0]) += af->data[1];

      if(af->data[0] == ABIL_HP)
      {
         UNIT_MAX_HIT(unit) = hit_limit(unit);
      }
   }
   else
   {
      if(static_cast<unsigned int>(skill_overflow(CHAR_ABILITY(unit, af->data[0]), af->data[1], set)) != 0U)
      {
         af->duration = 0; /* Stall the destruction until possible */
         return FALSE;     /* CANCEL the destruction               */
      }
      CHAR_ABILITY(unit, af->data[0]) -= af->data[1];

      if(af->data[0] == ABIL_HP)
      {
         UNIT_MAX_HIT(unit) = hit_limit(unit);
      }
   }

   return TRUE;
}

/* Data[0] = Amount of light sources */
auto apf_light(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   if(static_cast<unsigned int>(set) == 0U)
   {
      af->data[0] = -af->data[0];
   }

   if(IS_ROOM(unit))
   {
      UNIT_LIGHTS(unit) += af->data[0];
   }

   modify_bright(unit, af->data[0]);

   if(static_cast<unsigned int>(set) == 0U)
   {
      af->data[0] = -af->data[0];
   }

   return TRUE;
}

/* Data[0] = The new armour-type */
/* Data[1] = The original armour-type */
auto apf_natural_armour(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   if(!IS_CHAR(unit))
   {
      return TRUE;
   }

   if((is_in(af->data[0], ARM_CLOTHES, ARM_PLATE) == 0) || (af->data[0] < CHAR_NATURAL_ARMOUR(unit)))
   {
      af->data[0] = -1; /* Ineffective, other spell cancels this one */
      return TRUE;
   }

   if(static_cast<unsigned int>(set) != 0U)
   {
      unit_affected_type *taf;

      for(taf = UNIT_AFFECTED(unit); taf != nullptr; taf = taf->next)
      {
         if((taf->id == ID_NATURAL_ARMOUR) && (taf != af))
         {
            af->data[1] = taf->data[1];
            break;
         }
      }

      CHAR_NATURAL_ARMOUR(unit) = MAX(CHAR_NATURAL_ARMOUR(unit), af->data[0]);
   }
   else
   {
      /* Restore value... */
      CHAR_NATURAL_ARMOUR(unit) = af->data[1];
   }

   return TRUE;
}

/* Data[0] = The new speed            */
/* Data[2] = The original speed - [2] because it is not tested in the
             unequip_object affect remove match. */
auto apf_speed(unit_affected_type *af, unit_data *unit, bool set) -> bool
{
   if(!IS_CHAR(unit))
   {
      return TRUE;
   }

   if((af->data[0] < 4) || (af->data[2] < 0))
   {
      return TRUE;
   }

   if(static_cast<unsigned int>(set) != 0U)
   {
      unit_affected_type *taf;

      af->data[2] = CHAR_SPEED(unit);

      for(taf = UNIT_AFFECTED(unit); taf != nullptr; taf = taf->next)
      {
         if((taf->id == ID_SPEED) && (taf != af))
         {
            af->data[2] = -1;
            break;
         }
      }

      if(taf == nullptr)
      {
         CHAR_SPEED(unit) = af->data[0];
      }
   }
   else
   {
      CHAR_SPEED(unit) = af->data[2];
   }

   return TRUE;
}

/* --------------------------------------------------------------------- */
