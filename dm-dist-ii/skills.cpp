#include "skills.h"

#include "comm.h"
#include "common.h"
#include "damage_chart_type.h"
#include "db.h"
#include "files.h"
#include "interpreter.h"
#include "race_info_type.h"
#include "spelldef.h"
#include "textutil.h"
#include "tree_type.h"
#include "utility.h"
#include "utils.h"
#include "wpn_info_type.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>

auto skill_text(const struct skill_interval *si, int skill) -> const char *
{
   if(si != nullptr)
   {
      while(si->descr != nullptr)
      {
         if((skill <= si->skill) || ((si + 1)->descr == nullptr))
         {
            return si->descr;
         }

         si++;
      }
   }

   return "UNDEFINED";
}

int hit_location_table[] = /* Determine by using 2d8 */
   {WEAR_HEAD,             /* 2..3   =>  4.68% for Head  */
    WEAR_HEAD,
    WEAR_HANDS, /* 4      =>  4.69% for Hands */
    WEAR_ARMS,  /* 5..7   => 23.43% for Arms  */
    WEAR_ARMS,
    WEAR_ARMS,
    WEAR_BODY, /* 8..10  => 34.38% for Body  */
    WEAR_BODY,
    WEAR_BODY,
    WEAR_LEGS, /* 11..14 => 28.12% for legs  */
    WEAR_LEGS,
    WEAR_LEGS,
    WEAR_LEGS,
    WEAR_FEET, /* 15..16 => 4.68% for feet   */
    WEAR_FEET};

const char *spl_text[SPL_TREE_MAX + 1];
int8_t      racial_spells[SPL_TREE_MAX][PC_RACE_MAX];

const char *pc_races[PC_RACE_MAX + 1];
const char *pc_race_adverbs[PC_RACE_MAX + 1];

const char *ski_text[SKI_TREE_MAX + 1];
int8_t      racial_skills[SKI_TREE_MAX][PC_RACE_MAX];

const char *abil_text[ABIL_TREE_MAX + 1];
int8_t      racial_ability[ABIL_TREE_MAX][PC_RACE_MAX];

const char *wpn_text[WPN_TREE_MAX + 1];
int8_t      racial_weapons[WPN_TREE_MAX][PC_RACE_MAX];

/* ===================================================================== */

void        roll_description(unit_data *att, const char *text, int roll)
{
   if(roll >= 200)
   {
      switch(roll / 100)
      {
         case 2:
            act("Great $2t!", A_ALWAYS, att, text, nullptr, TO_CHAR);
            act("$1n makes a great $2t!", A_ALWAYS, att, text, nullptr, TO_ROOM);
            break;

         case 3:
            act("Superb $2t!", A_ALWAYS, att, text, nullptr, TO_CHAR);
            act("$1n makes a superb $2t!", A_ALWAYS, att, text, nullptr, TO_ROOM);
            break;

         default:
            act("Divine $2t!", A_ALWAYS, att, text, nullptr, TO_CHAR);
            act("$1n makes a divinely inspired $2t!", A_ALWAYS, att, text, nullptr, TO_ROOM);
            break;
      }
   }
   else if(roll <= -100)
   {
      act("You fumble!", A_ALWAYS, att, text, nullptr, TO_CHAR);
      act("$1n fumbles!", A_ALWAYS, att, text, nullptr, TO_ROOM);
   }
}

auto open_ended_roll(int size, int end) -> int
{
   int i;
   int s;

   s = i = number(1, size);

   if(i >= size - end + 1)
   {
      do
      {
         s += (i = number(1, size));
      } while(i >= size - end - 1);
   }
   else if(i <= end)
   {
      do
      {
         s -= (i = number(1, size));
      } while(i >= size - end - 1);
   }

   return s;
}

