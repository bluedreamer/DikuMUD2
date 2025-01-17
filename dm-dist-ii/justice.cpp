#include "justice.h"

#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "dijkstra.h"
#include "fight.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "movement.h"
#include "skills.h"
#include "structs.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "unit_vector_data.h"
#include "utility.h"
#include "utils.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

/* These are constant for all guards.                            */

#define CUFFS_ZONE "basis"
#define CUFFS_NAME "hand_cuffs"

/* ACCUSELOC_NAME must be the same room name in all zones.       */

#define ACCUSELOC_NAME "accuse_room"

/* The name of the safe where items are stored. The same safe could be
   loaded in different zones. */

#define SAFE_NAME "safe"

static int crime_serial_no = 0;

struct char_crime_data
{
   uint32_t                crime_nr;            /* global unique crime number  */
   uint8_t                 ticks_to_neutralize; /* ticks before crime deletes  */

   int                     id;                         /* id of offender PC           */
   char                    name_criminal[PC_MAX_NAME]; /* Name of offender            */

   char                    victim[31]; /* name of victim              */
   uint8_t                 crime_type; /* what crime? (kill, theft..) */
   uint8_t                 reported;   /* Has crime been reported?    */
   struct char_crime_data *next;       /* link->                      */
};

struct char_crime_data *crime_list = nullptr;

void                    offend_legal_state(unit_data *ch, unit_data *victim)
{
   if(!IS_SET(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE))
   {
      if((CHAR_COMBAT(victim) == nullptr) && !IS_SET(CHAR_FLAGS(victim), CHAR_LEGAL_TARGET))
      {
         SET_BIT(CHAR_FLAGS(victim), CHAR_SELF_DEFENCE);
      }
   }

   /* Test for LEGAL_TARGET bit */
   if(IS_SET(CHAR_FLAGS(victim), CHAR_PROTECTED) && !IS_SET(CHAR_FLAGS(victim), CHAR_LEGAL_TARGET) &&
      !IS_SET(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE))
   {
      SET_BIT(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET);
   }
}

/* ---------------------------------------------------------------------- */
/*                    M O V E M E N T   F U N C T I O N S                 */
/* ---------------------------------------------------------------------- */

/* SFUN_NPC_VISIT_ROOM. Used by npc_set_visit() to move back and    */
/* forth.                                                           */
/*                                                                  */
auto npc_visit_room(struct spec_arg *sarg) -> int
{
   visit_data *vd;
   int         i;

   vd = (visit_data *)sarg->fptr->data;

   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if(vd->data != nullptr)
      {
         (*vd->what_now)(nullptr, vd); /* Give it a chance to free data */

         free(vd);
         sarg->fptr->data = nullptr;
      }
      return SFR_BLOCK;
   }

   if(sarg->cmd->no != CMD_AUTO_TICK)
   {
      return vd->non_tick_return;
   }

   i = npc_move(sarg->owner, vd->go_to);

   if(i == MOVE_DEAD)
   { /* Npc died */
      return SFR_BLOCK;
   }

   if(i == MOVE_FAILED)
   {
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   if((i == MOVE_GOAL) && CHAR_IS_READY(sarg->owner)) /* Reached dest? */
   {
      /* We may not be finished walking - set to 0 */

      if((i = (*vd->what_now)(sarg->owner, vd)) == DESTROY_ME)
      {
         destroy_fptr(sarg->owner, sarg->fptr);
         return SFR_BLOCK;
      }

      /* vd->go_to = 0; */

      return i;
   }

   return SFR_SHARE; /* No other tick functions allowed (PRIORITY SET) */
}

/* Initiate a NPC to go to 'dest_room', call the 'what_now' and then */
/* return to its original room                                      */
/* The *data may be any datapointer which what_now can use          */
/*                                                                  */
void npc_set_visit(unit_data *npc, unit_data *dest_room, int what_now(const unit_data *, visit_data *), void *data, int non_tick_return)
{
   unit_data  *u;
   visit_data *vd;
   unit_fptr  *fp1;
   unit_fptr  *fp2;

   assert(IS_ROOM(dest_room));

   u = unit_room(npc);

   CREATE(vd, visit_data, 1);

   vd->non_tick_return = non_tick_return;
   vd->state           = 0;
   vd->start_room      = u;
   vd->dest_room       = dest_room;
   vd->go_to           = dest_room;
   vd->what_now        = what_now;
   vd->data            = data;

   create_fptr(npc, SFUN_NPC_VISIT_ROOM, WAIT_SEC * 5, SFB_PRIORITY | SFB_RANTIME | SFB_TICK, vd);

   /* Now move this created fptr down below SFUN_PROTECT_LAWFUL */
   fp1 = find_fptr(npc, SFUN_PROTECT_LAWFUL);

   if(fp1 != nullptr)
   {
      fp2 = UNIT_FUNC(npc);
      assert(fp2->next);
      UNIT_FUNC(npc) = UNIT_FUNC(npc)->next;
      fp2->next      = fp1->next;
      fp1->next      = fp2;
   }
}

