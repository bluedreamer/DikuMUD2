/* *********************************************************************** *
 * File   : combat.c                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Implementation of combat.h                                     *
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

#include "comm.h"
#include "fight.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

// The Global Combat List...

class cCombatList CombatList;

cCombatList::cCombatList()
{
   nMaxTop = 10;

   CREATE(pElems, class cCombat *, nMaxTop);

   nTop = 0;
   nIdx = -1;
}

cCombatList::~cCombatList()
{
   free(pElems);

   nMaxTop = 0;
   nTop    = 0;
}

void cCombatList::add(class cCombat *Combat)
{
   if(nTop >= nMaxTop)
   {
      nMaxTop += 10;
      RECREATE(pElems, class cCombat *, nMaxTop);
   }

   pElems[nTop++] = Combat;
}

void cCombatList::sub(class cCombat *pc)
{
   for(int i = 0; i < nTop; i++)
   {
      if(pElems[i] == pc)
      {
         if(nTop - i > 1)
         {
            memmove(&pElems[i], &pElems[i + 1], sizeof(class cCombat *) * (nTop - i - 1));
         }

         pElems[nTop - 1] = nullptr;

         nTop--;

         if(nIdx != -1) /* Uh oh, we are running through the list... */
         {
            if(i < nIdx)
            {
               nIdx--;
            }
         }
      }
   }
}

static auto combat_compare(const void *v1, const void *v2) -> int
{
   class cCombat *e1 = *((class cCombat **)v1);
   class cCombat *e2 = *((class cCombat **)v2);

   if(e1->When() > e2->When())
   {
      return +1;
   }
   if(e1->When() < e2->When())
      return -1;
   else
      return 0;
}

void cCombatList::Sort()
{
   if(nTop > 0)
   {
      qsort(pElems, nTop, sizeof(class cCombat *), combat_compare);
   }
}

