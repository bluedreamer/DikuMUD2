/* *********************************************************************** *
 * File   : postoffice.c                              Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Program for handling mail between players.                     *
 *                                                                         *
 * Idea   : Perhaps erase the mail index file, and use the blocked file    *
 *          reserved read/write capabilities as in player read/write.      *
 *          Reason I havn't is that reserved was made later than this.     *
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
/* Tue Jul 6 1993 HHS: added exchangable lib dir                           */

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "blkfile.h"
#include "comm.h"
#include "db.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "postoffice.h"
#include "structs.h"
#include "textutil.h"
#include "unixshit.h"
#include "utility.h"
#include "utils.h"

#define MAIL_MAX_AGE    SECS_PER_REAL_DAY * 90 /* 90 days lifetime */
#define MAIL_BLOCK_SIZE 128

extern char libdir[]; /* from dikumud.c */

struct mail_file_info_type *mailfile_info = nullptr;

int16_t   top_of_mailfile = 0;
BLK_FILE *mbf; /* Mail Block File */
auto      find_player_id(char *pName) -> int;

void read_mail_block(FILE *f, int16_t index)
{
   if(fseek(f, index * sizeof(struct mail_file_info_type), SEEK_SET) != 0)
   {
      assert(FALSE);
   }

   if(fread(&mailfile_info[index], sizeof(struct mail_file_info_type), 1, f) != 1)
   {
      assert(FALSE);
   }
}

void write_mail_block(FILE *f, int16_t index)
{
   if(fseek(f, index * sizeof(struct mail_file_info_type), SEEK_SET) != 0)
   {
      assert(FALSE);
   }

   if(fwrite(&mailfile_info[index], sizeof(struct mail_file_info_type), 1, f) != 1)
   {
      assert(FALSE);
   }
   fflush(f);
}

auto find_free_index() -> int16_t
{
   int16_t i;

   for(i = 0; i < top_of_mailfile; i++)
   {
      if(mailfile_info[i].handle == BLK_NULL)
      {
         return i;
      }
   }

   /* We need make larger array, remember to alloc more disk-space */
   if(top_of_mailfile == 0)
   {
      CREATE(mailfile_info, struct mail_file_info_type, 1);
   }
   else
   {
      RECREATE(mailfile_info, struct mail_file_info_type, top_of_mailfile + 1);
   }

   mailfile_info[top_of_mailfile].handle = BLK_NULL;
   mailfile_info[top_of_mailfile].loaded = FALSE;

   top_of_mailfile++;

   return (top_of_mailfile - 1);
}

/* Delete mail starting at index "index" */
void delete_mail(int16_t index)
{
   FILE *f;

   if((f = fopen_cache(str_cc(libdir, MAIL_FILE_NAME), "r+b")) == nullptr)
   {
      assert(FALSE);
   }

   assert(mailfile_info[index].handle != BLK_NULL);

   blk_delete(mbf, mailfile_info[index].handle);

   mailfile_info[index].handle = BLK_NULL;
   write_mail_block(f, index); /* Save on disk                      */

   fflush(f);
   /* Was fclose(f) */
}

/* Read mail from file, starting at index "index" */
/* String is allocated                            */
auto read_mail(int16_t index) -> char *
{
   char *msg;

   assert(mailfile_info[index].handle != BLK_NULL);

   mailfile_info[index].loaded = TRUE;
   msg                         = (char *)blk_read(mbf, mailfile_info[index].handle, nullptr);

   return msg;
}