/* ---------------------------------------------------------------------- */
/*                    A C C U S E   F U N C T I O N S                     */
/* ---------------------------------------------------------------------- */

/* The witness list takes care of itself */
void remove_crime(struct char_crime_data *crime)
{
   struct char_crime_data *c;

   if(crime == crime_list)
   {
      crime_list = crime->next;
   }
   else
   {
      for(c = crime_list; c->next != nullptr; c = c->next)
      {
         if(c->next == crime)
         {
            c->next = c->next->next;
            break;
         }
      }
   }

   free(crime);
}

auto new_crime_serial_no() -> int
{
   FILE *file;

   /* read next serial number to be assigned to crime */
   if((file = fopen_cache(str_cc(libdir, CRIME_NUM_FILE), "r+")) == nullptr)
   {
      slog(LOG_OFF, 0, "Can't open file 'crime-nr'");
      assert(FALSE);
   }

   rewind(file);
   int ms2020 = fscanf(file, "%d", &crime_serial_no);
   crime_serial_no++;
   rewind(file);
   fprintf(file, "%d", crime_serial_no);

   fflush(file);

   /* No fclose(file), using cache */

   return crime_serial_no;
}

void set_reward_char(unit_data *ch, int crimes)
{
   unit_affected_type *paf;
   unit_affected_type  af;
   int                 xp   = 0;
   int                 gold = 0;

   auto                lose_exp(unit_data * ch)->int;

   /* Just to make sure in case anyone gets randomly rewarded */
   REMOVE_BIT(CHAR_FLAGS(ch), CHAR_PROTECTED);
   SET_BIT(CHAR_FLAGS(ch), CHAR_OUTLAW);

   if((paf = affected_by_spell(ch, ID_REWARD)) != nullptr)
   {
      paf->data[0] = std::max(xp, paf->data[0]);
      paf->data[1] = std::max(gold, paf->data[1]);
      paf->data[2]++;
      return;
   }

   gold = money_round_up(CHAR_LEVEL(ch) * CHAR_LEVEL(ch) * 100, DEF_CURRENCY, 1);
   xp   = CHAR_LEVEL(ch) * 20 + 50;

   if(IS_PC(ch))
   {
      PC_CRIMES(ch) += crimes;
      xp = std::min(lose_exp(ch) / 2, xp);
   }

   af.id       = ID_REWARD;
   af.duration = 1;
   af.beat     = 0;
   af.firstf_i = TIF_REWARD_ON;
   af.tickf_i  = TIF_NONE;
   af.lastf_i  = TIF_REWARD_OFF;
   af.applyf_i = APF_NONE;

   af.data[0]  = xp;
   af.data[1]  = gold;
   af.data[2]  = 1;

   create_affect(ch, &af);
}

void set_witness(unit_data *criminal, unit_data *witness, int no, int type, int show = static_cast<int>(TRUE))
{
   unit_affected_type af;

   void               activate_accuse(unit_data * npc, uint8_t crime_type, const char *cname);

   if(!IS_PC(criminal))
   {
      return;
   }

   // Dont check for AWAKE here since the dead victim is to be a witness!

   if(witness == criminal)
   {
      return;
   }

   if(IS_NPC(witness) && !IS_SET(CHAR_FLAGS(witness), CHAR_PROTECTED))
   {
      return;
   }

   if(show != 0)
   {
      act(COLOUR_ATTN "You just witnessed a crime committed by $1n." COLOUR_NORMAL, A_ALWAYS, criminal, nullptr, witness, TO_VICT);
   }

   /* Create witness data */
   af.id       = ID_WITNESS;
   af.beat     = WAIT_SEC * 5 * 60; /* Every 5 minutes        */
   af.duration = CRIME_LIFE;
   af.data[0]  = no;
   af.data[1]  = type;
   af.data[2]  = PC_ID(criminal);

   af.firstf_i = TIF_NONE;
   af.tickf_i  = TIF_NONE;
   af.lastf_i  = TIF_NONE;
   af.applyf_i = APF_NONE;

   create_affect(witness, &af);

   if(IS_NPC(witness) && (show != 0))
   {
      activate_accuse(witness, type, UNIT_NAME(criminal));
   }
}

