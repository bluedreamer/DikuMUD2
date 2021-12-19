/* *********************************************************************** *
 * File   : board.c                                   Part of Valhalla MUD *
 * Version: 1.30                                                           *
 * Author : gnort@daimi.aau.dk                                             *
 *                                                                         *
 * Purpose: A bulletin board service.                                      *
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
/*
  Usage in zone file:
  special SFUN_BULLETIN_BOARD ["board-name [L###]"]

  Boards are kept in files in lib/boards/

  Later on, other exceptions to boardusage can easily, and without corrution
  of existing files, be added for guild (?), race, whatever.
*/

/* Tue Jul  6 1993 HHS: added exchangable lib dir                           */
/* Tue Aug  7 1993 Gnort: added reply due to popular (1) demand             */
/* Mon Sep 27 1993 Gnort: fixed bug introduced in init_board() (replicating */
/*                        boards)                                           */
/* 11/04/94  seifert: Added Statements to free() unfreed memory in read.    */
/* Jan 9, 1995 gnort: Changed the way filenames are used...                 */
/* Mar 2, 1995 seif : Fixed nasty free bug in write / remove                */

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "blkfile.h"
#include "comm.h"
#include "db.h"
#include "db_file.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "nanny.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"

#define MAX_MSGS 50 /* Maximum number of messages      */
/* Bugger, I just realized that this value can't be modified without
 * deleting/updating the board-files.  I'll have to look at that... /gnort
 */

#define MAX_MESSAGE_LENGTH 4000 /* Should be plenty                */
#define BOARD_BLK_SIZE     128  /* Size of the blocks              */

extern char libdir[]; /* from dikumud.c */

/* local structs */

struct board_message
{
   char  *header; /* Header of message               */
   char  *owner;  /* Who wrote it?                   */
   time_t time;   /* When?                           */
   char  *text;   /* What? (temporary)               */
};

struct board_info
{
   BLK_FILE            *bf;             /* File containing it all          */
   blk_handle          *handles;        /* The handles to access above file*/
   struct board_message msgs[MAX_MSGS]; /* Who, what, when                 */
   int                  min_level;      /* What level is required?         */
   struct board_info   *next;           /* Next board in linked list       */
};

/* globals */
extern descriptor_data *descriptor_list;

struct board_info *board_list = nullptr; /* Linked list of boards           */

#ifndef DOS
/* extern fncts */

#endif

/* local fncts */

auto show_board(const unit_data * /*ch*/, unit_data * /*board*/, struct board_info * /*tb*/, char * /*arg*/) -> int;
void write_board(unit_data * /*ch*/, struct board_info * /*tb*/, char * /*arg*/, unit_data * /*board*/);
auto read_board(unit_data * /*ch*/, struct board_info * /*tb*/, char * /*arg*/) -> int;
auto reply_board(unit_data * /*ch*/, struct board_info * /*tb*/, char * /*arg*/, unit_data *board) -> int;
auto remove_msg(unit_data * /*ch*/, struct board_info * /*tb*/, char * /*arg*/) -> int;
void save_board_msg(struct board_info * /*tb*/, int /*index*/, char * /*text*/);
void load_board_msg(struct board_info * /*tb*/, int /*index*/, bool /*text*/);
auto init_board(char * /*file_name*/) -> struct board_info *;
auto get_board(struct unit_fptr * /*fptr*/) -> struct board_info *;

auto board(struct spec_arg *sarg) -> int
{
   unit_data         *u;
   struct board_info *tb;
   char              *arg = (char *)sarg->arg;

   if(sarg->activator == nullptr || !IS_CHAR(sarg->activator) || !CHAR_DESCRIPTOR(sarg->activator))
   {
      return SFR_SHARE;
   }

   if((tb = get_board(sarg->fptr)) == nullptr)
   {
      return SFR_SHARE;
   }

   if(tb->min_level > CHAR_LEVEL(sarg->activator))
   {
      return SFR_SHARE;
   }

   switch(sarg->cmd->no)
   {
      case CMD_LOOK:
      case CMD_EXAMINE:
         return show_board(sarg->activator, sarg->owner, tb, (char *)sarg->arg);

      case CMD_WRITE:
         u = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_INVEN | FIND_UNIT_EQUIP);

         if(u != nullptr)
         {
            return SFR_SHARE;
         }

         u = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_SURRO);

         if(u != sarg->owner)
         {
            arg = (char *)sarg->arg;
         }

         write_board(sarg->activator, tb, arg, sarg->owner);
         return SFR_BLOCK;

      case CMD_REPLY:
         return reply_board(sarg->activator, tb, (char *)sarg->arg, sarg->owner);

      case CMD_READ:
         return read_board(sarg->activator, tb, (char *)sarg->arg);

      case CMD_REMOVE:
         return remove_msg(sarg->activator, tb, (char *)sarg->arg);

      default:
         return SFR_SHARE;
   }
}

