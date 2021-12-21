/* *********************************************************************** *
 * File   : tif_affect.c                              Part of Valhalla MUD *
 * Version: 2.03                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Affect system tick routines (TIF_).                            *
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

/* 23/07/92 seifert: Fixed grave bug in reordering af ticks                */
/* 23/08/83 jubal  : Crimes are zeroed at death                            */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "handler.h"
#include "comm.h"
#include "spells.h"
#include "skills.h"
#include "affect.h"
#include "db.h"
#include "utility.h"
#include "interpreter.h"
#include "magic.h"
#include "money.h"
#include "limits.h"
#include "fight.h"

/* Extern Functions */
int is_destructed(int i, void *ptr);


void tif_confusion_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   struct unit_fptr *fptr;

   if ((fptr = find_fptr(unit, SFUN_HUNTING)))
   {
      send_to_char("You feel confused.\n\r", unit);
      destroy_fptr(unit, fptr);
   }
}

void tif_confusion_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel confused.\n\r", unit);
   act("$1n seems confused.", A_HIDEINV, unit, 0, 0, TO_ROOM);
   tif_confusion_tick(af, unit);
}

void tif_confusion_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You less confused.\n\r", unit);
   act("$1n seem less confused.", A_HIDEINV, unit, 0, 0, TO_ROOM);
}

void tif_invisibility_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INVISIBLE))
   {
      send_to_char("Your body appears ghostly.\n\r", unit);
      act("$1n vanish into thin air.", A_HIDEINV, unit, 0, 0, TO_ROOM);
   }
}

void tif_invisibility_off(struct unit_affected_type *af,struct unit_data *unit)
{
   if (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INVISIBLE))
   {
      send_to_char("Your body is once again visible.\n\r", unit);
      act("$1n appears from thin air.", A_HIDEINV, unit, 0, 0, TO_ROOM);
   }
}



void tif_fear_check(struct unit_affected_type *af, struct unit_data *unit)
{
   struct unit_data *ch;
   char mbuf[MAX_INPUT_LENGTH] = {0};

  if (CHAR_FIGHTING(unit))
  {
    switch (number(0,2))
    {
     case 0:
      act("You are about to die!!!", A_SOMEONE, unit,0,0,TO_CHAR);
      do_flee(unit, mbuf, &cmd_auto_unknown);
      /* He could be dead now! */
      return;
     case 1:
      act("That really did HURT!", A_ALWAYS, unit, 0, 0, TO_CHAR);
      break;
     case 2:
      act("You wish your wounds would stop BLEEDING that much!",
	  A_ALWAYS, unit, 0, 0, TO_CHAR);
      break;
    }
    return;
  }
  else
  {
    /* Find someone else */
    for (ch = UNIT_CONTAINS(UNIT_IN(unit)); ch; ch = ch->next)
      if (ch != unit && IS_CHAR(ch))
	break;

    if (ch)
      switch(number(0,1))
      {
       case 0:
	act("$3n prepares to kill you.", A_SOMEONE, unit, 0, ch, TO_CHAR);
	act("$1n looks at you in a strange way.",
	    A_SOMEONE, unit, 0, ch, TO_VICT);
	act("$1n seems paranoid, looking at $3n.",
	    A_SOMEONE, unit, 0, ch, TO_NOTVICT);
	break;
       case 1:
	act("$3n wounds you fatally with $3s sword.",
	    A_SOMEONE, unit, 0, ch, TO_CHAR);
	act("$1n screams in agony.", A_SOMEONE, unit, 0, ch, TO_ROOM);
	break;
      }
    else
      switch (number(0, 2))
      {
       case 0:
	act("Someone tries to steal your weapon!",
	    A_ALWAYS, unit, 0, 0, TO_CHAR);
	break;
       case 1:
	act("Someone grins evilly.", A_ALWAYS, unit, 0, 0, TO_CHAR);
	break;
       case 2:
	act("The huge green dragon appears in a puff of smoke!",
	    A_ALWAYS, unit, 0, 0, TO_CHAR);
      }
  }
}

void tif_blind_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("A cloak of darkness falls around you.\n\r", unit);
}

void tif_blind_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("The veil of darkness disappears.\n\r", unit);
}

/* sneak */
void tif_sneak_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("Ok, you'll try to move silently for a while.\n\r",unit);
}

void tif_sneak_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You stop sneaking about.\n\r",unit);
}

void tif_sneak_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You sneak about unnoticed.\n\r",unit);
}