/* Assuming howmuch is > 0 then return the duration number of ticks */
/* at 30 seconds beat. "100" is perfect spell & gives 5 minutes.    */
/* Used on for example, hide, bless, raise str, etc.                */
auto skill_duration(int howmuch) -> int
{
   return std::max(2, howmuch / 10);
}

/* "Defender" must resist against attack from attacker.    */
/* I.e. attacker tries to steal from defender, or backstab */
/* or first aid, etc.                                      */
/* A result >= 0 means "resisted successfully by n.        */
/* A result < 0 means "failed resistance by n".            */
auto resistance_skill_check(int att_skill1, int def_skill1, int att_skill2, int def_skill2) -> int
{
   return open100() + att_skill1 + att_skill2 - def_skill1 - def_skill2 - 50;
}

auto weight_size(int lbs) -> int
{
   if(lbs <= 5)
   {
      return SIZ_TINY;
   }
   if(lbs <= 40)
   {
      return SIZ_SMALL;
   }
   if(lbs <= 160)
      return SIZ_MEDIUM;
   else if(lbs <= 500)
      return SIZ_LARGE;
   else
      return SIZ_HUGE;
}

auto weapon_fumble(unit_data *weapon, int roll) -> int
{
   assert(IS_OBJ(weapon) && (OBJ_TYPE(weapon) == ITEM_WEAPON));

   return static_cast<int>(roll <= weapon_chart[OBJ_VALUE(weapon, 0)].fumble);
}

auto object_two_handed(unit_data *obj) -> int
{
   if(OBJ_TYPE(obj) == ITEM_WEAPON)
   {
      if(wpn_info[OBJ_VALUE(obj, 0)].hands == 2)
      {
         return static_cast<int>(TRUE);
      }
   }

   if(IS_SET(OBJ_FLAGS(obj), OBJ_TWO_HANDS))
   {
      return static_cast<int>(TRUE);
   }

   return static_cast<int>(FALSE);
}

auto chart_damage(int roll, damage_chart_element_type *element) -> int
{
   if(element->alpha == 0)
   {
      slog(LOG_ALL, 0, "Error: Damage chart alpha is 0!");
      return element->basedam;
   }

   if(roll < element->offset)
   {
      return 0;
   }
   return element->basedam + ((roll - element->offset) / element->alpha);
}

/* Size is for natural attacks to limit max damage for such */
auto chart_size_damage(int roll, damage_chart_element_type *element, int lbs) -> int
{
   if(element->alpha == 0)
   {
      slog(LOG_ALL, 0, "Error: Damage chart alpha is 0!");
      return element->basedam;
   }

   if(roll < element->offset)
   {
      return 0;
   }
   switch(weight_size(lbs))
   {
      case SIZ_TINY:
         roll = std::min(100, roll);
         break;

      case SIZ_SMALL:
         roll = std::min(110, roll);
         break;

      case SIZ_MEDIUM:
         roll = std::min(130, roll);
         break;

      case SIZ_LARGE:
         roll = std::min(150, roll);
         break;
   }

   return element->basedam + ((roll - element->offset) / element->alpha);
}

auto weapon_damage(int roll, int weapon_type, int armour_type) -> int
{
   if(is_in(weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX) == 0)
   {
      slog(LOG_ALL, 0, "Illegal weapon type.");
      return 0;
   }

   if(is_in(armour_type, ARM_CLOTHES, ARM_PLATE) == 0)
   {
      slog(LOG_ALL, 0, "Illegal armour type.");
      armour_type = ARM_CLOTHES;
   }

   return chart_damage(roll, &(weapon_chart[weapon_type].element[armour_type]));
}

auto natural_damage(int roll, int weapon_type, int armour_type, int lbs) -> int
{
   if(is_in(weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX) == 0)
   {
      slog(LOG_ALL, 0, "Illegal weapon type.");
      return 0;
   }

   if(is_in(armour_type, ARM_CLOTHES, ARM_PLATE) == 0)
   {
      slog(LOG_ALL, 0, "Illegal armour type.");
      armour_type = ARM_CLOTHES;
   }

   return chart_size_damage(roll, &(weapon_chart[weapon_type].element[armour_type]), lbs);
}

