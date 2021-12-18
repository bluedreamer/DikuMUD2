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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "comm.h"
#include "db.h"
#include "files.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

extern struct file_index_type *slime_fi;

extern char libdir[];

static int               slime_count = 0;
struct file_index_type **slime_list  = nullptr;

static void slime_save()
{
   int   i;
   FILE *f;

   if((f = fopen(str_cc(libdir, SLIME_FILE), "wb")) == nullptr)
   {
      slog(LOG_ALL, 0, "Slime file could not be opened.");
      assert(FALSE);
   }

   for(i = 0; i < slime_count; i++)
   {
      fputs(slime_list[i]->zone->name, f);
      fputc(0, f);
      fputs(slime_list[i]->name, f);
      fputc(0, f);
   }
   fclose(f);
}

static void slime_add(struct file_index_type *sp)
{
   if(sp == nullptr)
   {
      return;
   }

   if(slime_count++ == 0)
   {
      CREATE(slime_list, struct file_index_type *, slime_count);
   }
   else
   {
      RECREATE(slime_list, struct file_index_type *, slime_count);
   }

   slime_list[slime_count - 1] = sp;
}

static void slime_remove(struct file_index_type *sp)
{
   int i;

   for(i = 0; i < slime_count; i++)
   {
      if(slime_list[i] == sp)
      {
         slime_list[i] = slime_list[slime_count - 1];
         slime_count--;
         if(slime_count == 0)
         {
            free(slime_list);
            slime_list = nullptr;
         }
         break;
      }
   }
}

auto is_slimed(struct file_index_type *sp) -> int
{
   int i;

   for(i = 0; i < slime_count; i++)
   {
      if(slime_list[i] == sp)
      {
         return TRUE;
      }
   }

   return FALSE;
}

auto slime_obj(struct spec_arg *sarg) -> int

{
   char                    buf[MAX_INPUT_LENGTH];
   char                    fi_name[MAX_INPUT_LENGTH];
   struct file_index_type *fi;

   if(is_command(sarg->cmd, "slime") == 0u)
   {
      return SFR_SHARE;
   }

   if(!IS_OVERSEER(sarg->activator))
   {
      send_to_char("Only overseers can use this function.\n\r", sarg->activator);
      return SFR_BLOCK;
   }

   sarg->arg = one_argument(sarg->arg, buf);

   if(is_abbrev(buf, "list") != 0u)
   {
      int i;
      send_to_char("List of slimed units:\n\r", sarg->activator);
      for(i = 0; i < slime_count; i++)
      {
         sprintf(buf, "%s@%s\n\r", slime_list[i]->name, slime_list[i]->zone->name);
         send_to_char(buf, sarg->activator);
      }

      return SFR_BLOCK;
   }

   sarg->arg = one_argument(sarg->arg, fi_name);

   fi = str_to_file_index(fi_name);

   if(fi == nullptr)
   {
      act("No such file index '$2t'.", A_ALWAYS, sarg->activator, fi_name, nullptr, TO_CHAR);
      return SFR_BLOCK;
   }

   if(fi->zone == find_zone(BASIS_ZONE))
   {
      act("Basis zone is not allowed slimed.", A_ALWAYS, sarg->activator, nullptr, nullptr, TO_CHAR);
      return SFR_BLOCK;
   }

   if(is_abbrev(buf, "add") != 0u)
   {
      if(is_slimed(fi) != 0)
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
   else if(is_abbrev(buf, "remove") != 0u)
   {
      if(is_slimed(fi) == 0)
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
      act("Please specify 'add' or 'remove'.", A_ALWAYS, sarg->activator, nullptr, nullptr, TO_CHAR);
   }

   return SFR_BLOCK;
}

void slime_boot()
{
   struct file_index_type *fi;
   CByteBuffer             cBuf(100);
   char                    buf1[256];
   char                    buf2[256];
   FILE                   *f;

   touch_file(str_cc(libdir, SLIME_FILE));
   if((f = fopen(str_cc(libdir, SLIME_FILE), "rb")) == nullptr)
   {
      slog(LOG_ALL, 0, "Slime file could not be opened.");
      assert(FALSE);
   }

   while(feof(f) == 0)
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