/* hide */
void tif_hide_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You try to hide yourself.\n\r",unit);
}

void tif_hide_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You stop hiding.\n\r",unit);
   if (af->tickf_i==TIF_HIDE_TICK)
     act("You suddenly notice that $1n is standing here.",
	 A_HIDEINV, unit, 0, 0, TO_ROOM);
}

void tif_hide_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   if (!(af->data[1]))
     return;

   switch(af->data[1]--)
   {
     case 1:
      send_to_char("Someone is bound to look here sometime.\n\r",unit);
      break;
     case 2:
      send_to_char("You master the ways of concealing yourself.\n\r",unit);
      break;
     case 3:
      send_to_char("You skillfully use the shadows.\n\r",unit);
      break;
     case 4:
      send_to_char("Surely noone can see you here.\n\r",unit);
      break;
     case 5:
      send_to_char("You cleverly hide away.\n\r",unit);
   }
}



void tif_nohide_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   if (!(af->data[1]))
     return;

   switch(af->data[1]--)
   {
     case 1:
      send_to_char("Someone is probably looking at you.\n\r",unit);
      break;
     case 2:
      send_to_char("Maybe you should practice this hide-thing.\n\r",unit);
      break;
     case 3:
      send_to_char("You skillfully avoid the shadows.\n\r",unit);
      break;
     case 4:
      send_to_char("Surely you can see noone here?\n\r",unit);
      break;
     case 5:
      send_to_char("You cleverly hide away.\n\r",unit);
   }
}

/* bless */
void tif_bless_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel divine forces helping you.\n\r", unit);
}

void tif_bless_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel divine forces abandoning you.\n\r", unit);
}

void tif_bless_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->duration==1)
     send_to_char("You sense the divine forces about to move away.\n\r", unit);
}

/* curse */
void tif_curse_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (IS_CHAR(unit))
     send_to_char("You feel that the gods are against you.\n\r", unit);
   act("A shadow falls upon $1n.", A_HIDEINV, unit, 0, 0, TO_ROOM);
   if (UNIT_IN(unit) && IS_CHAR(UNIT_IN(unit)))
     act("A shadow falls upon $3n.",A_HIDEINV,UNIT_IN(unit),0,unit,TO_CHAR);
}

void tif_curse_off(struct unit_affected_type *af, struct unit_data *unit)
{
   if (IS_CHAR(unit))
     send_to_char("You no longer feel that the gods are against you.\n\r",
		  unit);

   act("A shadow lifts from $1n.", A_HIDEINV, unit, 0, 0, TO_ROOM);
   if (UNIT_IN(unit) && IS_CHAR(UNIT_IN(unit)))
     act("A shadow lifts from $3n.",A_HIDEINV,UNIT_IN(unit),0,unit,TO_CHAR);
}

/* sanctuary */
void tif_sanctuary_on(struct unit_affected_type *af, struct unit_data *unit)
{
   act("$1n momentarily glows in a bright white light.",
       A_HIDEINV, unit, 0, 0, TO_ROOM);
   send_to_char("You momentarily glow in a bright white light.\n\r", unit);
}

void tif_sanctuary_off(struct unit_affected_type *af, struct unit_data *unit)
{
   act("$1n glows in a bright white light. Then it fades away.",
       A_HIDEINV, unit, 0, 0, TO_ROOM);
   send_to_char("You glow in a bright white light. "
		"Then it fades away.\n\r", unit);
}

void tif_sanctuary_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->duration==1)
   {
      act("$1n glows in a bright white light... The light flickers.",
	  A_HIDEINV, unit, 0, 0, TO_ROOM);
      send_to_char("You glow in a bright white light... "
		   "The light flickers.\n\r",unit);
   }
}

void tif_eyes_tingle(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("Your eyes begin to tingle.\n\r", unit);
}


void tif_torch_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   OBJ_VALUE(unit, 0)--;

   if (af->duration <= 4)
   {
      if (IS_CHAR(UNIT_IN(unit)))
	 act("Your $2N is getting dim.", A_HIDEINV,
	     UNIT_IN(unit),unit,0,TO_CHAR);
   }
}



void tif_light_add(struct unit_affected_type *af, struct unit_data *unit)
{
  if (!UNIT_IN(unit))
    return;

  /* If the thing is carried by a character */
  if (IS_CHAR(UNIT_IN(unit)))
  {
    act("Your $3N starts to glow.", A_HIDEINV,UNIT_IN(unit),0,unit,TO_CHAR);
    act("$1n's $3n starts to glow.",A_HIDEINV,UNIT_IN(unit),0,unit,TO_ROOM);
  }
  else if (UNIT_CONTAINS(UNIT_IN(unit)))
    act("The $3N starts to glow.", A_HIDEINV,
	UNIT_CONTAINS(UNIT_IN(unit)), 0, unit, TO_ALL);
}