auto show_board(const unit_data *ch, unit_data *board, struct board_info *tb, char *arg) -> int
{
   char  tmp[MAX_INPUT_LENGTH];
   char  buf[10000]; /* Enough with 50 messages */
   int   i;
   bool  shown = FALSE;
   char *c     = arg;

   if(find_unit(ch, &c, nullptr, FIND_UNIT_SURRO) != board)
   {
      return SFR_SHARE;
   }

   act("$1n studies $2n.", A_HIDEINV, ch, board, nullptr, TO_ROOM);

   strcpy(buf, "This is a bulletin board.\n\r"
               "Usage: READ/REPLY/REMOVE <msg #>, WRITE <header>\n\r");

   for(i = 0; i < MAX_MSGS; i++)
   {
      if(tb->handles[i] == BLK_NULL)
      {
         continue;
      }
      shown = TRUE;

      sprintf(tmp, "%2d - %s : %-*s (%s)\n\r", i + 1, timetodate(tb->msgs[i].time), 55 - (int)strlen(tb->msgs[i].owner), tb->msgs[i].header,
              tb->msgs[i].owner);
      strcat(buf, tmp);
   }

   if(!shown)
   {
      strcat(buf, "The board is empty.\n\r");
   }

   page_string(CHAR_DESCRIPTOR(ch), buf);

   return SFR_BLOCK;
}

struct board_save_info
{
   struct board_info *tb;
   int                index;
};

void compact_board(struct board_info *tb);

void edit_board(descriptor_data *d)
{
   /* Oh God, I hate this :-)  /gnort */
   /* Dont worry, I have fixed it... MS */
   /* Do the board save boogie... */

   assert(d->editref);

   int len = strlen(d->localstr);

   /* add final newline for board-messages, if needed */
   if(len < 2 || (len > 1 && (d->localstr)[len - 2] != '\n' && (d->localstr)[len - 1] != '\r'))
   {
      strcat(d->localstr, "\n\r");
   }

   save_board_msg(((struct board_save_info *)d->editref)->tb, ((struct board_save_info *)d->editref)->index, d->localstr);

   struct board_info *tb = ((struct board_save_info *)d->editref)->tb;

   free(d->editref);
   d->editref = nullptr;

   compact_board(tb);
}

/*
   Since 'remove' can not take properly care of rearranging messages, this
   routine is desgined to do so. Returns TRUE if something was compacted.
*/

void compact_board(struct board_info *tb)
{
   descriptor_data *d;
   int              index;
   int              compacted = FALSE;

   for(index = 0; index < MAX_MSGS - 1; index++)
   {
      if(tb->handles[index] == BLK_NULL)
      {
         break;
      }
   }

   if(index >= MAX_MSGS)
   {
      return;
   }

   /* We got a blank entry */

   for(; index < MAX_MSGS - 1; index++)
   {
      for(d = descriptor_list; d != nullptr; d = d->next)
      {
         if(d->postedit == edit_board && (d->editref != nullptr) && ((struct board_save_info *)d->editref)->tb == tb &&
            ((((struct board_save_info *)d->editref)->index == index) || ((struct board_save_info *)d->editref)->index == index + 1))
         {
            break; // Can't compact any more!
         }
      }

      if(d != nullptr)
      {
         break;
      }

      tb->msgs[index]    = tb->msgs[index + 1];
      tb->handles[index] = tb->handles[index + 1];

      if(tb->handles[index] != BLK_NULL)
      {
         compacted = TRUE;
      }

      tb->handles[index + 1]     = BLK_NULL;
      tb->msgs[index + 1].header = tb->msgs[index + 1].owner = nullptr;
   }

   if(compacted != 0)
   {
      compact_board(tb);
   }
   else
   {
      blk_write_reserved(tb->bf, tb->handles, MAX_MSGS * sizeof(blk_handle));
   }
}

