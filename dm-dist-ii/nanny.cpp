/* *********************************************************************** *
 * File   : nanny.c                                   Part of Valhalla MUD *
 * Version: 2.02                                                           *
 * Author : seifert and quinn (@diku.dk)                                   *
 *                                                                         *
 * Purpose: Menus in entry.                                                *
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

/* 27/07/92 seifert: Changed radically to use descriptor only              */
/* 30/07/92 seifert: Fixed bug with find descriptor & wiz check            */
/* 22/10/92 gnort  : Put pwd-hide on all operations in nanny.              */
/* 05/1/93  HHS    : Included wizlock                                      */

#include "interpreter.h"
#include "pcsave.h"
#include <arpa/inet.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#ifndef DOS /* AMIGA */
   #include <arpa/telnet.h>
#endif
#include "CAccountConfig.h"
#include "account.h"
#include "affect.h"
#include "ban.h"
#include "blkfile.h"
#include "comm.h"
#include "common.h"
#include "competition.h"
#include "connectionlog.h"
#include "constants.h"
#include "db.h"
#include "dilrun.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "main.h"
#include "modify.h"
#include "money.h"
#include "nanny.h"
#include "protocol.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "trie.h"
#include "utility.h"
#include "utils.h"
#include <climits>

#define STATE(d) ((d)->state)

void nanny_credit_card(descriptor_data *d, const char *arg);
void nanny_new_pwd(descriptor_data *d, char *arg);
void nanny_change_information(descriptor_data *d, const char *arg);
void nanny_change_terminal(descriptor_data *d, const char *arg);
void multi_close(struct multi_element *pe);
auto player_exists(const char *pName) -> int;
void save_player_file(unit_data *ch);

extern struct diltemplate *nanny_dil_tmpl;

/* *************************************************************************
 *  Stuff for controlling the non-playing sockets (get name, pwd etc)      *
 ************************************************************************* */

auto _parse_name(const char *arg, char *name) -> int
{
   int                i;
   static const char *illegal_names[] = {
      "on", /* I picture tell on/off & shout on/off ..*/
      "off", "someone", "guard", "cityguard", "captain", "typing", "undefined", "safe", "corpse",   "here",

      "you", "room",    "zone",  "world",     "group",   "reset",  "self",      "all",  "creators", nullptr};

   arg = skip_spaces(arg); /* skip whitespaces */

   /* i+1 = chars copied    */
   /* PC_MAX_NAME-1 = chars */

   for(i = 0; (*name = *arg) != 0; arg++, i++, name++)
   {
      if((*arg <= ' ') || (isalpha(*arg) == 0) || (i + 1) > (PC_MAX_NAME - 1))
      {
         return 1;
      }
   }

   if(i <= 2)
   { /* Names must be at least 3 chars due to passwords */
      return 1;
   }

   if(fill_word(name - i) != 0)
   { /* Don't allow fillwords */
      return 1;
   }

   if(search_block(name - i, illegal_names, TRUE) != -1)
   {
      return 1;
   }

   str_lower(name);

   return 0;
}

/* This is called from main_loop every n seconds (long duration) to check */
/* for idle time for any descriptors                                      */
void check_idle()
{
   descriptor_data *d;
   descriptor_data *next_d;
   time_t           now = time(nullptr);

   for(d = descriptor_list; d != nullptr; d = next_d)
   {
      next_d = d->next;
      d->timer++;
      if(descriptor_is_playing(d) == 0) /* Not in game yet */
      {
         if(d->fptr == nanny_get_name)
         {
            if(d->timer >= 2)
            {
               slog(LOG_ALL, 0, "Kicking out idle player waiting for name.");
               descriptor_close(d);
            }
         }
         else
         {
            if(d->timer >= 10)
            {
               slog(LOG_ALL, 0, "Kicking out player from menu.");
               descriptor_close(d);
            }
         }
      }
      else
      {
         if(d->timer >= 20 && IS_MORTAL(d->character))
         {
            slog(LOG_ALL, 0, "Kicking out idle player and making link-dead.");
            descriptor_close(d);
         }
         else if(IS_PC(d->character) && now - d->logon >= SECS_PER_REAL_HOUR / 3)
         {
            send_to_char("Autosave.\n\r", d->character);
            save_player(d->character);
            save_player_contents(d->character, TRUE); /* No compress */
            /* Only save one player per autosave... */
            return;
         }
      }
   }
}

/* clear some of the the working variables of a char */
void reset_char(unit_data *ch)
{
   /* Ok, this is test to avoid level 255's entering the game... */
   assert(CHAR_SEX(ch) != 255);
   assert(CHAR_POS(ch) != 255);

   UNIT_MAX_HIT(ch) = hit_limit(ch);
}

void connect_game(unit_data *pc)
{
   assert(CHAR_DESCRIPTOR(pc));

   PC_TIME(pc).connect        = time(nullptr);
   CHAR_DESCRIPTOR(pc)->logon = time(nullptr);

   CHAR_DESCRIPTOR(pc)->CreateBBS();

   set_descriptor_fptr(CHAR_DESCRIPTOR(pc), descriptor_interpreter, FALSE);

   no_players++;
   if(no_players > max_no_players)
   {
      max_no_players = no_players;
   }
}

void disconnect_game(unit_data *pc)
{
   CHAR_DESCRIPTOR(pc)->RemoveBBS();

   no_players--;
}