void tif_light_sub(struct unit_affected_type *af, struct unit_data *unit)
{
  if (!UNIT_IN(unit))
    return;

  /* If the thing is carried by a character */
  if (IS_CHAR(UNIT_IN(unit)))
  {
    act("Your $3N gets dimmer.", A_HIDEINV,UNIT_IN(unit),0,unit,TO_CHAR);
    act("$1n's $3N gets dimmer.",A_HIDEINV,UNIT_IN(unit),0,unit,TO_ROOM);
  }
  else if (UNIT_CONTAINS(UNIT_IN(unit)))
    act("The $3N gets dimmer.", A_HIDEINV,
	UNIT_CONTAINS(UNIT_IN(unit)),0,unit,TO_ALL);
}


void tif_sleep_on(struct unit_affected_type *af, struct unit_data *unit)
{
   act("You feel very tired.",A_ALWAYS,unit,0,0,TO_CHAR);
   if (CHAR_POS(unit) > POSITION_SLEEPING)
   {
      if (CHAR_FIGHTING(unit))
	 stop_fighting(unit);

      act("You fall asleep.",A_ALWAYS,unit,0,0,TO_CHAR);
      act("$1n falls asleep.",A_ALWAYS,unit,0,0,TO_ROOM);
      CHAR_POS(unit) = POSITION_SLEEPING;
   }
}


void tif_sleep_check(struct unit_affected_type *af, struct unit_data *unit)
{
   int hm;

   if (CHAR_POS(unit) > POSITION_SLEEPING)
   {
      hm = resistance_skill_check(af->data[0],
				  spell_ability(unit, ABIL_BRA, SPL_SLEEP),
				  af->data[1],
				  spell_defense_skill(unit, SPL_SLEEP));
      if (hm < 0)
      {
	 if (CHAR_FIGHTING(unit))
	    stop_fighting(unit);
	 act("You fall asleep.",A_ALWAYS,unit,0,0,TO_CHAR);
	 act("$1n falls asleep.",A_HIDEINV,unit,0,0,TO_ROOM);
	 CHAR_POS(unit) = POSITION_SLEEPING;
      }
   }
}


void tif_sleep_off(struct unit_affected_type *af, struct unit_data *unit)
{
   act("You feel less sleepy.",A_ALWAYS,unit,0,0,TO_CHAR);
   /* no no not a 'wake' here, remember he's still affacted */
}

void tif_protect_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel protected.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel less protected.",A_ALWAYS,unit,0,0,TO_CHAR);
}

void tif_protect_off(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel less protected.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel protected.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_hit_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more healthy.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel less healthy.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_hit_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_hit_on(af, unit);
   af->data[1] = -af->data[1];
}


void tif_mag_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more powerful.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel less powerful.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_mag_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_mag_on(af, unit);
   af->data[1] = -af->data[1];
}


void tif_div_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel closer to your god.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel parted from your god.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_div_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_div_on(af, unit);
   af->data[1] = -af->data[1];
}

void tif_str_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel stronger.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel weaker.",A_ALWAYS,unit,0,0,TO_CHAR);

   /* recalc_dex_red(unit); */
}


void tif_str_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_str_on(af, unit);
   af->data[1] = -af->data[1];
}


void tif_dex_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel dexterous.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel a little clumsy.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_dex_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_dex_on(af, unit);
   af->data[1] = -af->data[1];
}

void tif_con_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more robust.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel less robust.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_con_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_con_on(af, unit);
   af->data[1] = -af->data[1];
}


void tif_cha_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more authoritative.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel less authoritative.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_cha_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_cha_on(af, unit);
   af->data[1] = -af->data[1];
}


void tif_bra_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel smarter.",A_ALWAYS,unit,0,0,TO_CHAR);
   else
     act("You feel a little dumb.",A_ALWAYS,unit,0,0,TO_CHAR);
}


void tif_bra_off(struct unit_affected_type *af, struct unit_data *unit)
{
   af->data[1] = -af->data[1];
   tif_bra_on(af, unit);
   af->data[1] = -af->data[1];
}


