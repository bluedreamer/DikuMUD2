#pragma once

#include "unit_data.h"

class cCombat;

class cCombatList
{
public:
   cCombatList();
   ~cCombatList();
   void PerformViolence();
   void add(cCombat *pc);
   void sub(cCombat *pc);
   void status(const unit_data *ch) const;

private:
   void      Sort();

   cCombat **pElems;
   int       nMaxTop; // No of allocated elements
   int       nTop;    // Current Max
   int       nIdx;    // Updated when in Perform() and doing Sub()
};

extern cCombatList CombatList;
