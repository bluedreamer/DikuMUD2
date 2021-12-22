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

#ifndef _MUD_COMBAT_H
#define _MUD_COMBAT_H

#include <memory>
#include <vector>

class unit_data;

class cCombatList
{
public:
   cCombatList();
   ~cCombatList();
   void PerformViolence();
   void add(class cCombat *pc);
   void sub(class cCombat *pc);
   void status(const std::shared_ptr<unit_data> ch);

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
   cCombat(std::shared_ptr<unit_data> owner, int bMelee = FALSE);
   ~cCombat();

   std::shared_ptr<unit_data> Opponent(int i = 0);
   std::shared_ptr<unit_data> FindOpponent(std::shared_ptr<unit_data> tmp);

   inline std::shared_ptr<unit_data> Owner(void) { return pOwner; }
   inline std::shared_ptr<unit_data> Melee(void) { return pMelee; }
   inline int                        When() { return nWhen; }
   inline size_t                     NoOpponents() { return pOpponents.size(); }

   void changeSpeed(int delta);
   void setMelee(std::shared_ptr<unit_data> victim);
   void setCommand(char *arg);

   void addOpponent(std::shared_ptr<unit_data> victim, int bMelee);
   void subOpponent(std::shared_ptr<unit_data> victim);
   void status(const std::shared_ptr<unit_data> ch);

private:
   void add(std::shared_ptr<unit_data> victim);
   void sub(int idx);
   int  findOpponentIdx(std::shared_ptr<unit_data> tmp);

   int                                     nWhen;                     // What tick to attack / command at
   std::shared_ptr<unit_data>              pOwner;                    // The owning unit
   std::shared_ptr<unit_data>              pMelee;                    // The melee or kill pointer
   std::vector<std::shared_ptr<unit_data>> pOpponents;                // Array of opponents (given damage)
   char                                    cmd[MAX_INPUT_LENGTH + 1]; // A combat command
};

void set_fighting(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> vict, int bMelee = FALSE);
void stop_fighting(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> victim = NULL);
void stat_combat(const std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> u);

#endif