/* Return [0..200] for skill when defending with a weapon */
auto weapon_defense_skill(unit_data *ch, int skill) -> int
{
   int max;

   if(IS_PC(ch))
   {
      if(TREE_ISLEAF(wpn_tree, skill))
      {
         max = PC_WPN_SKILL(ch, skill) / 2;
      }
      else
      {
         max = PC_WPN_SKILL(ch, skill);
      }

      while(!TREE_ISROOT(wpn_tree, skill))
      {
         skill = TREE_PARENT(wpn_tree, skill);

         if(PC_WPN_SKILL(ch, skill) > max)
         {
            max = PC_WPN_SKILL(ch, skill);
         }
      }

      return max;
   }
   /* a NPC */
   if(TREE_ISLEAF(wpn_tree, skill))
   {
      skill = TREE_PARENT(wpn_tree, skill);
   }

   if(TREE_ISROOT(wpn_tree, skill))
   {
      max = NPC_WPN_SKILL(ch, skill);
   }
   else
   {
      max = NPC_WPN_SKILL(ch, skill) / 2;
   }

   while(!TREE_ISROOT(wpn_tree, skill))
   {
      skill = TREE_PARENT(wpn_tree, skill);

      if(NPC_WPN_SKILL(ch, skill) > max)
      {
         max = NPC_WPN_SKILL(ch, skill);
      }
   }

   return max;
}

/* Return [0..200] for skill when attacking with a weapon */
auto weapon_attack_skill(unit_data *ch, int skill) -> int
{
   if(IS_PC(ch))
   {
      return PC_WPN_SKILL(ch, skill) == 0 ? -50 : PC_WPN_SKILL(ch, skill);
   }

   if(TREE_ISLEAF(wpn_tree, skill))
   {
      skill = TREE_PARENT(wpn_tree, skill);
   }

   return NPC_WPN_SKILL(ch, skill);
}

/* Return the armour position of where one person hits another */
auto hit_location(unit_data *att, unit_data *def) -> int
{
   /* Maybe do height reductions later */

   return hit_location_table[dice(2, 8) - 2];
}

/* Return the effective dex of a person in armour ...             */
/* Later we will redo this function - as of now it doesn't matter */
/* what armour you wear                                           */
auto effective_dex(unit_data *ch) -> int
{
   return CHAR_DEX(ch);
}

/* ========================================================================= */