void tif_poison_on(struct unit_affected_type *af, struct unit_data *unit)
{
   act("You feel very ill.",A_ALWAYS,unit,0,0,TO_CHAR);
   act("$1n seems very ill.",A_ALWAYS,unit,0,0,TO_ROOM);
}


/* Data[0] The amount of hitpoints to loose (>=0)        */
/* Data[1] The amount of Mana points to loose (>=0)      */
/* Data[2] The amount of Endurance points to loose (>=0) */
void tif_poison_suffer(struct unit_affected_type *af, struct unit_data *unit)
{
   CHAR_MANA(unit) -= af->data[1];
   CHAR_ENDURANCE(unit) -= af->data[2];
   damage(unit, unit, 0, af->data[0], MSG_TYPE_OTHER,
	  MSG_OTHER_POISON, COM_MSG_EBODY);
   /* unit can be destructed now, but no problemo */
}

void tif_poison_off(struct unit_affected_type *af, struct unit_data *unit)
{
   act("You feel better.",A_ALWAYS,unit,0,0,TO_CHAR);
}

/* plague */
void tif_plague_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You suddenly feel very sick.\n\r",unit);
}

void tif_plague_tick(struct unit_affected_type *af, struct unit_data *unit)
{
}

void tif_plague_off(struct unit_affected_type *af, struct unit_data *unit)
{
  send_to_char("You feel much better.\n\r",unit);
}

/* insanity */
void tif_insanity_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel a battle with your own mind begins.\n\r", unit);
   act("A mad look appears on $1n's face",A_HIDEINV,unit,0,0,TO_ROOM);
}

void tif_insanity_tick(struct unit_affected_type *af, struct unit_data *unit)
{
}

void tif_insanity_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel the battle with your own mind is over.\n\r", unit);
   act("The mad look disappears from $1n's face.",A_HIDEINV,unit,0,0,TO_ROOM);
}

void tif_prot_evil_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel protected from the forces of evil.\n\r", unit);
}

void tif_prot_good_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel protected from the forces of good.\n\r", unit);
}

void tif_prot_evil_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel less protected from the forces of evil.\n\r", unit);
}

void tif_prot_good_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel less protected from the forces of good.\n\r", unit);
}

void tif_sustain_on(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You feel sustained.\n\r", unit);
}

void tif_sustain_tick(struct unit_affected_type *af, struct unit_data *unit)
{
   if (!IS_PC(unit))
     return;
   PC_COND(unit,0)=af->data[0];
   PC_COND(unit,1)=af->data[1];
   PC_COND(unit,2)=af->data[2];
}

void tif_sustain_off(struct unit_affected_type *af, struct unit_data *unit)
{
   send_to_char("You no longer feel sustained.\n\r", unit);
}

void tif_decay_corpse(struct unit_affected_type *af, struct unit_data *unit)
{
   /* Make routine to change the description of a corpse instead */
   if (ODD(af->duration)  && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_BURIED))
     act("The rotten stench of $1n is here.",A_SOMEONE, unit, 0, 0, TO_ROOM);
}

void tif_destroy_corpse(struct unit_affected_type *af, struct unit_data *unit)
{
   if (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_BURIED))
     act("A quivering horde of maggots consume $1n.",
	 A_SOMEONE,unit,0,0,TO_ROOM);
   extract_unit(unit);
}

void tif_buried_destruct(struct unit_affected_type *af, struct unit_data *unit)
{
   if (IS_SET(UNIT_FLAGS(unit), UNIT_FL_BURIED))
   {
      /* Empty the container and set buried status of contents */

      while (UNIT_CONTAINS(unit))
      {
	 struct unit_affected_type naf;

	 SET_BIT(UNIT_FLAGS(UNIT_CONTAINS(unit)), UNIT_FL_BURIED);

	 naf.id       = ID_BURIED;
	 naf.duration = 0;
	 naf.beat     = WAIT_SEC*SECS_PER_REAL_HOUR;
	 naf.firstf_i = TIF_NONE;
	 naf.tickf_i  = TIF_NONE;
	 naf.lastf_i  = TIF_BURIED_DESTRUCT;
	 naf.applyf_i = APF_NONE;
	 
	 create_affect(UNIT_CONTAINS(unit), &naf);

	 if (UNIT_IS_EQUIPPED(UNIT_CONTAINS(unit)))
	   unequip_object(UNIT_CONTAINS(unit));

	 unit_up(UNIT_CONTAINS(unit));
      }

      extract_unit(unit);
   }
}

