#include "comm.h"
#include "common.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "money.h"
#include "movement.h"
#include "spec_assign.h"
#include "structs.h"
#include "textutil.h"
#include "time_info_data.h"
#include "trie.h"
#include "unit_fptr.h"
#include "unit_vector_data.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"

#include <cctype>
#include <cstdarg> /* va_args in pain_error()        */
#include <cstdio>
#include <cstring>
#include <ctime>

/*

   Description of "Pseudo AI-Interface for NPCs" - 'PAIN'

   Following is the commands as given to the special function:


     A<string>

        TICK: Wait one tick.

        The supplied string will be displayed in the room using the
        act routine - $1X refers to the NPC.

        $1n is the name (title) of the NPC.
        $1s is his/her/its
        $1e is he/she/it
        $1m is him/her/it
        $1N is the first name of the npc.

        Example: "120 AYou notice that $1n appears as if $1e is...@"

     C<string>

        TICK: Wait one tick.

        The command will be sent through the command interpreter.

        Example: "110 Claugh@"

     d<instruction-no><string>

        If the PC found with the 'S' instruction is still in the room
        execute the command-string <string>. Otherwise continue at
        instruction <instruction-no> since the PC is no longer here.

        Example: "155 d150follow %s@"
                 "160 d150say %s, you smell of elderberry!@"
                 "170 d150say %s your brain is the size of a pea!@"

     D<instruction no><direction>

        TICK: Continue with next instruction immediately.

        Tests if a door in the specified direction is closed.
        If there is no door, then it is considered to be closed
        (to avoid an possible endless loop).
        If the internal command was successful, then execution
        continues at instruction number <instruction no>.

        Example: "150 D130north@"

     G<symbolic unit names>

        TICK: Continue with next instruction when done.

        Will cause the NPC to go to the specified location.
        If the location is a room it will go to that room,
        otherwise it will go to the npc/object.
        PAIN Routine will not continue until the NPC has
        reached its goal. NPC will be very keen on opening
        doors/unlocking etc.

        Example: "100 Gmidgaard/cth_nave@"
                 "50 Gelfdom/bones@"

     H<instruction no><string>

        TICK: Continue with next instruction immediately.

        Tests if the npc is has item <string> in inventory, or equipped.
        If the internal command was successful, then execution
        continues at instruction number <instruction no>.

        Example: "140 H110city key@"


     i<command>@<symbolic-npc-name>@

        TICK: Pauses until condition has been satisfied, then
              pauses one tick.

        Waits until the npc <symbolic-npc-name> has executed the
        command <command>, and then continues with next instruction
        immediately. This can be used as a way to make NPC's
        interact with each other. For example the guard says
        ('say' command) 'give me some food for the prisoner'.
        The cook might react on 'guard' and 'say', load some
        food and give it to the guard. Meanwhile the guard
        waits in a loop checking if he has got the food in
        his inventory.

        Example: "170 isay@king_cook@"
                 "180 iask@kingc/king_cook@"


     I<instruction no><string>

        TICK: Continue with next instruction immediately.

        If the unit <string> is in the same room as the npc, then
        execute <instruction no>, otherwise execute next instruction.
        Will not find itself.

        Example: "200 I180city key@"

     J<instruction no>

        TICK: Wait one tick.

        The execution will continue at the instruction <instruction no>.
        First instruction has number one. Will wait for tick after jump.

        Example: "130 J100@"


     L<instruction no><direction>

        TICK: Continue with next instruction immediately.

        Tests if a door in the specified direction is locked.
        If there is no door, then it is considered to be locked
        (to avoid an possible endless loop).
        If the internal command was successful, then execution
        continues at instruction number <instruction no>.

        Example: "160 L170east@"

     l<symbolic name>

        TICK: Continue with next instruction immediately.

        Loads the unit 'symbolic name' into the npc owing the PAIN
        routine. The command is always successful.

        Example: "170 lkingc/gruel@"

     r<symbolic name>!<room>    REMOTE-LOAD NOT YET TESTED

        TICK: Continue with next instruction immediately.

        Loads the unit 'symbolic name' into the specified room.
        The command is always successful (or the PAIN wont exist).

        Example: "170 volcano/lava_warning!volcano/lava_path2"

     R<instruction no>,<num>

        TICK: Continue with next instruction immediately.

        If a random number in the interval [1..100] is less than or
        equals to <num>, then goto <instruction no>, otherwise execute
        next instruction.

        Example: "190 R130,80@"

     S<instruction-no>

        TICK: Continues immediately if PC found.
              Otherwise waits one tick.

        Scans the room for a random Player (PC). If found the reference to
        the player is stored in memory, and the npc remembers this value.
        Otherwise instruction <instruction-no> is executed at next tick.

        Example: "150 S150@"

     T<instruction no>,<value>  : NOT YET TESTED

        Tick: Wait one tick

        A kind of janitor service. Will pick up any object worth
        less than <value>, corpses and drink containers.
        If trash was picked up, then execution continues at
        instruction number <instruction no> (next tick).
        The trash is extracted, this might be changed?

        Example: " 90 T200,50@"


     W<instruction no>,<time>

        TICK: Continue with next instruction immediately.

        If the current time (24 hour system [0..23]) is as specified,
        then immediately execute the instruction <instruction no>.
        Otherwise continue with next instruction at next tick.

        Example: "180 W200,7@"

     *<string>

        TICK: N/A.

        The string is treated as a remark.

        Example: "10 *This is a remark which ends here:@"

     M<instruction no>@<cmd>@<keywords>@<timeout>  : NOT YET TESTED

       TICK: waits until condition is true or timeout

       This instruction waits to receive a message from
       the PC found with the 'S' command.

       It will continue at <instruction no> immediately after
       condition is true.

       If timed out, it will continue with the next
       instruction immediately.

       If there is a message from a PC that matches the one
       found with the 'S' command, and the message is a command
       of type <cmd>, with an argument that contains at least
       one of the <keywords> (seperated with '#') the condition
       is true. If the timeout is passed in (<timeout> ticks)
       the condition is false.

       The command can be any one of the commands, or
       _dead,_combat,_unknown for the special messages.
       if _unknown the command is part of the argument
       examined! if _dead or _combat no argument is
       supplied.

Each command must be prefixed by a line-number (just like basic).

I truly hope some kind soul will make a small compiler from a nicer
syntax to the PAIN language. I'm sure a lot of people, including myself,
would appreciate it. But I have more important stuff to do, like
making arrest routines, and the like. --Michael Seifert

These have not yet been made/or have been deleted

     K<instruction-with-one-%s>

        TICK: Wait one tick.

        Finds random PC in room and executes the instruction
        on it, where the name of the pc it set into the %s.
        If no PC is found, no instruction is executed.

        Example: "150 Ksteal coins from %s"

     P<instruction no><symbolic unit name>

        TICK: Waits one tick.

        Inventory is searched for <symbolic unit name>. If
        <symbolic unit name> is null (none) then the first
        object in inventory is selected. The selected unit
        is then destroyed if it was found.

        If something was purged execution continues at the
        next instruction - otherwise at <instruction no>.

        Examples: "799 P10basis/corpse"
                  "799 P10@"

*/