static void race_read()
{
   int   dummy;
   int   idx = -1;
   char  pTmp[256];
   char *pCh;
   FILE *fl;
   char  tmp[256];

   touch_file(str_cc(libdir, RACE_DEFS));
   if((fl = fopen(str_cc(libdir, RACE_DEFS), "rb")) == nullptr)
   {
      slog(LOG_ALL, 0, "unable to create lib/" RACE_DEFS);
      exit(0);
   }

   while(feof(fl) == 0)
   {
      char *ms2020 = fgets(pTmp, sizeof(pTmp) - 1, fl);
      str_remspc(pTmp);

      if((pCh = strchr(pTmp, '=')) != nullptr)
      {
         *pCh = 0;
         pCh  = skip_blanks(pCh + 1);
         strip_trailing_blanks(pCh);
         str_lower(pTmp);
      }

      if(pCh == nullptr || (static_cast<unsigned int>(str_is_empty(pCh)) != 0U))
      {
         continue;
      }

      if(strncmp(pTmp, "index", 5) == 0)
      {
         idx = atoi(pCh);
         if((static_cast<unsigned int>(str_is_number(pCh)) == 0U) || (is_in(idx, 0, PC_RACE_MAX - 1) == 0))
         {
            slog(LOG_ALL, 0, "Race boot error: %s", pCh);
            idx = -1;
         }
         continue;
      }

      if(idx == -1)
      {
         continue;
      }

      if(strncmp(pTmp, "name", 4) == 0)
      {
         if(pc_races[idx] != nullptr)
         {
            free(pc_races[idx]);
         }
         pc_races[idx] = str_dup(pCh);
      }
      else if(strncmp(pTmp, "adverb", 6) == 0)
      {
         if(pc_race_adverbs[idx] != nullptr)
         {
            free(pc_race_adverbs[idx]);
         }
         pc_race_adverbs[idx] = str_dup(pCh);
      }
      else if(strncmp(pTmp, "height male", 11) == 0)
      {
         pCh                                  = str_next_word(pCh, tmp);
         race_info[idx].male.height           = atoi(tmp);

         pCh                                  = str_next_word(pCh, tmp);
         race_info[idx].male.height_dice.reps = atoi(tmp);

         pCh                                  = str_next_word(pCh, tmp);
         race_info[idx].male.height_dice.size = atoi(tmp);
      }
      else if(strncmp(pTmp, "height female", 13) == 0)
      {
         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].female.height           = atoi(tmp);

         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].female.height_dice.reps = atoi(tmp);

         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].female.height_dice.size = atoi(tmp);
      }
      else if(strncmp(pTmp, "weight male", 11) == 0)
      {
         pCh                                  = str_next_word(pCh, tmp);
         race_info[idx].male.weight           = atoi(tmp);

         pCh                                  = str_next_word(pCh, tmp);
         race_info[idx].male.weight_dice.reps = atoi(tmp);

         pCh                                  = str_next_word(pCh, tmp);
         race_info[idx].male.weight_dice.size = atoi(tmp);
      }
      else if(strncmp(pTmp, "weight female", 13) == 0)
      {
         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].female.weight           = atoi(tmp);

         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].female.weight_dice.reps = atoi(tmp);

         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].female.weight_dice.size = atoi(tmp);
      }
      else if(strncmp(pTmp, "lifespan male", 13) == 0)
      {
         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].male.lifespan           = atoi(tmp);

         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].male.lifespan_dice.reps = atoi(tmp);

         pCh                                    = str_next_word(pCh, tmp);
         race_info[idx].male.lifespan_dice.size = atoi(tmp);
      }
      else if(strncmp(pTmp, "lifespan female", 15) == 0)
      {
         pCh                                      = str_next_word(pCh, tmp);
         race_info[idx].female.lifespan           = atoi(tmp);

         pCh                                      = str_next_word(pCh, tmp);
         race_info[idx].female.lifespan_dice.reps = atoi(tmp);

         pCh                                      = str_next_word(pCh, tmp);
         race_info[idx].female.lifespan_dice.size = atoi(tmp);
      }
      else if(strncmp(pTmp, "age", 3) == 0)
      {
         pCh                          = str_next_word(pCh, tmp);
         race_info[idx].age           = atoi(tmp);

         pCh                          = str_next_word(pCh, tmp);
         race_info[idx].age_dice.reps = atoi(tmp);

         pCh                          = str_next_word(pCh, tmp);
         race_info[idx].age_dice.size = atoi(tmp);
      }
      else
      {
         slog(LOG_ALL, 0, "Race boot unknown string: %s", pTmp);
      }
   }

   fclose(fl);
}

diltemplate *playerinit_tmpl;
diltemplate *nanny_dil_tmpl;

