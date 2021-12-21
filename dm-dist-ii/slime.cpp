/* *********************************************************************** *
 * File   : slime.c                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Slime routines.                                                *
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

#include "comm.h"
#include "db.h"
#include "files.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

extern std::shared_ptr<file_index_type> slime_fi;

extern char libdir[];

// struct file_index_type **slime_list = NULL;
std::vector<std::shared_ptr<file_index_type>> slime_list;

static void slime_save(void)
{
   FILE *f;

   if(!(f = fopen(str_cc(libdir, SLIME_FILE), "wb")))
   {
      slog(LOG_ALL, 0, "Slime file could not be opened.");
      assert(FALSE);
   }

   for(const auto &slime : slime_list)
   {
      fputs(slime->zone->name, f);
      fputc(0, f);
      fputs(slime->name, f);
      fputc(0, f);
   }
   fclose(f);
}

static void slime_add(std::shared_ptr<file_index_type> sp)
{
   if(sp == NULL)
      return;

   slime_list.push_back(sp);
}

static void slime_remove(std::shared_ptr<file_index_type> sp)
{
   for(auto i = slime_list.begin(); i != slime_list.end();)
   {
      if(*i == sp)
      {
         i = slime_list.erase(i);
         break;
      }
      else
      {
         ++i;
      }
   }
}

int is_slimed(std::shared_ptr<file_index_type> sp)
{
   for(auto i = slime_list.begin(); i != slime_list.end(); ++i)
      if(*i == sp)
         return TRUE;

   return FALSE;
}

int slime_obj(struct spec_arg *sarg)
{
   char                             buf[MAX_INPUT_LENGTH], fi_name[MAX_INPUT_LENGTH];
   std::shared_ptr<file_index_type> fi;

   if(!is_command(sarg->cmd, "slime"))
      return SFR_SHARE;

   if(!IS_OVERSEER(sarg->activator))
   {
      send_to_char("Only overseers can use this function.\n\r", sarg->activator);
      return SFR_BLOCK;
   }

   sarg->arg = one_argument(sarg->arg, buf);

   if(is_abbrev(buf, "list"))
   {
      send_to_char("List of slimed units:\n\r", sarg->activator);
      for(const auto &slime : slime_list)
      {
         sprintf(buf, "%s@%s\n\r", slime->name, slime->zone->name);
         send_to_char(buf, sarg->activator);
      }

      return SFR_BLOCK;
   }

   sarg->arg = one_argument(sarg->arg, fi_name);

   fi = str_to_file_index(fi_name);

   if(fi == NULL)
   {
      act("No such file index '$2t'.", A_ALWAYS, sarg->activator, fi_name, 0, TO_CHAR);
      return SFR_BLOCK;
   }

   if(fi->zone == find_zone(BASIS_ZONE))
   {
      act("Basis zone is not allowed slimed.", A_ALWAYS, sarg->activator, 0, 0, TO_CHAR);
      return SFR_BLOCK;
   }

   if(is_abbrev(buf, "add"))
   {
      if(is_slimed(fi))
      {
         send_to_char("Already slimed.\n\r", sarg->activator);
      }
      else
      {
         slime_add(fi);
         slime_save();
         send_to_char("Added.\n\r", sarg->activator);
      }
   }
   else if(is_abbrev(buf, "remove"))
   {
      if(!is_slimed(fi))
      {
         send_to_char("No such file index is slimed!\n\r", sarg->activator);
      }
      else
      {
         slime_remove(fi);
         slime_save();
         send_to_char("Removed.\n\r", sarg->activator);
      }
   }
   else
   {
      act("Please specify 'add' or 'remove'.", A_ALWAYS, sarg->activator, 0, 0, TO_CHAR);
   }

   return SFR_BLOCK;
}

void slime_boot(void)
{
   std::shared_ptr<file_index_type> fi;
   CByteBuffer                      cBuf(100);
   char                             buf1[256], buf2[256];
   FILE                            *f;

   touch_file(str_cc(libdir, SLIME_FILE));
   if(!(f = fopen(str_cc(libdir, SLIME_FILE), "rb")))
   {
      slog(LOG_ALL, 0, "Slime file could not be opened.");
      assert(FALSE);
   }

   while(!feof(f))
   {
      fstrcpy(&cBuf, f);
      strcpy(buf1, (char *)cBuf.GetData());

      fstrcpy(&cBuf, f);
      strcpy(buf2, (char *)cBuf.GetData());

      fi = find_file_index(buf1, buf2);
      slime_add(fi);
   }
   fclose(f);
}