void reconnect_game(descriptor_data *d, unit_data *ch)
{
   assert(UNIT_IN(ch));
   assert(!UNIT_IN(d->character));
   assert(!CHAR_DESCRIPTOR(ch));

   CHAR_DESCRIPTOR(d->character) = nullptr;
   extract_unit(d->character);
   d->character        = ch;
   CHAR_DESCRIPTOR(ch) = d;
   ActivateDil(ch);

   connect_game(ch);

   send_to_char("Reconnecting.\n\r", ch);

   if(CHAR_LAST_ROOM(ch) && (CHAR_LAST_ROOM(ch) != UNIT_IN(ch)))
   {
      act("$1n has reconnected, and is moved to another location.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
      unit_from_unit(ch);
      unit_to_unit(ch, CHAR_LAST_ROOM(ch));
      CHAR_LAST_ROOM(ch) = nullptr;
   }
   act("$1n has reconnected.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
   slog(LOG_BRIEF, UNIT_MINV(ch), "%s[%s] has reconnected.", PC_FILENAME(ch), CHAR_DESCRIPTOR(ch)->host);
   CHAR_DESCRIPTOR(ch)->logon = time(nullptr);
   PC_TIME(ch).connect        = time(nullptr);
   set_descriptor_fptr(d, descriptor_interpreter, FALSE);
}

void update_lasthost(unit_data *pc, uint32_t s_addr)
{
   if((int32_t)s_addr == -1)
   {
      return;
   }

   for(int i = 0; i < 5; i++)
   {
      if(PC_LASTHOST(pc)[i] == s_addr)
      {
         return;
      }
   }

   memmove(&PC_LASTHOST(pc)[0], &PC_LASTHOST(pc)[1], sizeof(uint32_t) * 4);
   PC_LASTHOST(pc)[4] = s_addr;
}

/* Set 'd' to 'ch' and enter the game.                            */
/* If ch has UNIT_IN set, then it is because ch must be link dead */
/*   and thus a reconnect is performed.                           */
/* If UNIT_IN is not set, then the char must be put inside the    */
/*   game, and his inventory loaded.                              */
void enter_game(unit_data *ch)
{
   unit_data       *load_room;
   descriptor_data *i;
   char             buf[256];
   time_t           last_connect = PC_TIME(ch).connect;

   extern struct command_info cmd_info[];

   auto player_has_mail(unit_data * ch)->uint8_t;
   auto ContentsFileName(const char *)->char *;
   void start_all_special(unit_data * u);

   assert(ch);
   assert(!UNIT_IN(ch));

   if(CHAR_DESCRIPTOR(ch))
   {
      ConnectionBegin(ch);

      update_lasthost(ch, inet_addr(CHAR_DESCRIPTOR(ch)->host));

      CHAR_DESCRIPTOR(ch)->timer       = 0;
      CHAR_DESCRIPTOR(ch)->prompt_mode = PROMPT_EXPECT;
      CHAR_DESCRIPTOR(ch)->logon       = time(nullptr);
      PC_TIME(ch).connect              = time(nullptr);
      set_descriptor_fptr(CHAR_DESCRIPTOR(ch), descriptor_interpreter, FALSE);

      ActivateDil(ch);

      connect_game(ch);
   }

   reset_char(ch);

   insert_in_unit_list(ch);

   if(CHAR_LAST_ROOM(ch))
   {
      load_room          = CHAR_LAST_ROOM(ch);
      CHAR_LAST_ROOM(ch) = nullptr;
   }
   else
   {
      load_room = hometown_unit(PC_HOME(ch));
   }

   unit_to_unit(ch, load_room);

   if(CHAR_DESCRIPTOR(ch)) /* Only do these things if player is connected */
   {
      sprintf(buf, "%s has entered the world.\n\r", UNIT_NAME(ch));

      for(i = descriptor_list; i != nullptr; i = i->next)
      {
         if((descriptor_is_playing(i) != 0) && i->character != ch && CHAR_CAN_SEE(CHAR_ORIGINAL(i->character), ch) &&
            IS_PC(CHAR_ORIGINAL(i->character)) && IS_SET(PC_FLAGS(CHAR_ORIGINAL(i->character)), PC_INFORM) &&
            (same_surroundings(ch, i->character) == 0u))
         {
            send_to_descriptor(buf, i);
         }
      }

      act("$1n has arrived.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
   }

   /* New player stats. Level can be zero after reroll while ID is not. */
   if((CHAR_LEVEL(ch) == 0) && PC_IS_UNSAVED(ch))
   {
      void start_player(unit_data * ch);

      slog(LOG_BRIEF, 0, "%s[%s] (GUEST) has entered the game.", PC_FILENAME(ch), CHAR_DESCRIPTOR(ch)->host);

      PC_ID(ch) = new_player_id();

      start_player(ch);
   }
   else
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_look(ch, mbuf, &cmd_info[CMD_DOWN]);
   }

   if(file_exists(ContentsFileName(PC_FILENAME(ch))) != 0u)
   {
      auto rent_calc(unit_data * ch, time_t savetime)->uint32_t;

      load_contents(PC_FILENAME(ch), ch);
      rent_calc(ch, last_connect);

      /* XXXX This isn't very nice, but it works!  /gnort
       * CHAR_MONEY is set if the version of player is too low.
       */
      if(CHAR_MONEY(ch))
      {
         extern void tax_player(unit_data * ch);

         CHAR_MONEY(ch) = nullptr;
         tax_player(ch);
      }
   }

   competition_enroll(ch);

   if(player_has_mail(ch) != 0u)
   {
      send_to_char(COLOUR_ATTN "You have a great urge to visit the postoffice.\n\r" COLOUR_NORMAL, ch);
   }

   if(IS_ULTIMATE(ch) && PC_IS_UNSAVED(ch))
   {
      save_player(ch);
   }

   start_affect(ch);      /* Activate affect ticks */
   start_all_special(ch); /* Activate fptr ticks   */
}

void set_descriptor_fptr(descriptor_data *d, void (*fptr)(descriptor_data *, const char *), bool call)
{
   if(d->fptr == interpreter_string_add)
   {
      if(d->localstr != nullptr)
      {
         free(d->localstr);
      }

      d->localstr = nullptr;
      d->editref  = nullptr;
      d->postedit = nullptr;
      d->editing  = nullptr;
   }

   d->fptr = fptr;
   if(call != 0u)
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      d->state                    = 0;
      (d->fptr)(d, mbuf);
   }
   else
   {
      d->state = 1;
   }
}

/* Return TRUE if help is given (found)... */
auto nanny_help_check(descriptor_data *d, char *arg, char *def) -> int
{
   char buf[200 + MAX_INPUT_LENGTH];

   auto help_base(descriptor_data * d, char *arg)->int;

   arg = skip_spaces(arg);

   arg = str_next_word(arg, buf);

   str_lower(buf);
   if(!(strcmp("help", buf) == 0 || strcmp("hel", buf) == 0 || strcmp("he", buf) == 0 || strcmp("h", buf) == 0 || strcmp(buf, "?") == 0))
   {
      return FALSE;
   }

   if(str_is_empty(arg) != 0u)
   {
      arg = def;
   }

   if(help_base(d, arg) == 0)
   {
      sprintf(buf, "There is no help on the subject '%s'.\n\r", arg);
      send_to_descriptor(buf, d);
   }
   return TRUE;
}

void nanny_close(descriptor_data *d, const char *arg)
{
   if(STATE(d)++ == 0)
   {
      send_to_descriptor(g_cServerConfig.m_pGoodbye, d);
      send_to_descriptor("\r\nPress [enter] to quit.", d);
      return;
   }

   descriptor_close(d);
}

void nanny_motd(descriptor_data *d, char *arg)
{
   if(STATE(d)++ == 0)
   {
      page_string(d, g_cServerConfig.m_pMotd);
      page_string(d, "\n\r\n*** PRESS RETURN: ");
      return;
   }

   set_descriptor_fptr(d, nanny_menu, TRUE);
}

void nanny_newbie(descriptor_data *d, char *arg)
{
   if((str_is_empty(g_cServerConfig.m_pNewbie) == 0u) && STATE(d)++ == 0)
   {
      page_string(d, g_cServerConfig.m_pNewbie);
      page_string(d, "\n\r\n*** PRESS RETURN: ");
      return;
   }

   set_descriptor_fptr(d, nanny_motd, TRUE);
}

void nanny_throw(descriptor_data *d, const char *arg)
{
   descriptor_data *td;
   unit_data       *u;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Already playing!\n\r"
                         "Throw the other copy out? (Y/N) ",
                         d);
      return;
   }

   if(*arg == 'y' || *arg == 'Y')
   {
      while((td = find_descriptor(PC_FILENAME(d->character), d)) != nullptr)
      {
         descriptor_close(td);
      }

      for(u = unit_list; u != nullptr; u = u->gnext)
      {
         if(IS_PC(u) && str_ccmp(PC_FILENAME(d->character), PC_FILENAME(u)) == 0)
         {
            assert(!CHAR_DESCRIPTOR(u));
            assert(UNIT_IN(u));

            if(PC_IS_UNSAVED(u))
            {
               send_to_descriptor("Guest, purging all connections - "
                                  "please retry.\n\r",
                                  d);
               extract_unit(u);
               return;
            }

            reconnect_game(d, u);
            return;
         }
      }

      if(player_exists(PC_FILENAME(d->character)) == 0)
      {
         send_to_descriptor("Guest, purging all connections - "
                            "please retry.\n\r",
                            d);
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      set_descriptor_fptr(d, nanny_motd, TRUE);
      return;
   }
   if(*arg == 'n' || *arg == 'N')
   {
      send_to_descriptor("Ok, goodbye then.\n\r", d);
      set_descriptor_fptr(d, nanny_close, TRUE);
   }
   else
      send_to_descriptor("Please type Yes or No: ", d);
}

