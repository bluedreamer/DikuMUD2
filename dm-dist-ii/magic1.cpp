#include "act_movement.h"
#include "affect.h"
#include "comm.h"
#include "door_data.h"
#include "fight.h"
#include "handler.h"
#include "magic.h"
#include "skills.h"
#include "spell_args.h"
#include "spells.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_vector_data.h"
#include "utility.h"
#include "utils.h"
#include "values.h"
#include "zon_basis.h"

#include <cstdio>

void modify_hit(unit_data *ch, int hit);

void spell_remove_curse(spell_args *sa)
{
   /*
      Spell Remove Curse:
      This spell removes the gods bad attention from a character.
      */
   unit_data          *u;
   unit_affected_type *af;

   /* removes curse on unit and contents (if obj) */
   /* each unit gets to save (only fair)          */

   if((af = affected_by_spell(sa->target, ID_CURSE)) != nullptr)
   {
      /* it IS hard to remove curse */
      sa->hm = spell_resistance(sa->medium, sa->target, SPL_REMOVE_CURSE);

      if(sa->hm < 0)
      {
         send_to_char("You fail.\n\r", sa->caster);
         return;
      }

      destroy_affect(af);
   }

   for(u = UNIT_CONTAINS(sa->target); u != nullptr; u = u->next)
   {
      sa->hm = spell_resistance(sa->medium, sa->target, SPL_REMOVE_CURSE);
      if(sa->hm > 0)
      {
         if((af = affected_by_spell(u, ID_CURSE)) != nullptr)
         {
            destroy_affect(af);
         }
      }
   }
}

void spell_cause_wounds_1(spell_args *sa)
{
   /*
      Spell Cause Light Wounds
      This spell causes light wounds to the sa->target character.
      */

   if(CHAR_IS_HUMANOID(sa->target) || CHAR_IS_MAMMAL(sa->target))
   {
      sa->hm = spell_offensive(sa, SPL_CAUSE_WOUNDS_1);
   }
   else
   {
      int effect = dil_effect(sa->pEffect, sa);

      sa->hm     = -1;

      damage(sa->caster,
             sa->target,
             nullptr,
             0,
             MSG_TYPE_SPELL,
             SPL_CAUSE_WOUNDS_1,
             COM_MSG_MISS,
             static_cast<int>(static_cast<int>(effect) == 0));
   }
}

void spell_cause_wounds_2(spell_args *sa)
{
   if(CHAR_IS_HUMANOID(sa->target) || CHAR_IS_MAMMAL(sa->target))
   {
      sa->hm = spell_offensive(sa, SPL_CAUSE_WOUNDS_2);
   }
   else
   {
      int effect = dil_effect(sa->pEffect, sa);

      sa->hm     = -1;

      damage(sa->caster,
             sa->target,
             nullptr,
             0,
             MSG_TYPE_SPELL,
             SPL_CAUSE_WOUNDS_2,
             COM_MSG_MISS,
             static_cast<int>(static_cast<int>(effect) == 0));
   }
}

void spell_cause_wounds_3(spell_args *sa)
{
   if(CHAR_IS_HUMANOID(sa->target) || CHAR_IS_MAMMAL(sa->target))
   {
      sa->hm = spell_offensive(sa, SPL_CAUSE_WOUNDS_3);
   }
   else
   {
      int effect = dil_effect(sa->pEffect, sa);
      sa->hm     = -1;

      damage(sa->caster,
             sa->target,
             nullptr,
             0,
             MSG_TYPE_SPELL,
             SPL_CAUSE_WOUNDS_3,
             COM_MSG_MISS,
             static_cast<int>(static_cast<int>(effect) == 0));
   }
}

/* EVIL */
void spell_dispel_evil(spell_args *sa)
{
   unit_affected_type *af;

   provoked_attack(sa->target, sa->caster);

   if(!UNIT_IS_EVIL(sa->target) || (sa->hm < 0))
   {
      send_to_char("You fail.\n\r", sa->caster);
      return;
   }

   if((af = affected_by_spell(sa->target, ID_PROT_GOOD)) != nullptr)
   {
      destroy_affect(af);
      return;
   }

   sa->hm = spell_offensive(sa, SPL_DISPEL_EVIL);
}

/* EVIL */
void spell_dispel_good(spell_args *sa)
{
   unit_affected_type *af;

   provoked_attack(sa->target, sa->caster);

   if(UNIT_IS_GOOD(sa->caster) || !UNIT_IS_GOOD(sa->target) || (sa->hm < 0))
   {
      sa->hm = -1;
      send_to_char("You fail.\n\r", sa->caster);
      return;
   }

   if((af = affected_by_spell(sa->target, ID_PROT_EVIL)) != nullptr)
   {
      sa->hm = -2;
      destroy_affect(af);
      return;
   }

   sa->hm = spell_offensive(sa, SPL_DISPEL_GOOD);
}