static void  race_init()
{
   int i;

   for(i = 0; i < PC_RACE_MAX; i++)
   {
      memset(&race_info[i], 0, sizeof(struct race_info_type));

      pc_races[i]        = nullptr;
      pc_race_adverbs[i] = nullptr;
   }

   pc_races[PC_RACE_MAX]        = nullptr;
   pc_race_adverbs[PC_RACE_MAX] = nullptr;

   playerinit_tmpl              = find_dil_template("playerinit@basis");
   if(playerinit_tmpl == nullptr)
   {
      slog(LOG_ALL, 0, "No 'playerinit@basis' DIL template.");
   }
   else
   {
      if(playerinit_tmpl->argc != 0)
      {
         slog(LOG_ALL, 0, "playerinit@basis(); not defined correctly.");
         playerinit_tmpl = nullptr;
      }
   }

   nanny_dil_tmpl = find_dil_template("nanny@basis");
   if(nanny_dil_tmpl == nullptr)
   {
      slog(LOG_ALL, 0, "No 'nanny@basis' DIL template.");
   }
   else
   {
      if((nanny_dil_tmpl->argc != 1) || (nanny_dil_tmpl->argt[0] != DILV_SP))
      {
         slog(LOG_ALL, 0, "nanny@basis(string); not defined correctly.");
         nanny_dil_tmpl = nullptr;
      }
   }
}

void boot_race()
{
   race_init();
   race_read();
}

/* ========================================================================= */

static void ability_read()
{
   int   dummy;
   int   idx = -1;
   char  pTmp[256];
   char *pCh;
   FILE *fl;

   touch_file(str_cc(libdir, ABILITY_DEFS));
   if((fl = fopen(str_cc(libdir, ABILITY_DEFS), "rb")) == nullptr)
   {
      slog(LOG_ALL, 0, "unable to create lib/" ABILITY_DEFS);
      exit(0);
   }

   while(feof(fl) == 0)
   {
      char *ms2020 = fgets(pTmp, sizeof(pTmp) - 1, fl);
      str_remspc(pTmp);

      if((pCh = strchr(pTmp, '=')) != nullptr)
      {
         *pCh = 0;
         pCh  = skip_blanks(pCh + 1);
         strip_trailing_blanks(pCh);
      }

      str_lower(pTmp);
      strip_trailing_blanks(pTmp);

      if(pCh == nullptr || (static_cast<unsigned int>(str_is_empty(pCh)) != 0U))
      {
         continue;
      }

      if(strncmp(pTmp, "index", 5) == 0)
      {
         idx = atoi(pCh);
         if((static_cast<unsigned int>(str_is_number(pCh)) == 0U) || (is_in(idx, 0, ABIL_TREE_MAX - 1) == 0))
         {
            slog(LOG_ALL, 0, "Ability boot error: %s", pCh);
            idx = -1;
         }
         continue;
      }

      if(idx == -1)
      {
         continue;
      }

      if(strncmp(pTmp, "name", 4) == 0)
      {
         if(abil_text[idx] != nullptr)
         {
            free(abil_text[idx]);
         }
         abil_text[idx] = str_dup(pCh);
      }
      else if(strncmp(pTmp, "race ", 5) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, -3, +3) == 0)
         {
            continue;
         }

         int ridx = search_block(pTmp + 5, pc_races, TRUE);

         if(ridx == -1)
         {
            slog(LOG_ALL, 0, "Abilities: Illegal race in: %s", pTmp);
         }
         else
         {
            racial_ability[idx][ridx] = dummy;
         }
      }
      else
      {
         slog(LOG_ALL, 0, "Ability boot unknown string: %s", pTmp);
      }
   }

   fclose(fl);
}

static void ability_init()
{
   int i;

   for(i = 0; i < ABIL_TREE_MAX; i++)
   {
      abil_tree[i].parent = i;
      abil_tree[i].isleaf = static_cast<uint8_t>(TRUE);
      abil_text[i]        = nullptr;

      for(int j = 0; j < PC_RACE_MAX; j++)
      {
         racial_ability[i][j] = 0;
      }
   }

   abil_tree[ABIL_TREE_MAX].parent = -1;
   abil_tree[ABIL_TREE_MAX].isleaf = static_cast<uint8_t>(FALSE);
   abil_text[ABIL_TREE_MAX]        = nullptr;
}

void boot_ability()
{
   ability_init();
   ability_read();
}

/* ========================================================================= */

