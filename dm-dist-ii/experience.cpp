
#include "affect.h"
#include "comm.h"
#include "common.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "magic.h"
#include "movement.h"
#include "shi_info_type.h"
#include "skills.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <algorithm>

auto kludge_bonus(int level, int points) -> int
{
   if(level <= 20)
   {
      return 0;
   }

   if(level <= 50)
   {
      return ((5 * (level - 20)) * std::min(100, points)) / 100;
   }

   int b;
   int expected;

   expected = 100 + (level - 50);

   b        = 150;
   b += (5 * (level - 50) * std::min(expected, points)) / expected;

   return b;
}

/* Let 'def' attempt to use his shield against 'att'. If the shield */
/* is magic, then it is easier to use. Material & magic does also   */
/* offer half the additional protection in case it is used          */
/* successfully).                                                   */
/* Returns 0 or less if unsuccessful, or 1..100 for the blocking    */
/* chance                                                           */

auto shield_bonus(unit_data *att, unit_data *def, unit_data **pDef_shield) -> int
{
   unit_data *def_shield;
   int        def_shield_bonus = 0;

   int        att_dex;
   int        def_dex;
   int        hm;

   att_dex = effective_dex(att);
   def_dex = effective_dex(def);

   /* If attacker can't see the defender, then the defender has a   */
   /* much better effective dexterity (since attacker can't see him */
   if(!CHAR_CAN_SEE(att, def))
   {
      att_dex -= 12;
   }

   if((def_shield = equipment_type(def, WEAR_SHIELD, ITEM_SHIELD)) != nullptr)
   {
      int shield_bonus = 0;

      if(is_in(OBJ_VALUE(def_shield, 0), SHIELD_SMALL, SHIELD_LARGE) == 0)
      {
         slog(LOG_ALL, 0, "Illegal shield type.");
         OBJ_TYPE(def_shield) = ITEM_TRASH;
      }
      else if((is_in(OBJ_VALUE(def_shield, 1), -25, 25) == 0) || (is_in(OBJ_VALUE(def_shield, 2), -25, 25) == 0))
      {
         slog(LOG_ALL, 0, "Illegal shield bonus");
         OBJ_TYPE(def_shield) = ITEM_TRASH;
      }
      else
      {
         shield_bonus = OBJ_VALUE(def_shield, 1) + OBJ_VALUE(def_shield, 2);

         /* Let's make a shield check - CAN_SEE does affect this too */
         hm           = resistance_skill_check(def_dex + shield_bonus,
                                     att_dex,
                                     IS_PC(def) ? PC_SKI_SKILL(def, SKI_SHIELD) : def_dex,
                                     IS_PC(att) ? PC_SKI_SKILL(att, SKI_SHIELD) : att_dex);

         if(hm >= 0)
         { /* Successful Shield use */
            def_shield_bonus = shi_info[OBJ_VALUE(def_shield, 0)].melee + shield_bonus / 2;
         }
      }
   } /* End of Shield */

   if(pDef_shield != nullptr)
   {
      *pDef_shield = def_shield;
   }

   return def_shield_bonus;
}

auto spell_bonus(
   unit_data *att, unit_data *medium, unit_data *def, int hit_loc, int spell_number, int *pDef_armour_type, unit_data **pDef_armour) -> int
{
   int        att_spl_knowledge;
   int        def_spl_knowledge;
   int        att_bonus;
   int        def_bonus;
   int        def_armour_type;
   unit_data *def_armour;
   int        hm;

   att_bonus = CHAR_OFFENSIVE(att);
   def_bonus = CHAR_DEFENSIVE(def);

   /* If attacker can't see the defender, then the defender has a   */
   /* much better effective dexterity (since attacker can't see him */
   if(!CHAR_CAN_SEE(att, def))
   {
      att_bonus -= 12;
   }

   if((UNIT_IS_GOOD(att) && (affected_by_spell(def, ID_PROT_GOOD) != nullptr)) ||
      (UNIT_IS_EVIL(att) && (affected_by_spell(def, ID_PROT_EVIL) != nullptr)))
   {
      def_bonus += 10;
   }

   if((def_armour = equipment_type(def, hit_loc, ITEM_ARMOR)) != nullptr)
   {
      def_armour_type = OBJ_VALUE(def_armour, 0);
      if(is_in(OBJ_VALUE(def_armour, 1), -25, 25) != 0)
      {
         def_bonus += OBJ_VALUE(def_armour, 1) + OBJ_VALUE(def_armour, 2);
      }
      else
      {
         slog(LOG_ALL, 0, "Illegal armour bonus.");
      }
   }
   else
   {
      def_armour_type = CHAR_NATURAL_ARMOUR(def);
   }

   if(pDef_armour_type != nullptr)
   {
      *pDef_armour_type = def_armour_type;
   }

   if(pDef_armour != nullptr)
   {
      *pDef_armour = def_armour;
   }

   att_spl_knowledge = spell_attack_skill(medium, spell_number);
   def_spl_knowledge = spell_defense_skill(def, spell_number);

   att_spl_knowledge += kludge_bonus(CHAR_LEVEL(att), att_spl_knowledge);
   def_spl_knowledge += kludge_bonus(CHAR_LEVEL(def), def_spl_knowledge) / 2;

   if(CHAR_AWAKE(def))
   {
      hm = (5 * (spell_attack_ability(medium, spell_number) - spell_ability(def, ABIL_BRA, spell_number))) / 2 +
           2 * (att_spl_knowledge - def_spl_knowledge) - def_bonus;
   }
   else
   {
      hm = (5 * spell_attack_ability(medium, spell_number)) / 2 + 2 * spell_attack_skill(medium, spell_number) - def_bonus;
   }

   return std::max(-50, hm);
}