void write_board(unit_data *ch, struct board_info *tb, char *arg, unit_data *board)
{
   int              i;
   descriptor_data *d;

   for(i = 0; i < MAX_MSGS; i++)
   {
      if(tb->handles[i] == BLK_NULL)
      {
         for(d = descriptor_list; d != nullptr; d = d->next)
         {
            if(d->postedit == edit_board && (d->editref != nullptr) && ((struct board_save_info *)d->editref)->tb == tb &&
               ((struct board_save_info *)d->editref)->index == i)
            {
               break;
            }
         }

         if(d == nullptr)
         {
            break;
         }
      }
   }

   if(i >= MAX_MSGS)
   {
      send_to_char("Sorry, the board is full.\n\r", ch);
      return;
   }

   arg = skip_spaces(arg);

   if(*arg == 0)
   {
      send_to_char("You have to write a headline!\n\r", ch);
      return;
   }

   if(strlen(arg) + strlen(TITLENAME(ch)) > 55) /* Evil, but it works */
   {
      send_to_char("Sorry, headline too long.\n\r", ch);
      return;
   }

   /* We crashed here once on Wed May 22, 1996 */
   if(tb->msgs[i].text != nullptr)
   { /* was somebody writing, and lost link? */
      free(tb->msgs[i].text);
   }

   if(tb->msgs[i].header != nullptr)
   {
      free(tb->msgs[i].header);
   }
   if(tb->msgs[i].owner != nullptr)
   {
      free(tb->msgs[i].owner);
   }

   tb->msgs[i].header = str_dup(arg);
   tb->msgs[i].owner  = str_dup(TITLENAME(ch));
   tb->msgs[i].time   = time(nullptr);
   tb->msgs[i].text   = str_dup("");

   send_to_char("You begin to write a message on the board.\n\r", ch);
   act("$1n starts to write a message.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);

   struct board_save_info *bsi;

   CREATE(bsi, struct board_save_info, 1);

   bsi->tb    = tb;
   bsi->index = i;

   CHAR_DESCRIPTOR(ch)->postedit = edit_board;
   CHAR_DESCRIPTOR(ch)->editing  = board;
   CHAR_DESCRIPTOR(ch)->editref  = bsi;

   set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add, TRUE);
}

auto reply_board(unit_data *ch, struct board_info *tb, char *arg, unit_data *board) -> int
{
   char number[MAX_INPUT_LENGTH];
   char head[80];
   int  msg;

   one_argument(arg, number);

   if(str_is_number(number) == 0u)
   {
      return SFR_SHARE;
   }

   msg = atoi(number);

   if(tb->handles[0] == BLK_NULL)
   {
      send_to_char("But the board is empty!\n\r", ch);
      return SFR_BLOCK;
   }

   if(msg < 1 || msg > MAX_MSGS || tb->handles[msg - 1] == BLK_NULL)
   {
      send_to_char("That message exists only within the boundaries of your"
                   " mind...\n\r",
                   ch);
      return SFR_BLOCK;
   }

   /* Add a 'Re: ' */
   if(str_nccmp(tb->msgs[msg - 1].header, "re: ", 4) != 0)
   { /* No re: */
      sprintf(head, "Re: %.*s", 51 - (int)strlen(TITLENAME(ch)), tb->msgs[msg - 1].header);
   }
   else
   { /* Just use existing re: */
      sprintf(head, "%.*s", 55 - (int)strlen(TITLENAME(ch)), tb->msgs[msg - 1].header);
   }

   write_board(ch, tb, head, board);
   return SFR_BLOCK;
}