void nanny_dil(descriptor_data *d, char *arg)
{
   class extra_descr_data *exd;

   exd = UNIT_EXTRA_DESCR(d->character)->find_raw("$nanny");

   if((exd != nullptr) && (str_is_empty(exd->names.Name(1)) == 0u))
   {
      char buf[256];

      strcpy(buf, exd->names.Name(1));

      if(nanny_help_check(d, arg, buf) != 0)
      {
         page_string(d, "\n\r**Press Return**");

         int i = atoi(exd->descr.String());
         exd->descr.Reassign(itoa(i - 1));
         return;
      }
   }

   if(nanny_dil_tmpl != nullptr)
   {
      struct dilprg *prg;

      prg          = dil_copy_template(nanny_dil_tmpl, d->character, nullptr);
      prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

      prg->sp->vars[0].val.string = str_dup(arg);

      dil_activate(prg);
   }

   if(UNIT_EXTRA_DESCR(d->character)->find_raw("$nanny") == nullptr)
   {
      if(str_is_empty(PC_GUILD(d->character)) == 0u)
      {
         char buf[256];

         void join_guild(unit_data * ch, char *guild_name);

         strcpy(buf, PC_GUILD(d->character));
         join_guild(d->character, buf);
      }

      set_descriptor_fptr(d, nanny_newbie, TRUE);
   }
}

void nanny_pwd_confirm(descriptor_data *d, char *arg)
{
   unit_data *u;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Please retype password: ", d);
      send_to_descriptor(CONTROL_ECHO_OFF, d);
      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON "\n\r", d);

   if(strncmp(crypt(arg, PC_FILENAME(d->character)), PC_PWD(d->character), 10) != 0)
   {
      send_to_descriptor("Passwords don't match.\n\r", d);
      set_descriptor_fptr(d, nanny_new_pwd, TRUE);
      return;
   }

   /* See if guest is in game, if so - a guest was LD       */
   /* Password has now been redefined                       */
   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_PC(u) && (str_ccmp(PC_FILENAME(u), PC_FILENAME(d->character)) == 0))
      {
         assert(!CHAR_DESCRIPTOR(u));
         assert(UNIT_IN(u));

         reconnect_game(d, u);
         return;
      }
   }

   set_descriptor_fptr(d, nanny_dil, TRUE);
}

auto check_pwd(descriptor_data *d, char *pwd) -> int
{
   int i;
   int bA;
   int bNA;

   if(strlen(pwd) < 5)
   {
      send_to_descriptor("\n\rThe chosen password is too short.\n\r"
                         "Your password has to be at least 5 characters and "
                         "must contain both letters and digits.\n\r\n\r",
                         d);
      return FALSE;
   }

   if(strlen(pwd) > 10)
   {
      pwd[10] = 0;
   }

   bA  = FALSE;
   bNA = FALSE;

   for(i = 0; pwd[i] != 0; i++)
   {
      if(isalpha(pwd[i]) != 0)
      {
         bA = TRUE;
      }
      else
      {
         bNA = TRUE;
      }
   }

   if(bA == FALSE)
   {
      send_to_descriptor("\n\rThe chosen password only contains letters.\n\r"
                         "Your password has to be at least 5 characters and "
                         "must contain both letters and digits.\n\r\n\r",
                         d);
      return FALSE;
   }

   if(bNA == FALSE)
   {
      send_to_descriptor("\n\rThe chosen password only contains "
                         "non-letters (digits).\n\r"
                         "Your password has to be at least 5 characters and "
                         "must contain both letters and digits.\n\r\n\r",
                         d);
      return FALSE;
   }

   return TRUE;
}

void nanny_new_pwd(descriptor_data *d, char *arg)
{
   if(STATE(d)++ == 0)
   {
      char buf[100];

      sprintf(buf, "Give me a new password for %s: ", UNIT_NAME(d->character));
      send_to_descriptor(buf, d);
      send_to_descriptor(CONTROL_ECHO_OFF, d);
      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON "\n\r", d);

   if(check_pwd(d, arg) == 0)
   {
      set_descriptor_fptr(d, nanny_new_pwd, TRUE);
      return;
   }

   strncpy(PC_PWD(d->character), crypt(arg, PC_FILENAME(d->character)), 10);
   *(PC_PWD(d->character) + 10) = '\0';

   set_descriptor_fptr(d, nanny_pwd_confirm, TRUE);
}

void nanny_change_pwd_confirm(descriptor_data *d, char *arg)
{
   void nanny_change_pwd(descriptor_data * d, char *arg);

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Please retype password: ", d);
      send_to_descriptor(CONTROL_ECHO_OFF, d);
      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON "\n\r", d);

   if(strncmp(crypt(arg, PC_FILENAME(d->character)), PC_PWD(d->character), 10) != 0)
   {
      send_to_descriptor("Passwords don't match.\n\r", d);
      set_descriptor_fptr(d, nanny_change_pwd, TRUE);
      return;
   }

   set_descriptor_fptr(d, nanny_change_terminal, TRUE);
}

void nanny_change_pwd(descriptor_data *d, char *arg)
{
   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Enter your new password: ", d);
      send_to_descriptor(CONTROL_ECHO_OFF, d);
      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON "\n\r", d);

   if(check_pwd(d, arg) == 0)
   {
      set_descriptor_fptr(d, nanny_change_pwd, TRUE);
      return;
   }

   strncpy(PC_PWD(d->character), crypt(arg, PC_FILENAME(d->character)), 10);
   *(PC_PWD(d->character) + 10) = '\0';

   set_descriptor_fptr(d, nanny_change_pwd_confirm, TRUE);
}

void nanny_kill_confirm(descriptor_data *d, char *arg)
{
   unit_data *u;

   auto delete_player(const char *)->int;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor(COLOUR_ATTN "\n\r\n\r"
                                     "WARNING! IF YOU ERASE YOUR PLAYER IT WILL BE GONE FOREVER!\n\r"
                                     "If you really want to erase this player enter your password now, please: " COLOUR_NORMAL,
                         d);

      send_to_descriptor(CONTROL_ECHO_OFF, d);

      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON "\n\r", d);

   if(strncmp(crypt(arg, PC_PWD(d->character)), PC_PWD(d->character), 10) != 0)
   {
      /* Wrong password */
      send_to_descriptor("Deletion not confirmed, you are safe again.", d);
      set_descriptor_fptr(d, nanny_change_terminal, TRUE);
   }
   else
   {
      char buf[100];
      strcpy(buf, PC_FILENAME(d->character));

      if(find_descriptor(buf, d) != nullptr)
      {
         send_to_descriptor("A copy is connected, get rid of "
                            "it first.\n\r",
                            d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;
      }

      for(u = unit_list; u != nullptr; u = u->gnext)
      {
         if(IS_PC(u) && (str_ccmp(PC_FILENAME(u), PC_FILENAME(d->character)) == 0))
         {
            send_to_descriptor("A copy is connected, "
                               "get rid of it first.\n\r",
                               d);
            set_descriptor_fptr(d, nanny_change_terminal, TRUE);
            return;
         }
      }

      slog(LOG_ALL, 0, "%s deleted %sself from menu.", PC_FILENAME(d->character), B_HMHR(d->character));
      send_to_descriptor("\n\rThank you, hope to see you soon.\n\r", d);
      delete_player(PC_FILENAME(d->character));
      set_descriptor_fptr(d, nanny_close, TRUE);
      return;
   }
}