/* If 'att' hits 'def' on 'hit_loc' then what is his basic attack */
/* modification? This value should then be added to an open roll  */
/* and looked up upon the damage_charts. If armour type points    */
/* to anything, then it will be set to the defenders armour_type  */
/* which should be used upon lookup                               */

auto melee_bonus(unit_data  *att,
                 unit_data  *def,
                 int         hit_loc,
                 int        *pAtt_weapon_type,
                 unit_data **pAtt_weapon,
                 int        *pDef_armour_type,
                 unit_data **pDef_armour,
                 int         primary) -> int
{
   int        att_dex;
   int        att_bonus;
   int        att_wpn_knowledge;
   int        def_dex;
   int        def_bonus;
   int        def_wpn_knowledge;

   unit_data *att_wpn;
   int        att_wpn_type;

   int        def_armour_type;
   unit_data *def_armour;

   int        hm;

   att_dex   = effective_dex(att);
   def_dex   = effective_dex(def);

   att_bonus = CHAR_OFFENSIVE(att);
   def_bonus = CHAR_DEFENSIVE(def);

   if((pAtt_weapon_type != nullptr) && (is_in(*pAtt_weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX - 1) != 0))
   {
      att_wpn           = nullptr;
      att_wpn_type      = *pAtt_weapon_type;
      att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
   }
   else
   {
      if(primary != 0)
      {
         att_wpn = equipment_type(att, WEAR_WIELD, ITEM_WEAPON);
      }
      else
      {
         att_wpn = equipment_type(att, WEAR_HOLD, ITEM_WEAPON);
      }

      if(att_wpn != nullptr)
      {
         att_wpn_type      = OBJ_VALUE(att_wpn, 0); /* [0] is category */
         att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
         if(is_in(OBJ_VALUE(att_wpn, 1), -25, 25) != 0)
         {
            att_bonus += OBJ_VALUE(att_wpn, 1) + OBJ_VALUE(att_wpn, 2);
         }
      }
      else
      {
         att_wpn_type      = CHAR_ATTACK_TYPE(att);
         att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
      }
   }

   if(char_dual_wield(att) != 0)
   {
      int dual_skill;

      if(IS_PC(att))
      {
         dual_skill = PC_SKI_SKILL(att, SKI_DUAL_WIELD);
      }
      else
      {
         dual_skill = CHAR_DEX(att);
      }

      if(primary != 0)
      {
         att_bonus -= std::max(0, 25 - (dual_skill / 4));
      }
      else
      {
         att_bonus -= std::max(0, 50 - (dual_skill / 4));
      }
   }

   def_wpn_knowledge = weapon_defense_skill(def, att_wpn_type);

   if(CHAR_FIGHTING(def) != att)
   {
      def_bonus -= 25;
   }

   /* If attacker can't see the defender, then the defender has a   */
   /* much better effective dexterity (since attacker can't see him */
   if(!CHAR_CAN_SEE(att, def))
   {
      def_bonus += 25;
   }

   if(!CHAR_CAN_SEE(def, att))
   {
      att_bonus += 25;
   }

   /* Slaying Weapons */
   if((att_wpn != nullptr) && OBJ_VALUE(att_wpn, 3) == CHAR_RACE(def))
   {
      att_bonus += 25;
   }

   if((UNIT_IS_GOOD(att) && (affected_by_spell(def, ID_PROT_GOOD) != nullptr)) ||
      (UNIT_IS_EVIL(att) && (affected_by_spell(def, ID_PROT_EVIL) != nullptr)))
   {
      def_bonus += 20;
   }

   if((def_armour = equipment_type(def, hit_loc, ITEM_ARMOR)) != nullptr)
   {
      def_armour_type = OBJ_VALUE(def_armour, 0);
      if(is_in(OBJ_VALUE(def_armour, 1), -25, 25) != 0)
      {
         def_bonus += OBJ_VALUE(def_armour, 1) + OBJ_VALUE(def_armour, 2);
      }
      else
      {
         slog(LOG_ALL, 0, "Illegal armour bonus.");
      }
   }
   else
   {
      def_armour_type = CHAR_NATURAL_ARMOUR(def);
   }

   if(pAtt_weapon_type != nullptr)
   {
      *pAtt_weapon_type = att_wpn_type;
   }

   if(pAtt_weapon != nullptr)
   {
      *pAtt_weapon = att_wpn;
   }

   if(pDef_armour_type != nullptr)
   {
      *pDef_armour_type = def_armour_type;
   }

   if(pDef_armour != nullptr)
   {
      *pDef_armour = def_armour;
   }

   att_wpn_knowledge += kludge_bonus(CHAR_LEVEL(att), att_wpn_knowledge);
   def_wpn_knowledge += kludge_bonus(CHAR_LEVEL(def), def_wpn_knowledge) / 2;

   if(CHAR_AWAKE(def))
   {
      hm = (5 * (CHAR_STR(att) - def_dex)) / 2 + (2 * (att_wpn_knowledge - def_wpn_knowledge)) + att_bonus - def_bonus;
   }
   else
   {
      hm = (5 * CHAR_STR(att)) / 2 + att_bonus + 2 * att_wpn_knowledge + 50;
   }

   // This results in a 5% hm increase per "level"

   return std::max(-50, hm);
}