#define PAIN_SEPARATOR '@'

struct pain_type
{
   uint16_t              top; /* No of commands          */
   uint16_t              idx; /* Current command pointer */
   struct pain_cmd_type *cmds;

   void                 *vars[1]; /* Global Working Variable! */
};

struct pain_cmd_type
{
   int (*func)(unit_data *npc, struct pain_type *pain);

   int32_t gotoline;
   int32_t data[2];
   void   *ptr[2];
};

/* Intended to be used for error-reports */
static unit_data *p_error_unit = nullptr;

/* ---------------------------------------------------------------------- */
/*                      P A I N   F U N C T I O N S                       */
/* ---------------------------------------------------------------------- */

void              pain_error(const char *str, ...)
{
   char    buf[MAX_STRING_LENGTH];
   va_list args;

   va_start(args, str);
   vsprintf(buf, str, args);
   va_end(args);

   buf[155] = 0; /* Make sure it is not too long! */

   szonelog(UNIT_FI_ZONE(p_error_unit), "%s@%s: %s", UNIT_FI_NAME(p_error_unit), UNIT_FI_ZONENAME(p_error_unit), buf);
}

void pain_next_cmd(struct pain_type *pain)
{
   if(++pain->idx >= pain->top)
   {
      pain->idx = 0;
   }
}

