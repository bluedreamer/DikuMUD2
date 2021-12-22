/* *********************************************************************** *
 * File   : fight.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Fight include file                                             *
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

#ifndef _MUD_FIGHT_H
#define _MUD_FIGHT_H

#include "essential.h"

struct SFightColorSet
{
   char *pAttacker;
   char *pDefender;
   char *pOthers;
};

class unit_data *raw_kill(std::shared_ptr<unit_data> ch);

int provoked_attack(class unit_data *victim, class unit_data *ch);

void update_pos(std::shared_ptr<unit_data> victim);

void damage(std::shared_ptr<unit_data> ch,
            std::shared_ptr<unit_data> victim,
            std::shared_ptr<unit_data> medium,
            int               damage,
            int               attackcat,
            int               weapontype,
            int               hitloc,
            int               bDisplay = TRUE);

int  pk_test(std::shared_ptr<unit_data> att, std::shared_ptr<unit_data> def, int message);
int  one_hit(std::shared_ptr<unit_data> att, std::shared_ptr<unit_data> def, int bonus, int wpn_type, int primary = TRUE);
int  simple_one_hit(std::shared_ptr<unit_data> att, std::shared_ptr<unit_data> def);
int  char_dual_wield(std::shared_ptr<unit_data> ch);
void melee_violence(std::shared_ptr<unit_data> ch, int primary);
int  melee_bonus(std::shared_ptr<unit_data> att,
                 std::shared_ptr<unit_data> def,
                 int                hit_loc,
                 int               *pAtt_weapon_type,
                 std::shared_ptr<unit_data> *pAtt_weapon,
                 int               *pDef_armour_type,
                 std::shared_ptr<unit_data> *pDef_armour,
                 int                primary = TRUE);
int  shield_bonus(std::shared_ptr<unit_data> att, std::shared_ptr<unit_data> def, std::shared_ptr<unit_data> *pDef_shield);
#endif