auto read_board(unit_data *ch, struct board_info *tb, char *arg) -> int
{
   char number[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int  msg;

   one_argument(arg, number);

   if(str_is_number(number) == 0u)
   {
      return SFR_SHARE;
   }

   msg = atoi(number);

   if(tb->handles[0] == BLK_NULL)
   {
      send_to_char("But the board is empty!\n\r", ch);
      return SFR_BLOCK;
   }

   if(msg < 1 || msg > MAX_MSGS || tb->handles[msg - 1] == BLK_NULL)
   {
      send_to_char("That message exists only within the boundaries of your"
                   " mind...\n\r",
                   ch);
      return SFR_BLOCK;
   }

   load_board_msg(tb, msg - 1, TRUE);

   sprintf(buf, "Message %d : %s (%s)\n\r\n\r%s", msg, tb->msgs[msg - 1].header, tb->msgs[msg - 1].owner,
           tb->msgs[msg - 1].text != nullptr ? tb->msgs[msg - 1].text : "");

   if(tb->msgs[msg - 1].text != nullptr)
   {
      free(tb->msgs[msg - 1].text);
      tb->msgs[msg - 1].text = nullptr;
   }

   page_string(CHAR_DESCRIPTOR(ch), buf);
   return SFR_BLOCK;
}

auto remove_msg(unit_data *ch, struct board_info *tb, char *arg) -> int
{
   int  msg;
   int  index;
   char number[MAX_INPUT_LENGTH];

   one_argument(arg, number);

   if(str_is_number(number) == 0u)
   {
      return SFR_SHARE;
   }

   msg = atoi(number);

   if(tb->handles[0] == BLK_NULL)
   {
      send_to_char("But the board is empty!\n\r", ch);
      return SFR_BLOCK;
   }

   if(msg < 1 || msg > MAX_MSGS || tb->handles[msg - 1] == BLK_NULL)
   {
      send_to_char("That message exists only within the boundaries of your"
                   " mind...\n\r",
                   ch);
      return SFR_BLOCK;
   }

   index = msg - 1;

   if(IS_MORTAL(ch) && (str_ccmp(tb->msgs[index].owner, TITLENAME(ch)) != 0))
   {
      send_to_char("You may only remove your own messages.\n\r", ch);
      return SFR_BLOCK;
   }

   free(tb->msgs[index].header);
   free(tb->msgs[index].owner);

   blk_delete(tb->bf, tb->handles[index]);

   tb->handles[index]     = BLK_NULL;
   tb->msgs[index].header = tb->msgs[index].owner = nullptr;

   send_to_char("Message removed.\n\r", ch);
   act("$1n just removed message $2d.", A_HIDEINV, ch, &msg, nullptr, TO_ROOM);

   blk_write_reserved(tb->bf, tb->handles, MAX_MSGS * sizeof(blk_handle));

   compact_board(tb);

#ifdef BUGGY
   /* SHit, this does NOT work because of the string_add routine which changes
      the POINTER to text[index]!!! (See write_msg) */

   for(d = descriptor_list; d; d = d->next)
      if(d->board.tb == tb)
      {
         assert(d->board.index != index); /* Can't happen */
         if(d->board.index > index)
            --d->board.index;
      }

   for(; index < MAX_MSGS - 1; index++)
   {
      tb->msgs[index]    = tb->msgs[index + 1];
      tb->handles[index] = tb->handles[index + 1];
   }

#endif

   return SFR_BLOCK;
}

void save_board_msg(struct board_info *tb, int index, char *text)
{
   uint8_t   *buffer;
   uint8_t   *start;
   blk_length len;
   uint8_t    buf[4 * MAX_STRING_LENGTH];

   if(tb->handles[index] != BLK_NULL)
   {
      blk_delete(tb->bf, tb->handles[index]);
   }

   start = buffer = buf;

   bwrite_string(&buffer, tb->msgs[index].header);
   bwrite_string(&buffer, tb->msgs[index].owner);
   bwrite_uint32_t(&buffer, tb->msgs[index].time);

   tb->msgs[index].text = str_dup(text);
   bwrite_string(&buffer, tb->msgs[index].text);

   len = buffer - start;

   assert(len < 4 * MAX_STRING_LENGTH);

   tb->handles[index] = blk_write(tb->bf, start, len);

   blk_write_reserved(tb->bf, tb->handles, MAX_MSGS * sizeof(blk_handle));

   free(tb->msgs[index].text);
   tb->msgs[index].text = nullptr;
}

void load_board_msg(struct board_info *tb, int index, bool text)
{
   uint8_t *buffer;
   uint8_t *keep;
   char     buf[MAX_STRING_LENGTH];

   keep = buffer = (uint8_t *)blk_read(tb->bf, tb->handles[index], nullptr);

   if(buffer == nullptr) /* Read error */
   {
      tb->msgs[index].header = str_dup("DESTROYED");
      tb->msgs[index].owner  = str_dup("none");
      tb->msgs[index].time   = 0;
      tb->msgs[index].text   = nullptr;

      return;
   }

   if(text)
   {
      bread_strcpy(&buffer, buf); /* Skip Header */
      bread_strcpy(&buffer, buf); /* Skip Owner  */
      bread_uint32_t(&buffer);
      tb->msgs[index].text = bread_str_alloc(&buffer);
   }
   else
   {
      tb->msgs[index].header = bread_str_alloc(&buffer);
      tb->msgs[index].owner  = bread_str_alloc(&buffer);
      tb->msgs[index].time   = bread_uint32_t(&buffer);
      tb->msgs[index].text   = nullptr;
   }

   free(keep);
}

auto init_board(char *file_name) -> struct board_info *
{
   int                i;
   struct board_info *tb;

   touch_file(file_name);

   CREATE(tb, struct board_info, 1);

   if((tb->bf = blk_open(file_name, BOARD_BLK_SIZE)) == nullptr)
   {
      assert(FALSE);
   }

   tb->min_level = 0;
   tb->next      = board_list;
   board_list    = tb;

   if((tb->handles = (blk_handle *)blk_read_reserved(tb->bf, nullptr)) == nullptr)
   {
      CREATE(tb->handles, blk_handle, MAX_MSGS);
      for(i = 0; i < MAX_MSGS; i++)
      {
         tb->handles[i] = BLK_NULL;
      }
   }

   for(i = 0; i < MAX_MSGS; i++)
   {
      if(tb->handles[i] == BLK_NULL)
      {
         tb->msgs[i].header = nullptr;
         tb->msgs[i].owner  = nullptr;
         tb->msgs[i].time   = 0;
         tb->msgs[i].text   = nullptr;
      }
      else
      {
         load_board_msg(tb, i, FALSE); /* no text loaded, only headers */
      }
   }

   return tb;
}

auto get_board(struct unit_fptr *fptr) -> struct board_info *
{
   struct board_info *tb;
   char               file_name[200];
   char               file_name2[200];
   int                level = 0;

   if(fptr->data != nullptr)
   {
      char *lp;

      lp = one_argument((char *)fptr->data, file_name);
      lp = skip_spaces(lp);

      switch(*lp++)
      {
         case 'L':
            level = atoi(lp);
            break;
            /* Here there be Dragons */
      }
   }
   else
   {
      slog(LOG_ALL, 0, "Board without file-name initialization...");
      return nullptr;
   }

   sprintf(file_name2, "%sboards/%s", libdir, file_name);

   for(tb = board_list; tb != nullptr; tb = tb->next)
   {
      if(strcmp(file_name2, tb->bf->name) == 0)
      {
         break;
      }
   }

   if(tb == nullptr)
   {
      tb = init_board(file_name2);
   }

   tb->min_level = MAX(level, tb->min_level);

   return tb;
}

void do_boards(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data         *u;
   struct unit_fptr  *f = nullptr;
   struct board_info *b;
   char               buf[256];
   char               tmp[256];

   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_OBJ(u) && UNIT_IN(u) && UNIT_MINV(u) <= CHAR_LEVEL(ch) && ((f = find_fptr(u, SFUN_BULLETIN_BOARD)) != nullptr))
      {
         b = get_board(f);
         if(b->min_level <= (CHAR_LEVEL(ch)))
         {
            one_argument((char *)f->data, tmp);

            sprintf(buf, "%-30s %-12s [%s@%s]\n\r", UNIT_SEE_TITLE(ch, u), tmp, UNIT_FI_NAME(UNIT_IN(u)), UNIT_FI_ZONENAME(UNIT_IN(u)));
            send_to_char(buf, ch);
         }
      }
   }
}