/* Return TRUE when done... */
auto base_string_add(descriptor_data *d, char *str) -> bool
{
   char *scan;
   int   terminator = 0;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Terminate with a '@'.\n\r\n\r", d);
      if(d->localstr != nullptr)
      {
         slog(LOG_ALL, 0, "Spooky localstr in base_string_add - tell papi.");
         free(d->localstr); // Spooky!
      }
      d->localstr = nullptr;
      return FALSE;
   }

   /* determine if this is the terminal string, and truncate if so */
   for(scan = str; *scan != 0; scan++)
   {
      if((terminator = static_cast<int>(*scan == '@' && scan[1] == '\0')) != 0)
      {
         *scan = '\0';
         break;
      }
   }

   if(MAX_STRING_LENGTH - (d->localstr != nullptr ? strlen(d->localstr) : 0) < strlen(str))
   {
      str[MAX_STRING_LENGTH - (d->localstr != nullptr ? strlen(d->localstr) : 0)] = '\0';
      terminator                                                                  = 1;

      send_to_descriptor("String too long - Truncated.\n\r", d);
   }

   if(d->localstr == nullptr)
   {
      CREATE(d->localstr, char, strlen(str) + 3);
      strcpy(d->localstr, str);
   }
   else
   {
      RECREATE(d->localstr, char, strlen(d->localstr) + strlen(str) + 3);
      strcat(d->localstr, str);
   }

   if(terminator != 0)
   {
      if(d->postedit != nullptr)
      {
         d->postedit(d);
      }

      if(d->localstr != nullptr)
      {
         free(d->localstr);
      }

      d->localstr = nullptr;
      d->editref  = nullptr;
      d->postedit = nullptr;
      d->editing  = nullptr;

      return TRUE;
   }
   strcat(d->localstr, "\n\r");

   return FALSE;
}

/* Add user input to the 'current' string (as defined by d->str) */
void interpreter_string_add(descriptor_data *d, const char *str)
{
   if(base_string_add(d, str) != 0u)
   {
      set_descriptor_fptr(d, descriptor_interpreter, FALSE);
   }
}

/* Removes empty descriptions and makes ONE newline after each. */

void nanny_fix_descriptions(unit_data *u)
{
   struct extra_descr_data *exd;
   char                     buf[1024];

   for(exd = UNIT_EXTRA_DESCR(u); exd != nullptr; exd = exd->next)
   {
      if(exd->names.Name() != nullptr)
      {
         strcpy(buf, exd->names.Name());
      }
      else
      {
         *buf = 0;
      }

      if((exd->names.Name() == nullptr) || (search_block(buf, bodyparts, TRUE) != 0))
      {
         if(str_is_empty(exd->descr.String()) != 0u)
         {
            UNIT_EXTRA_DESCR(u) = UNIT_EXTRA_DESCR(u)->remove(exd);
            exd                 = UNIT_EXTRA_DESCR(u);
            nanny_fix_descriptions(u);
            return;
         }

         char buf[MAX_STRING_LENGTH];
         strcpy(buf, exd->descr.String());
         strip_trailing_blanks(buf);
         exd->descr.Reassign(buf);
      }
   }
}

/* Add user input to the 'current' string (as defined by d->str) */
void nanny_string_add(descriptor_data *d, char *str)
{
   if(base_string_add(d, str) != 0u)
   {
      nanny_fix_descriptions(d->character);
      set_descriptor_fptr(d, nanny_change_information, TRUE);
   }
}

void nanny_wizi(descriptor_data *d, char *arg)
{
   if(STATE(d)++ == 0)
   {
      send_to_descriptor("\n\rEnter the desired Wizinvis level: ", d);
      return;
   }

   int i = atoi(arg);
   if(!is_in(i, 0, CHAR_LEVEL(d->character)))
      send_to_descriptor("Invalid wizi level.\n\r", d);
   else
      UNIT_MINV(d->character) = i;

   set_descriptor_fptr(d, nanny_menu, TRUE);
}

void nanny_background(descriptor_data *d, char *arg)
{
   /*
      if (STATE(d)++ == 0)
      {
         extern char *story;
         send_to_descriptor(story, d);
         send_to_descriptor("\n\r*** Press return.", d);
         return;
      } */
   set_descriptor_fptr(d, nanny_menu, TRUE);
}

void list_body_parts(descriptor_data *d)
{
   char  Buf[MAX_STRING_LENGTH];
   char *c = Buf;
   int   i;

   *c = 0;

   for(i = 0; bodyparts[i] != nullptr; i++)
   {
      sprintf(c, "%2d) %-12s   ", i + 1, bodyparts[i]);
      if((i + 1) % 4 == 0)
      {
         strcat(c, "\n\r");
      }
      TAIL(c);
   }
   if((i + 1) % 4 != 0)
   {
      strcat(c, "\n\r");
   }
   strcat(c, "\n\r");

   send_to_descriptor(Buf, d);
}

void nanny_describe_bodypart(descriptor_data *d, char *arg)
{
   char                     Buf[1000];
   int                      i;
   struct extra_descr_data *exd;

   STATE(d)++;

   if(STATE(d) == 1)
   {
      send_to_descriptor("Which body-part do you wish to describe "
                         "(L for a list, R to return): ",
                         d);
      return;
   }

   if(toupper(*arg) == 'L')
   {
      list_body_parts(d);
      STATE(d) = 0;
      nanny_describe_bodypart(d, arg);
      return;
   }
   if(toupper(*arg) == 'R')
   {
      set_descriptor_fptr(d, nanny_change_information, TRUE);
      return;
   }

   i = atoi(arg);
   if(is_in(i, 1, 29) == 0)
   {
      send_to_descriptor("No such bodypart, please choose 1 to 29.\n\r\n\r", d);
      STATE(d) = 0;
      nanny_describe_bodypart(d, arg);
      return;
   }

   /* Add the description unless it is already there */

   exd = unit_find_extra(bodyparts[i - 1], d->character);

   if((exd != nullptr) && (str_is_empty(exd->descr.String()) == 0u))
   {
      sprintf(Buf, "\n\rYour current '%s' description:\n\r", bodyparts[i - 1]);
      send_to_descriptor(Buf, d);
      send_to_descriptor(exd->descr.String(), d);
   }

   sprintf(Buf,
           "\n\rEnter a text you'd like others to see"
           " when they look at your %s.\n\r",
           bodyparts[i - 1]);

   send_to_descriptor(Buf, d);

   if(exd == nullptr)
   {
      char **namelist = create_namelist();
      namelist        = add_name(bodyparts[i - 1], namelist);

      exd = UNIT_EXTRA_DESCR(d->character) = UNIT_EXTRA_DESCR(d->character)->add((const char **)namelist, nullptr);
      free_namelist(namelist);
   }

   d->editref  = exd;
   d->postedit = edit_extra;
   d->editing  = d->character;

   set_descriptor_fptr(d, nanny_string_add, TRUE);
}

