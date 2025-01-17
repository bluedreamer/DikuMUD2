#include "common.h"
#include "dmc.h"
#include "money.h"
#include "skills.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "utility.h"

#include <cctype>
#include <cstdarg> /* va_args in dmc_error() */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

extern struct arm_info_type shi_info[];

/* PS Algorithm 3                                                      */
/* This algorithm returns the total amount of hitpoints possessed when */
/* 'hpp' hit-point-points is current.                                  */
/* Example: hitpoint_total(4) returns total amount of hitpoints when   */
/*          hpp == 4                                                   */
auto                        hitpoint_total(int hpp) -> int
{
   return 10 + 3 * hpp;
}

const char *error_zone_name = "";

/* if fatal is 2, it will NEVER be fatal */
void        dmc_error(int fatal, const char *fmt, ...)
{
   extern int fatal_warnings;

   char       buf[512];
   char       filename[128];
   va_list    args;
   FILE      *f;

   extern int errcon;

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   fprintf(stderr, "%s: %s\n", (fatal != 0) && fatal != 2 ? "FATAL" : "WARNING", buf);
   sprintf(filename, "%s.err", error_zone_name);

   if((f = fopen(filename, "a")) != nullptr)
   {
      fprintf(f, "%s\n", buf);
      fclose(f);
   }

   if(fatal != 2 && ((fatal != 0) || (fatal_warnings != 0)))
   {
      errcon = 1;
   }
}

/*  MONEY
 */
auto legal_amount(struct unit_data *u) -> unsigned long
{
   unsigned long res = IRON_MULT * CHAR_LEVEL(u);

   if(CHAR_LEVEL(u) < 21)
   {
      res *= 5;
   }
   else if(CHAR_LEVEL(u) < 101)
   {
      res *= 10;
   }
   else if(CHAR_LEVEL(u) < 151)
   {
      res *= 15;
   }
   else if(CHAR_LEVEL(u) < 201)
   {
      res *= 20;
   }
   else
   {
      res *= 25;
   }

   return res;
}

auto convert_back_money(unsigned long val) -> const char *
{
   return ""; /* Not programmed yet! */
}

auto convert_money(char *str) -> amount_t
{
   long int val1;
   long int val2;
   amount_t res = 0;
   char    *c;

   if(str == nullptr)
   {
      return 0;
   }

   while((c = strchr(str, '~')) != nullptr)
   {
      sscanf(str, "%ld %ld", &val1, &val2);
      if(val1 > 0)
      {
         if(val2 == -1)
         {
            res += val1;
         }
         else if(is_in(val2, DEF_CURRENCY, MAX_MONEY) != 0)
         {
            res += val1 * val2;
         }
         else
         {
            dmc_error(static_cast<int>(TRUE), "Weird money error");
            return 1;
         }
      }
      str = c + 1;
   }

   return res;
}

/* WEAPONS                               */
/* Input:  Value[0] is weapon category   */
/*         Value[1] is material bonus    */
/*         Value[2] is magic bonus       */
/*         Value[3] is slaying race      */
/*                                       */
/* Output: [3] as input                  */
void dmc_set_weapon(struct unit_data *weapon)
{
   int category;
   int craftsmanship;
   int magic_bonus;
   int slay;

   category      = OBJ_VALUE(weapon, 0);
   craftsmanship = OBJ_VALUE(weapon, 1);
   magic_bonus   = OBJ_VALUE(weapon, 2);
   slay          = OBJ_VALUE(weapon, 3);

   if(is_in(category, WPN_GROUP_MAX, WPN_TREE_MAX - 1) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal weapon category in '%s'.", UNIT_IDENT(weapon));
      OBJ_TYPE(weapon) = ITEM_TRASH;
      return;
   }

   if(is_in(craftsmanship, -25, 25) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal bonus (-100..25 allowed) in '%s'.", UNIT_IDENT(weapon));
      OBJ_TYPE(weapon) = ITEM_TRASH;
      return;
   }

   if(is_in(magic_bonus, -25, 25) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal bonus (-100..25 allowed) in '%s'.", UNIT_IDENT(weapon));
      OBJ_TYPE(weapon) = ITEM_TRASH;
      return;
   }

   set_weapon(weapon);
}