void tif_valhalla_ret(struct unit_affected_type *af, struct unit_data *unit)
{
   if (!IS_PC(unit))
     return;

   CHAR_POS(unit) = POSITION_STANDING;
   REMOVE_BIT(PC_FLAGS(unit), PC_SPIRIT);

   act("\n\rYou have a strange feeling...\n\r",A_ALWAYS,unit,0,0,TO_CHAR);
   act("$1n materializes and vanish.",A_HIDEINV,unit,0,0,TO_ROOM);

   unit_from_unit(unit);
   unit_to_unit(unit, hometown_unit(PC_HOME(unit)));

   act("The spirit of $1n materializes.",A_HIDEINV,unit,0,0,TO_ROOM);
   act("You feel dizzy.",A_ALWAYS,unit,0,0,TO_CHAR);

   PC_COND(unit, FULL) = 24;
   PC_COND(unit, THIRST) = 24;
   PC_COND(unit, DRUNK) = 0;

   CHAR_MANA(unit)      = mana_limit(unit);
   CHAR_ENDURANCE(unit) = move_limit(unit);
   UNIT_HIT(unit)       = UNIT_MAX_HIT(unit);

   if (!is_destructed(DR_UNIT, unit))
   {
      save_player(unit);
      save_player_contents(unit, TRUE);
   }
}


void tif_jail_wait(struct unit_affected_type *af, struct unit_data *unit)
{
}

/* Get thrown out of jail */
void tif_jail_release(struct unit_affected_type *af, struct unit_data *unit)
{
}


void tif_spl_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more skilled at $2t.",
	 A_ALWAYS, unit, spl_text[af->data[0]], 0, TO_CHAR);
   else
     act("You feel less skilled at $2t.",
	 A_ALWAYS,unit, spl_text[af->data[0]], 0, TO_CHAR);
}

void tif_spl_off(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel less skilled at $2t.",
	 A_ALWAYS, unit, spl_text[af->data[0]], 0, TO_CHAR);
   else
     act("You feel more skilled at $2t.",
	 A_ALWAYS,unit, spl_text[af->data[0]], 0, TO_CHAR);
}

void tif_ski_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more skilled in $2t.",
	 A_ALWAYS, unit, ski_text[af->data[0]], 0, TO_CHAR);
   else
     act("You feel less skilled in $2t.",
	 A_ALWAYS,unit, ski_text[af->data[0]], 0, TO_CHAR);
}

void tif_ski_off(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel less skilled in $2t.",
	 A_ALWAYS, unit, ski_text[af->data[0]], 0, TO_CHAR);
   else
     act("You feel more skilled in $2t.",
	 A_ALWAYS,unit, ski_text[af->data[0]], 0, TO_CHAR);
}

void tif_wpn_on(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel more skilled at the $2t fighting style.",
	 A_ALWAYS, unit, wpn_text[af->data[0]], 0, TO_CHAR);
   else
     act("You feel less skilled at the $2t fighting style.",
	 A_ALWAYS,unit, wpn_text[af->data[0]], 0, TO_CHAR);
}

void tif_wpn_off(struct unit_affected_type *af, struct unit_data *unit)
{
   if (af->data[1] > 0)
     act("You feel less skilled at the $2t fighting style.",
	 A_ALWAYS, unit, wpn_text[af->data[0]], 0, TO_CHAR);
   else
     act("You feel more skilled at the $2t fighting style.",
	 A_ALWAYS,unit, wpn_text[af->data[0]], 0, TO_CHAR);
}

void tif_armour_on(struct unit_affected_type *af, struct unit_data *unit)
{
   const char *c = "pale skin like";

   switch (CHAR_NATURAL_ARMOUR(unit))
   {
     case ARM_LEATHER:
      c = "a dark leather like";
      break;

     case ARM_HLEATHER:
      c = "a dark bark like";
      break;

     case ARM_CHAIN:
      c = "an ebony coloured bone like";
      break;

     case ARM_PLATE:
      c = "a grey stone like";
      break;

     default:
      af->data[0] = ARM_CLOTHES;
      break;
   }

   act("Your skin transforms into $2t substance.",
       A_ALWAYS, unit, c, 0, TO_CHAR);

   act("$1n's skin transforms into $2t substance.",
       A_ALWAYS, unit, c, 0, TO_ROOM);
}

void tif_speed_on(struct unit_affected_type *af, struct unit_data *unit)
{
   act("You feel faster...",
       A_ALWAYS, unit, 0, 0, TO_CHAR);
}