/* Save the letter pointed to by str in the mail file */
void mail_mail(int receipient, char *rcv_name, struct unit_data *sender, const char *str)
{
   FILE                 *f;
   int16_t               index;
   blk_length            len;
   char                  from[MAX_STRING_LENGTH];
   char                 *b;
   struct time_info_data t;
   char                 *all;

   void mudtime_strcpy(struct time_info_data * time, char *str);
   auto mud_date(time_t t)->struct time_info_data;

   if((f = fopen_cache(str_cc(libdir, MAIL_FILE_NAME), "r+b")) == nullptr)
   {
      assert(FALSE);
   }

   index = find_free_index();

   mailfile_info[index].loaded     = FALSE;
   mailfile_info[index].date       = time(nullptr);
   mailfile_info[index].receipient = receipient;
   mailfile_info[index].sender     = PC_ID(sender);

   b = from;
   sprintf(b, "From %s at ", UNIT_NAME(sender));
   TAIL(b);

   t = mud_date(mailfile_info[index].date);
   mudtime_strcpy(&t, b);
   TAIL(b);
   strcpy(b, "\n\r\n\r");

   CREATE(all, char, strlen(from) + strlen(str) + 1);
   strcpy(all, from);
   strcat(all, str);

   len                         = strlen(all) + 1;
   mailfile_info[index].handle = blk_write(mbf, all, len);

   write_mail_block(f, index);

   fflush(f); /* Was fclose(f); */
   free(all);
}

/* Return index to the next letter of a player */
auto player_next_mail(struct unit_data *ch, int16_t index) -> int16_t
{
   int i;

   assert(index >= 0);

   for(i = index; i < top_of_mailfile; i++)
   {
      if((mailfile_info[i].handle != BLK_NULL) && (mailfile_info[i].loaded == 0u) && (mailfile_info[i].receipient == PC_ID(ch)))
      {
         return i;
      }
   }

   return -1;
}

/* Return index to the first letter of the player */
auto player_has_mail(struct unit_data *ch) -> uint8_t
{
   return static_cast<uint8_t>(player_next_mail(ch, 0) != -1);
}

void mail_boot()
{
   long                       size;
   uint32_t                   now;
   int16_t                    index;
   int16_t                    wi;
   FILE                      *f;
   struct mail_file_info_type maildata;

   touch_file(str_cc(libdir, MAIL_BLOCK_NAME));
   touch_file(str_cc(libdir, MAIL_FILE_NAME));

   mbf = blk_open(str_cc(libdir, MAIL_BLOCK_NAME), MAIL_BLOCK_SIZE);

   if((f = fopen_cache(str_cc(libdir, MAIL_FILE_NAME), "r+b")) == nullptr)
   {
      assert(FALSE);
   }

   size = fsize(f) / sizeof(struct mail_file_info_type);

   if(size >= 1)
   {
      CREATE(mailfile_info, struct mail_file_info_type, size);
      top_of_mailfile = size;
   }
   rewind(f);

   now = time(nullptr);

   for(index = 0;; index++)
   {
      if(fread(&maildata, sizeof(struct mail_file_info_type), 1, f) != 1)
      {
         break;
      }

      if(index >= top_of_mailfile)
      {
         wi = find_free_index();
      }
      else
      {
         wi = index;
      }

      mailfile_info[wi] = maildata;
   }

   /* Was fclose(f) */
   fflush(f);
}

auto eat_and_delete(struct spec_arg *sarg) -> int
{
   int16_t           index;
   struct unit_data *u;
   char             *arg = (char *)sarg->arg;

   if(sarg->cmd->no == CMD_EAT)
   {
      u = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_INVEN);
      if(u != sarg->owner)
      {
         return SFR_SHARE;
      }

      index = *((int16_t *)sarg->fptr->data);
      act("$1n eats $2n.", A_SOMEONE, sarg->activator, sarg->owner, nullptr, TO_ROOM);
      act("You eat the $2N and erase it permanently.", A_SOMEONE, sarg->activator, sarg->owner, nullptr, TO_CHAR);
      delete_mail(index);
      extract_unit(sarg->owner);
      return SFR_BLOCK;
   }
   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      index                       = *((int16_t *)sarg->fptr->data);
      mailfile_info[index].loaded = FALSE;
   }

   return SFR_SHARE;
}