void pain_gotoline(struct pain_type *pain)
{
   if(pain->cmds[pain->idx].gotoline >= pain->top)
   {
      pain_error("PAIN error - jump to undefined instruction number.");
      pain_next_cmd(pain);
   }
   else
   {
      pain->idx = pain->cmds[pain->idx].gotoline;
   }
}

/* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */

/* 'A' command */
auto pain_act(unit_data *npc, struct pain_type *pain) -> int
{
   act((char *)pain->cmds[pain->idx].ptr[0], A_SOMEONE, npc, nullptr, nullptr, TO_ROOM);
   pain_next_cmd(pain);
   return static_cast<int>(FALSE); /* Stop command loop until next tick */
}

/* 'C' command */
auto pain_command(unit_data *npc, struct pain_type *pain) -> int
{
   command_interpreter(npc, (char *)pain->cmds[pain->idx].ptr[0]);
   pain_next_cmd(pain);
   return static_cast<int>(FALSE); /* Stop command loop until next tick */
}

/* 'D' Command */
auto pain_closed(unit_data *npc, struct pain_type *pain) -> int
{
   int i;

   if(IS_ROOM(UNIT_IN(npc)) && (ROOM_EXIT(UNIT_IN(npc), pain->cmds[pain->idx].data[0]) != nullptr))
   {
      i = ROOM_EXIT(UNIT_IN(npc), pain->cmds[pain->idx].data[0])->exit_info;
      if(IS_SET(i, EX_CLOSED))
      {
         pain_gotoline(pain);
         return static_cast<int>(TRUE); /* Continue command loop immediately */
      }
   }

   /* Not closed */
   pain_next_cmd(pain);
   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'd' command */
auto pain_charcmd(unit_data *npc, struct pain_type *pain) -> int
{
   unit_data *u;
   char       buf[MAX_INPUT_LENGTH];

   if(pain->vars[0] == nullptr) /* Is there a PC target? */
   {
      pain_gotoline(pain);
      return static_cast<int>(FALSE); /* Wait a tick */
   }

   for(u = UNIT_CONTAINS(UNIT_IN(npc)); u != nullptr; u = u->next)
   {
      if(u == pain->vars[0])
      {
         break;
      }
   }

   if(u == nullptr)
   {
      pain->vars[0] = nullptr;
      pain_gotoline(pain);
      return static_cast<int>(FALSE); /* Wait a tick */
   }

   /* Great! The CHAR is still here! */
   sprintf(buf, (char *)pain->cmds[pain->idx].ptr[0], UNIT_NAME(u));
   command_interpreter(npc, buf);
   pain_next_cmd(pain);
   return static_cast<int>(FALSE); /* Stop command loop until next tick */
}

/* 'G' command */
auto pain_goto(unit_data *npc, struct pain_type *pain) -> int
{
   file_index_type *fi;
   int              res;

   fi = (file_index_type *)pain->cmds[pain->idx].ptr[0];

   if(fi->room_ptr != nullptr)
   {
      res = npc_move(npc, fi->room_ptr);
   }
   else
   {
      pain_error("PAIN 'G' : This part has not yet been made!");
      pain_next_cmd(pain);
      return static_cast<int>(FALSE);
   }

   if(res == MOVE_GOAL)
   { /* Destination reached! */
      pain_next_cmd(pain);
   }

   return static_cast<int>(FALSE); /* Stop command loop until next tick */
}

/* 'H' command */
auto pain_has(unit_data *npc, struct pain_type *pain) -> int
{
   char *c;

   c = (char *)pain->cmds[pain->idx].ptr[0];

   if(find_unit(npc, &c, nullptr, FIND_UNIT_IN_ME) != nullptr)
   {
      pain_gotoline(pain);
      return static_cast<int>(TRUE); /* Continue command loop immediately */
   }

   /* Not found */
   pain_next_cmd(pain);
   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'I' command */
auto pain_in_room(unit_data *npc, struct pain_type *pain) -> int
{
   unit_data *u;
   char      *c;
   char       buf[256];

   c = (char *)pain->cmds[pain->idx].ptr[0];

   if((u = find_unit(npc, &c, nullptr, FIND_UNIT_SURRO)) != nullptr)
   {
      if(u != npc)
      {
         pain_gotoline(pain);
         return static_cast<int>(TRUE);
      }

      c = (char *)pain->cmds[pain->idx].ptr[0];
      sprintf(buf, "2.%s", c);
      if(find_unit(npc, &c, nullptr, FIND_UNIT_SURRO) != nullptr)
      {
         pain_gotoline(pain);
         return static_cast<int>(TRUE);
      }
   }

   pain_next_cmd(pain);

   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'i' command */
auto pain_intercept(unit_data *npc, struct pain_type *pain) -> int
{
   pain_next_cmd(pain);

   return static_cast<int>(FALSE); /* Stop ticking right away. :) */
}

/* 'J' command */
auto pain_jump(unit_data *npc, struct pain_type *pain) -> int
{
   pain_gotoline(pain);
   return static_cast<int>(FALSE); /* Command loop will stall one tick */
}

/* 'L' command */
auto pain_locked(unit_data *npc, struct pain_type *pain) -> int
{
   int i;

   if(IS_ROOM(UNIT_IN(npc)) && (ROOM_EXIT(UNIT_IN(npc), pain->cmds[pain->idx].data[0]) != nullptr))
   {
      i = ROOM_EXIT(UNIT_IN(npc), pain->cmds[pain->idx].data[0])->exit_info;
      if(IS_SET(i, EX_LOCKED))
      {
         pain_gotoline(pain);
         return static_cast<int>(TRUE); /* Continue command loop immediately */
      }
   }
   else
   { /* Not closed */
      pain_next_cmd(pain);
   }

   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'l' command */
auto pain_load(unit_data *npc, struct pain_type *pain) -> int
{
   unit_data *u;

   u = read_unit((file_index_type *)pain->cmds[pain->idx].ptr[0]);
   unit_to_unit(u, npc);
   pain_next_cmd(pain);
   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'r' command */
auto pain_remote_load(unit_data *npc, struct pain_type *pain) -> int
{
   unit_data *u;

   u = read_unit((file_index_type *)pain->cmds[pain->idx].ptr[0]);
   unit_to_unit(u, (unit_data *)pain->cmds[pain->idx].ptr[1]);
   pain_next_cmd(pain);
   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'R' command */
auto pain_random(unit_data *npc, struct pain_type *pain) -> int
{
   if(number(1, 100) <= pain->cmds[pain->idx].data[0])
   {
      pain_gotoline(pain);
   }
   else
   {
      pain_next_cmd(pain);
   }

   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'S' command */
auto pain_scan_pc(unit_data *npc, struct pain_type *pain) -> int
{
   scan4_unit(npc, UNIT_ST_PC);

   int i = number(0, unit_vector.top);

   if((unit_vector.top < 1) || !CHAR_CAN_SEE(npc, UVI(i)))
   {
      pain_gotoline(pain);
      return static_cast<int>(FALSE); /* Wait a tick */
   }

   pain->vars[0] = unit_vector.units[i];
   pain_next_cmd(pain);
   return static_cast<int>(TRUE); /* Wait no more */
}

/* 'T' command */
auto pain_trash(unit_data *npc, struct pain_type *pain) -> int
{
   unit_data *u;

   auto       obj_trade_price(unit_data * u)->amount_t;

   for(u = UNIT_CONTAINS(UNIT_IN(npc)); u != nullptr; u = u->next)
   {
      if(UNIT_WEAR(u, MANIPULATE_TAKE) && (UNIT_WEIGHT(u) <= 200) &&
         ((OBJ_TYPE(u) == ITEM_DRINKCON) || (obj_trade_price(u) <= (int32_t)pain->cmds[pain->idx].data[0])))
      {
         act("$1n picks up $3n.", A_SOMEONE, npc, nullptr, u, TO_ROOM);
         /* unit_down(u, npc); */
         extract_unit(u); /* Not sure this is fair... */
         pain_gotoline(pain);
         return static_cast<int>(FALSE); /* Wait one tick */
      }
   }

   pain_next_cmd(pain);
   return static_cast<int>(FALSE); /* Wait one tick */
}

/* 'W' command */
auto pain_wait(unit_data *npc, struct pain_type *pain) -> int
{
   time_info_data time_info;

   time_info = mud_date(time(nullptr));

   if(pain->cmds[pain->idx].data[0] == time_info.hours)
   {
      pain_gotoline(pain);
      return static_cast<int>(TRUE); /* Continue command loop immediately */
   }
   pain_next_cmd(pain);

   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* 'M' command */
auto pain_waitmsg(unit_data *npc, struct pain_type *pain) -> int
{
   if(pain->cmds[pain->idx].data[0] != 0)
   {
      pain->cmds[pain->idx].data[0]--;
      return static_cast<int>(FALSE); /* Wait yet another tick */
   }

   pain_gotoline(pain);
   return static_cast<int>(TRUE); /* Continue command loop immediately */
}

/* ---------------------------------------------------------------------- */
/*                P A I N   E X E C U T I O N   L O O P                   */
/* ---------------------------------------------------------------------- */

void pain_free(struct pain_type *p)
{
   int i;

   if(p->top > 0)
   {
      for(i = 0; i < p->top; i++)
      {
         if(p->cmds[i].ptr[0] != nullptr)
         {
            if((p->cmds[i].func == pain_act) || (p->cmds[i].func == pain_command) || (p->cmds[i].func == pain_charcmd) ||
               (p->cmds[i].func == pain_has) || (p->cmds[i].func == pain_in_room))
            {
               free(p->cmds[i].ptr[0]);
            }
            else if(p->cmds[i].func == pain_waitmsg)
            {
               free_namelist((char **)p->cmds[i].ptr[0]);
            }
         }
      }
      free(p->cmds);
   }
   free(p);
}

auto pain_exec(struct spec_arg *sarg) -> int
{
   int               i;
   int               cont;
   struct pain_type *p;

   p = (struct pain_type *)sarg->fptr->data;

   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if(p != nullptr)
      {
         pain_free(p);
      }
      sarg->fptr->data = nullptr;
      return SFR_SHARE;
   }

   if(!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "PAIN on unit %s@%s", UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   if(is_destructed(DR_UNIT, sarg->owner) != 0)
   {
      return SFR_BLOCK;
   }

   /* for pain-error-info */
   p_error_unit = sarg->owner;

   if(p->cmds[p->idx].func == pain_intercept)
   {
      if((sarg->cmd->no == p->cmds[p->idx].data[0]) && (p->cmds[p->idx].ptr[0] == UNIT_FILE_INDEX(sarg->activator)))
      {
         pain_intercept(sarg->owner, p);
         return SFR_SHARE;
      }
      return SFR_SHARE;
   }

   if((p->cmds[p->idx].func == pain_waitmsg) && (sarg->cmd->no == p->cmds[p->idx].data[0]) && ((p->vars[0]) != nullptr) &&
      (p->vars[0] == sarg->activator) && ((is_name(sarg->arg, (const char **)p->cmds[p->idx].ptr[0])) != nullptr))
   {
      cont = static_cast<int>(TRUE);
      pain_next_cmd(p);
      return SFR_SHARE;
   }

   if((sarg->cmd->no == CMD_AUTO_TICK) && !CHAR_FIGHTING(sarg->owner))
   {
      for(cont = static_cast<int>(TRUE), i = 0; (cont != 0) && (i < 100); i++)
      {
         cont = ((*p->cmds[p->idx].func)(sarg->owner, p));
      }

      if(i >= 100)
      {
         pain_error("PAIN - Endless loop in execution.");
         pain_free(p);
         sarg->fptr->data = nullptr;
         destroy_fptr(sarg->owner, sarg->fptr);
         return SFR_SHARE;
      }
      /*
          if (IS_SET(sarg->fptr->flags, SFB_SUPREME))
            return SFR_SUPREME;
          else
            return SFR_SHARE;
      */
   }

   return SFR_SHARE; /* Other commands may pass through */
}

/* ---------------------------------------------------------------------- */
/*                  P A I N   I N I T   R O U T I N E S                   */
/* ---------------------------------------------------------------------- */

/* Gets the next string from buf up until the next '@' mark */
/*                                                          */
auto pi_getstr(char *c, char *buf) -> char *
{
   for(; ((*buf = *c) != 0) && (*c++ != PAIN_SEPARATOR); buf++)
   {
      ;
   }

   *buf = '\0'; /* Erase last separator */

   if(*c != 0)
   {
      return c - 1;
   }
   return c;
}

/* Gets the next string from buf up until the next '!' mark */
/*                                                          */
auto pi_getstr_excla(char *c, char *buf) -> char *
{
   for(; ((*buf = *c) != 0) && (*c++ != '!'); buf++)
   {
      ;
   }

   *buf = '\0'; /* Erase last separator */

   if(*c != 0)
   {
      return c - 1;
   }
   return c;
}

auto pi_getnum(char *c, int *res) -> char *
{
   if(isdigit(*c) == 0)
   {
      *res = -1;
   }
   else
   {
      *res = 0;
   }

   for(*res = 0; (*c != 0) && (isdigit(*c) != 0); c++)
   {
      *res = 10 * (*res) + (*c - '0');
   }

   return c;
}

struct line_no_convert
{
   int symbolic;
   int actual;
};

auto translate_line(int top, int sym, struct line_no_convert *line_numbers) -> int
{
   int i;
   for(i = 0; i < top; i++)
   {
      if(sym == line_numbers[i].symbolic)
      {
         return line_numbers[i].actual;
      }
   }

   return -1;
}

auto pain_doinit(unit_data *npc, char *text) -> struct pain_type *
{
   int                      i;
   int                      j;
   int                      top;
   int                      error;
   char                     buf[MAX_STRING_LENGTH];
   char                     zone[80];
   char                     name[80];
   char                    *b = nullptr;
   struct pain_type        *pain;

   struct pain_cmd_type    *cmd_list     = nullptr;
   struct line_no_convert  *line_numbers = nullptr;

   struct pain_cmd_type     cmd;
   struct line_no_convert   line_no = {-1, -1};
   struct command_info     *cmd_ptr;
   char                   **namelist = nullptr;

   extern struct trie_type *intr_trie;

   static const char       *specmsg[] = {"_dead", "_combat", "_unknown", nullptr};

   top                                = 0;
   i                                  = 0;
   error                              = static_cast<int>(FALSE);

   while((*text != 0) && (error == 0))
   {
      /* Skip all blanks */
      if((*text == ' ') || (*text == '\n') || (*text == '\r'))
      {
         text++;
         continue;
      }

      cmd.gotoline = -1;
      cmd.data[0] = cmd.data[1] = 0;
      cmd.ptr[0] = cmd.ptr[1] = nullptr;
      cmd.func                = nullptr;

      /* Read symbolic line number */
      text                    = pi_getnum(text, &i);
      if(i == -1)
      {
         error = static_cast<int>(TRUE);
         continue;
      }
      if(i <= line_no.symbolic)
      {
         pain_error("PAIN: 'Repeating' or 'less than previous' symbolic "
                    "line num.");
         error = static_cast<int>(TRUE);
         continue;
      }
      line_no.symbolic = i;
      line_no.actual   = top;

      while((*text != 0) && ((*text == ' ') || (*text == '\n') || (*text == '\r')))
      {
         text++;
      }

      if(*text == 0)
      {
         continue;
      }

      switch(*text++)
      {
         case 'A':
            text       = pi_getstr(text, buf);
            cmd.ptr[0] = str_dup(buf);
            cmd.func   = pain_act;
            break;

         case 'C':
            text       = pi_getstr(text, buf);
            cmd.ptr[0] = str_dup(buf);
            cmd.func   = pain_command;
            break;

         case 'd':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text         = pi_getstr(text, buf);
            cmd.ptr[0]   = str_dup(buf);
            cmd.func     = pain_charcmd;
            break;

         case 'D':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text         = pi_getstr(text, buf);
            i            = search_block(buf, dirs, FALSE); /* partial match */
            cmd.data[0]  = i;
            cmd.func     = pain_closed;
            if(i == -1)
            {
               pain_error("PAIN - Illegal direction in Closed door test");
               error = static_cast<int>(TRUE);
            }
            break;

         case 'G':
            text       = pi_getstr(text, buf);
            cmd.ptr[0] = pc_str_to_file_index(npc, buf);
            cmd.func   = pain_goto;
            if(cmd.ptr[0] == nullptr)
            {
               pain_error("Unknown unit referece in PAIN-Goto: %s", buf);
               error = static_cast<int>(TRUE);
               continue;
            }
            break;

         case 'H':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text         = pi_getstr(text, buf);
            cmd.ptr[0]   = str_dup(buf);
            cmd.func     = pain_has;
            break;

         case 'I':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text         = pi_getstr(text, buf);
            cmd.ptr[0]   = str_dup(buf);
            cmd.func     = pain_in_room;
            break;

         case 'i':
            text = pi_getstr(text, buf); /* Command */
            if((cmd_ptr = (struct command_info *)search_trie(buf, intr_trie)) == nullptr)
            {
               pain_error("PAIN - Illegal interpreter 'command'"
                          " reference: %s",
                          buf);
               error = static_cast<int>(TRUE);
            }
            cmd.data[0] = cmd_ptr->no;

            text++; /* Skip '@' */
            text = pi_getstr(text, buf);
            split_fi_ref(buf, zone, name);
            if(*zone == 0)
            {
               strcpy(zone, UNIT_FI_ZONENAME(npc));
            }

            if((cmd.ptr[0] = find_file_index(zone, name)) == nullptr)
            {
               pain_error("PAIN - Illegal symbolic reference: %s/%s", zone, name);
               error = static_cast<int>(TRUE);
            }
            cmd.func = pain_intercept;
            break;

         case 'J':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            cmd.func     = pain_jump;
            break;

         case 'L':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text         = pi_getstr(text, buf);
            i            = search_block(buf, dirs, FALSE); /* partial match */
            cmd.data[0]  = i;
            cmd.func     = pain_locked;
            if(i == -1)
            {
               pain_error("PAIN - Illegal direction in Lock door test");
               error = static_cast<int>(TRUE);
            }
            break;

         case 'l':
            text = pi_getstr(text, buf);
            split_fi_ref(buf, zone, name);
            if((cmd.ptr[0] = find_file_index(zone, name)) == nullptr)
            {
               pain_error("PAIN - Illegal symbolic reference: %s", buf);
               error = static_cast<int>(TRUE);
            }
            cmd.func = pain_load;
            break;

         case 'R':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text++; /* Skip ',' */
            text        = pi_getnum(text, &i);
            cmd.data[0] = i;
            cmd.func    = pain_random;
            break;

         case 'r':
            text = pi_getstr_excla(text, buf);
            split_fi_ref(buf, zone, name);
            if((cmd.ptr[0] = find_file_index(zone, name)) == nullptr)
            {
               pain_error("PAIN - Illegal symbolic reference: %s", buf);
               error = static_cast<int>(TRUE);
            }

            text = pi_getstr(text, buf);
            split_fi_ref(buf, zone, name);
            if((cmd.ptr[1] = world_room(zone, name)) == nullptr)
            {
               pain_error("PAIN 'r': - Illegal symbolic ROOM reference: %s", buf);
               error = static_cast<int>(TRUE);
            }
            cmd.func = pain_remote_load;
            break;

         case 'S':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            cmd.func     = pain_scan_pc;
            break;

         case 'T':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text++; /* Skip ',' */
            text        = pi_getnum(text, &i);
            cmd.data[0] = i;
            cmd.func    = pain_trash;
            break;

         case 'W':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text++; /* Skip ',' */
            text        = pi_getnum(text, &i);
            cmd.data[0] = i;
            cmd.func    = pain_wait;
            break;

         case 'M':
            text         = pi_getnum(text, &i);
            cmd.gotoline = i;
            text         = pi_getstr(text, buf);
            if((cmd_ptr = (struct command_info *)search_trie(buf, intr_trie)) == nullptr)
            {
               if((i = search_block(buf, specmsg, TRUE)) != -1)
               {
                  pain_error("PAIN - Illegal interpreter 'command'"
                             " reference: %s",
                             buf);
                  error = static_cast<int>(TRUE);
               }
               else
               {
                  switch(i)
                  {
                     case 0:
                        cmd.data[0] = CMD_AUTO_DEATH;
                        break;
                     case 1:
                        cmd.data[0] = CMD_AUTO_COMBAT;
                        break;
                     case 2:
                        cmd.data[0] = CMD_AUTO_UNKNOWN;
                        break;
                  }
               }
            }
            else
            {
               cmd.data[0] = cmd_ptr->no;
            }
            text       = pi_getstr(text, buf);
            namelist   = create_namelist();
            cmd.ptr[0] = namelist;
            for(b = buf; *b != 0; b = pi_getstr(b, name))
            {
               add_name(name, namelist);
            }
            text        = pi_getnum(text, &i);
            cmd.data[1] = i;
            cmd.func    = pain_waitmsg;
            break;

         case '*':
            text = pi_getstr(text, buf);
            if(*text != 0)
            { /* Skip separator if any (or end) */
               text++;
            }
            continue; /* Skip alloc stuff */
            break;

         default:
            pain_error("Illegal routine in pain init: %s", text);
            error = static_cast<int>(TRUE);
            break;
      } /* switch */

      if(error == 0)
      {
         if(++top == 1)
         {
            CREATE(line_numbers, struct line_no_convert, 1);
            CREATE(cmd_list, struct pain_cmd_type, 1);
         }
         else
         {
            RECREATE(line_numbers, struct line_no_convert, top);
            RECREATE(cmd_list, struct pain_cmd_type, top);
         }
         line_numbers[top - 1] = line_no;
         cmd_list[top - 1]     = cmd;
      }
      if(*text != 0)
      { /* Skip the pain separator, or the 0 when at end */
         text++;
      }
   }

   if(cmd_list != nullptr)
   {
      CREATE(pain, struct pain_type, 1);
      pain->top  = top;
      pain->idx  = 0;
      pain->cmds = cmd_list;
      for(i = 0; (i < top) && (error == 0); i++)
      {
         if(cmd_list[i].gotoline != -1)
         {
            if((j = translate_line(top, cmd_list[i].gotoline, line_numbers)) == -1)
            {
               pain_error("PAIN: Unknown symbolic line number %d", cmd_list[i].gotoline);
               error = static_cast<int>(TRUE);
            }
            else
            {
               cmd_list[i].gotoline = j;
            }
         }
      }

      free(line_numbers);

      if(error != 0)
      {
         pain_error("PAIN error - PAIN aborted.");
         pain_free(pain);
         return nullptr;
      }
      return pain;
   }

   return nullptr;
}

auto pain_init(struct spec_arg *sarg) -> int
{
   struct pain_type *p;

   if(sarg->fptr->data != nullptr)
   {
      if(sarg->cmd->no == CMD_AUTO_EXTRACT)
      {
         free(sarg->fptr->data);
         sarg->fptr->data = nullptr;
         return SFR_SHARE;
      }

      p_error_unit = sarg->owner;

      if((p = pain_doinit(sarg->owner, (char *)sarg->fptr->data)) != nullptr)
      {
         sarg->fptr->index = SFUN_PAIN;

         free(sarg->fptr->data);
         sarg->fptr->data = nullptr;

         sarg->fptr->data = p;
         if(sarg->fptr->heart_beat < WAIT_SEC * 5)
         {
            pain_error("PAIN had less than 5 sec. PAIN beat.");
            sarg->fptr->heart_beat = WAIT_SEC * 15;
         }
      }
      else
      {
         destroy_fptr(sarg->owner, sarg->fptr);
      }
   }

   return SFR_SHARE;
}
