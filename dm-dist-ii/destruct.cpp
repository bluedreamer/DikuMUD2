#include "affect.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "mobact.h"
#include "nanny.h"
#include "structs.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "utility.h"
#include "utils.h"

/*
   This method is possible since we know that the only problem occurs when:

      An event triggers affect_beat or special_event.

      During the call from either of these to some other routine - the
      unit may be destructed together with its original fptr/affect.

      Now the event routine request's a new event because it doesn't know
      that the affect/func was destructed.

   We can use this method to determine if a unit has been destructed.
   All destructed units, affects and funcs are registered and can
   be tested for - for example by 'is_destructed(, ch)'.

   All registered destructed 'things' are cleared each time the
   program reaches 'top level' (game loop).

   Thus these routines are to be used in the following way:

      in affect_beat and special_event we must test if the
      unit was destructed, if so a new event is not reordered.

      in special routines, skills and ticks we can test if
      the person has been killed by using (example wiz command 'at'):

         unit_from_unit(ch);
         unit_to_unit(ch, dest_room);
         command_interpreter(ch, arg);
         if (is_destructed(DR_UNIT, ch))
            return;
         unit_from_unit(ch);
         unit_to_unit(ch, org_room);
*/

/* May only be called by clear_destuct! */
void destruct_unit(unit_data *unit)
{
   descriptor_data *d;
   int              in_menu = static_cast<int>(FALSE);

   extern descriptor_data *descriptor_list;

   /* Remove all snooping, snoopers and return from any body */
   if(IS_CHAR(unit))
   {
      if(CHAR_DESCRIPTOR(unit) != nullptr)
      {
         assert(IS_PC(unit));

         in_menu = static_cast<int>(TRUE);

         stop_all_special(unit);
         stop_affect(unit);

         if(UNIT_IN(unit) != nullptr)
         {
            set_descriptor_fptr(CHAR_DESCRIPTOR(unit), nanny_menu, TRUE);
         }
         else
         {
            set_descriptor_fptr(CHAR_DESCRIPTOR(unit), nanny_close, TRUE);
         }
      }

      assert(!CHAR_IS_SWITCHED(unit));
      assert(!CHAR_IS_SNOOPING(unit));
      assert(!CHAR_IS_SNOOPED(unit));

      /* If the PC which is switched is extracted, then unswitch */
      if(IS_PC(unit) && (CHAR_DESCRIPTOR(unit) == nullptr))
      {
         for(d = descriptor_list; d != nullptr; d = d->next)
         {
            assert(d->original != unit);
         }
      }

      assert(!CHAR_FOLLOWERS(unit));
      assert(!CHAR_MASTER(unit));
      assert(!CHAR_COMBAT(unit));
   }
   else if(!IS_OBJ(unit))
   {
      slog(LOG_OFF, 0, "Extract on something not a char or an obj.");
      assert(FALSE);
   }

   while(UNIT_CONTAINS(unit) != nullptr)
   {
      if(IS_OBJ(UNIT_CONTAINS(unit)) && (OBJ_EQP_POS(UNIT_CONTAINS(unit)) != 0u))
      {
         unequip_object(UNIT_CONTAINS(unit));
      }
      destruct_unit(UNIT_CONTAINS(unit));
   }

   if(in_menu == 0)
   {
      /* Call functions of the unit which have any data                     */
      /* that they might want to work on.                                   */
      while(UNIT_FUNC(unit) != nullptr)
      {
         destroy_fptr(unit, UNIT_FUNC(unit)); /* Unlinks, no free */
      }

      while(UNIT_AFFECTED(unit) != nullptr)
      {
         unlink_affect(unit, UNIT_AFFECTED(unit));
      }
   }

   if(UNIT_IN(unit) != nullptr)
   {
      unit_from_unit(unit);
   }

   if(UNIT_FILE_INDEX(unit) != nullptr)
   {
      UNIT_FILE_INDEX(unit)->no_in_mem--;
   }

   if((unit_list == unit) || (unit->gnext != nullptr) || (unit->gprevious != nullptr))
   {
      remove_from_unit_list(unit);
   }

   if(in_menu == 0)
   {
      delete unit;
   }
}

/* Registered by affect.c low level destruct affect */
/* (unlink affect). Used only by affect_beat.       */
/* Registered by destroy fptr */
/* Used only by special_event */
/* Registered by extract_unit */
/* Used lots of places        */

/* 3 Arrays of registered destructed things     */
void **destructed[3] = {nullptr, nullptr, nullptr};

/* 3 Arrays which indicate size of above arrays */
int destructed_top[3] = {-1, -1, -1};

/* 3 Arrays which point to first free entry     */
int destructed_idx[3] = {-1, -1, -1};

void destruct_resize(int i)
{
   if(destructed_top[i] == -1)
   {
      CREATE(destructed[i], void *, 20);
      destructed_top[i] = 20;
      destructed_idx[i] = 0;
   }
   else
   {
      RECREATE(destructed[i], void *, 20 + destructed_top[i]);
      destructed_top[i] += 20;
   }
}

/* May only be called from extract_unit, destroy_ftpr and unlink_affect */
void register_destruct(int i, void *ptr)
{
   assert(ptr);

   if(destructed_idx[i] >= destructed_top[i])
   {
      destruct_resize(i);
   }

   destructed[i][destructed_idx[i]] = ptr;
   destructed_idx[i]++;
}

/* May only be called from comm.c event loop */
void clear_destructed()
{
   unit_fptr *f;
   int        i;

   for(i = 0; i < destructed_idx[DR_AFFECT]; i++)
   {
      free(destructed[DR_AFFECT][i]);
   }
   destructed_idx[DR_AFFECT] = 0;

   for(i = 0; i < destructed_idx[DR_FUNC]; i++)
   {
      f = (unit_fptr *)destructed[DR_FUNC][i];
      if(f->data != nullptr)
      {
         free(f->data);
      }
      free(f);
   }
   destructed_idx[DR_FUNC] = 0;

   for(i = 0; i < destructed_idx[DR_UNIT]; i++)
   {
      destruct_unit((unit_data *)destructed[DR_UNIT][i]);
   }
   destructed_idx[DR_UNIT] = 0;
}

auto is_destructed(int i, void *ptr) -> int
{
   int n;

   assert(ptr);

   for(n = 0; n < destructed_idx[i]; n++)
   {
      if(destructed[i][n] == ptr)
      {
         return static_cast<int>(TRUE);
      }
   }

   return static_cast<int>(FALSE);
}