auto base_melee(unit_data *att, unit_data *def, int hit_loc) -> int
{
   int        ocp;
   int        bonus;
   unit_data *ocf;

   assert(CHAR_COMBAT(def));

   ocp = CHAR_POS(def);
   ocf = CHAR_FIGHTING(def);
   CHAR_COMBAT(def)->setMelee(att);
   CHAR_POS(def) = POSITION_FIGHTING;

   bonus         = melee_bonus(att, def, hit_loc, nullptr, nullptr, nullptr, nullptr);

   CHAR_POS(def) = ocp;
   CHAR_COMBAT(def)->setMelee(ocf);

   return bonus;
}

/* Disregards how likely it is to be hit and only concentrates on the       */
/* danger involved in combat (i.e. how fast would you die in worst case)    */
/* Returns number of rounds it takes att to kill def                        */

auto base_consider(unit_data *att, unit_data *def) -> int
{
   int        ocp;
   int        bonus;
   unit_data *ocf;
   int        att_wpn_type;
   int        def_arm_type;
   int        dam;

   ocp           = CHAR_POS(def);
   ocf           = CHAR_FIGHTING(def);
   CHAR_POS(def) = POSITION_FIGHTING;

   att_wpn_type  = WPN_ROOT;

   bonus         = melee_bonus(att, def, WEAR_BODY, &att_wpn_type, nullptr, &def_arm_type, nullptr);

   CHAR_POS(def) = ocp;

   dam           = weapon_damage(50 + bonus, att_wpn_type, def_arm_type);

   if(dam <= 0)
   {
      return 10000;
   }
   return UNIT_MAX_HIT(def) / dam; /* Rounds to die.... */
}

void do_consider(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *vict;
   int        rtd;
   char      *oarg = arg;

   if(IS_PC(ch) && PC_SKI_SKILL(ch, SKI_CONSIDER) == 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("You consider Life, the Universe and everything.\n\r", ch);
      return;
   }

   if((vict = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO)) == nullptr)
   {
      send_to_char("No such person around.\n\r", ch);
      return;
   }

   if(!IS_CHAR(vict))
   {
      backdoor(ch, arg, cmd);
      send_to_char("It must be dead already?\n\r", ch);
      return;
   }

   if(vict == ch)
   {
      send_to_char("Easy! Very easy indeed!\n\r", ch);
      return;
   }

   if(IS_SET(CHAR_FLAGS(vict), CHAR_PROTECTED))
   {
      send_to_char("Why not just walk to the jail yourself?\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(vict) - 10 >= CHAR_LEVEL(ch))
   {
      act("$3e is probably out of your league.", A_SOMEONE, ch, nullptr, vict, TO_CHAR);
   }

   rtd = base_consider(vict, ch);

   if(rtd <= 1)
   {
      send_to_char("RUN AWAY!\n\r", ch);
   }
   else if(rtd <= 2)
   {
      send_to_char("You ARE mad!\n\r", ch);
   }
   else if(rtd <= 3)
   {
      send_to_char("Very risky indeed!\n\r", ch);
   }
   else if(rtd <= 4)
   {
      send_to_char("Quite a risk.\n\r", ch);
   }
   else if(rtd <= 5)
   {
      send_to_char("Perhaps you would have time to flee.\n\r", ch);
   }
   else if(rtd <= 7)
   {
      send_to_char("You got a fair chance of fleeing.\n\r", ch);
   }
   else if(rtd <= 10)
   {
      send_to_char("You got a very good chance of fleeing.\n\r", ch);
   }
   else
   {
      send_to_char("Plenty of time to flee.\n\r", ch);
   }

   send_done(ch, nullptr, vict, rtd, cmd, oarg);
}

/* Return the quality modifier for the given monster. Modifier depends on */
/* the hand or weapon quality and armour or toughness quality.            */
auto experience_modification(unit_data *att, unit_data *def) -> int
{
   return base_melee(def, att, WEAR_BODY) - base_melee(att, def, WEAR_BODY);
}
