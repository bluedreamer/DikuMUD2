#pragma once
#include "unit_data.h"
class cCombat
{
   friend class cCombatList;

public:
   cCombat(unit_data *owner, int bMelee = static_cast<int>(FALSE));
   ~cCombat();

   auto Opponent(int i = 0) -> unit_data *;
   auto FindOpponent(unit_data *tmp) -> unit_data *;

   inline auto               Owner() -> unit_data               *{ return pOwner; }
   inline auto               Melee() -> unit_data               *{ return pMelee; }
   [[nodiscard]] inline auto When() const -> int { return nWhen; }
   [[nodiscard]] inline auto NoOpponents() const -> int { return nNoOpponents; }

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
