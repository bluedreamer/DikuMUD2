#include "extra.h"

#include "constants.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

extra_descr_data::extra_descr_data()
{
   next = nullptr;
}

extra_descr_data::~extra_descr_data()
{
   next = nullptr;
}

void extra_descr_data::AppendBuffer(CByteBuffer *pBuf)
{
   uint8_t  u;
   int      i = 0;
   uint32_t nOrgPos;
   uint32_t nPos;

   nOrgPos = pBuf->GetLength();

   pBuf->Append8(0); /* Assume no extra description */

   extra_descr_data *e = this;

   /* While description is non null, keep writing */
   for(; e != nullptr; e = e->next)
   {
      i++;
      pBuf->AppendString(e->descr.StringPtr());
      e->names.AppendBuffer(pBuf);
   }

   assert(i <= 255);

   if(i > 0)
   {
      nPos = pBuf->GetLength();
      pBuf->SetLength(nOrgPos);
      u = i;
      pBuf->Append8(u); /* Assume no extra description */
      pBuf->SetLength(nPos);
   }
}

void extra_descr_data::free_list()
{
   class extra_descr_data *tmp_descr;
   class extra_descr_data *ex;

   ex = this;

   while(ex != nullptr)
   {
      tmp_descr = ex;
      ex        = ex->next;
      delete tmp_descr;
   }
}

/*                                                            */
/* Only matches on complete letter by letter match!           */
/* The empty word "" or NULL matches an empty namelist.       */
/*                                                            */

auto extra_descr_data::find_raw(const char *word) -> class extra_descr_data *
{
   uint32_t i;

   // MS2020 warning. if (this == 0) return NULL;

   for(class extra_descr_data *exd = this; exd != nullptr; exd = exd->next)
   {
      if(exd->names.Length() < 1)
      {
         if(static_cast<unsigned int>(str_is_empty(word)) != 0U)
         {
            return exd;
         }
      }
      else
      {
         for(i = 0; i < exd->names.Length(); i++)
         {
            if(str_ccmp(word, exd->names.Name(i)) == 0)
            {
               return exd;
            }
         }
      }
   }

   return nullptr;
}

/*                                                            */
/* Adds an extra description just before the one given (this) */
/*                                                            */

auto extra_descr_data::add(const char **names, const char *descr) -> class extra_descr_data *
{
   class extra_descr_data *new_ex;

   new_ex = new(class extra_descr_data);

   if(names != nullptr)
   {
      new_ex->names.CopyList(names);
   }

   if(static_cast<unsigned int>(str_is_empty(descr)) == 0U)
   {
      new_ex->descr.Reassign(descr);
   }

   new_ex->next = this;

   return new_ex;
}

/*                                                            */
/* Adds an extra description just before the one given (this) */
/*                                                            */

auto extra_descr_data::add(const char *name, const char *descr) -> class extra_descr_data *
{
   const char *names[2];

   names[0] = name;
   names[1] = nullptr;

   return add(names, descr);
}

auto extra_descr_data::remove(class extra_descr_data *exd) -> class extra_descr_data *
{
   class extra_descr_data *list = this;

   assert(list && exd);

   if(list == exd)
   {
      list = exd->next;
      delete exd;

      return list;
   }
   else
   {
      class extra_descr_data *pex;

      for(pex = list; (pex != nullptr) && (pex->next != nullptr); pex = pex->next)
      {
         if(pex->next == exd)
         {
            pex->next = exd->next;
            delete exd;
            return list;
         }
      }
   }

   /* This should not be possible */

   error(HERE, "Remove extra descr got unmatching list and exd.\n");
   return nullptr;
}

auto extra_descr_data::remove(const char *name) -> class extra_descr_data *
{
   // MS2020 warning if (this == NULL)     return NULL;

   class extra_descr_data *tex = find_raw(name);

   if(tex != nullptr)
   {
      return this->remove(tex);
   }
   return this;
}

/* ===================================================================== */
/*   The following should really be in the unit_data class, but we dont  */
/*   have it yet...                                                      */

/*  We don't want people to be able to see $-prefixed extras
 *  so check for that...
 */
auto unit_find_extra(const char *word, unit_data *unit) -> class extra_descr_data *
{
   word = skip_spaces(word);

   if((unit != nullptr) && *word != '$')
   {
      class extra_descr_data *i;

      word = skip_spaces(word);

      for(i = UNIT_EXTRA_DESCR(unit); i != nullptr; i = i->next)
      {
         if(i->names.Name() != nullptr)
         {
            if(i->names.Name(0)[0] == '$')
            {
               continue;
            }

            if(i->names.IsName((char *)word) != nullptr)
            {
               return i;
            }
         }
         else if(UNIT_NAMES(unit).IsName((char *)word) != nullptr)
         {
            return i;
         }
      }
   }

   return nullptr;
}

auto char_unit_find_extra(unit_data *ch, unit_data **target, char *word, unit_data *list) -> class extra_descr_data *
{
   class extra_descr_data *exd = nullptr;

   for(; list != nullptr; list = list->next)
   {
      if(CHAR_CAN_SEE(ch, list) && ((exd = unit_find_extra(word, list)) != nullptr))
      {
         if(target != nullptr)
         {
            *target = list;
         }
         return exd;
      }
   }

   if(target != nullptr)
   {
      *target = nullptr;
   }

   return nullptr;
}

auto unit_find_extra_string(unit_data *ch, char *word, unit_data *list) -> const char *
{
   class extra_descr_data *exd = char_unit_find_extra(ch, nullptr, word, list);

   if(exd != nullptr)
   {
      return exd->descr.String();
   }

   return nullptr;
}