/* ARMOURS                                */
/* INPUT:  Value[0] = Category            */
/*         Value[1] = Bonus               */
/*         Value[2] = ...                 */
void dmc_set_armour(struct unit_data *armour)
{
   int category;
   int craftsmanship;
   int magic_bonus;

   category      = OBJ_VALUE(armour, 0);
   craftsmanship = OBJ_VALUE(armour, 1);
   magic_bonus   = OBJ_VALUE(armour, 2);

   if(is_in(category, ARM_CLOTHES, ARM_PLATE) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal armour category in '%s'.", UNIT_IDENT(armour));
      OBJ_TYPE(armour) = ITEM_TRASH;
      return;
   }

   if(is_in(craftsmanship, -100, 25) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal bonus (-100..25 allowed) in '%s'.", UNIT_IDENT(armour));
      OBJ_TYPE(armour) = ITEM_TRASH;
      return;
   }

   if(is_in(magic_bonus, -100, 25) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal bonus (-100..25 allowed) in '%s'.", UNIT_IDENT(armour));
      OBJ_TYPE(armour) = ITEM_TRASH;
      return;
   }

   set_armour(armour);
}

/* SHIELDS                                  */
/* INPUT:  Value[0] = Shield Category       */
/*         Value[1] = Shield Bonus          */
/*         Value[2] = ...                   */
/*                                          */

void dmc_set_shield(struct unit_data *shield)
{
   int category;
   int craftsmanship;
   int magic_bonus;

   category      = OBJ_VALUE(shield, 0);
   craftsmanship = OBJ_VALUE(shield, 1);
   magic_bonus   = OBJ_VALUE(shield, 2);

   if(is_in(category, SHIELD_SMALL, SHIELD_LARGE) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal shield category in '%s'.", UNIT_IDENT(shield));
      OBJ_TYPE(shield) = ITEM_TRASH;
      return;
   }

   if(is_in(craftsmanship, -100, 25) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal bonus (-100..25 allowed) in '%s'.", UNIT_IDENT(shield));
      OBJ_TYPE(shield) = ITEM_TRASH;
      return;
   }

   if(is_in(magic_bonus, -100, 25) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal bonus (-100..25 allowed) in '%s'.", UNIT_IDENT(shield));
      OBJ_TYPE(shield) = ITEM_TRASH;
      return;
   }

   set_shield(shield);
}