static void weapon_read()
{
   int   dummy;
   int   idx = -1;
   char  pTmp[256];
   char *pCh;
   FILE *fl;

   touch_file(str_cc(libdir, WEAPON_DEFS));
   if((fl = fopen(str_cc(libdir, WEAPON_DEFS), "rb")) == nullptr)
   {
      slog(LOG_ALL, 0, "unable to create lib file");
      exit(0);
   }

   while(feof(fl) == 0)
   {
      char *ms2020 = fgets(pTmp, sizeof(pTmp) - 1, fl);
      str_remspc(pTmp);

      if((pCh = strchr(pTmp, '=')) != nullptr)
      {
         *pCh = 0;
         pCh  = skip_blanks(pCh + 1);
         strip_trailing_blanks(pCh);
      }

      str_lower(pTmp);
      strip_trailing_blanks(pTmp);

      if(pCh == nullptr || (static_cast<unsigned int>(str_is_empty(pCh)) != 0U))
      {
         continue;
      }

      if(strncmp(pTmp, "index", 5) == 0)
      {
         idx = atoi(pCh);
         if((static_cast<unsigned int>(str_is_number(pCh)) == 0U) || (is_in(idx, WPN_ROOT, WPN_TREE_MAX - 1) == 0))
         {
            slog(LOG_ALL, 0, "Weapon boot error: %s", pCh);
            idx = -1;
         }
         continue;
      }

      if(idx == -1)
      {
         continue;
      }

      if(strncmp(pTmp, "name", 4) == 0)
      {
         if(wpn_text[idx] != nullptr)
         {
            free(wpn_text[idx]);
         }
         wpn_text[idx] = str_dup(pCh);
      }
      else if(strncmp(pTmp, "shield", 6) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, SHIELD_M_BLOCK, SHIELD_M_USELESS) != 0)
         {
            wpn_info[idx].shield = dummy;
         }
      }
      else if(strncmp(pTmp, "sphere", 6) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, WPN_ROOT, WPN_GROUP_MAX - 1) != 0)
         {
            wpn_tree[idx].parent = dummy;
         }
      }
      else if(strncmp(pTmp, "race ", 5) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, -3, +3) == 0)
         {
            continue;
         }

         int ridx = search_block(pTmp + 5, pc_races, TRUE);

         if(ridx == -1)
         {
            slog(LOG_ALL, 0, "Weapons: Illegal race in: %s", pTmp);
         }
         else
         {
            racial_weapons[idx][ridx] = dummy;
         }
      }
      else if(strncmp(pTmp, "fumble", 6) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, 0, 99) != 0)
         {
            weapon_chart[idx].fumble = dummy;
         }
      }
      else if(strncmp(pTmp, "hands", 5) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, 1, 2) != 0)
         {
            wpn_info[idx].hands = dummy;
         }
      }
      else if(strncmp(pTmp, "speed", 5) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, 0, 12) != 0)
         {
            wpn_info[idx].speed = dummy;
         }
      }
      else if(strncmp(pTmp, "type", 4) == 0)
      {
         dummy = atoi(pCh);
         switch(dummy)
         {
            case WPNT_SLASH:
            case WPNT_PIERCE:
            case WPNT_BLUDGEON:
               wpn_info[idx].type = dummy;
         }
      }
      else if(strncmp(pTmp, "attack ", 7) == 0)
      {
         char tmp[256];
         int  i1;
         int  i2;
         int  i3;
         int  idx2 = -1;

         pCh       = str_next_word(pCh, tmp);
         i1        = atoi(tmp);
         pCh       = str_next_word(pCh, tmp);
         i2        = atoi(tmp);
         pCh       = str_next_word(pCh, tmp);
         i3        = atoi(tmp);

         if(i3 <= 0)
         {
            slog(LOG_ALL, 0, "Weapon init %d: Illegal damage series %d %d %d.", idx, i1, i2, i3);
            continue;
         }

         if(strncmp(pTmp + 7, "clothes", 7) == 0)
         {
            idx2 = ARM_CLOTHES;
         }
         else if(strncmp(pTmp + 7, "sleather", 8) == 0)
         {
            idx2 = ARM_LEATHER;
         }
         else if(strncmp(pTmp + 7, "hleather", 8) == 0)
         {
            idx2 = ARM_HLEATHER;
         }
         else if(strncmp(pTmp + 7, "chain", 5) == 0)
         {
            idx2 = ARM_CHAIN;
         }
         else if(strncmp(pTmp + 7, "plate", 5) == 0)
         {
            idx2 = ARM_PLATE;
         }

         if(idx2 != -1)
         {
            weapon_chart[idx].element[idx2].offset  = i1;
            weapon_chart[idx].element[idx2].basedam = i2;
            weapon_chart[idx].element[idx2].alpha   = i3;
         }
      }
      else
      {
         slog(LOG_ALL, 0, "Weapon boot unknown string: %s", pTmp);
      }
   }

   fclose(fl);
}