void nanny_change_width(descriptor_data *d, char *arg)
{
   int i;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Enter new screen width (40 to 132): ", d);
      return;
   }

   i = atoi(arg);
   if(is_in(i, 40, 132) == 0)
   {
      send_to_descriptor("Invalid scrren width, please choose a number "
                         "in the range 40 to 132.\n\r",
                         d);
      set_descriptor_fptr(d, nanny_change_width, TRUE);
      return;
   }

   PC_SETUP_WIDTH(d->character) = i;
   set_descriptor_fptr(d, nanny_change_terminal, TRUE);
}

void nanny_change_height(descriptor_data *d, char *arg)
{
   int i;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("Enter new screen height (15 to 60): ", d);
      return;
   }

   i = atoi(arg);
   if(is_in(i, 15, 60) == 0)
   {
      send_to_descriptor("Invalid screen width, please choose a number "
                         "in the range 15 to 60.\n\r",
                         d);
      set_descriptor_fptr(d, nanny_change_height, TRUE);
      return;
   }

   PC_SETUP_HEIGHT(d->character) = i;
   set_descriptor_fptr(d, nanny_change_terminal, TRUE);
}

void nanny_charge_confirm(descriptor_data *d, char *arg)
{
   assert(!PC_IS_UNSAVED(d->character));

   if(STATE(d)++ == 0)
   {
      if(PC_ACCOUNT(d->character).cracks == 2)
      {
         send_to_descriptor(COLOUR_ATTN "\n\rTHIS IS YOUR LAST CHANCE TO "
                                        "ENTER THE CORRECT CODE - PLEASE CHECK "
                                        "CAREFULLY!\n\r\n\r" COLOUR_NORMAL,
                            d);
      }

      send_to_descriptor("Please confirm your transaction by entering the last four digits of \n\r"
                         "your credit card. Press enter to cancel.\n\r"
                         "Last four digits of your credit card: " CONTROL_ECHO_OFF,
                         d);
      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON "\n\r", d);

   if(str_is_empty(arg) != 0u)
   {
      send_to_descriptor(COLOUR_ATTN "\n\r\n\rYou have cancelled your "
                                     "transaction.\n\r" COLOUR_NORMAL,
                         d);
      set_descriptor_fptr(d, nanny_credit_card, TRUE);
      return;
   }

   int i = atoi(arg);

   if(i != PC_ACCOUNT(d->character).last4)
   {
      send_to_descriptor(COLOUR_ATTN "\n\r\n\rCode is incorrect - "
                                     "Please Retry.\n\r" COLOUR_NORMAL,
                         d);

      if(++PC_ACCOUNT(d->character).cracks >= 3)
      {
         send_to_descriptor(COLOUR_ATTN "\n\r\n\rYOU HAVE FAILED TO ENTER THE CORRECT "
                                        "CODE, PLEASE CONTACT STAFF FOR REOPENING YOUR "
                                        "CREDIT CARD.\n\r" COLOUR_NORMAL,
                            d);
         PC_ACCOUNT(d->character).last4  = -1;
         PC_ACCOUNT(d->character).cracks = 0;

         save_player_file(d->character);
         set_descriptor_fptr(d, nanny_menu, TRUE);
         return;
      }

      save_player_file(d->character);

      set_descriptor_fptr(d, nanny_charge_confirm, TRUE);
      return;
   }

   send_to_descriptor(COLOUR_ATTN "\n\rYour player has been credited, "
                                  "you can now continue playing.\n\r" COLOUR_NORMAL,
                      d);

   char *c = d->history;
   char  word[200];
   int   j;

   uint32_t price = 0;

   switch(c[0])
   {
      case 'I':
         c     = str_next_word(c + 1, word); /* The price */
         price = atoi(word) * 100;
         assert(price > 0);
         account_insert(d->character, d->character, price);
         account_cclog(d->character, price);
         break;

      case 'F':
         c = str_next_word(c + 1, word); /* The days */
         i = atoi(word);
         assert(i > 0);
         c     = str_next_word(c, word); /* The price */
         price = atoi(word);
         assert(price > 0);
         account_flatrate_change(d->character, d->character, i);
         account_cclog(d->character, price);
         break;

      default:
         error(HERE, "Unknown account action");
   }

   slog(LOG_ALL, 255, "%s authorized CC transaction of %.2f", UNIT_NAME(d->character), (double)price / 100.0);

   PC_ACCOUNT(d->character).cracks = 0;

   save_player_file(d->character);

   set_descriptor_fptr(d, nanny_menu, TRUE);
}

void nanny_charge(descriptor_data *d, char *arg)
{
   char  buf[1024];
   int   i;
   char *p;

   if(STATE(d)++ == 0)
   {
      sprintf(buf,
              "\n\r"
              "Please enter an amount which you authorize "
              "to withdraw from your credit card. Choose 0 to cancel."
              "\n\r"
              "\n\rAmount in %s: ",
              g_cAccountConfig.m_pCoinName);
      send_to_descriptor(buf, d);
      return;
   }

   for(; (*arg != 0) && (isdigit(*arg) == 0); arg++)
   {
      ;
   }

   for(p = arg; (*p != 0) && (isdigit(*p) != 0); p++)
   {
      ;
   }
   *p = 0;

   i = atoi(arg);

   if(str_is_number(arg) == 0u)
   {
      send_to_descriptor(COLOUR_ATTN "\n\rYou did not enter a number, "
                                     "please retry.\n\r" COLOUR_NORMAL,
                         d);
      set_descriptor_fptr(d, nanny_credit_card, TRUE);
      return;
   }

   if(i == 0)
   {
      send_to_descriptor(COLOUR_ATTN "Your transaction has been "
                                     "cancelled.\n\r" COLOUR_NORMAL,
                         d);
      set_descriptor_fptr(d, nanny_credit_card, TRUE);
      return;
   }

   if(is_in(i, g_cAccountConfig.m_nMinCharge / 100, g_cAccountConfig.m_nMaxCharge / 100) == 0)
   {
      sprintf(buf,
              COLOUR_ATTN "\n\rPlease choose an amount between "
                          "%d and %d %s.\n\r" COLOUR_NORMAL,
              g_cAccountConfig.m_nMinCharge / 100, g_cAccountConfig.m_nMaxCharge / 100, g_cAccountConfig.m_pCoinName);
      send_to_descriptor(buf, d);
      set_descriptor_fptr(d, nanny_charge, TRUE);
      return;
   }

   sprintf(buf, "\n\rYou have chosen to withdraw an amount of %d %s.\n\r", i, g_cAccountConfig.m_pCoinName);
   send_to_descriptor(buf, d);

   /* This is dirty... :) */
   char Buf[200];
   sprintf(Buf, "I %d", i);
   strcpy(d->history, Buf);

   set_descriptor_fptr(d, nanny_charge_confirm, TRUE);
}