void set_points(struct unit_data *u)
{
   int i;
   int sum;
   int max;
   int spoints;
   int apoints;

   if(is_in(CHAR_ATTACK_TYPE(u), WPN_FIST, WPN_CRUSH) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal hand attack type %d.", CHAR_ATTACK_TYPE(u));
      CHAR_ATTACK_TYPE(u) = WPN_FIST;
   }

   if(is_in(CHAR_LEVEL(u), 0, 199) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal level in  '%s'.", UNIT_IDENT(u));
      CHAR_LEVEL(u) = 0;
   }

   for(i = sum = 0; i < ABIL_TREE_MAX; i++)
   {
      sum += CHAR_ABILITY(u, i);
   }

   if(sum != 100)
   {
      dmc_error(static_cast<int>(TRUE), "Abilities in '%s' sums up to %d,and not 100.", UNIT_IDENT(u), sum);
   }

   for(i = sum = 0; i < WPN_GROUP_MAX; i++)
   {
      sum += NPC_WPN_SKILL(u, i);
   }

   for(i = 0; i < SPL_GROUP_MAX; i++)
   {
      sum += NPC_SPL_SKILL(u, i);
   }

   if(sum != 100)
   {
      dmc_error(static_cast<int>(TRUE), "Spells&weapons in '%s' sums up to %d, and not 100.", UNIT_IDENT(u), sum);
   }

   /* It's "*2" because each training session gives the player 2 ability
      points */

   apoints = 2 * ability_point_total(CHAR_LEVEL(u));
   apoints = (12 * apoints) / 10;

   // apoints = ((100+20-CHAR_LEVEL(u)/10)*apoints) / 100; /* 120% - 100% */

   /* It's "*5" because each training session gives the player 5 skill
      points.
      Remember that monsters do not have nearly as many weapons / spells */
   spoints = (5 * ability_point_total(CHAR_LEVEL(u))) / 2;
   spoints = (12 * spoints) / 10;

   // spoints = ((100+20-CHAR_LEVEL(u)/10)*spoints) / 100; /* 120% - 100% */

   if((i = distribute_points(&CHAR_ABILITY(u, 0), ABIL_TREE_MAX, apoints, CHAR_LEVEL(u))) != 0)
   {
      dmc_error(static_cast<int>(FALSE), "%s - An ability is %d points.", UNIT_IDENT(u), i);
   }

   if((i = distribute_points(&NPC_WPN_SKILL(u, 0), WPN_GROUP_MAX, spoints, CHAR_LEVEL(u))) != 0)
   {
      dmc_error(static_cast<int>(FALSE), "%s - A weapon skill exceeds %d points.", UNIT_IDENT(u), i);
   }

   for(max = i = 0; i < WPN_GROUP_MAX; i++)
   {
      if(NPC_WPN_SKILL(u, i) > max)
      {
         max = NPC_WPN_SKILL(u, i);
      }
   }

   NPC_WPN_SKILL(u, WPN_ROOT) = max / 4;

   if((i = distribute_points(&NPC_SPL_SKILL(u, 0), SPL_GROUP_MAX, spoints, CHAR_LEVEL(u))) != 0)
   {
      dmc_error(static_cast<int>(FALSE), "%s - A spell skill exceeds %d points.", UNIT_IDENT(u), i);
   }

   for(max = i = 0; i < SPL_GROUP_MAX; i++)
   {
      if(NPC_SPL_SKILL(u, i) > max)
      {
         max = NPC_SPL_SKILL(u, i);
      }
   }

   NPC_SPL_SKILL(u, SPL_ALL) = max / 4;

   if(is_in(CHAR_NATURAL_ARMOUR(u), ARM_CLOTHES, ARM_PLATE) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "%s, Illegal natural armour type.", UNIT_IDENT(u));
      CHAR_NATURAL_ARMOUR(u) = ARM_PLATE;
   }

   if(is_in(CHAR_ATTACK_TYPE(u), WPN_GROUP_MAX, WPN_TREE_MAX - 1) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "Illegal attack category in '%s'.", UNIT_IDENT(u));
      CHAR_ATTACK_TYPE(u) = WPN_FIST;
   }

   UNIT_HIT(u) = UNIT_MAX_HIT(u) = hitpoint_total(CHAR_HPP(u));
}

void set_room_data(struct unit_data *u)
{
   SET_BIT(UNIT_MANIPULATE(u), MANIPULATE_ENTER);
}

void show_info(struct unit_data *npc)
{
   static int first = static_cast<int>(FALSE);

   if(first == 0)
   {
      fprintf(stderr, "                      STR  DEX  CON  HIT  BRA  CHA  MAG  DIV\n\r\n\r");
      first = static_cast<int>(TRUE);
   }

   fprintf(stderr,
           "%-20s %4d %4d %4d %4d %4d %4d %4d %4d\n\r",
           UNIT_IDENT(npc),
           CHAR_STR(npc),
           CHAR_DEX(npc),
           CHAR_CON(npc),
           UNIT_MAX_HIT(npc),
           CHAR_BRA(npc),
           CHAR_CHA(npc),
           CHAR_MAG(npc),
           CHAR_DIV(npc));
}

