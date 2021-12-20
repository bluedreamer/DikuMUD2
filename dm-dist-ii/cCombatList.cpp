#include "cCombatList.h"

#include "cCombat.h"
#include "comm.h"
#include "fight.h"
#include "interpreter.h"
#include "textutil.h"
#include "utils.h"
#include "values.h"

#include <algorithm>
#include <cstring>

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
   cCombat *e1 = *((cCombat **)v1);
   cCombat *e2 = *((cCombat **)v2);

   if(e1->When() > e2->When())
   {
      return +1;
   }
   if(e1->When() < e2->When())
   {
      return -1;
   }
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
   int bAnyaction = static_cast<int>(FALSE);

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
         pElems[nIdx]->nWhen = std::max(0, pElems[nIdx]->nWhen - SPEED_DEFAULT);
      }
   }

   do
   {
      for(nIdx = 0; nIdx < nTop; nIdx++)
      {
         bAnyaction = static_cast<int>(FALSE);

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
            bAnyaction = static_cast<int>(TRUE);

            free(c);
         }
         else
         {
            if(CHAR_FIGHTING(pElems[nIdx]->pOwner))
            {
               if(char_dual_wield(pElems[nIdx]->pOwner) != 0)
               {
                  bAnyaction = static_cast<int>(TRUE);
                  melee_violence(pElems[nIdx]->pOwner, static_cast<int>(tmp->nWhen <= (SPEED_DEFAULT + 1) / 2));
                  if((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                  {
                     tmp->nWhen += std::max(2, (1 + CHAR_SPEED(tmp->pOwner)) / 2);
                  }
               }
               else
               {
                  bAnyaction = static_cast<int>(TRUE);
                  melee_violence(pElems[nIdx]->pOwner, static_cast<int>(TRUE));
                  if((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                  {
                     tmp->nWhen += std::max(4, static_cast<int>(CHAR_SPEED(tmp->pOwner)));
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
