#pragma once
/* *********************************************************************** *
 * File   : combat.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: The global combat list, perform violence and combat speed.     *
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

#include "unit_data.h"

class cCombatList
{
public:
   cCombatList();
   ~cCombatList();
   void PerformViolence();
   void add(class cCombat *pc);
   void sub(class cCombat *pc);
   void status(const unit_data *ch) const;

private:
   void Sort();

   class cCombat **pElems;
   int             nMaxTop; // No of allocated elements
   int             nTop;    // Current Max
   int             nIdx;    // Updated when in Perform() and doing Sub()
};

class cCombat
{
   friend class cCombatList;

public:
   cCombat(unit_data *owner, int bMelee = FALSE);
   ~cCombat();

   auto Opponent(int i = 0) -> unit_data *;
   auto FindOpponent(unit_data *tmp) -> unit_data *;

   inline auto Owner() -> unit_data * { return pOwner; }
   inline auto Melee() -> unit_data * { return pMelee; }
   inline auto When() const -> int { return nWhen; }
   inline auto NoOpponents() const -> int { return nNoOpponents; }

   void changeSpeed(int delta);
   void setMelee(unit_data *victim);
   void setCommand(char *arg);

   void addOpponent(unit_data *victim, int bMelee);
   void subOpponent(unit_data *victim);
   void status(const unit_data *ch);

private:
   void add(unit_data *victim);
   void sub(int idx);
   auto findOpponentIdx(unit_data *tmp) -> int;

   int         nWhen;                     // What tick to attack / command at
   unit_data  *pOwner;                    // The owning unit
   unit_data  *pMelee;                    // The melee or kill pointer
   unit_data **pOpponents;                // Array of opponents (given damage)
   int         nNoOpponents;              // Number of opponents
   char        cmd[MAX_INPUT_LENGTH + 1]; // A combat command
};

extern class cCombatList CombatList;

void set_fighting(unit_data *ch, unit_data *vict, int bMelee = FALSE);
void stop_fighting(unit_data *ch, unit_data *victim = NULL);

void stat_combat(const unit_data *ch, unit_data *u);