static void weapon_init()
{
   int i;

   for(i = 0; i < WPN_TREE_MAX; i++)
   {
      weapon_chart[i].fumble = 0;
      for(auto &j : weapon_chart[i].element)
      {
         j.offset  = 100;
         j.basedam = 0;
         j.alpha   = 100;
      }

      wpn_info[i].hands  = 1;
      wpn_info[i].type   = WPNT_SLASH;
      wpn_info[i].speed  = 0;
      wpn_info[i].shield = SHIELD_M_BLOCK;

      wpn_tree[i].parent = WPN_ROOT;

      if(i < WPN_GROUP_MAX)
      {
         wpn_tree[i].isleaf = static_cast<uint8_t>(FALSE);
      }
      else
      {
         wpn_tree[i].isleaf = static_cast<uint8_t>(TRUE);
      }

      wpn_text[i] = nullptr;

      /* Racial weapons are all zero */
      for(int j = 0; j < PC_RACE_MAX; j++)
      {
         racial_weapons[i][j] = 0;
      }
   }

   wpn_tree[WPN_TREE_MAX].parent = -1;
   wpn_tree[WPN_TREE_MAX].isleaf = static_cast<uint8_t>(FALSE);
   wpn_text[WPN_TREE_MAX]        = nullptr;
}

void boot_weapon()
{
   weapon_init();
   weapon_read();
}

/* ========================================================================= */