void tif_speed_off(struct unit_affected_type *af, struct unit_data *unit)
{
   act("You feel slower...",
       A_ALWAYS, unit, 0, 0, TO_CHAR);
}

/* --------------------------------------------------------------------- */
void tif_reward_on(struct unit_affected_type *af, struct unit_data *unit);
void tif_reward_off(struct unit_affected_type *af, struct unit_data *unit);

struct tick_function_type tif[] =
{
   {"Decay Corpse",tif_decay_corpse},
   {"Destroy Corpse",tif_destroy_corpse},
   {"Valhalla Return",tif_valhalla_ret},
   {"Hitpoints On",tif_hit_on},
   {"Hitpoints Off", tif_hit_off},
   {"Magic Mod On", tif_mag_on},
   {"Magic Mod Off", tif_mag_off},
   {"Divine Mod On", tif_div_on},
   {"Divine Mod Off", tif_div_off},
   {"Strength Mod On", tif_str_on},
   {"Strength Mod Off", tif_str_off},
   {"Dexterity Mod On", tif_dex_on},
   {"Dexterity Mod Off", tif_dex_off},
   {"Constitution Mod On", tif_con_on},
   {"Constitution Mod On", tif_con_off},
   {"Charisma Mod On", tif_cha_on},
   {"Charisma Mod Off", tif_cha_off},
   {"Brain Mod On", tif_bra_on},
   {"Brain Mod Off", tif_bra_off},
   {"XXX", tif_armour_on},
   {"XXX", tif_armour_on},
   {"Spell-skill Mod On", tif_protect_on},
   {"Spell-skill Mod Off", tif_protect_off},
   {"Poison On", tif_poison_on},
   {"Poison Off", tif_poison_off},
   {"Poison Suffer", tif_poison_suffer},
   {"Light/Dark Add", tif_light_add},
   {"Light/Dark Sub", tif_light_sub},
   {"Torch burning countdown", tif_torch_tick},
   {"Eyes tingles", tif_eyes_tingle},
   {"Blind On", tif_blind_on},
   {"Blind Off", tif_blind_off},
   {"Fear Tick Check", tif_fear_check},
   {"Invisibility On", tif_invisibility_on},
   {"Invisibility Off", tif_invisibility_off},
   {"Sneak on", tif_sneak_on},
   {"Sneak off", tif_sneak_off},
   {"Sneak tick", tif_sneak_tick},
   {"Hide on", tif_hide_on},
   {"Hide off", tif_hide_off},
   {"Nide tick", tif_hide_tick},
   {"Nohide tick", tif_nohide_tick},
   {"sleep on", tif_sleep_on},
   {"sleep check", tif_sleep_check},
   {"sleep off", tif_sleep_off},
   {"bless on", tif_bless_on},
   {"bless tick", tif_bless_tick},
   {"bless off", tif_bless_off},
   {"curse on", tif_curse_on},
   {"curse off", tif_curse_off},
   {"sancruary on", tif_sanctuary_on},
   {"sanctuary tick", tif_sanctuary_tick},
   {"sanctuary off", tif_sanctuary_off},
   {"insanity on", tif_insanity_on},
   {"insanity tick", tif_insanity_tick},
   {"insanity off", tif_insanity_off},
   {"protect from evil on", tif_prot_evil_on},
   {"protect from evil off", tif_prot_evil_off},
   {"sustain on", tif_sustain_on},
   {"sustain tick", tif_sustain_tick},
   {"sustain off", tif_sustain_off},
   {"jail waiting",tif_jail_wait},
   {"jail release",tif_jail_release},
   {"plague on",tif_plague_on},
   {"plague tick",tif_plague_tick},
   {"plague off",tif_plague_off},
   {"reward on", tif_reward_on},
   {"reward off", tif_reward_off},
   {"spell on", tif_spl_on},
   {"spell off", tif_spl_off},
   {"skill on", tif_ski_on},
   {"skill off", tif_ski_off},
   {"weapon on", tif_wpn_on},
   {"weapon off", tif_wpn_off},
   {"confusion on", tif_confusion_on},
   {"confusion off", tif_confusion_off},
   {"confusion tick", tif_confusion_tick},
   {"buried destruct", tif_buried_destruct},
   {"protection from good on", tif_prot_good_on},
   {"protection from good off", tif_prot_good_off},
   {"natural armour on", tif_armour_on},
   {"natural armour off", tif_armour_on},
   {"speed better", tif_speed_on},
   {"speed worse", tif_speed_off}
};