void add_crime(unit_data *criminal, unit_data *victim, int type)
{
   struct char_crime_data *crime;

   assert(IS_PC(criminal));

   if(static_cast<unsigned int>(str_is_empty(UNIT_NAME(criminal))) != 0U)
   {
      slog(LOG_ALL, 0, "JUSTICE: NULL name in criminal");
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(UNIT_NAME(victim))) != 0U)
   {
      slog(LOG_ALL, 0, "JUSTICE: NULL name in victim");
      return;
   }

   /* add new crime crime_list */
   CREATE(crime, struct char_crime_data, 1);
   crime->next     = crime_list;
   crime_list      = crime;

   crime->crime_nr = new_crime_serial_no();
   crime->id       = PC_ID(criminal);
   strcpy(crime->name_criminal, UNIT_NAME(criminal));
   strncpy(crime->victim, UNIT_NAME(victim), 30);
   crime->crime_type          = type;
   crime->ticks_to_neutralize = CRIME_LIFE + 2;
   crime->reported            = static_cast<uint8_t>(FALSE);
}

auto crime_victim_name(int crime_no, int id) -> const char *
{
   struct char_crime_data *c;

   for(c = crime_list; c != nullptr; c = c->next)
   {
      if((c->crime_nr == (uint32_t)crime_no) && (c->id == id))
      {
         return c->name_criminal;
      }
   }

   return "";
}

void log_crime(unit_data *criminal, unit_data *victim, uint8_t crime_type, bool active)
{
   int i;
   int j;

   /* When victim is legal target you can't get accused from it. */
   if(IS_SET(CHAR_FLAGS(victim), CHAR_LEGAL_TARGET) && ((crime_type == CRIME_MURDER) || (crime_type == CRIME_PK)))
   {
      return;
   }

   scan4_unit(victim, UNIT_ST_PC | UNIT_ST_NPC);

   if(criminal != nullptr)
   {
      if(!IS_PC(criminal))
      {
         return;
      }

      add_crime(criminal, victim, crime_type);

      set_witness(criminal, victim, crime_serial_no, crime_type, active);

      for(i = 0; i < unit_vector.top; i++)
      {
         if(CHAR_CAN_SEE(UVI(i), criminal))
         {
            set_witness(criminal, UVI(i), crime_serial_no, crime_type, active);
         }
      }
      return;
   }

   for(j = 0; j < unit_vector.top; j++)
   {
      if(IS_PC(UVI(j)))
      {
         if((CHAR_COMBAT(UVI(j)) != nullptr) && (CHAR_COMBAT(UVI(j))->FindOpponent(victim) != nullptr))
         {
            add_crime(UVI(j), victim, crime_type);

            for(i = 0; i < unit_vector.top; i++)
            {
               if(CHAR_CAN_SEE(UVI(i), UVI(j)))
               {
                  set_witness(UVI(j), UVI(i), crime_serial_no, crime_type, active);
               }
            }
         }
      }
   }
}

/* Got to have this loaded somewhere */

void save_accusation(struct char_crime_data *crime, const unit_data *accuser)
{
   FILE *file;

   if((file = fopen_cache(str_cc(libdir, CRIME_ACCUSE_FILE), "a+")) == nullptr)
   {
      slog(LOG_OFF, 0, "register_crime: can't open file.");
      assert(FALSE);
   }

   time_t t = time(nullptr);

   fprintf(file,
           "%5u  %4d  %4d %1d [%s]  [%s]   %12lu %s",
           crime->crime_nr,
           crime->id,
           crime->crime_type,
           crime->reported,
           UNIT_NAME((unit_data *)accuser),
           crime->victim,
           t,
           ctime(&t));
   fflush(file);
   /* Was fclose(file) */
}