void process_affects(unit_data *pUnit)
{
   unit_affected_type *pAf;
   int                 firstf;
   int                 tickf;
   int                 lastf;
   int                 applyf;

   for(pAf = UNIT_AFFECTED(pUnit); pAf != nullptr; pAf = pAf->next)
   {
      firstf = static_cast<int>(pAf->firstf_i == TIF_NONE);
      tickf  = static_cast<int>(pAf->tickf_i == TIF_NONE);
      lastf  = static_cast<int>(pAf->lastf_i == TIF_NONE);
      applyf = static_cast<int>(pAf->applyf_i == APF_NONE);

      switch(pAf->id)
      {
         case ID_TRANSFER_MAG:
            firstf = static_cast<int>(pAf->firstf_i == TIF_MAG_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_MAG_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_MAG)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_DIV:
            firstf = static_cast<int>(pAf->firstf_i == TIF_DIV_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_DIV_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_DIV)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_STR:
            firstf = static_cast<int>(pAf->firstf_i == TIF_STR_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_STR_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_STR)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_DEX:
            firstf = static_cast<int>(pAf->firstf_i == TIF_DEX_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_DEX_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_DEX)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_CON:
            firstf = static_cast<int>(pAf->firstf_i == TIF_CON_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_CON_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_CON)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_CHA:
            firstf = static_cast<int>(pAf->firstf_i == TIF_CHA_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_CHA_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_CHA)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_BRA:
            firstf = static_cast<int>(pAf->firstf_i == TIF_BRA_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_BRA_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_BRA)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_TRANSFER_HPP:
            firstf = static_cast<int>(pAf->firstf_i == TIF_HIT_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_HIT_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_ABILITY);
            if(pAf->data[0] != ABIL_HP)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -20, 5) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -20..+5 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_HIDDEN_DIFFICULTY:
            if(is_in(pAf->data[0], 0, 5) == 0)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal direction %d!", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
               pAf->data[0] = 0;
            }
            if(is_in(pAf->data[1], 0, SKILL_MAX) == 0)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal skill %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
               pAf->data[0] = 0;
            }
            break;

         case ID_CURSE:
            applyf = static_cast<int>(pAf->applyf_i == APF_MOD_OBJ_FLAGS);
            break;

         case ID_TRANSFER_CHARFLAGS:
            firstf = static_cast<int>(pAf->firstf_i == TIF_EYES_TINGLE);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_EYES_TINGLE);
            applyf = static_cast<int>(pAf->applyf_i == APF_MOD_CHAR_FLAGS);
            break;

         case ID_SPELL_TRANSFER:
            firstf = static_cast<int>(pAf->firstf_i == TIF_SPL_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_SPL_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_SPELL_ADJ);
            if(is_in(pAf->data[0], 0, SPL_TREE_MAX) == 0)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -10, 10) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -10..+10 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_SKILL_TRANSFER:
            firstf = static_cast<int>(pAf->firstf_i == TIF_SKI_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_SKI_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_SKILL_ADJ);
            if(is_in(pAf->data[0], 0, SKI_TREE_MAX) == 0)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -10, 10) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -10..+10 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_WEAPON_TRANSFER:
            firstf = static_cast<int>(pAf->firstf_i == TIF_WPN_INC);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_WPN_DEC);
            applyf = static_cast<int>(pAf->applyf_i == APF_WEAPON_ADJ);
            if(is_in(pAf->data[0], 0, WPN_TREE_MAX) == 0)
            {
               dmc_error(static_cast<int>(TRUE), "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->data[0], pAf->id);
            }
            if(is_in(pAf->data[1], -10, 10) == 0)
            {
               dmc_error(static_cast<int>(FALSE), "%s: Adjustment %d outside -10..+10 in ID %d.", UNIT_IDENT(pUnit), pAf->data[1], pAf->id);
            }
            break;

         case ID_PROT_EVIL_TRANSFER:
            firstf = static_cast<int>(pAf->firstf_i == TIF_PROT_EVIL_ON);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_PROT_EVIL_OFF);
            tickf  = static_cast<int>(pAf->tickf_i == TIF_NONE);
            applyf = static_cast<int>(pAf->applyf_i == APF_NONE);
            break;

         case ID_PROT_GOOD_TRANSFER:
            firstf = static_cast<int>(pAf->firstf_i == TIF_PROT_GOOD_ON);
            lastf  = static_cast<int>(pAf->lastf_i == TIF_PROT_GOOD_OFF);
            tickf  = static_cast<int>(pAf->tickf_i == TIF_NONE);
            applyf = static_cast<int>(pAf->applyf_i == APF_NONE);
            break;

         case ID_TRANSFER_SPEED:
            firstf = static_cast<int>((pAf->firstf_i == TIF_SPEED_BETTER) || (pAf->firstf_i == TIF_SPEED_WORSE));
            lastf  = static_cast<int>((pAf->lastf_i == TIF_SPEED_BETTER) || (pAf->lastf_i == TIF_SPEED_WORSE));
            tickf  = static_cast<int>(pAf->tickf_i == TIF_NONE);
            applyf = static_cast<int>(pAf->applyf_i == APF_SPEED);
            break;

         default:
            dmc_error(static_cast<int>(TRUE), "%s: Illegal affect ID (%d).", UNIT_IDENT(pUnit), pAf->id);
            break;
      }

      if(firstf == static_cast<int>(FALSE))
      {
         dmc_error(static_cast<int>(TRUE), "%s: Illegal firstf %d in id %d.", UNIT_IDENT(pUnit), pAf->firstf_i, pAf->id);
      }
      if(tickf == static_cast<int>(FALSE))
      {
         dmc_error(static_cast<int>(TRUE), "%s: Illegal tickf %d in id %d.", UNIT_IDENT(pUnit), pAf->tickf_i, pAf->id);
      }
      if(lastf == static_cast<int>(FALSE))
      {
         dmc_error(static_cast<int>(TRUE), "%s: Illegal lastf %d in id %d.", UNIT_IDENT(pUnit), pAf->lastf_i, pAf->id);
      }
      if(applyf == static_cast<int>(FALSE))
      {
         dmc_error(static_cast<int>(TRUE), "%s: Illegal applyf %d in id %d.", UNIT_IDENT(pUnit), pAf->applyf_i, pAf->id);
      }
   }
}

