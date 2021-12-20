#include "zone_type.h"

#include "essential.h"
#include "file_index_type.h"
#include "zone_reset_cmd.h"

zone_type::zone_type()
{
   name     = nullptr;
   notes    = nullptr;
   help     = nullptr;
   filename = nullptr;

   fi = nullptr;
   ba = nullptr;

   zri  = nullptr;
   next = nullptr;

   tmpl   = nullptr;
   tmplba = nullptr;

   spmatrix = nullptr;

   access = 255;
}

zone_type::~zone_type()
{
   if(name != nullptr)
   {
      free(name);
   }

   if(title != nullptr)
   {
      free(title);
   }

   if(notes != nullptr)
   {
      free(notes);
   }

   if(help != nullptr)
   {
      free(help);
   }

   if(filename != nullptr)
   {
      free(filename);
   }

   file_index_type *p;
   file_index_type *nextfi;

   for(p = fi; p != nullptr; p = nextfi)
   {
      nextfi = p->next;
      delete p;
   }

   zone_reset_cmd *pzri;
   zone_reset_cmd *nextzri;

   for(pzri = zri; pzri != nullptr; pzri = nextzri)
   {
      nextzri = pzri->next;
      free(pzri);
   }

   /*
   diltemplate *pt, *nextpt;

   for (pt = tmpl; pt; pt = nextpt)
   {
      nextpt = pt->next;

      free(pt->prgname);
      free(pt->argt);
      free(pt->core);
      free(pt->vart);

      free(pt);
   }
*/

   // struct bin_search_type *ba;    /* Pointer to binarray of type      */
   // diltemplate *tmpl;      /* DIL templates in zone            */
   // struct bin_search_type *tmplba;/* Pointer to binarray of type      */

   free(spmatrix);
}