static void skill_read()
{
   int                  dummy;
   int                  idx = -1;
   char                 pTmp[256];
   char                *pCh;
   FILE                *fl;
   struct command_info *cmdptr = nullptr;

   touch_file(str_cc(libdir, SKILL_DEFS));
   if((fl = fopen(str_cc(libdir, SKILL_DEFS), "rb")) == nullptr)
   {
      slog(LOG_ALL, 0, "unable to read lib/%s", SKILL_DEFS); // MS2020
      exit(0);
   }

   while(feof(fl) == 0)
   {
      char *ms2020 = fgets(pTmp, sizeof(pTmp) - 1, fl);
      str_remspc(pTmp);

      if((pCh = strchr(pTmp, '=')) != nullptr)
      {
         *pCh = 0;
         pCh  = skip_blanks(pCh + 1);
         strip_trailing_blanks(pCh);
      }

      str_lower(pTmp);
      strip_trailing_blanks(pTmp);

      if(pCh == nullptr || (static_cast<unsigned int>(str_is_empty(pCh)) != 0U))
      {
         continue;
      }

      if(strncmp(pTmp, "index", 5) == 0)
      {
         cmdptr = nullptr;
         idx    = atoi(pCh);
         if((static_cast<unsigned int>(str_is_number(pCh)) == 0U) || (is_in(idx, 0, SKI_TREE_MAX - 1) == 0))
         {
            slog(LOG_ALL, 0, "Skill boot error, no index: %s", pCh);
            idx = -1;
         }
         continue;
      }
      if(strncmp(pTmp, "cmdindex", 8) == 0)
      {
         int cmdidx = atoi(pCh);
         cmdptr     = nullptr;

         if(!str_is_number(pCh))
         {
            slog(LOG_ALL, 0, "Skill boot error, no cmdindex: %s", pCh);
         }
         else
         {
            extern struct command_info cmd_info[];

            for(int i = 0; *cmd_info[i].cmd_str != 0; i++) // MS2020, missing *
            {
               if(cmd_info[i].no == cmdidx)
               {
                  cmdptr = &cmd_info[i];
                  break;
               }
            }
            if(cmdptr == nullptr)
            {
               slog(LOG_ALL, 0, "No interpreter equivalent for CMD_ %d.", cmdidx);
            }
         }
         continue;
      }

      if(idx != -1)
      {
         if(strncmp(pTmp, "name", 4) == 0)
         {
            if(ski_text[idx] != nullptr)
            {
               free(ski_text[idx]);
            }
            ski_text[idx] = str_dup(pCh);
            continue;
         }
         if(strncmp(pTmp, "race ", 5) == 0)
         {
            dummy = atoi(pCh);
            if(is_in(dummy, -3, +3) == 0)
            {
               continue;
            }

            int ridx = search_block(pTmp + 5, pc_races, TRUE);

            if(ridx == -1)
            {
               slog(LOG_ALL, 0, "Skills: Illegal race in: %s", pTmp);
            }
            else
            {
               racial_skills[idx][ridx] = dummy;
            }
            continue;
         }
      }

      if(cmdptr == nullptr)
      {
         slog(LOG_ALL, 0, "cmdptr not found for skill after index %d! %s=%s", idx, pTmp, pCh);
         continue;
      }

      if(strncmp(pTmp, "command", 7) == 0)
      {
         if((static_cast<unsigned int>(str_is_empty(pCh)) == 0U) && strcmp(pCh, cmdptr->cmd_str) != 0)
         {
            cmdptr->cmd_str = str_dup(pCh);
         }
         continue;
      }
      if(strncmp(pTmp, "turns", 5) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, 1, 4 * PULSE_VIOLENCE) != 0)
         {
            cmdptr->combat_speed  = dummy;
            cmdptr->combat_buffer = static_cast<uint8_t>(TRUE);
         }
      }
      else if(strncmp(pTmp, "minpos", 6) == 0)
      {
         dummy = atoi(pCh);
         if(is_in(dummy, POSITION_DEAD, POSITION_STANDING) != 0)
         {
            cmdptr->minimum_position = dummy;
         }
      }
      else if(strncmp(pTmp, "func", 4) == 0)
      {
         if(cmdptr->tmpl != nullptr)
         {
            free(cmdptr->tmpl);
         }

         if((cmdptr->tmpl = find_dil_template(pCh)) == nullptr)
         {
            slog(LOG_ALL, 0, "No such DIL template %s.", pCh);
         }
      }
      else
      {
         slog(LOG_ALL, 0, "Skill boot unknown string: %s", pTmp);
      }
   }

   fclose(fl);
}

static void skill_init()
{
   int i;

   for(i = 0; i < SKI_TREE_MAX; i++)
   {
      ski_tree[i].parent = i;
      ski_tree[i].isleaf = static_cast<uint8_t>(TRUE);

      ski_text[i]        = nullptr;

      /* Racial skills are all zero */
      for(int j = 0; j < PC_RACE_MAX; j++)
      {
         racial_skills[i][j] = 0;
      }
   }

   ski_tree[SKI_TREE_MAX].parent = -1;
   ski_tree[SKI_TREE_MAX].isleaf = static_cast<uint8_t>(FALSE);
   ski_text[SKI_TREE_MAX]        = nullptr;
}

void boot_skill()
{
   skill_init();
   skill_read();
}