static void crime_counter(unit_data *criminal, int incr, int first_accuse)
{
   if((PC_CRIMES(criminal) + incr) / CRIME_NONPRO > PC_CRIMES(criminal) / CRIME_NONPRO)
   {
      if(!IS_SET(CHAR_FLAGS(criminal), CHAR_OUTLAW))
      {
         REMOVE_BIT(CHAR_FLAGS(criminal), CHAR_PROTECTED);
      }
   }

   if((PC_CRIMES(criminal) + incr) / CRIME_OUTLAW > PC_CRIMES(criminal) / CRIME_OUTLAW)
   {
      SET_BIT(CHAR_FLAGS(criminal), CHAR_OUTLAW | CHAR_PROTECTED);
   }

   /* I see no reason for first_accuse????
      if (first_accuse &&
       ((PC_CRIMES(criminal)+incr) / CRIME_REWARD >
        PC_CRIMES(criminal) / CRIME_REWARD)) */

   if(((PC_CRIMES(criminal) + incr) / CRIME_REWARD > PC_CRIMES(criminal) / CRIME_REWARD))
   {
      // PC_CRIMES(criminal) += incr done in set_reward!
      set_reward_char(criminal, incr);
   }
   else
   {
      PC_CRIMES(criminal) += incr;
   }
}

static void update_criminal(const unit_data *deputy, const char *pPlyName, int pidx, struct char_crime_data *crime, int first_accuse)
{
   unit_data *criminal = nullptr;
   int        loaded   = static_cast<int>(FALSE);
   int        incr;

   void       save_player_file(unit_data * pc);

   /* Modified find_descriptor */
   for(criminal = unit_list; criminal != nullptr; criminal = criminal->gnext)
   {
      if(IS_PC(criminal) && PC_ID(criminal) == pidx)
      {
         act("$1n tells you, 'You are in trouble, you good-for-nothing ...'", A_SOMEONE, deputy, nullptr, criminal, TO_VICT);
         break;
      }
   }

   /* These should be protectedmob<lawenforcer<pc * crimeseriousness */
   if(first_accuse != 0)
   {
      incr = crime->crime_type;
   }
   else
   {
      incr = CRIME_EXTRA;
   }

   if(criminal == nullptr)
   {
      criminal = load_player(pPlyName);
      if(criminal == nullptr)
      {
         return;
      }

      loaded = static_cast<int>(TRUE);
   }

   if(criminal != nullptr)
   {
      crime_counter(criminal, incr, first_accuse);
   }

   if(loaded != 0)
   {
      descriptor_data *d = find_descriptor(pPlyName, nullptr);

      if((d != nullptr) && (d->character != nullptr))
      {
         crime_counter(d->character, incr, first_accuse);
      }

      save_player_file(criminal);
      extract_unit(criminal);
   }
}