void process_funcs(unit_data *u)
{
   unit_fptr *fptr;

   for(fptr = UNIT_FUNC(u); fptr != nullptr; fptr = fptr->next)
   {
      if(is_in(fptr->index, 0, SFUN_TOP_IDX) == 0)
      {
         dmc_error(static_cast<int>(TRUE),
                   "%s: Function index not in legal SFUN_XX range.",
                   UNIT_IDENT(u),
                   (float)fptr->heart_beat / (float)PULSE_SEC);
         fptr->index = 0;
      }

      if(fptr->heart_beat != 0)
      {
         if(fptr->heart_beat < WAIT_SEC * 3)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Heartbeat in function is only "
                      "%.1f seconds! You'll use too much CPU "
                      "(use minimum 3 seconds).",
                      UNIT_IDENT(u),
                      (float)fptr->heart_beat / (float)PULSE_SEC);
            fptr->heart_beat = PULSE_SEC * 10;
         }
         else if(fptr->heart_beat > 60000)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Heartbeat in function is "
                      "%.1f seconds! That's probably an error?",
                      UNIT_IDENT(u),
                      (float)fptr->heart_beat / (float)PULSE_SEC);
            fptr->heart_beat = PULSE_SEC * 60 * 60;
         }
      }
   }
}

void check_namelist(struct unit_data *unit, class cNamelist *nl)
{
   char buf[128];

   if(nl != nullptr)
   {
      uint32_t i;
      int      len;

      for(i = 0; i < nl->Length(); i++)
      {
         if(strlen(nl->Name(i)) > 60)
         {
            dmc_error(static_cast<int>(TRUE), "Name '%s' too long (>60) in unit %s@", nl->Name(i), UNIT_IDENT(unit));
            exit(1);
         }
         strcpy(buf, skip_spaces(nl->Name(i)));
         str_remspc(buf);
         strip_trailing_spaces(buf);
         nl->Substitute(i, buf);
      }

      for(i = 0; i < nl->Length(); i++)
      {
         if((str_nccmp("self ", nl->Name(i), 5) == 0) || (str_ccmp("self", nl->Name(i)) == 0))
         {
            dmc_error(static_cast<int>(TRUE),
                      "Reserved word 'self' used in start of "
                      "name in %s@",
                      UNIT_IDENT(unit));
         }

         len = strlen(nl->Name(i));

         if(nl->Name(i)[len - 1] == 's' && nl->Name(i)[len - 2] == '\'')
         {
            dmc_error(static_cast<int>(TRUE), "%s@: name [%s] not allowed to end in 's", UNIT_IDENT(unit), nl->Name(i));
         }
      }

      class cNamelist tmp;

      if(nl->Length() > 1)
      {
         tmp.AppendName(nl->Name(0));

         for(i = 1; i < nl->Length(); i++)
         {
            if(tmp.IsNameRaw(nl->Name(i)) != nullptr)
            {
               dmc_error(static_cast<int>(TRUE),
                         "Name order error (or matching names) "
                         "for '%s' in %s@",
                         nl->Name(i),
                         UNIT_IDENT(unit));
            }
            tmp.AppendName(nl->Name(i));
         }
      }
   }
}

