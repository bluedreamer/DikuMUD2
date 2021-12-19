#pragma once

#include "unit_data.h"

struct visit_data
{
   int        state;
   unit_data *go_to;

   unit_data *start_room;
   unit_data *dest_room;

   /* Return DESTROY_ME to destroy moving function        */
   /*        SFR_SHARE to allow lower functions to handle */
   /*        SFR_SUPREME to not allow lower functions     */
   int (*what_now)(const unit_data *, struct visit_data *);

   void *data;
   int   non_tick_return; /* What to return upon non-ticks (SFR_...) */
};
