/* *********************************************************************** *
 * File   : skills.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for skills.c                                            *
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

#ifndef _MUD_SKILLS_H
#define _MUD_SKILLS_H

#include "essential.h"
#include "values.h"

#include <memory>

struct skill_interval
{
   const int   skill;
   const char *descr;
};

const char *skill_text(const struct skill_interval *si, int skill);

/* ---------------- COMBAT MESSAGE SYSTEM -------------------- */

#define COM_MAX_MSGS 60

#define COM_MSG_DEAD  -1
#define COM_MSG_MISS  -2
#define COM_MSG_NODAM -3
#define COM_MSG_EBODY -4

/* ---------------- RACES -------------------- */

#define RACE_IS_HUMANOID(race) ((race) <= RACE_OTHER_HUMANOID)

#define CHAR_IS_MAMMAL(ch) RACE_IS_MAMMAL(CHAR_RACE(ch))

#define RACE_IS_MAMMAL(race) (((race) > RACE_OTHER_HUMANOID) && ((race) <= RACE_OTHER_MAMMAL))

#define RACE_IS_UNDEAD(race) (((race) > RACE_OTHER_CREATURE) && ((race) <= RACE_OTHER_UNDEAD))

#define CHAR_IS_HUMANOID(ch) RACE_IS_HUMANOID(CHAR_RACE(ch))

#define CHAR_IS_UNDEAD(ch) RACE_IS_UNDEAD(CHAR_RACE(ch))

struct dice_type
{
   ubit16 reps;
   ubit16 size;
};

struct base_race_info_type
{
   ubit16           height;
   struct dice_type height_dice;

   ubit16           weight;
   struct dice_type weight_dice;

   ubit16           lifespan;
   struct dice_type lifespan_dice;
};

struct race_info_type
{
   struct base_race_info_type male;
   struct base_race_info_type female;

   ubit16           age;
   struct dice_type age_dice;
};

struct damage_chart_element_type
{
   int offset;  /* When does damage start         */
   int basedam; /* The damage given at 'offset'   */
   int alpha;   /* Step size of damage as 1/alpha */
};

struct damage_chart_type
{
   int fumble; /* from 01 - fuble => fumble      */

   struct damage_chart_element_type element[5];
};

#define TREE_PARENT(tree, node)      (tree[node].parent)
#define TREE_GRANDPARENT(tree, node) (tree[tree[node].parent].parent)
#define TREE_ISROOT(tree, node)      ((node) == (tree[node].parent))
#define TREE_ISLEAF(tree, node)      (tree[node].isleaf)

struct tree_type
{
   int   parent;
   ubit8 isleaf;
};

/* Tree has a pointer to parent for each node. 0 pointer from root */
struct wpn_info_type
{
   int   hands;  /* 0=N/A, 1 = 1, 2 = 1.5, 3 = 2          */
   int   speed;  /* Speed modification by weapon 0..      */
   int   type;   /* Is the weapon slashing/piercing...    */
   ubit8 shield; /* Shield method SHIELD_M_XXX            */
};

#define DEMIGOD_LEVEL_XP (40000000)

class unit_data;
int        object_two_handed(std::shared_ptr<unit_data> obj);
void       roll_description(std::shared_ptr<unit_data> att, const char *text, int roll);
int        open_ended_roll(int size, int end);
inline int open100(void)
{
   return open_ended_roll(100, 5);
}

int resistance_skill_check(int att_skill1, int def_skill1, int att_skill2, int def_skill2);
int resistance_level_check(int att_level, int def_level, int att_skill, int def_skill);
int skill_duration(int howmuch);

int weapon_fumble(std::shared_ptr<unit_data> weapon, int roll);
int chart_damage(int roll, struct damage_chart_element_type *element);
int chart_size_damage(int roll, struct damage_chart_element_type *element, int lbs);
int weapon_damage(int roll, int weapon_type, int armour_type);
int natural_damage(int roll, int weapon_type, int armour_type, int lbs);

int basic_char_tgh_absorb(std::shared_ptr<unit_data> ch);
int basic_armor_absorb(std::shared_ptr<unit_data> armour, int att_type);
int basic_char_absorb(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> armor, int att_type);

int basic_char_weapon_dam(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> weapon);
int basic_char_hand_dam(std::shared_ptr<unit_data> ch);
int char_weapon_dam(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> weapon);
int char_hand_dam(std::shared_ptr<unit_data> ch);

int relative_level(int l1, int l2);
int weapon_defense_skill(std::shared_ptr<unit_data> ch, int skill);
int weapon_attack_skill(std::shared_ptr<unit_data> ch, int skill);
int hit_location(std::shared_ptr<unit_data> att, std::shared_ptr<unit_data> def);
int effective_dex(std::shared_ptr<unit_data> ch);

int  av_value(int abila, int abilb, int skilla, int skillb);
int  av_howmuch(int av);
int  av_makes(int av);
void check_fitting(std::shared_ptr<unit_data> u);

#endif /* _MUD_SKILLS_H */