void process_unit(struct unit_data *u)
{
   int               i;
   extra_descr_data *exd;

   extern int        verbose;

   process_affects(u);
   process_funcs(u);

   check_namelist(u, &UNIT_NAMES(u));

   for(exd = UNIT_EXTRA_DESCR(u); exd != nullptr; exd = exd->next)
   {
      check_namelist(u, &exd->names);
   }

   if(IS_ROOM(u))
   {
      for(i = 0; i < 6; i++)
      {
         if(ROOM_EXIT(u, i) != nullptr)
         {
            check_namelist(u, &ROOM_EXIT(u, i)->open_name);
         }
      }
   }

   if(!IS_ROOM(u) && UNIT_TITLE(u).String())
   {
      if(isupper(*UNIT_TITLE(u).String()))
      {
         char buf[MAX_STRING_LENGTH];
         str_next_word_copy(UNIT_TITLE(u).String(), buf);
         if(fill_word(buf) != 0)
         {
            dmc_error(FALSE, "%s: Title CASE seems to be wrong for '%s'", UNIT_IDENT(u), UNIT_TITLE(u).String());
         }
      }

      i = strlen(UNIT_TITLE(u).String());

      if((i > 0) && !isalpha(UNIT_TITLE(u).String()[i - 1]))
      {
         dmc_error(FALSE, "%s: Title ends with non-alphabet character '%s'", UNIT_IDENT(u), UNIT_TITLE(u).String());
      }
   }

   if(is_in(UNIT_WEIGHT(u), 0, 2000) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "%s: Illegal weight %d (expected 0..2000).", UNIT_IDENT(u), UNIT_WEIGHT(u));
      UNIT_WEIGHT(u) = 0;
   }
   UNIT_WEIGHT(u) = UNIT_BASE_WEIGHT(u);

   if(is_in(UNIT_ALIGNMENT(u), -1000, +1000) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "%s: Illegal alignment %d (expected -1000..+1000).", UNIT_IDENT(u), UNIT_ALIGNMENT(u));
      UNIT_ALIGNMENT(u) = 0;
   }

   if(is_in(UNIT_LIGHTS(u), -6, 6) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "%s: Illegal light %d (expected -6..+6).", UNIT_IDENT(u), UNIT_LIGHTS(u));
      UNIT_LIGHTS(u) = 0;
   }

   if(is_in(UNIT_BRIGHT(u), -6, 6) == 0)
   {
      dmc_error(static_cast<int>(TRUE), "%s: Illegal bright %d (expected -6..+6).", UNIT_IDENT(u), UNIT_BRIGHT(u));
      UNIT_BRIGHT(u) = 0;
   }

   switch(UNIT_TYPE(u))
   {
      case UNIT_ST_ROOM:
         set_room_data(u);
         break;

      case UNIT_ST_OBJ:
         switch(OBJ_TYPE(u))
         {
            case ITEM_MONEY:
               OBJ_PRICE(u)    = OBJ_VALUE(u, 0);
               OBJ_VALUE(u, 0) = 0;

               if(UNIT_TITLE(u).StringPtr())
               {
                  int i;

                  for(i = 0; i <= MAX_MONEY; i++)
                  {
                     if(!strcmp(UNIT_TITLE(u).String(), money_types[i].abbrev))
                     {
                        break;
                     }
                  }

                  if(i > MAX_MONEY)
                  {
                     dmc_error(TRUE, "Not a legal money denominator (%s) on %s", UNIT_TITLE(u).String(), UNIT_IDENT(u));
                  }
                  else
                  {
                     OBJ_VALUE(u, 0) = i;
                  }
               }
               else
               {
                  dmc_error(static_cast<int>(TRUE), "No money denominator on %s", UNIT_IDENT(u));
               }
               break;

            case ITEM_DRINKCON:
               if(is_in(OBJ_VALUE(u, 2), 0, LIQ_MAX) == 0)
               {
                  dmc_error(static_cast<int>(TRUE), "Illegal drinkcontainer liquid type in '%s'.", UNIT_IDENT(u));
                  OBJ_TYPE(u) = ITEM_TRASH;
                  return;
               }

               if(UNIT_NAMES(u).Length() < 2)
               {
                  dmc_error(2,
                            "Drinkcontainer '%s' has less than two names. "
                            "Last name must be the liquid-type name (%s).",
                            UNIT_IDENT(u),
                            drinks[OBJ_VALUE(u, 2)]);
               }
               else
               {
                  if((OBJ_VALUE(u, 1) != 0) && (str_ccmp(UNIT_NAMES(u).Name(UNIT_NAMES(u).Length() - 1), drinks[OBJ_VALUE(u, 2)]) != 0))
                  {
                     dmc_error(2,
                               "Drinkcontainer '%s' has last name "
                               "'%s' which is different from expected "
                               "liquid-type name '%s'.",
                               UNIT_IDENT(u),
                               UNIT_NAMES(u).Name(UNIT_NAMES(u).Length() - 1),
                               drinks[OBJ_VALUE(u, 2)]);
                  }
               }
               break;
            case ITEM_WEAPON:
               dmc_set_weapon(u);
               break;
            case ITEM_ARMOR:
               dmc_set_armour(u);
               break;
            case ITEM_SHIELD:
               dmc_set_shield(u);
               break;
         }
         break;

      case UNIT_ST_NPC:
         set_points(u);
         CHAR_MANA(u)      = 100;
         CHAR_ENDURANCE(u) = 100;

         if(is_in(CHAR_SPEED(u), SPEED_MIN, SPEED_MAX) == 0)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Illegal Combat Speed %d - expected [%d..%d].",
                      UNIT_IDENT(u),
                      CHAR_SPEED(u),
                      SPEED_MIN,
                      SPEED_MAX);
         }

         if(CHAR_EXP(u) >= 500)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Illegal experience bonus %d: substantially above "
                      " the 100 default!",
                      UNIT_IDENT(u),
                      CHAR_EXP(u));
            CHAR_EXP(u) = 100;
         }
         else if(CHAR_EXP(u) < -500)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Illegal experience penalty %d is less than "
                      "-500 XP.",
                      UNIT_IDENT(u),
                      CHAR_EXP(u));
            CHAR_EXP(u) = -500;
         }

         if(is_in(CHAR_OFFENSIVE(u), -1000, 1000) == 0)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Illegal offensive bonus %d%%"
                      " (expected -1000 .. +1000).",
                      UNIT_IDENT(u),
                      CHAR_OFFENSIVE(u));
         }

         if(is_in(CHAR_DEFENSIVE(u), -1000, 1000) == 0)
         {
            dmc_error(static_cast<int>(TRUE),
                      "%s: Illegal defensive bonus %d%%"
                      " (expected -1000 .. +1000).",
                      UNIT_IDENT(u),
                      CHAR_DEFENSIVE(u));
         }

         if(legal_amount(u) < (unsigned long)convert_money(CHAR_MONEY(u)))
         {
            char buf[512];
            sprintf(buf, "%s", money_string(legal_amount(u), local_currency(u), TRUE));

            dmc_error(static_cast<int>(FALSE),
                      "Too much money on %s.  (%s vs %s)",
                      UNIT_IDENT(u),
                      money_string(convert_money(CHAR_MONEY(u)), local_currency(u), TRUE),
                      buf);

            /* Eventually (?):
            free(CHAR_MONEY(u));
            CHAR_MONEY(u) = str_dup(convert_back_money(legal_amount(u)));
            */
         }

         if(verbose != 0)
         {
            show_info(u);
         }
         break;
   }
}

