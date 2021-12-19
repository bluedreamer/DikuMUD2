#include "cCombatList.h"
#include "comm.h"
#include "fight.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

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
      while(CHAR_COMBAT(ch) != nullptr)
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

   if(CHAR_COMBAT(u) == nullptr)
   {
      act("No combat structure on '$2n'", A_ALWAYS, ch, u, nullptr, TO_CHAR);
   }
   else
   {
      CHAR_COMBAT(u)->status(ch);
   }
}