void spell_repel_undead_1(spell_args *sa)
{
   unit_affected_type af;

   if(!CHAR_IS_UNDEAD(sa->target))
   {
      sa->hm = -1;
      return;
   }

   provoked_attack(sa->target, sa->caster);

   if(sa->hm >= 0)
   {
      af.id       = ID_FEAR;
      af.duration = skill_duration(sa->hm);
      af.beat     = WAIT_SEC * 30;
      af.data[0] = af.data[1] = af.data[2] = 0;

      af.firstf_i                          = TIF_FEAR_CHECK;
      af.tickf_i                           = TIF_FEAR_CHECK;
      af.lastf_i                           = TIF_NONE;
      af.applyf_i                          = APF_NONE;

      create_affect(sa->target, &af);
   }
}

void spell_repel_undead_2(spell_args *sa)
{
   unit_data         *u;
   unit_affected_type af;
   int                p;

   scan4_unit(sa->caster, UNIT_ST_PC | UNIT_ST_NPC);

   for(p = 0; p < unit_vector.top; p++)
   {
      u = unit_vector.units[p];

      if(CHAR_IS_UNDEAD(u) && CHAR_CAN_SEE(u, sa->caster))
      {
         sa->hm = spell_resistance(sa->medium, u, SPL_REPEL_UNDEAD_2);
         if(sa->hm >= 0)
         {
            af.id       = ID_FEAR;
            af.duration = skill_duration(sa->hm);
            af.beat     = WAIT_SEC * 30;
            af.data[0] = af.data[1] = af.data[2] = 0;

            af.firstf_i                          = TIF_FEAR_CHECK;
            af.tickf_i                           = TIF_FEAR_CHECK;
            af.lastf_i                           = TIF_NONE;
            af.applyf_i                          = APF_NONE;

            create_affect(u, &af);
         }
         else
         {
            provoked_attack(u, sa->caster);
         }
      }
   }
}

void spell_cure_blind(spell_args *sa)
{
   unit_affected_type *af = nullptr;

   if(sa->hm >= 0)
   {
      if((af = affected_by_spell(sa->target, ID_BLIND_CHAR)) != nullptr)
      {
         destroy_affect(af);
      }
      else
      {
         sa->hm = -1;
      }
   }
}

void spell_sanctuary(spell_args *sa)
{
   unit_affected_type af;

   if(affected_by_spell(sa->target, ID_SANCTUARY) != nullptr)
   {
      sa->hm = -1;
      return;
   }

   if(sa->hm >= 0)
   {
      af.id       = ID_SANCTUARY;
      af.duration = skill_duration(sa->hm);
      af.beat     = WAIT_SEC * 30;
      af.firstf_i = TIF_SANCTUARY_ON;
      af.tickf_i  = TIF_SANCTUARY_TICK;
      af.lastf_i  = TIF_SANCTUARY_OFF;
      af.applyf_i = APF_NONE;
      af.data[0] = af.data[1] = af.data[2] = 0;
      create_affect(sa->target, &af);
   }
}

void spell_sustain(spell_args *sa)
{
   unit_affected_type af;

   if(affected_by_spell(sa->target, ID_SUSTAIN) != nullptr)
   {
      return;
   }

   if(sa->hm >= 0)
   {
      af.id       = ID_SUSTAIN;
      af.duration = skill_duration(sa->hm);
      af.beat     = WAIT_SEC * 30;
      af.firstf_i = TIF_SUSTAIN_ON;
      af.tickf_i  = TIF_SUSTAIN_TICK;
      af.lastf_i  = TIF_SUSTAIN_OFF;
      af.applyf_i = APF_NONE;
      af.data[0]  = IS_PC(sa->target) ? PC_COND(sa->target, 0) : 24;
      af.data[1]  = IS_PC(sa->target) ? PC_COND(sa->target, 1) : 24;
      af.data[2]  = IS_PC(sa->target) ? PC_COND(sa->target, 2) : 24;
      create_affect(sa->target, &af);
   }
}

