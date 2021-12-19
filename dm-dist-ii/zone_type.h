#pragma once

#include "bin_search_type.h"
#include "namelist.h"
class file_index_type;
class zone_reset_cmd;

/* A linked/sorted list of all the zones in the game */
class zone_type
{
public:
   zone_type();
   ~zone_type();

   cNamelist creators; /* List of creators of zone         */
   char     *name;     /* Unique in list                   */
   char     *title;    /* A nice looking zone title        */
   char     *notes;    /* Creator notes to zone            */
   char     *help;     /* User-Help to zone                */
   char     *filename; /* The filename of this file        */

   file_index_type *fi;   /* Pointer to list of file-index's  */
   bin_search_type *ba;   /* Pointer to binarray of type      */
   zone_reset_cmd  *zri;  /* List of Zone reset commands      */
   zone_type       *next; /* Next Zone                        */

   struct diltemplate *tmpl;   /* DIL templates in zone            */
   bin_search_type    *tmplba; /* Pointer to binarray of type      */

   uint8_t **spmatrix; /* Shortest Path Matrix             */

   uint16_t zone_no;    /* Zone index counter (spmatrix)    */
   uint16_t no_of_fi;   /* Number of fi's in the list       */
   uint16_t zone_time;  /* How often to reset zone          */
   uint16_t no_rooms;   /* The number of rooms              */
   uint8_t  reset_mode; /* when/how to reset zone           */
   uint16_t no_tmpl;    /* number of DIL templates          */

   uint8_t access;    /* Access Level 0 = highest (root)  */
   uint8_t loadlevel; /* Level required to load items     */
   uint8_t payonly;   /* TRUE when only 4 paying players  */

   struct
   {
      int pressure; /* How is the pressure ( Mb )            */
      int change;   /* How fast and what way does it change. */
      int sky;      /* How is the sky.                       */
      int base;     /* What is the basis pressure like?      */
   } weather;
};