auto postman(struct spec_arg *sarg) -> int
{
   char                    *b;
   char                    *arg = (char *)sarg->arg;
   struct extra_descr_data *exd;
   struct descriptor_data  *d;
   struct unit_data        *letter;
   char                     tmpname[MAX_INPUT_LENGTH];
   int16_t                  index;
   int16_t                 *tmp;
   long                     rcp;
   currency_t               currency = local_currency(sarg->owner);
   amount_t                 postage;

   extern struct file_index_type *letter_fi;

   if((CHAR_POS(sarg->owner) < POSITION_STANDING) || (sarg->activator == nullptr))
   {
      return SFR_SHARE;
   }

   if((sarg->cmd->no != CMD_MAIL) && (sarg->cmd->no != CMD_REQUEST))
   {
      return SFR_SHARE;
   }

   if(!IS_PC(sarg->activator) || PC_IS_UNSAVED(sarg->activator))
   {
      send_to_char("You can't do that!", sarg->activator);
      return SFR_BLOCK;
   }

   if(sarg->cmd->no == CMD_MAIL)
   {
      if((letter = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_INVEN)) == nullptr)
      {
         send_to_char("You don't seem to have such a letter?\n\r", sarg->activator);
         return SFR_BLOCK;
      }
      if(!IS_OBJ(letter) || OBJ_TYPE(letter) != ITEM_NOTE)
      {
         act("$1n says, '$3n, I'll only accept notes to be delivered.'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
         return SFR_BLOCK;
      }

      exd = unit_find_extra(UNIT_NAME(letter), letter);
      if((exd == nullptr) || (str_is_empty(exd->descr.String()) != 0u))
      {
         act("$1n says, 'How stupid of you $3n, the note is empty!", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
         return SFR_BLOCK;
      }

      one_argument(arg, tmpname);

      if(str_is_empty(tmpname) != 0u)
      {
         act("$1n says, 'You must say who the letter should be sent to!'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
         return SFR_BLOCK;
      }

      if((rcp = find_player_id(tmpname)) == -1)
      {
         act("$1n says, 'Sorry $3n, but I don't know that address.'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
         return SFR_BLOCK;
      }

      postage = exd->descr.Length() / 20;
      postage = MAX(1, MIN(50, postage));

      act("$3n gives $1n a letter.", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_NOTVICT);

      if(char_can_afford(sarg->activator, postage, currency) == 0u)
      {
         act("$1n says, '$3n - even though you dont have any money for "
             "postage I'll send it anyway.'",
             A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      }
      else
      {
         act("$1n says, '$3n, I'll take $2t as postage.'", A_SOMEONE, sarg->owner, money_string(postage, currency, TRUE), sarg->activator,
             TO_ROOM);

         money_transfer(sarg->activator, sarg->owner, postage, currency);
      }

      mail_mail(rcp, tmpname, sarg->activator, exd->descr.String());

      /* It would be less extreme to travel through descriptor_list */
      /* but I'm not up to considering all exceptions               */
      if((d = find_descriptor(tmpname, nullptr)) != nullptr)
      {
         if(descriptor_is_playing(d) != 0)
         {
            send_to_char("You feel a sudden urge to visit the post office.\n\r", d->character);
         }
      }

      /* Delete the note to be mailed */
      extract_unit(letter);
      return SFR_BLOCK;
   }
   /* Request Mail */
   if((index = player_next_mail(sarg->activator, 0)) < 0)
   {
      act("$1n says, 'No mail for you today $3n.'", A_SOMEONE, sarg->owner, 0, sarg->activator,
          UNIT_MINV(sarg->activator) ? TO_VICT : TO_ROOM);
      return SFR_BLOCK;
   }

   do
   {
      /* Make this into a loaded note */
      letter = read_unit(letter_fi);
      unit_to_unit(letter, sarg->activator);
      b = read_mail(index);

      if(b == NULL)
         UNIT_EXTRA_DESCR(letter) = UNIT_EXTRA_DESCR(letter)->add((char *)NULL, "DESTROYED BY MAILBOX ERROR!");
      else
      {
         UNIT_EXTRA_DESCR(letter) = UNIT_EXTRA_DESCR(letter)->add((char *)NULL, b);
         free(b);
      }

      CREATE(tmp, int16_t, 1);
      *tmp = index;
      create_fptr(letter, SFUN_EAT_AND_DELETE, 0, SFB_CMD, tmp);
   } while((index = player_next_mail(sarg->activator, index + 1)) >= 0);

   act("$1n gives you your mail.", A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
   act("$1n gives $3n $3s mail.", A_SOMEONE, sarg->owner, 0, sarg->activator, TO_NOTVICT);

   return SFR_BLOCK;
}