void nanny_credit_card(descriptor_data *d, const char *arg)
{
   char Buf[200];

   assert(g_cServerConfig.m_bAccounting);

   if(PC_ACCOUNT(d->character).last4 == -1)
   {
      char mbuf[MAX_INPUT_LENGTH];
      strcpy(mbuf, "credit card registration");

      if(STATE(d)++ == 0)
      {
         do_help(d->character, mbuf, &cmd_auto_unknown);
      }
      else
      {
         set_descriptor_fptr(d, nanny_menu, TRUE);
      }
      return;
   }

   uint32_t discount = PC_ACCOUNT(d->character).discount;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("\n\rCredit Card Menu\n\r\n\r", d);

      if(discount > 0)
      {
         sprintf(Buf,
                 "  Your current overall discount is %d%% and has"
                 "  already been applied to the flat rate offers below."
                 "\n\r\n\r",
                 discount);
         send_to_descriptor(Buf, d);
      }

      send_to_descriptor("  " COLOUR_MENU "1" COLOUR_NORMAL ") Withdrawal on your Credit Card into your MUD account.\n\r", d);

      if(g_cAccountConfig.m_flatrate[0].days > 0)
      {
         /*	 sprintf(Buf, "  "COLOUR_MENU"2"COLOUR_NORMAL
                  ") Authorize a credit card withdrawal of %.2f dollar for "
                  "%d days flat rate\n\r",
                  ((100-discount)*((double) g_cAccountConfig.flatrate[0].price
                  / 100.0)/100),
                  g_cAccountConfig.flatrate[0].days); */

         sprintf(Buf, "  " COLOUR_MENU "2" COLOUR_NORMAL ") %s\n\r", g_cAccountConfig.m_flatrate[0].pMessage);
         send_to_descriptor(Buf, d);
      }

      if(g_cAccountConfig.m_flatrate[1].days > 0)
      {
         /* sprintf(Buf, "  "COLOUR_MENU"3"COLOUR_NORMAL
            ") Authorize a credit card withdrawal of %.2f dollar for "
            "%d days flat rate\n\r",
            ((100-discount)*((double) g_cAccountConfig.flatrate[1].price /
            100.0)/100),
            g_cAccountConfig.flatrate[1].days);
            */
         sprintf(Buf, "  " COLOUR_MENU "3" COLOUR_NORMAL ") %s\n\r", g_cAccountConfig.m_flatrate[1].pMessage);

         send_to_descriptor(Buf, d);
      }

      send_to_descriptor("  " COLOUR_MENU "H" COLOUR_NORMAL ") Read help "
                         "about security\n\r",
                         d);
      send_to_descriptor("  " COLOUR_MENU "R" COLOUR_NORMAL ") Return to "
                         "Main Menu\n\r",
                         d);

      send_to_descriptor("\n\rMake your choice: ", d);
      return;
   }

   switch(toupper(*arg))
   {
      case '1':
         set_descriptor_fptr(d, nanny_charge, TRUE);
         return;

      case '2':
         if(g_cAccountConfig.m_flatrate[0].days <= 0)
         {
            send_to_descriptor("Please choose 1, H or R.\n\r", d);
            set_descriptor_fptr(d, nanny_credit_card, TRUE);
            return;
         }

         set_descriptor_fptr(d, nanny_charge_confirm, TRUE);
         sprintf(Buf, "F %d %d", g_cAccountConfig.m_flatrate[0].days, ((100 - discount) * g_cAccountConfig.m_flatrate[0].price) / 100);
         strcpy(d->history, Buf);
         break;

      case '3':
         if(g_cAccountConfig.m_flatrate[1].days <= 0)
         {
            send_to_descriptor("Please choose 1, H or R.\n\r", d);
            set_descriptor_fptr(d, nanny_credit_card, TRUE);
            return;
         }
         set_descriptor_fptr(d, nanny_charge_confirm, TRUE);
         sprintf(Buf, "F %d %d", g_cAccountConfig.m_flatrate[1].days, ((100 - discount) * g_cAccountConfig.m_flatrate[1].price) / 100);
         strcpy(d->history, Buf);
         break;

      case 'H':
         char mbuf[MAX_INPUT_LENGTH];
         strcpy(mbuf, "credit card security");

         do_help(d->character, mbuf, &cmd_auto_unknown);
         return;

      case 'R':
         set_descriptor_fptr(d, nanny_menu, TRUE);
         return;

      default:
         send_to_descriptor("Please choose 1, H or R.\n\r", d);
         set_descriptor_fptr(d, nanny_credit_card, TRUE);
         break;
   }
}