auto accuse(struct spec_arg *sarg) -> int
{
   unit_affected_type     *af;
   struct char_crime_data *crime;

   int                     crime_type = 0; /* {CRIME_MURDER,CRIME_STEALING} */
   char                    arg1[80];
   char                    arg2[80]; /* will hold accused and crime    */
   int                     pid;

   auto                    find_player_id(char *)->int;

   /* legal command ? */
   if(static_cast<unsigned int>(is_command(sarg->cmd, "accuse")) == 0U)
   {
      return SFR_SHARE;
   }

   strcpy(arg1, UNIT_NAME(sarg->owner));

   if(IS_NPC(sarg->activator) && CHAR_POS(sarg->owner) == POSITION_SLEEPING)
   {
      do_wake(sarg->activator, arg1, sarg->cmd);
   }

   if(CHAR_POS(sarg->owner) < POSITION_SLEEPING || CHAR_POS(sarg->owner) == POSITION_FIGHTING)
   {
      act("$1n seems busy right now.", A_SOMEONE, sarg->owner, sarg->activator, nullptr, TO_ROOM);
      return SFR_BLOCK;
   }

   /* legal args ? */
   argument_interpreter(sarg->arg, arg1, arg2); /* extract args from arg :) */

   if(static_cast<unsigned int>(str_is_empty(arg1)) != 0U)
   {
      act("$1n says, 'Yes... who?'", A_SOMEONE, sarg->owner, sarg->activator, nullptr, TO_ROOM);
      return SFR_BLOCK;
   }

   if(str_is_empty(arg2))
   {
      act("$1n says, 'What do you wish to accuse $3t of?'", A_SOMEONE, sarg->owner, sarg->activator, arg1, TO_ROOM);
      return SFR_BLOCK;
   }

   /* ok, what does it want ? */
   if((strcmp(arg2, "murder")) == 0)
   {
      crime_type = CRIME_MURDER;
      act("$1n says, 'Murder... lets see', and looks through his files.", A_SOMEONE, sarg->owner, sarg->activator, nullptr, TO_ROOM);
   }
   else if((strcmp(arg2, "stealing")) == 0)
   {
      crime_type = CRIME_STEALING;
      act("$1n says, 'Stealing... lets see', and looks through his files.", A_SOMEONE, sarg->owner, sarg->activator, nullptr, TO_ROOM);
   }
   else
   {
      act("$1n says, 'Are you accusing of murder or stealing?'", A_SOMEONE, sarg->owner, sarg->activator, nullptr, TO_ROOM);
      return SFR_BLOCK;
   }

   if((pid = find_player_id(arg1)) == -1)
   {
      act("$1n says, 'I have never heard of this so called $2t.'", A_SOMEONE, sarg->owner, arg1, sarg->activator, TO_ROOM);
      return SFR_BLOCK;
   }

   act("$1n accuses $3t of $2t.", A_SOMEONE, sarg->activator, arg2, arg1, TO_ROOM);
   act("$1n says, 'Ah yes... $2t'", A_SOMEONE, sarg->owner, arg2, sarg->activator, TO_ROOM);

   for(crime = crime_list; crime != nullptr; crime = crime->next)
   {
      if(pid == crime->id)
      {
         if((crime_type == CRIME_MURDER) && (crime->crime_type != CRIME_MURDER) && (crime->crime_type != CRIME_PK))
         {
            continue;
         }

         if((crime_type == CRIME_STEALING) && (crime->crime_type != CRIME_STEALING))
         {
            continue;
         }

         /* Check the witness */
         for(af = UNIT_AFFECTED(sarg->activator); af != nullptr; af = af->next)
         {
            if((af->id == ID_WITNESS) && (af->data[0] == (int)crime->crime_nr))
            {
               act("$1n says, 'Thank you very much $3N, I will stop $2t.'", A_SOMEONE, sarg->owner, arg1, sarg->activator, TO_ROOM);

               if((crime->reported) == 0U)
               {
                  update_criminal(sarg->owner, arg1, pid, crime, static_cast<int>(TRUE));
               }
               else
               {
                  update_criminal(sarg->owner, arg1, pid, crime, static_cast<int>(FALSE));
               }

               /* Mark crime as already reported */
               crime->reported = static_cast<uint8_t>(TRUE);
               save_accusation(crime, sarg->activator);

               if(UNIT_ALIGNMENT(sarg->activator) > -1000)
               {
                  UNIT_ALIGNMENT(sarg->activator) += 100;
               }

               destroy_affect(af);

               return SFR_BLOCK;
            }
         }
      }
   }

   act("$1n says, 'Sorry $3n, but I don't find your evidence convincing.'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);

   return SFR_BLOCK;
}

void update_crimes()
{
   struct char_crime_data *c;
   struct char_crime_data *next_dude;

   for(c = crime_list; c != nullptr; c = next_dude)
   {
      next_dude = c->next;
      if((--(c->ticks_to_neutralize)) <= 0)
      {
         remove_crime(c);
      }
   }
}

/* ---------------------------------------------------------------------- */
/*                      A C C U S E   F U N C T I O N S                   */
/* ---------------------------------------------------------------------- */

struct npc_accuse_data
{
   char   *criminal_name;
   uint8_t crime_type;
   int     was_wimpy;
};

/* For use with the walk.c system. When at captain accuse the criminal */
/* and then return to previous duties                                  */
/*                                                                     */
auto npc_accuse(const unit_data *npc, visit_data *vd) -> int
{
   char                    str[80];
   unit_affected_type     *af;
   struct npc_accuse_data *nad;

   nad = (struct npc_accuse_data *)vd->data;

   if(npc == nullptr)
   {
      free(nad->criminal_name);
      free(nad);
      vd->data = nullptr;
      return DESTROY_ME;
   }

   switch(vd->state)
   {
      case 0:
         af = affected_by_spell(npc, ID_WITNESS);
         if(af == nullptr)
         {
            vd->state++;
            return SFR_BLOCK;
         }

         strcpy(str, "accuse ");

         strcat(str, crime_victim_name(af->data[0], af->data[2]));

         if((af->data[1] == CRIME_MURDER) || (af->data[1] == CRIME_PK))
         {
            strcat(str, " murder");
         }
         else
         {
            strcat(str, " stealing");
         }
         command_interpreter((unit_data *)npc, str);
         return SFR_BLOCK;

      case 1:
         if(nad->was_wimpy == 0)
         {
            REMOVE_BIT(CHAR_FLAGS(npc), CHAR_WIMPY);
         }
         vd->go_to = vd->start_room;
         vd->state++;
         return SFR_BLOCK;
   }

   return DESTROY_ME;
}

void activate_accuse(unit_data *npc, uint8_t crime_type, const char *cname)
{
   struct npc_accuse_data *nad;
   unit_data              *prison;
   unit_fptr              *fptr;
   visit_data             *vd;

   /* GEN: get accuse room in here */
   /* How to find the nearest accuse room? */
   /* Name of the accusation location must be the same everywhere!  */

   /* If we are already on the way to accuse, it will take care
      of everything */
   if(((fptr = find_fptr(npc, SFUN_NPC_VISIT_ROOM)) != nullptr) && (affected_by_spell(npc, ID_WITNESS) != nullptr))
   {
      vd = (visit_data *)fptr->data;
      if((vd != nullptr) && (vd->what_now == npc_accuse) && (vd->state == 0))
      {
         return; /* Do nothing */
      }
   }

   /* We don't want captain to accuse, he can't activate himself! */
   if(find_fptr(npc, SFUN_ACCUSE) != nullptr)
   {
      return;
   }

   if(((prison = world_room(UNIT_IN(npc)->fi->zone->name, ACCUSELOC_NAME)) != nullptr) ||
      ((prison = world_room(npc->fi->zone->name, ACCUSELOC_NAME)) != nullptr))
   {
      CREATE(nad, struct npc_accuse_data, 1);
      nad->criminal_name = str_dup(cname);
      nad->crime_type    = crime_type;
      nad->was_wimpy     = IS_SET(CHAR_FLAGS(npc), CHAR_WIMPY);

      SET_BIT(CHAR_FLAGS(npc), CHAR_WIMPY);

      npc_set_visit(npc, prison, npc_accuse, nad, SFR_SHARE);
   }
   else
   {
      act("$1n complains about the lack of law and order in this place.", A_HIDEINV, npc, nullptr, nullptr, TO_ROOM);
   }
}

/* ---------------------------------------------------------------------- */
/*                      A R R E S T   F U N C T I O N S                   */
/* ---------------------------------------------------------------------- */

static auto crime_in_progress(unit_data *att, unit_data *def) -> int
{
   if((att != nullptr) && (def != nullptr))
   {
      /* If the attacker is attacking someone protected, or if the
         attacker is protected and is attacking someone non-protected
         then go in action */

      if(!IS_SET(CHAR_FLAGS(att), CHAR_SELF_DEFENCE) &&
         (((IS_SET(CHAR_FLAGS(def), CHAR_PROTECTED) && !IS_SET(CHAR_FLAGS(def), CHAR_LEGAL_TARGET)) ||
           (!IS_SET(CHAR_FLAGS(att), CHAR_PROTECTED) && IS_SET(CHAR_FLAGS(def), CHAR_PROTECTED)))))
      {
         return static_cast<int>(TRUE);
      }
   }
   return static_cast<int>(FALSE);
}

/* Help another friendly guard! :-) */
/*                                  */
auto guard_assist(const unit_data *npc, visit_data *vd) -> int
{
   char mbuf[MAX_INPUT_LENGTH] = {0};
   switch(vd->state++)
   {
      case 0:
         strcpy(mbuf, "peer");
         command_interpreter((unit_data *)npc, mbuf);
         return SFR_BLOCK;

      case 1: /* Just wait a little while... */
         return SFR_BLOCK;

      case 2:
         if(CHAR_COMBAT(npc) != nullptr)
         {
            vd->state = 2;
            return SFR_BLOCK;
         }
         vd->go_to = vd->start_room;
         return SFR_BLOCK;
   }

   /* We are done, kill me! */
   return DESTROY_ME;
}

/* 'Guard' needs help. Call his friends... :-)   */
/*                                               */
void call_guards(unit_data *guard)
{
   struct zone_type *zone;
   unit_fptr        *fptr;
   unit_data        *u;
   int               ok;

   if(!IS_ROOM(UNIT_IN(guard)))
   {
      return;
   }

   zone = unit_zone(guard);

   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_NPC(u) && IS_ROOM(UNIT_IN(u)) && zone == UNIT_FILE_INDEX(UNIT_IN(u))->zone && u != guard)
      {
         ok = static_cast<int>(FALSE);
         for(fptr = UNIT_FUNC(u); fptr != nullptr; fptr = fptr->next)
         {
            if(fptr->index == SFUN_PROTECT_LAWFUL)
            {
               ok = static_cast<int>(TRUE);
            }
            else if(fptr->index == SFUN_NPC_VISIT_ROOM)
            {
               ok = static_cast<int>(FALSE);
               break;
            }
         }
         if((ok != 0) && (number(0, 5) == 0) && (find_fptr(u, SFUN_ACCUSE) == nullptr))
         {
            npc_set_visit(u, UNIT_IN(guard), guard_assist, nullptr, SFR_SHARE);
         }
      }
   }
}

