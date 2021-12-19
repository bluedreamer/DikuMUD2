#include "cCombat.h"
#include "cCombatList.h"
#include "comm.h"
#include "fight.h"
#include "utility.h"
#include "utils.h"
#include "values.h"

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
   int bWas = static_cast<int>(FALSE);

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
      bWas   = static_cast<int>(TRUE);
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
void cCombat::addOpponent(unit_data *victim, int bMelee = static_cast<int>(FALSE))
{
   // This if is needed since we call recursively for the victim

   if(FindOpponent(victim) == nullptr)
   {
      add(victim);

      if(CHAR_COMBAT(victim) == nullptr)
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