void nanny_change_terminal(descriptor_data *d, const char *arg)
{
   int n;

   if(STATE(d)++ == 0)
   {
      char buf[2000];

      sprintf(buf,
              "\n\r"
              /*	      "Terminal setup:\n\r"
              "  1. Width  is %d.\n\r"
              "  2. Height is %d.\n\r"
              "  3. Telnet usage is %s.\n\r"
              "  4. Echo is %s.\n\r"
              "  5. Prompt redraw is %s.\n\r\n\r"

              "Emulation setup (currently %s)\n\r"
              "  A. DUMB\n\r"
              "  B. TTY\n\r"
              "  C. ANSI\n\r"
              */

              "Color setup (currently %c):\n\r"
              "  I. White text / black background type.\n\r"
              "  J. Black text / white background type.\n\r"
              "  K. White text / blue background type.\n\r\n\r"

              "  E. Erase your Player\n\r"
              "  P. Change your Password\n\r"
              "  R. Return to main menu\n\r\n\r"
              "Make your choice: ",
              /*
              PC_SETUP_WIDTH(d->character),
              PC_SETUP_HEIGHT(d->character),
              PC_SETUP_TELNET(d->character) ? "on" : "off",
              PC_SETUP_ECHO(d->character) ? "on" : "off",
              PC_SETUP_REDRAW(d->character) ?  "on" : "off",
              PC_SETUP_EMULATION(d->character) == TERM_DUMB ? "DUMB" :
              (PC_SETUP_EMULATION(d->character) == TERM_TTY ? "TTY" :
               (PC_SETUP_EMULATION(d->character) == TERM_ANSI ? "ANSI"
                : "VT100")),*/
              'I' + PC_SETUP_COLOUR(d->character));

      send_to_descriptor(buf, d);
      return;
   }

   switch(toupper(*arg))
   {
      case 'I':
         PC_SETUP_COLOUR(d->character) = 0;
         send_to_descriptor("Now using white text on black.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'J':
         PC_SETUP_COLOUR(d->character) = 1;
         send_to_descriptor("Now using black text on white.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'K':
         PC_SETUP_COLOUR(d->character) = 2;
         send_to_descriptor("Now using white text on blue.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'A':
         PC_SETUP_EMULATION(d->character) = TERM_DUMB;
         send_to_descriptor("Now using DUMB emulation.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'B':
         PC_SETUP_EMULATION(d->character) = TERM_TTY;
         send_to_descriptor("Now using TTY emulation.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'C':
         PC_SETUP_EMULATION(d->character) = TERM_ANSI;
         send_to_descriptor("Now using ANSI emulation.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case '1':
         set_descriptor_fptr(d, nanny_change_width, TRUE);
         return;

      case '2':
         set_descriptor_fptr(d, nanny_change_height, TRUE);
         return;

      case '3':
         PC_SETUP_TELNET(d->character) = static_cast<uint8_t>(!PC_SETUP_TELNET(d->character));
         send_to_descriptor("Telnet option toggled.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case '4':
         PC_SETUP_ECHO(d->character) = static_cast<uint8_t>(!PC_SETUP_ECHO(d->character));
         send_to_descriptor("Echo option toggled.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case '5':
         PC_SETUP_REDRAW(d->character) = static_cast<uint8_t>(!PC_SETUP_REDRAW(d->character));
         send_to_descriptor("Redraw option toggled.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'E':
         set_descriptor_fptr(d, nanny_kill_confirm, TRUE);
         return;

      case 'P':
         set_descriptor_fptr(d, nanny_change_pwd, TRUE);
         return;

      case 'R':
         if(!PC_IS_UNSAVED(d->character))
         {
            save_player_file(d->character);
         }

         set_descriptor_fptr(d, nanny_menu, TRUE);
         MplexSendSetup(d);
         return;

      default:
         send_to_descriptor("Please choose 1-5, A-C, I-K, E, P or R.\n\r", d);
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         break;
   }
}

void nanny_view_descriptions(descriptor_data *d, char *arg)
{
   char                     Buf[1000];
   struct extra_descr_data *exd;
   int                      i;

   STATE(d)++;

   for(exd = UNIT_EXTRA_DESCR(d->character), i = 1; exd != nullptr; i++, exd = exd->next)
   {
      if(exd->names.Name() != nullptr)
      {
         strcpy(Buf, exd->names.Name());
      }
      else
      {
         *Buf = 0;
      }

      if(i >= STATE(d))
      {
         if((exd->names.Name() == nullptr) || search_block(Buf, bodyparts, TRUE) != -1)
         {
            if(exd->names.Name() == nullptr)
            {
               send_to_descriptor("Your default description:\n\r\n\r", d);
            }
            else
            {
               sprintf(Buf, "Your %s description:\n\r\n\r", exd->names.Name());
               send_to_descriptor(Buf, d);
            }

            send_to_descriptor(exd->descr.String(), d);
            send_to_descriptor("\n\rPress return for next", d);
            break;
         }
      }
   }

   if(exd == nullptr)
   {
      set_descriptor_fptr(d, nanny_change_information, TRUE);
   }
}

void nanny_change_information(descriptor_data *d, const char *arg)
{
   struct extra_descr_data *exd;

   if(STATE(d)++ == 0)
   {
      send_to_descriptor("\n\rDescription Change\n\r\n\r"
                         "  1. Enter your General Description\n\r"
                         "  2. Enter Body Part Description\n\r"
                         "  V. View all your Descriptions\n\r"
                         "  R. Return to Main Menu\n\r\n\r"
                         "Make your choice: ",
                         d);
      return;
   }

   switch(toupper(*arg))
   {
      case '1':
         exd = unit_find_extra(UNIT_NAME(d->character), d->character);

         if((exd != nullptr) && (str_is_empty(exd->descr.String()) == 0u))
         {
            send_to_descriptor("\n\rYour current description:\n\r", d);
            send_to_descriptor(exd->descr.String(), d);
         }

         send_to_descriptor("\n\rEnter a text you'd like others to see"
                            " when they look at you.\n\r",
                            d);

         if(exd == nullptr)
         {
            exd = UNIT_EXTRA_DESCR(d->character) = UNIT_EXTRA_DESCR(d->character)->add((char *)nullptr, nullptr);
         }

         d->editref  = exd;
         d->postedit = edit_extra;
         d->editing  = d->character;

         set_descriptor_fptr(d, nanny_string_add, TRUE);
         return;

      case '2':
         set_descriptor_fptr(d, nanny_describe_bodypart, TRUE);
         return;

      case 'V':
         set_descriptor_fptr(d, nanny_view_descriptions, TRUE);
         return;

      case 'R':
         if(!PC_IS_UNSAVED(d->character))
         {
            save_player_file(d->character);
         }
         set_descriptor_fptr(d, nanny_menu, TRUE);
         return;

      default:
         send_to_descriptor("Please choose 1, 2 or R.\n\r", d);
         set_descriptor_fptr(d, nanny_change_information, TRUE);
         break;
   }
}

void nanny_menu(descriptor_data *d, const char *arg)
{
   static int wizi_level = 0;

   if(STATE(d)++ == 0)
   {
      if(wizi_level == 0)
      {
         extern struct trie_type *intr_trie;

         auto *cmd_ptr = (struct command_info *)search_trie("wizinv", intr_trie);

         if(cmd_ptr != nullptr)
         {
            wizi_level = cmd_ptr->minimum_level;
         }
      }
      send_to_descriptor("\n\rWelcome to " MUD_NAME "\n\r\n" COLOUR_MENU "  1" COLOUR_NORMAL ") Enter " MUD_NAME "\n\r" COLOUR_MENU
                         "  2" COLOUR_NORMAL ") Change "
                         "Description\n\r" COLOUR_MENU "  3" COLOUR_NORMAL ") Change Setup\n\r",
                         d);

      if(CHAR_LEVEL(d->character) >= wizi_level)
      {
         send_to_descriptor(COLOUR_MENU "  W" COLOUR_NORMAL ") Set Wizi-Level\n\r", d);
      }

      if((g_cAccountConfig.m_bCreditCard != 0) && (PC_ACCOUNT(d->character).last4 != -1))
      {
         send_to_descriptor(COLOUR_MENU "  C" COLOUR_NORMAL ") Charge on Credit "
                                        "Card\n\r",
                            d);
      }

      send_to_descriptor(COLOUR_MENU "  0" COLOUR_NORMAL ") Exit " MUD_NAME "\n\r", d);

      send_to_descriptor("\n\rMake your choice: ", d);
      return;
   }

   switch(toupper(*arg))
   {
      case '0':
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;

      case '1':
         if(account_is_closed(d->character) != 0)
         {
            account_closed(d->character);
            set_descriptor_fptr(d, nanny_menu, TRUE);
         }
         else
         {
            send_to_descriptor(g_cServerConfig.m_pWelcome, d);
            enter_game(d->character);
         }
         return;

      case '2':
         set_descriptor_fptr(d, nanny_change_information, TRUE);
         return;

      case '3':
         set_descriptor_fptr(d, nanny_change_terminal, TRUE);
         return;

      case 'C':
         if(g_cServerConfig.m_bAccounting != 0)
         {
            set_descriptor_fptr(d, nanny_credit_card, TRUE);
            return;
         }
         break;

      case 'W':
         if(CHAR_LEVEL(d->character) >= wizi_level)
         {
            set_descriptor_fptr(d, nanny_wizi, TRUE);
            return;
         }
         break;
   }

   send_to_descriptor("\n\rWrong option.\n\r", d);
   set_descriptor_fptr(d, nanny_menu, TRUE);
}

void nanny_existing_pwd(descriptor_data *d, char *arg)
{
   char             buf[200];
   descriptor_data *td;
   unit_data       *u;

   /* PC_ID(d->character) can be -1 when a newbie is in the game and
      someone logins with the same name! */

   STATE(d)++;

   if(STATE(d) == 1)
   {
      if(PC_CRACK_ATTEMPTS(d->character) > 2)
      {
         sprintf(buf,
                 COLOUR_ATTN "\n\rATTENTION: Your password has been "
                             "attempted cracked %d times since your last logon."
                             " Press [enter] and wait for the password prompt." COLOUR_NORMAL,
                 PC_CRACK_ATTEMPTS(d->character));
         send_to_descriptor(buf, d);
         d->wait = MIN(30, PC_CRACK_ATTEMPTS(d->character)) * 2 * PULSE_SEC;
         return;
      }
      STATE(d)++;
   }

   if(STATE(d) == 2)
   {
      sprintf(buf, "Welcome back %s, please enter your password: ", UNIT_NAME(d->character));
      send_to_descriptor(buf, d);
      send_to_descriptor(CONTROL_ECHO_OFF, d);
      return;
   }

   send_to_descriptor(CONTROL_ECHO_ON, d);

   if(str_is_empty(arg) != 0u)
   {
      UNIT_NAMES(d->character).Free();
      send_to_descriptor("\n\r\n\rWrong password, please login again.", d);
      set_descriptor_fptr(d, nanny_close, TRUE);
      // set_descriptor_fptr(d, nanny_get_name, FALSE);
      return;
   }

   if(strncmp(crypt(arg, PC_PWD(d->character)), PC_PWD(d->character), 10) != 0)
   {
      if(str_is_empty(arg) == 0u)
      {
         slog(LOG_ALL, 0, "%s entered a wrong password [%s].", PC_FILENAME(d->character), d->host);
         PC_CRACK_ATTEMPTS(d->character)++;

         if((td = find_descriptor(PC_FILENAME(d->character), d)) != nullptr)
         {
            send_to_descriptor(COLOUR_ATTN "\n\rSomeone just attempted to login under "
                                           "your name using an illegal password.\n\r" COLOUR_NORMAL,
                               td);
            PC_CRACK_ATTEMPTS(td->character)++;
            d->wait = PULSE_SEC * 5 + PC_CRACK_ATTEMPTS(td->character) * PULSE_SEC;
         }
         else if(!PC_IS_UNSAVED(d->character))
         {
            save_player_file(d->character);
         }
      }

      send_to_descriptor("\n\rWrong password.\n\r", d);
      set_descriptor_fptr(d, nanny_close, TRUE);
      return;
   }

   PC_CRACK_ATTEMPTS(d->character) = 0;

   sprintf(buf, "\rWelcome back %s, you last visited " MUD_NAME " on %s\r", UNIT_NAME(d->character), ctime(&PC_TIME(d->character).connect));
   send_to_descriptor(buf, d);

   if((td = find_descriptor(PC_FILENAME(d->character), d)) != nullptr)
   {
      set_descriptor_fptr(d, nanny_throw, TRUE);
      return;
   }

   /* See if player is in game (guests are not created in file entries) */
   /* Enters game (reconnects) if true                                  */
   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_PC(u) && str_ccmp(PC_FILENAME(u), PC_FILENAME(d->character)) == 0)
      {
         assert(!CHAR_DESCRIPTOR(u));
         assert(UNIT_IN(u));

         reconnect_game(d, u);
         return;
      }
   }

   /* Ok, he wasn't Link Dead, lets enter the game via menu */
   slog(LOG_BRIEF, CHAR_LEVEL(d->character), "%s[%s] has connected.", PC_FILENAME(d->character), d->host);

   send_to_descriptor("\n\r", d);
   set_descriptor_fptr(d, nanny_motd, TRUE);
}

void nanny_name_confirm(descriptor_data *d, char *arg)
{
   if(STATE(d)++ == 0)
   {
      char buf[100];

      sprintf(buf, "Did I get that right, %s (Y/N/H)? ", UNIT_NAME(d->character));
      send_to_descriptor(buf, d);
      return;
   }

   char mbuf[MAX_INPUT_LENGTH];
   strcpy(mbuf, "login");

   if(nanny_help_check(d, arg, mbuf) != 0)
   {
      STATE(d) = 0;
      nanny_name_confirm(d, arg);
      return;
   }

   if(*arg == 'y' || *arg == 'Y')
   {
      if(site_banned(d->host) != NO_BAN)
      {
         show_ban_text(d->host, d);
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      send_to_descriptor("New character.\n\r", d);
      set_descriptor_fptr(d, nanny_new_pwd, TRUE);
   }
   else if(*arg == 'n' || *arg == 'N')
   {
      UNIT_NAMES(d->character).Free();
      send_to_descriptor("Ok, what IS it, then? ", d);
      set_descriptor_fptr(d, nanny_get_name, FALSE);
   }
   else
   {
      send_to_descriptor("Please type Yes, No or Help: ", d);
   }
}

void nanny_get_name(descriptor_data *d, char *arg)
{
   char             tmp_name[100];
   descriptor_data *td;
   int              n;

   if(str_is_empty(arg) != 0u)
   {
      set_descriptor_fptr(d, nanny_close, TRUE);
      return;
   }

   if(_parse_name(arg, tmp_name) != 0)
   {
      send_to_descriptor("Illegal name, please try another.\n\r", d);
      send_to_descriptor("Name: ", d);
      return;
   }

   if(player_exists(tmp_name) != 0)
   {
      unit_data *ch;

      if(site_banned(d->host) == BAN_TOTAL)
      {
         show_ban_text(d->host, d);
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      /* Known player, lets load his character information. */
      ch = load_player(tmp_name);

      if(ch == nullptr)
      {
         send_to_descriptor("LOAD ERROR! PLEASE MAKE A NOTE OF ANY "
                            "SPECIAL ACTIONS YOU MAY HAVE TAKEN JUST "
                            "PRIOR TO YOUR LAST QUIT OR PERHAPS EVEN "
                            "GAME CRASH. ENTER WITH A GUEST AND CONTACT "
                            "A GOD.",
                            d);
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      CHAR_DESCRIPTOR(d->character) = nullptr;
      extract_unit(d->character);

      CHAR_DESCRIPTOR(ch) = d;
      d->character        = ch;

      if((wizlock != 0) && CHAR_LEVEL(d->character) < wizlock)
      {
         send_to_descriptor("Sorry, the game is wizlocked for "
                            "your level.\n\r",
                            d);
         slog(LOG_BRIEF, 0, "Wizlock lockout for %s.", PC_FILENAME(d->character));
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      MplexSendSetup(d);
      set_descriptor_fptr(d, nanny_existing_pwd, TRUE);
      return;
   }

   /* Check for wizlock */
   if(wizlock)
   {
      send_to_descriptor("Sorry, no new players now, the game "
                         "is wizlocked!\n\r",
                         d);
      slog(LOG_BRIEF, 0, "Wizlock lockout for %s.", PC_FILENAME(d->character));
      set_descriptor_fptr(d, nanny_close, TRUE);
      return;
   }

   /* New player                                           */
   /* Check for both duplicate descriptors, and link death */

   /* all in lowercase... */
   strcpy(PC_FILENAME(d->character), tmp_name);

   CAPC(tmp_name);
   UNIT_NAMES(d->character).AppendName(tmp_name);
   strcpy(PC_PWD(d->character), "");

   /* If someone is connected, we borrow his pwd */
   if((td = find_descriptor(tmp_name, d)))
   {
      strcpy(PC_PWD(d->character), PC_PWD(td->character));
      set_descriptor_fptr(d, nanny_existing_pwd, TRUE);
      return;
   }

   /* Check for LD after PWD confirmation, this is due to */
   /* that the PWD is lost when a guests link is lost!    */

   set_descriptor_fptr(d, nanny_name_confirm, TRUE);
}