/* This routine protects lawful characters                               */
/* SFUN_PROTECT_LAWFUL                                                   */
auto protect_lawful(struct spec_arg *sarg) -> int
{
   int i;

   if((sarg->cmd->no < CMD_AUTO_DEATH) || !CHAR_AWAKE(sarg->owner) || (CHAR_COMBAT(sarg->owner) != nullptr) ||
      sarg->activator == sarg->owner)
   {
      return SFR_SHARE;
   }

   if((sarg->cmd->no == CMD_AUTO_TICK))
   {
      scan4_unit(sarg->owner, UNIT_ST_NPC); /* Implicit can see */

      for(i = 0; i < unit_vector.top; i++)
      {
         if(CHAR_CAN_SEE(sarg->owner, UVI(i)) && ((find_fptr(UVI(i), SFUN_AGGRESSIVE) != nullptr) ||
                                                  ((find_fptr(UVI(i), SFUN_AGGRES_REVERSE_ALIGN) != nullptr) && UNIT_IS_EVIL(UVI(i)))))
         {
            SET_BIT(CHAR_FLAGS(UVI(i)), CHAR_LEGAL_TARGET);
            act("$1n blows in a small whistle!  'UUIIIIIIIHHHHH'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
            call_guards(sarg->owner);
            simple_one_hit(sarg->owner, UVI(i));
            REMOVE_BIT(CHAR_FLAGS(UVI(i)), CHAR_SELF_DEFENCE);
            return SFR_BLOCK;
         }
      }
   }
   else
   {
      if(sarg->cmd->no == CMD_AUTO_DEATH)
      {
         if(crime_in_progress(CHAR_FIGHTING(sarg->activator), sarg->activator) != 0)
         {
            simple_one_hit(sarg->owner, CHAR_FIGHTING(sarg->activator));
            REMOVE_BIT(CHAR_FLAGS(sarg->activator), CHAR_SELF_DEFENCE);
            return SFR_BLOCK;
         }
      }
      else /* COMBAT */
      {
         if(crime_in_progress(sarg->activator, CHAR_FIGHTING(sarg->activator)) != 0)
         {
            simple_one_hit(sarg->owner, sarg->activator);
            REMOVE_BIT(CHAR_FLAGS(sarg->activator), CHAR_SELF_DEFENCE);
            return SFR_BLOCK;
         }
      }
   }

   return SFR_SHARE;
}

/* This routine blows the whistle when a crime is in progress            */
/* SFUN_WHISTLE                                                          */
auto whistle(struct spec_arg *sarg) -> int
{
   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      sarg->fptr->data = nullptr;
      return SFR_SHARE;
   }

   if((sarg->cmd->no < CMD_AUTO_COMBAT) || (sarg->activator == nullptr) || !IS_CHAR(sarg->activator))
   {
      return SFR_SHARE;
   }

   if(CHAR_POS(sarg->activator) < POSITION_STUNNED)
   {
      return SFR_SHARE;
   }

   if(sarg->fptr->data != nullptr)
   {
      if(scan4_ref(sarg->owner, (unit_data *)sarg->fptr->data) == nullptr)
      {
         sarg->fptr->data = nullptr;
      }
      else
      {
         return SFR_SHARE;
      }
   }

   if(CHAR_AWAKE(sarg->owner) && (CHAR_COMBAT(sarg->activator) != nullptr) && CHAR_CAN_SEE(sarg->owner, sarg->activator))
   {
      if(crime_in_progress(sarg->activator, CHAR_FIGHTING(sarg->activator)) != 0)
      {
         sarg->fptr->data = sarg->activator;

         act("$1n blows in a small whistle!  'UUIIIIIIIHHHHH'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
         call_guards(sarg->owner);
         simple_one_hit(sarg->owner, sarg->activator);
         REMOVE_BIT(CHAR_FLAGS(sarg->activator), CHAR_SELF_DEFENCE);
         return SFR_SHARE;
      }
   }

   return SFR_SHARE;
}

/* -------------------------------------------------------------- */

auto reward_give(struct spec_arg *sarg) -> int
{
   void                gain_exp(unit_data * ch, int gain);

   unit_data          *u;
   unit_affected_type *paf;
   currency_t          cur;

   if(sarg->cmd->no != CMD_GIVE)
   {
      return SFR_SHARE;
   }

   u = UNIT_CONTAINS(sarg->owner);

   do_give(sarg->activator, (char *)sarg->arg, sarg->cmd);

   if(UNIT_CONTAINS(sarg->owner) == u)
   { /* Was it given nothing? */
      return SFR_BLOCK;
   }

   if((paf = affected_by_spell(UNIT_CONTAINS(sarg->owner), ID_REWARD)) == nullptr)
   {
      act("$1n says, 'Thank you $3n, that is very nice of you.'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      return SFR_BLOCK;
   }

   act("$1n says, '$3n, receieve this as a token of our gratitude.'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);

   cur = local_currency(sarg->owner);

   if(IS_PC(sarg->activator))
   {
      gain_exp(sarg->activator, std::min(level_xp(CHAR_LEVEL(sarg->activator)), paf->data[0]));
   }

   money_to_unit(sarg->activator, paf->data[1], cur);

   extract_unit(UNIT_CONTAINS(sarg->owner));

   return SFR_BLOCK;
}

auto reward_board(struct spec_arg *sarg) -> int
{
   unit_data          *u;
   unit_affected_type *af    = nullptr;
   int                 found = static_cast<int>(FALSE);
   char                buf[256];
   char               *c = (char *)sarg->arg;

   if(sarg->cmd->no != CMD_LOOK)
   {
      return SFR_SHARE;
   }

   if(find_unit(sarg->activator, &c, nullptr, FIND_UNIT_SURRO) != sarg->owner)
   {
      return SFR_SHARE;
   }

   act("$1n looks at the board of rewards.", A_ALWAYS, sarg->activator, nullptr, nullptr, TO_ROOM);

   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_CHAR(u))
      {
         if((af = affected_by_spell(u, ID_REWARD)) != nullptr)
         {
            found = static_cast<int>(TRUE);
            sprintf(buf,
                    "%s (%s) wanted dead for %d xp "
                    "and %s (%d crimes).\n\r",
                    UNIT_NAME(u),
                    IS_PC(u) ? "Player" : "Monster",
                    af->data[0],
                    money_string(af->data[1], local_currency(sarg->owner), FALSE),
                    af->data[2]);
            send_to_char(buf, sarg->activator);
         }
         else if(IS_SET(CHAR_FLAGS(u), CHAR_OUTLAW) && IS_SET(CHAR_FLAGS(u), CHAR_PROTECTED))
         {
            sprintf(buf, "%s (%s) wanted alive for imprisonment.\n\r", UNIT_NAME(u), IS_PC(u) ? "Player" : "Monster");
            send_to_char(buf, sarg->activator);
            found = static_cast<int>(TRUE);
         }
         else if(IS_SET(CHAR_FLAGS(u), CHAR_OUTLAW))
         {
            sprintf(buf, "%s (%s) wanted dead or alive.\n\r", UNIT_NAME(u), IS_PC(u) ? "Player" : "Monster");
            send_to_char(buf, sarg->activator);
            found = static_cast<int>(TRUE);
         }
      }
   }

   if(found == 0)
   {
      send_to_char("No rewards currently offered.\n\r", sarg->activator);
   }

   return SFR_BLOCK;
}

void tif_reward_on(unit_affected_type *af, unit_data *unit)
{
   if(IS_CHAR(unit))
   {
      REMOVE_BIT(CHAR_FLAGS(unit), CHAR_PROTECTED);
      SET_BIT(CHAR_FLAGS(unit), CHAR_OUTLAW);

      if(CHAR_AWAKE(unit))
      {
         send_to_char("You feel wanted...\n\r", unit);
         act("$1n suddenly seems a little paranoid.", A_HIDEINV, unit, nullptr, nullptr, TO_ROOM);
      }
   }
   else
   {
      act("You realize that the $1N is perhaps worth something.", A_HIDEINV, unit, nullptr, nullptr, TO_ROOM);
   }
}

void tif_reward_off(unit_affected_type *af, unit_data *unit)
{
   if(IS_CHAR(unit))
   {
      REMOVE_BIT(CHAR_FLAGS(unit), CHAR_OUTLAW);
   }
}

/* -------------------------------------------------------------- */

void boot_justice()
{
   touch_file(str_cc(libdir, CRIME_NUM_FILE));
   touch_file(str_cc(libdir, CRIME_ACCUSE_FILE));
}
