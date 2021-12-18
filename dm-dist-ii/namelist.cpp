/* *********************************************************************** *
 * File   : namelist.c                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ Class of the popular Namelist                              *
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
#include <cstring>

#include "db_file.h"
#include "essential.h"
#include "namelist.h"
#include "textutil.h"

cNamelist::cNamelist()
{
   namelist = nullptr;
   length   = 0;
}

void cNamelist::CopyList(const char *const *const list)
{
   if(list != nullptr)
   {
      for(int i = 0; list[i] != nullptr; i++)
      {
         AppendName(list[i]);
      }
   }
}

void cNamelist::CopyList(class cNamelist *cn)
{
   for(uint32_t i = 0; i < cn->Length(); i++)
   {
      AppendName(cn->Name(i));
   }
}

void cNamelist::AppendBuffer(CByteBuffer *pBuf)
{
   for(uint32_t i = 0; i < length; i++)
   {
      if(str_is_empty(Name(i)) == 0u)
      {
         pBuf->AppendString(Name(i));
      }
   }

   pBuf->AppendString("");
}

auto cNamelist::ReadBuffer(CByteBuffer *pBuf) -> int
{
   char *c;

   for(;;)
   {
      if(pBuf->SkipString(&c) != 0)
      {
         return 1;
      }

      if(*c != 0)
      {
         AppendName(c);
      }
      else
      {
         break;
      }
   }

   return 0;
}

void cNamelist::bread(uint8_t **b)
{
   char *c;

   for(;;)
   {
      c = bread_str_skip(b);

      if(*c != 0)
      {
         AppendName(c);
      }
      else
      {
         break;
      }
   }
}

void cNamelist::catnames(char *s)
{
   uint32_t i;
   bool  ok = FALSE;

   strcpy(s, "{");
   TAIL(s);
   if(Length() > 0)
   {
      for(i = 0; i < Length(); i++)
      {
         ok = TRUE;
         sprintf(s, "\"%s\",", Name(i));
         TAIL(s);
      }
      if(ok != 0u)
      {
         s--; /* remove the comma */
      }
   }
   strcpy(s, "}");
}

void cNamelist::Remove(uint32_t idx)
{
   if(length > idx)
   {
      delete namelist[idx];
      namelist[idx] = namelist[length - 1];
      length--;
      if(length == 0)
      {
         free(namelist);
         namelist = nullptr;
      }
   }
}

void cNamelist::RemoveName(const char *name)
{
   uint32_t i;
   uint32_t j;

   for(i = 0; i < length; i++)
   {
      for(j = 0; namelist[i]->String()[j] != 0; j++)
      {
         if(tolower(name[j]) != tolower(namelist[i]->String()[j]))
         {
            break;
         }
      }

      if(namelist[i]->String()[j] == 0)
      {
         if((name[j] == 0) || isaspace(name[j]))
         {
            Remove(i);
            return;
         }
      }
   }
}

void cNamelist::Substitute(uint32_t idx, const char *newname)
{
   if(length > idx)
   {
      delete namelist[idx];
      namelist[idx] = new cStringInstance(newname);
   }
}

cNamelist::cNamelist(const char **list)
{
   namelist = nullptr;
   length   = 0;

   CopyList(list);
}

void cNamelist::Free()
{
   uint32_t i;

   for(i = 0; i < length; i++)
   {
      delete namelist[i];
   }

   if(namelist != nullptr)
   {
      free(namelist);
   }

   namelist = nullptr;
   length   = 0;
}

cNamelist::~cNamelist()
{
   Free();
}

auto cNamelist::Duplicate() -> cNamelist *
{
   auto  *pNl = new cNamelist;
   uint32_t i   = 0;

   for(i = 0; i < length; i++)
   {
      pNl->AppendName((char *)namelist[i]->String());
   }

   return pNl;
}

auto cNamelist::IsNameRaw(const char *name) -> const char *
{
   uint32_t i;
   uint32_t j;

   for(i = 0; i < length; i++)
   {
      for(j = 0; namelist[i]->String()[j] != 0; j++)
      {
         if(tolower(name[j]) != tolower(namelist[i]->String()[j]))
         {
            break;
         }
      }

      if(namelist[i]->String()[j] == 0)
      {
         if((name[j] == 0) || isaspace(name[j]))
         {
            return ((char *)name) + j;
         }
      }
   }

   return nullptr;
}

auto cNamelist::IsName(const char *name) -> const char *
{
   char buf[MAX_STRING_LENGTH];

   name = skip_spaces(name);

   if(str_is_empty(name) != 0u)
   {
      return nullptr;
   }

   strcpy(buf, name);
   str_remspc(buf);

   return IsNameRaw(buf);
}

/* Returns -1 if no name matches, or 0.. for the index in the namelist */
auto cNamelist::IsNameRawIdx(const char *name) -> const int
{
   uint32_t i;
   uint32_t j;

   for(i = 0; i < length; i++)
   {
      for(j = 0; namelist[i]->String()[j] != 0; j++)
      {
         if(tolower(name[j]) != tolower(namelist[i]->String()[j]))
         {
            break;
         }
      }

      if(namelist[i]->String()[j] == 0)
      {
         if((name[j] == 0) || isaspace(name[j]))
         {
            return i;
         }
      }
   }

   return -1;
}

/* As IsNameRawIdx */
auto cNamelist::IsNameIdx(const char *name) -> const int
{
   char buf[MAX_STRING_LENGTH];

   name = skip_spaces(name);

   if(str_is_empty(name) != 0u)
   {
      return -1;
   }

   strcpy(buf, name);
   str_remspc(buf);

   return IsNameRawIdx(buf);
}

auto cNamelist::Name(uint32_t idx) -> const char *
{
   if(idx < length)
   {
      return namelist[idx]->String();
   }

   return nullptr;
}

auto cNamelist::InstanceName(uint32_t idx) -> cStringInstance *
{
   if(idx < length)
   {
      return namelist[idx];
   }

   return nullptr;
}

void cNamelist::AppendName(const char *name)
{
   if(str_is_empty(name) == 0u)
   {
      length++;

      if(namelist == nullptr)
      {
         CREATE(namelist, class cStringInstance *, length);
      }
      else
      {
         RECREATE(namelist, class cStringInstance *, length);
      }

      namelist[length - 1] = new cStringInstance(name);
   }
}

void cNamelist::PrependName(const char *name)
{
   if(str_is_empty(name) == 0u)
   {
      length++;

      if(namelist == nullptr)
      {
         CREATE(namelist, class cStringInstance *, length);
      }
      else
      {
         RECREATE(namelist, class cStringInstance *, length);
      }

      if(length > 1)
      {
         memmove(&namelist[1], &namelist[0], sizeof(class cStringInstance *) * (length - 1));
      }

      namelist[0] = new cStringInstance(name);
   }
}