void spell_lock(spell_args *sa)
{
   door_data *a_door = nullptr;

   char       mbuf[MAX_INPUT_LENGTH];
   strcpy(mbuf, sa->arg);

   if((static_cast<unsigned int>(str_is_empty(sa->arg)) != 0U) || (a_door = locate_lock(sa->caster, mbuf)) == nullptr)
   {
      send_to_char("The spell fails.\n\r", sa->caster);
      return;
   }

   if((a_door->flags == nullptr) || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
   {
      send_to_char("The spell has no effect.\n\r", sa->caster);
   }
   else if(!IS_SET(*a_door->flags, EX_CLOSED))
   {
      send_to_char("The spell fails.\n\r", sa->caster);
   }
   else if(IS_SET(*a_door->flags, EX_LOCKED))
   {
      send_to_char("The spell fails!\n\r", sa->caster);
   }
   else
   {
      if(a_door->thing == nullptr) /* a door */
      {
         assert(a_door->room);
         sa->hm = spell_resistance(sa->medium, a_door->room, SPL_LOCK);
      }
      else
      {
         sa->hm = spell_resistance(sa->medium, a_door->thing, SPL_LOCK);
      }

      if(sa->hm >= 0)
      {
         SET_BIT(*a_door->flags, EX_LOCKED);
         act("*cluck*", A_SOMEONE, sa->caster, nullptr, nullptr, TO_ALL);

         if(a_door->reverse != nullptr)
         {
            if(UNIT_CONTAINS(a_door->reverse) != nullptr)
            {
               act("*cluck*", A_SOMEONE, UNIT_CONTAINS(a_door->reverse), nullptr, nullptr, TO_ALL);
            }

            if(a_door->rev_flags != nullptr)
            {
               SET_BIT(*a_door->rev_flags, EX_LOCKED);
            }
         }
      }
   }
}

void spell_unlock(spell_args *sa)
{
   door_data *a_door = nullptr;

   char       mbuf[MAX_INPUT_LENGTH];
   strcpy(mbuf, sa->arg);

   if((static_cast<unsigned int>(str_is_empty(sa->arg)) != 0U) || ((a_door = locate_lock(sa->caster, mbuf)) == nullptr))
   {
      send_to_char("The spell failed.\n\r", sa->caster);
      return;
   }

   if((a_door->flags == nullptr) || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
   {
      send_to_char("The spell failed.\n\r", sa->caster);
   }
   else if(!IS_SET(*a_door->flags, EX_CLOSED))
   {
      send_to_char("The spell failed.\n\r", sa->caster);
   }
   else if(!IS_SET(*a_door->flags, EX_LOCKED))
   {
      send_to_char("The spell failed.\n\r", sa->caster);
   }
   else
   {
      if(a_door->room != nullptr) /* a door */
      {
         sa->hm = spell_resistance(sa->medium, a_door->room, SPL_UNLOCK);
      }
      else
      {
         assert(a_door->thing);
         sa->hm = spell_resistance(sa->medium, a_door->thing, SPL_UNLOCK);
      }

      if(sa->hm >= 0)
      {
         REMOVE_BIT(*a_door->flags, EX_LOCKED);
         act("*click*", A_SOMEONE, sa->caster, a_door->name, nullptr, TO_ALL);

         if(a_door->reverse != nullptr)
         {
            if(UNIT_CONTAINS(a_door->reverse) != nullptr)
            {
               act("*cluck*", A_SOMEONE, UNIT_CONTAINS(a_door->reverse), nullptr, nullptr, TO_ALL);
            }

            if(a_door->rev_flags != nullptr)
            {
               REMOVE_BIT(*a_door->rev_flags, EX_LOCKED);
            }
         }
      }
   }
}

void spell_identify_1(spell_args *sa)
{
   static struct obj_type_t ident_obj_type[] = {
      {"Well, you shouldn't see this.\n\r"
       "Please report",
       {0, 0, 0, 0, 0}}, /*UNUSED    */
      {"A lightsource with %s hours left,\n\r"
       "and an illumination of %s.",
       {0, 0, 0, 0, 0}}, /*LIGHT     */
      {"A scroll with power %s, which contains\n\r"
       "the spells: '%s', '%s', '%s'",
       {0, 1, 1, 1, 0}}, /*SCROLL    */
      {"A wand with power %s and %s charges left."
       "\n\rIt fires the spells: '%s', '%s'",
       {0, 0, 1, 1, 0}}, /*WAND      */
      {"A staff with power %s and %s charges left."
       "\n\rIt fires the spells: '%s', '%s'",
       {0, 0, 1, 1, 0}}, /*STAFF     */
      {"This weapon is a %s.\n\r"
       "It is of craftsmanship %s\n\r"
       "Magical modifier is %s\n\r",
       {2, 0, 0, 3, 3}},                          /*WEAPON    */
      {"This is a fireweapon.", {0, 0, 0, 0, 0}}, /*FIREWEAPON*/
      {"This is a missile.", {0, 0, 0, 0, 0}},    /*MISSILE   */
      {"This is a treasure!", {0, 0, 0, 0, 0}},   /*TREASURE  */
      {"This armour has:%s\n\r"
       "Craftsmanship %s\n\r"
       "Magical modifier %s\n\r",
       {3, 0, 0, 3, 3}}, /*ARMOR     */
      {"This potion with power %s\n\r"
       "has the spells: '%s', '%s', '%s'",
       {0, 1, 1, 1, 0}},                                 /*POTION    */
      {"This is cloth or equivalent.", {0, 0, 0, 0, 0}}, /*WORN      */
      {"", {0, 0, 0, 0, 0}},                             /*OTHER     */
      {"", {0, 0, 0, 0, 0}},                             /*TRASH     */
      {"", {0, 0, 0, 0, 0}},                             /*TRAP      */
      {"", {0, 0, 0, 0, 0}},                             /*CONTAINER */
      {"This is a note.", {0, 0, 0, 0, 0}},              /*NOTE      */
      {"This is a drinkcontainer.", {0, 0, 0, 0, 0}},    /*DRINKCON  */
      {"This is a key.", {0, 0, 0, 0, 0}},               /*KEY       */
      {"This is food.", {0, 0, 0, 0, 0}},                /*FOOD      */
      {"This is money.", {0, 0, 0, 0, 0}},               /*MONEY     */
      {"This is a pen.", {0, 0, 0, 0, 0}},               /*PEN       */
      {"This is a boat.", {0, 0, 0, 0, 0}},              /*BOAT      */
      {"", {0, 0, 0, 0, 0}},                             /*SPELL     */
      {"", {0, 0, 0, 0, 0}},                             /*BOOK      */
      {"This shield has:%s\n\r"
       "Craftsmanship %s%%\n\r"
       "Magical modifier %s%%\n\r",
       {3, 0, 0, 3, 3}} /*SHIELD    */
   };

   extra_descr_data *exd;
   char              buffer[1024];

   if(sa->hm < 0)
   {
      return;
   }

   *buffer = '\0';

   assert(IS_OBJ(sa->target)); /* it _is_ an object */

   if(ITEM_LIGHT <= OBJ_TYPE(sa->target) && OBJ_TYPE(sa->target) <= ITEM_SHIELD)
   {
      char  extra[256];
      char *str = stat_obj_data(sa->target, ident_obj_type);

      *extra    = '\0';

      switch(OBJ_TYPE(sa->target))
      {
         case ITEM_CONTAINER:
            sprintf(extra, "This is a %s.\n\r", affected_by_spell(sa->target, ID_CORPSE) != nullptr ? "corpse" : "container");
            break;

         case ITEM_BOAT:
            sprintf(extra, "It can contain %d weight units.\n\r", UNIT_CAPACITY(sa->target));
            break;

         case ITEM_DRINKCON:
         case ITEM_FOOD:
            if(OBJ_VALUE(sa->target, 3) != 0)
            {
               sprintf(extra, "It is poisoned.\n\r");
            }
            break;
      }

      sprintf(buffer, "%s\n\r%s", str, extra);
   }

   exd = UNIT_EXTRA_DESCR(sa->target)->find_raw("$identify");

   if(*buffer == 0)
   {
      if(exd == nullptr)
      {
         send_to_char("You get no insight into the secret of this item.\n\r", sa->caster);
      }
   }
   else
   {
      send_to_char(buffer, sa->caster);
   }

   if((exd = UNIT_EXTRA_DESCR(sa->target)->find_raw("$identify")) != nullptr)
   {
      send_to_char(exd->descr.String(), sa->caster);
   }
}

void spell_identify_2(spell_args *sa)
{
   extra_descr_data *exd;
   int               i = 0;

   if(sa->hm < 0)
   {
      return;
   }

   if((exd = UNIT_EXTRA_DESCR(sa->target)->find_raw("$identify")) != nullptr)
   {
      send_to_char(exd->descr.String(), sa->caster);
      i = 1;
   }

   if((exd = UNIT_EXTRA_DESCR(sa->target)->find_raw("$improved identify")) != nullptr)
   {
      send_to_char(exd->descr.String(), sa->caster);
      i = 1;
   }

   if(i == 0)
   {
      send_to_char("There is nothing exceptional to learn "
                   "about this item.\n\r",
                   sa->caster);
   }
}