void init_unit(struct unit_data *u)
{
   int i;

   u->next             = nullptr;
   UNIT_EXTRA_DESCR(u) = nullptr;

   UNIT_KEY(u)         = nullptr;
   UNIT_MANIPULATE(u)  = 0;
   UNIT_FLAGS(u)       = 0;
   UNIT_BASE_WEIGHT(u) = 1;
   UNIT_CAPACITY(u)    = 0;
   UNIT_HIT(u) = UNIT_MAX_HIT(u) = 100;
   UNIT_ALIGNMENT(u)             = 0;
   UNIT_OPEN_FLAGS(u)            = 0;
   UNIT_LIGHTS(u)                = 0;
   UNIT_BRIGHT(u)                = 0;
   UNIT_CHARS(u)                 = 0;
   UNIT_AFFECTED(u)              = nullptr;
   UNIT_SIZE(u)                  = 180; /* 180cm default */

   switch(UNIT_TYPE(u))
   {
      case UNIT_ST_NPC:
         UNIT_BASE_WEIGHT(u) = UNIT_WEIGHT(u) = 120; /* lbs default */
         CHAR_MONEY(u)                        = nullptr;
         CHAR_EXP(u)                          = 100; /* 100 XP per default at your own level */
         CHAR_FLAGS(u)                        = 0;
         CHAR_ATTACK_TYPE(u)                  = WPN_FIST;
         CHAR_NATURAL_ARMOUR(u)               = ARM_HLEATHER;
         CHAR_SPEED(u)                        = 12;
         CHAR_RACE(u)                         = RACE_HUMAN;
         CHAR_SEX(u)                          = SEX_NEUTRAL;
         CHAR_LEVEL(u)                        = 1;
         CHAR_POS(u)                          = POSITION_STANDING;
         NPC_DEFAULT(u)                       = POSITION_STANDING;
         for(i = 0; i < ABIL_TREE_MAX; i++)
         {
            CHAR_ABILITY(u, i) = 0;
         }
         CHAR_STR(u) = 40; /* % */
         CHAR_DEX(u) = 30;
         CHAR_CON(u) = 10;
         CHAR_CHA(u) = 2;
         CHAR_BRA(u) = 3;
         CHAR_HPP(u) = 15;

         for(i = 0; i < WPN_GROUP_MAX; i++)
         {
            NPC_WPN_SKILL(u, i) = 0;
         }
         NPC_WPN_SKILL(u, WPN_UNARMED) = 100; /* % */

         for(i = 0; i < SPL_GROUP_MAX; i++)
         {
            NPC_SPL_SKILL(u, i) = 0;
         }
         NPC_FLAGS(u)      = 0;
         CHAR_OFFENSIVE(u) = 0;
         CHAR_DEFENSIVE(u) = 0;

         break;

      case UNIT_ST_OBJ:
         for(i = 0; i <= 3; i++)
         {
            OBJ_VALUE(u, i) = 0;
         }
         OBJ_FLAGS(u)      = 0;
         OBJ_PRICE(u)      = 0;
         OBJ_PRICE_DAY(u)  = 0;
         OBJ_TYPE(u)       = ITEM_OTHER;
         OBJ_RESISTANCE(u) = 0;
         break;

      case UNIT_ST_ROOM:
         UNIT_CAPACITY(u)    = 30000;
         UNIT_BASE_WEIGHT(u) = UNIT_WEIGHT(u) = 10;
         UNIT_IN(u)                           = nullptr;
         for(i = 0; i <= 5; i++)
         {
            ROOM_EXIT(u, i) = nullptr;
         }
         ROOM_FLAGS(u)      = 0;
         ROOM_LANDSCAPE(u)  = SECT_CITY;
         ROOM_RESISTANCE(u) = 0;
         break;
   }
}