void cCombatList::PerformViolence()
{
   int bAnyaction = FALSE;

   if(nTop < 1)
   {
      return;
   }

   Sort();

   // Happens just ONCE per turn, give everybody 12 actions...
   for(nIdx = 0; nIdx < nTop; nIdx++)
   {
      if(pElems[nIdx]->nWhen > 0)
      {
         pElems[nIdx]->nWhen = MAX(0, pElems[nIdx]->nWhen - SPEED_DEFAULT);
      }
   }

   do
   {
      for(nIdx = 0; nIdx < nTop; nIdx++)
      {
         bAnyaction = FALSE;

         if(pElems[nIdx]->nWhen >= SPEED_DEFAULT)
         {
            break; // The rest are larger...
         }

         class cCombat *tmp = pElems[nIdx];

         if(pElems[nIdx]->cmd[0] != 0) // Execute a combat command...
         {
            char *c = str_dup(pElems[nIdx]->cmd);

            pElems[nIdx]->cmd[0] = 0;
            command_interpreter(pElems[nIdx]->pOwner, c);
            bAnyaction = TRUE;

            free(c);
         }
         else
         {
            if(CHAR_FIGHTING(pElems[nIdx]->pOwner))
            {
               if(char_dual_wield(pElems[nIdx]->pOwner) != 0)
               {
                  bAnyaction = TRUE;
                  melee_violence(pElems[nIdx]->pOwner, static_cast<int>(tmp->nWhen <= (SPEED_DEFAULT + 1) / 2));
                  if((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                  {
                     tmp->nWhen += MAX(2, (1 + CHAR_SPEED(tmp->pOwner)) / 2);
                  }
               }
               else
               {
                  bAnyaction = TRUE;
                  melee_violence(pElems[nIdx]->pOwner, TRUE);
                  if((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                  {
                     tmp->nWhen += MAX(4, CHAR_SPEED(tmp->pOwner));
                  }
               }
            }
         }
      }
      Sort();
   } while((bAnyaction != 0) && nTop > 0 && pElems[0]->nWhen < SPEED_DEFAULT);

   nIdx = -1;
}

void cCombatList::status(const unit_data *ch) const
{
   char buf[MAX_STRING_LENGTH];

   sprintf(buf, "The Global Combat List contains [%d] entries.\n\r", nTop);
   send_to_char(buf, ch);
}

/* ======================================================================= */
/*                                                                         */
/*                         The Combat Element                              */
/*                                                                         */
/* ======================================================================= */

cCombat::cCombat(unit_data *owner, int bMelee)
{
   assert(owner);

   pOwner = owner;
   pMelee = nullptr;

   nWhen  = SPEED_DEFAULT;
   cmd[0] = 0;

   pOpponents   = nullptr;
   nNoOpponents = 0;

   CombatList.add(this);
}

cCombat::~cCombat()
{
   while(nNoOpponents > 0)
   {
      subOpponent(pOpponents[nNoOpponents - 1]); // Faster sub at tail
   }

   if(pOpponents != nullptr)
   {
      free(pOpponents);
      pOpponents = nullptr;
   }

   CHAR_COMBAT(pOwner) = nullptr;

   CombatList.sub(this);

   CHAR_POS(pOwner) = POSITION_STANDING;
   update_pos(pOwner);

   pOwner = nullptr;
}

void cCombat::setCommand(char *arg)
{
   strncpy(cmd, arg, MAX_INPUT_LENGTH);
   cmd[MAX_INPUT_LENGTH] = 0;
}

void cCombat::changeSpeed(int delta)
{
   nWhen += delta;
}

auto cCombat::findOpponentIdx(unit_data *target) -> int
{
   for(int i = 0; i < nNoOpponents; i++)
   {
      if(pOpponents[i] == target)
      {
         return i;
      }
   }

   return -1;
}

auto cCombat::FindOpponent(unit_data *victim) -> unit_data *
{
   int i = findOpponentIdx(victim);

   if(i == -1)
   {
      return nullptr;
   }
   return pOpponents[i];
}

void cCombat::add(unit_data *victim)
{
   assert(victim);

   nNoOpponents++;

   if(nNoOpponents == 1)
   {
      CREATE(pOpponents, unit_data *, 1);
   }
   else
   {
      RECREATE(pOpponents, unit_data *, nNoOpponents);
   }

   pOpponents[nNoOpponents - 1] = victim;
}

void cCombat::sub(int idx)
{
   int bWas = FALSE;

   if(idx == -1)
   {
      return;
   }

   assert(nNoOpponents > 0);
   assert(idx >= 0 && idx < nNoOpponents);

   // Never mind about realloc, it will be free'd soon anyhow... how long
   // can a combat take anyway?

   if(pOpponents[idx] == pMelee)
   {
      pMelee = nullptr;
      bWas   = TRUE;
   }

   if(nNoOpponents - idx > 1)
   {
      memmove(&pOpponents[idx], &pOpponents[idx + 1], sizeof(unit_data *) * (nNoOpponents - idx - 1));
   }

   pOpponents[nNoOpponents - 1] = nullptr;
   nNoOpponents--;

   if(nNoOpponents < 1)
   {
      free(pOpponents);
      pOpponents = nullptr;
      delete this; // We are done...
   }
   else if(bWas != 0)
   {
      setMelee(Opponent(number(0, nNoOpponents - 1)));
   }
}

void cCombat::setMelee(unit_data *victim)
{
   pMelee = victim;
}

// Add another opponent. A very important feature is, that opponents
// always exists as pairs and if one is removed so is the other.
//
void cCombat::addOpponent(unit_data *victim, int bMelee = FALSE)
{
   // This if is needed since we call recursively for the victim

   if(FindOpponent(victim) == nullptr)
   {
      add(victim);

      if(!CHAR_COMBAT(victim))
      {
         CHAR_COMBAT(victim) = new cCombat(victim, bMelee);
      }

      CHAR_COMBAT(victim)->add(pOwner);
   }

   if((bMelee != 0) && pMelee == nullptr)
   {
      setMelee(victim);
   }
}

void cCombat::subOpponent(unit_data *victim)
{
   if(nNoOpponents < 1)
   {
      return;
   }

   int i = findOpponentIdx(victim);

   if(i == -1)
   { // Not found
      return;
   }

   CHAR_COMBAT(victim)->sub(CHAR_COMBAT(victim)->findOpponentIdx(pOwner));
   sub(i);
}

auto cCombat::Opponent(int i) -> unit_data *
{
   if(i >= nNoOpponents)
   {
      return nullptr;
   }
   return pOpponents[i];
}

void cCombat::status(const unit_data *ch)
{
   char buf[MAX_STRING_LENGTH];
   int  i;

   sprintf(buf,
           "Combat Status of '%s':\n\r"
           "Combat Speed [%d]  Turn [%d]\n\r"
           "Melee Opponent '%s'\n\r"
           "Total of %d Opponents:\n\r",
           STR(UNIT_NAME(pOwner)), CHAR_SPEED(pOwner), nWhen, CHAR_FIGHTING(pOwner) ? STR(UNIT_NAME(CHAR_FIGHTING(pOwner))) : "NONE",
           nNoOpponents);

   send_to_char(buf, ch);

   for(i = 0; i < nNoOpponents; i++)
   {
      sprintf(buf, "   %s\n\r", STR(UNIT_NAME(pOpponents[i])));
      send_to_char(buf, ch);
   }
}

/* ======================================================================= */
/*                                                                         */
/*                         Utility Routines                                */
/*                                                                         */
/* ======================================================================= */

/* start one char fighting another (yes, it is horrible, I know... )  */
void set_fighting(unit_data *ch, unit_data *vict, int bMelee)
{
   if(ch == vict)
   {
      return;
   }

   /* No check for awake! If you die, FIGHTING() is set to point to murderer */

   if((is_destructed(DR_UNIT, ch) != 0) || (is_destructed(DR_UNIT, vict) != 0))
   {
      return;
   }

   if(CHAR_COMBAT(ch) == nullptr)
   {
      CHAR_COMBAT(ch) = new cCombat(ch, bMelee);
   }

   CHAR_COMBAT(ch)->addOpponent(vict, bMelee);

   CHAR_POS(ch) = POSITION_FIGHTING;
}

/* remove a char from the list of fighting chars */
void stop_fighting(unit_data *ch, unit_data *victim)
{
   if(victim == nullptr) // Stop all combat...
   {
      while(CHAR_COMBAT(ch))
      {
         CHAR_COMBAT(ch)->subOpponent(CHAR_COMBAT(ch)->Opponent());
      }
   }
   else
   {
      CHAR_COMBAT(ch)->subOpponent(victim);
   }

   if(CHAR_COMBAT(ch) == nullptr)
   {
      REMOVE_BIT(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE);
      CHAR_POS(ch) = POSITION_STANDING;
      update_pos(ch);
   }
}

/* ======================================================================= */
/*                                                                         */
/*                         Status Routines                                 */
/*                                                                         */
/* ======================================================================= */

void stat_combat(const unit_data *ch, unit_data *u)
{
   if(!IS_CHAR(u))
   {
      act("$2n is not a pc / npc.", A_ALWAYS, ch, u, nullptr, TO_CHAR);
      return;
   }

   CombatList.status(ch);

   if(!CHAR_COMBAT(u))
   {
      act("No combat structure on '$2n'", A_ALWAYS, ch, u, nullptr, TO_CHAR);
   }
   else
   {
      CHAR_COMBAT(u)->status(ch);
   }
}
