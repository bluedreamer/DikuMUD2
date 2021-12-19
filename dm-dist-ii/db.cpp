#include "reception.h"

/* *********************************************************************** *
 * File   : db.c                                      Part of Valhalla MUD *
 * Version: 1.25                                                           *
 * Author : seifert@diku.dk and quinn@diku.dk                              *
 *                                                                         *
 * Purpose: Database stuff.                                                *
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
/* Tue Jul 6 1993 HHS: added exchangable zon dir                           */
/* 23/08/93 jubal   : check for no fi's in generate_bin_arrays             */
/* 23/08/93 jubal   : check for error before returning find_file_index     */
/* 12/09/94 gnort   : cleaned up a bit                                     */
/* 01/07/95 HHS     : added template loading and checking                  */

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "account.h"
#include "affect.h"
#include "comm.h"
#include "common.h"
#include "connectionlog.h"
#include "db.h"
#include "db_file.h"
#include "dil.h"
#include "dilrun.h"
#include "files.h"
#include "handler.h"
#include "money.h"
#include "skills.h"
#include "structs.h"
#include "textutil.h"
#include "unixshit.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"
#include <climits>

int        room_number;         /* For counting numbers in rooms */
unit_data *unit_list = nullptr; /* The global unit_list          */

/* Global permanent element of zone info */
struct zone_info_type zone_info = {0, nullptr, nullptr, nullptr};

extern char     zondir[];
extern uint32_t memory_total_alloc;

auto create_direction_data() -> struct room_direction_data *;

/*  Generate array of bin_search_type for the zone_list, and for each
 *  zone's file_index's.
 */
void generate_bin_arrays()
{
   file_index_type *fi;
   class zone_type *z;
   int              i;

   /* Generate array for zones */
   CREATE(zone_info.ba, struct bin_search_type, zone_info.no_of_zones);
   for(i = 0, z = zone_info.zone_list; z != nullptr; z = z->next, i++)
   {
      zone_info.ba[i].block   = z;
      zone_info.ba[i].compare = z->name;
   }

   /* Generate array for file indexes for each zone */
   for(z = zone_info.zone_list; z != nullptr; z = z->next)
   {
      if(z->no_of_fi != 0u)
      {
         CREATE(z->ba, struct bin_search_type, z->no_of_fi);
         for(fi = z->fi, i = 0; fi != nullptr; fi = fi->next, i++)
         {
            z->ba[i].block   = fi;
            z->ba[i].compare = fi->name;
         }
      }

      if(z->no_tmpl != 0u)
      {
         struct diltemplate *tmpl;

         CREATE(z->tmplba, struct bin_search_type, z->no_tmpl);
         for(tmpl = z->tmpl, i = 0; tmpl != nullptr; tmpl = tmpl->next, i++)
         {
            z->tmplba[i].block   = tmpl;
            z->tmplba[i].compare = tmpl->prgname;
         }
      }
   }
}

/* Resolves DIL templates loaded boottime */
void resolve_templates()
{
   class zone_type    *z;
   struct diltemplate *tmpl;
   int                 i;
   int                 j;
   int                 valid;

   /* all zones */
   for(z = zone_info.zone_list; z != nullptr; z = z->next)
   {
      /* all templates in zone */
      for(tmpl = z->tmpl; tmpl != nullptr; tmpl = tmpl->next)
      {
         /* all external references */
         for(i = 0; i < tmpl->xrefcount; i++)
         {
            tmpl->extprg[i] = find_dil_template(tmpl->xrefs[i].name);
            valid           = 1;

            if(tmpl->extprg[i] != nullptr)
            {
               /* check argument count and types */
               if((tmpl->xrefs[i].rtnt != tmpl->extprg[i]->rtnt) || (tmpl->xrefs[i].argc != tmpl->extprg[i]->argc))
               {
                  valid = 0;
               }
               for(j = 0; j < tmpl->xrefs[i].argc; j++)
               {
                  if(tmpl->xrefs[i].argt[j] != tmpl->extprg[i]->argt[j])
                  {
                     valid = 0;
                  }
               }
            }
            else
            {
               /* ERROR MESSAGE HERE */
               szonelog(z, "Cannot resolve external reference '%s'", tmpl->xrefs[i].name);
            }
            /* Typecheck error ! */
            if(valid == 0)
            {
               tmpl->extprg[i] = nullptr;
               /* ERROR MESSAGE HERE */
               szonelog(z, "Error typechecking reference to '%s'", tmpl->xrefs[i].name);
            }
         }
      }
   }
}

/* Generate and read DIL templates */
auto generate_templates(FILE *f, struct zone_type *zone) -> struct diltemplate *
{
   struct diltemplate *tmpllist;
   struct diltemplate *tmpl;
   CByteBuffer         Buf;
   uint32_t            tmplsize = 0;
   char                nBuf[256];
   char                zBuf[256];

   tmpllist = nullptr;

   /*
    * The global templates are preceded with their length
    * written by write_template() in db_file.c
    */

   if(fread(&(tmplsize), sizeof(uint32_t), 1, f) != 1)
   {
      error(HERE, "Failed to fread() tmplsize");
   }

   while((tmplsize != 0u) && (feof(f) == 0))
   {
      Buf.FileRead(f, tmplsize);

      tmpl = bread_diltemplate(&Buf);

      if(tmpl != nullptr)
      {
         struct diltemplate *tfi2;
         struct diltemplate *tfi1;

         tmpl->zone = zone;

         split_fi_ref(tmpl->prgname, zBuf, nBuf);

         free(tmpl->prgname);

         tmpl->prgname = str_dup(nBuf);

         /* Link into list of indexes */

         if(tmpllist == nullptr)
         {
            tmpllist = tmpl; /* If list is empty */
         }
         else
         {
            for(tfi2 = nullptr, tfi1 = tmpllist; tfi1 != nullptr; tfi1 = tfi1->next)
            {
               if(strcmp(tfi1->prgname, tmpl->prgname) > 0)
               {
                  break;
               }
               tfi2 = tfi1;
            }
            if(tfi2 == nullptr)
            {
               tmpl->next = tmpllist;
               tmpllist   = tmpl;
            }
            else
            {
               tmpl->next = tfi1;
               tfi2->next = tmpl;
            }
         }

         zone->no_tmpl++;
      }
      /* next size */
      if(fread(&(tmplsize), sizeof(uint32_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() tmplsize");
      }
   }

   return tmpllist;
}

/* Generate index's for each unit in the file 'f', zone 'zone' */
auto generate_file_indexes(FILE *f, class zone_type *zone) -> file_index_type *
{
   file_index_type *fi;
   file_index_type *fi_list;
   file_index_type *tfi1;
   file_index_type *tfi2;
   CByteBuffer      cBuf(100);

   fi_list     = nullptr;
   room_number = 0;

   for(;;)
   {
      fstrcpy(&cBuf, f);

      if(feof(f) != 0)
      {
         break;
      }

      fi = new file_index_type();
      zone->no_of_fi++;

      fi->name     = str_dup((char *)cBuf.GetData());
      fi->zone     = zone;
      fi->room_ptr = nullptr;
      fi->crc      = 0;

      if(fread(&(fi->type), sizeof(uint8_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() fi->type");
      }

      if(fread(&(fi->length), sizeof(uint32_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() fi->length");
      }

      if(fread(&(fi->crc), sizeof(uint32_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() fi->crc");
      }

      if(fi->type == UNIT_ST_ROOM)
      {
         fi->room_no = room_number++;
      }
      else
      {
         fi->room_no = 0;
      }

      /* We are now positioned at first data byte */
      fi->filepos   = ftell(f);
      fi->no_in_mem = 0;

      /* Seek forward to next index, so we are ready */
      fseek(f, fi->filepos + fi->length, SEEK_SET);

      /* Link into list of indexes */
      if(fi_list == nullptr)
      {
         fi_list = fi; /* If list is empty */
      }
      else
      {
         for(tfi2 = nullptr, tfi1 = fi_list; tfi1 != nullptr; tfi1 = tfi1->next)
         {
            if(strcmp(tfi1->name, fi->name) > 0)
            {
               break;
            }
            tfi2 = tfi1;
         }
         if(tfi2 == nullptr)
         {
            fi->next = fi_list;
            fi_list  = fi;
         }
         else
         {
            fi->next   = tfi1;
            tfi2->next = fi;
         }
      }
   }

   return fi_list;
}

/* Call this routine at boot time, to index all zones */
void generate_zone_indexes()
{
   class zone_type *z;
   class zone_type *tz1;
   class zone_type *tz2;
   extern int       mud_bootzone;
   char             zone[82];
   char             tmpbuf[82];
   char             filename[82 + 41];
   char             buf[MAX_STRING_LENGTH];
   CByteBuffer      cBuf(MAX_STRING_LENGTH);
   FILE            *f;
   FILE            *zone_file;
   char            *c;
   uint8_t          access;
   uint8_t          loadlevel;
   uint8_t          payonly;

   zone_info.no_of_zones = 0;
   zone_info.zone_list   = nullptr;

   if((zone_file = fopen(str_cc(zondir, ZONE_FILE_LIST), "r")) == nullptr)
   {
      slog(LOG_OFF, 0, "Could not open file containing filenames of zones: %s", ZONE_FILE_LIST);
      exit(0);
   }

   for(;;)
   {
      /* Get name of next zone-file */
      if(fgets(buf, 80, zone_file) == nullptr)
      {
         break;
      }

      if(*skip_blanks(buf) == '#')
      {
         continue;
      }

      c = str_next_word_copy(buf, zone);

      if(str_is_empty(zone) != 0u)
      {
         break;
      }

      sprintf(filename, "%s%s.data", zondir, zone);

      /* Skip password */
      c = str_next_word_copy(c, tmpbuf);

      /* Skip priority level for DIL compiler */
      c = str_next_word_copy(c, tmpbuf);

      /* Read priority level */
      c = str_next_word_copy(c, tmpbuf);

      if(str_is_number(tmpbuf) != 0u)
      {
         access = atoi(tmpbuf);
      }
      else
      {
         access = 255;
      }

      /* Read load level */
      c = str_next_word_copy(c, tmpbuf);

      if(str_is_number(tmpbuf) != 0u)
      {
         loadlevel = MAX(200, atoi(tmpbuf));
      }
      else
      {
         loadlevel = 200;
      }

      /* Read pay only */
      c = str_next_word_copy(c, tmpbuf);

      if(str_is_number(tmpbuf) != 0u)
      {
         payonly = atoi(tmpbuf);
      }
      else
      {
         payonly = FALSE;
      }

      if((f = fopen_cache(filename, "rb")) == nullptr)
      {
         slog(LOG_OFF, 0, "Could not open data file: %s", filename);
         continue; /* Next file, please */
      }

      if(fsize(f) <= 3)
      {
         slog(LOG_OFF, 0, "Data file empty: %s", filename);
         continue; /* Next file, please */
      }

      slog(LOG_ALL, 0, "Indexing %s AC[%3d] LL[%d] PO[%d]", filename, access, loadlevel, payonly);

      z = new(class zone_type);
      zone_info.no_of_zones++;

      z->zone_no  = zone_info.no_of_zones - 1;
      z->filename = str_dup(zone);

      fstrcpy(&cBuf, f);
      z->name = str_dup((char *)cBuf.GetData());

      int ms_tmp   = fread(&z->weather.base, sizeof(int), 1, f);
      z->access    = access;
      z->loadlevel = loadlevel;
      z->payonly   = payonly;

      /* More data read here */
      fstrcpy(&cBuf, f);
      z->notes = str_dup((char *)cBuf.GetData());

      fstrcpy(&cBuf, f);
      z->help = str_dup((char *)cBuf.GetData());

      for(;;)
      {
         fstrcpy(&cBuf, f);

         if(cBuf.GetData()[0] == 0)
         {
            break;
         }

         z->creators.AppendName((char *)cBuf.GetData());
      }

      fstrcpy(&cBuf, f);

      if(cBuf.GetData()[0] != 0)
      {
         z->title = str_dup((char *)cBuf.GetData());
      }
      else
      {
         z->title = nullptr;
      }

      /* read templates */
      z->no_tmpl = 0;
      z->tmpl    = generate_templates(f, z);

      z->no_of_fi = 0;
      z->zri      = nullptr;
      z->fi       = generate_file_indexes(f, z);
      z->no_rooms = room_number; /* Number of rooms in the zone */

      fflush(f); /* Don't fclose(f); since we are using _cache */

      /* Link Z into zone_info.zone_list */

      if(zone_info.zone_list == nullptr)
      {
         zone_info.zone_list = z; /* If list is empty */
      }
      else
      {
         for(tz2 = nullptr, tz1 = zone_info.zone_list; tz1 != nullptr; tz1 = tz1->next)
         {
            if(strcmp(tz1->name, z->name) > 0)
            {
               break;
            }
            tz2 = tz1;
         }
         if(tz2 == nullptr)
         {
            z->next             = zone_info.zone_list;
            zone_info.zone_list = z;
         }
         else
         {
            z->next   = tz1;
            tz2->next = z;
         }
      }
   }
   fclose(zone_file);

   if(zone_info.zone_list == nullptr || zone_info.no_of_zones <= 0)
   {
      slog(LOG_ALL, 0, "Basis zone is minimum reqired environment!");
      exit(0);
   }

   generate_bin_arrays();

   resolve_templates();
   mud_bootzone = 0;

   /* Allocate memory for the largest possible file-buffer */
   /* filbuffer_length = MAX(filbuffer_length + 1, 16384); */
   // CREATE(filbuffer, uint8_t, filbuffer_length + 1);
   // slog(LOG_OFF, 0, "Max length for filebuffer is %d bytes.", filbuffer_length);
}

/* Reason why here, and not in db_file.c: Dmc.c would then need affect.c
 *
 * Stored As:
 *   <no of affects (0..255)>
 *   <duration>
 *   <id>
 *   <beat>
 *   <data[3]>
 *   <firstf_i>
 *   <tickf_i>
 *   <lastf_i>
 *   <applyf_i>
 *
 *  Will only link the affect since it is used by both players and by
 *  other units. If the affect should also have an actual effect, then it
 *  must be followed by the function call 'apply_affects'.
 */
auto bread_affect(CByteBuffer *pBuf, unit_data *u, uint8_t nVersion) -> int
{
   struct unit_affected_type af;
   int                       i;
   uint8_t                   t8;
   uint16_t                  t16;

   auto link_alloc_affect(unit_data * unit, struct unit_affected_type * orgaf)->struct unit_affected_type *;

   if(nVersion <= 56)
   {
      if(pBuf->Read8(&t8) != 0)
      {
         return 1;
      }
      i = t8;
   }
   else
   {
      if(pBuf->Read16(&t16) != 0)
      {
         return 1;
      }
      i = t16;
   }

   for(; 0 < i; i--)
   {
      if(pBuf->Read16(&af.duration) != 0)
      {
         return 1;
      }

      if(pBuf->Read16(&af.id) != 0)
      {
         return 1;
      }

      if(pBuf->Read16(&af.beat) != 0)
      {
         return 1;
      }

      /*if (af.id > ID_TOP_IDX)
      {
         slog(LOG_ALL, 0, "Illegal affect index in bread_affect - corrupt.");
         corrupt = TRUE;
         return;
      }*/

      if(pBuf->Read32(&af.data[0]) != 0)
      {
         return 1;
      }

      if(pBuf->Read32(&af.data[1]) != 0)
      {
         return 1;
      }

      if(pBuf->Read32(&af.data[2]) != 0)
      {
         return 1;
      }

      if(pBuf->Read16(&af.firstf_i) != 0)
      {
         return 1;
      }

      if(pBuf->Read16(&af.tickf_i) != 0)
      {
         return 1;
      }

      if(pBuf->Read16(&af.lastf_i) != 0)
      {
         return 1;
      }

      if(pBuf->Read16(&af.applyf_i) != 0)
      {
         return 1;
      }

      /* Don't call, don't apply and don't set up tick for this affect (yet) */
      link_alloc_affect(u, &af);
   }

   return 0;
}

struct zone_type *unit_error_zone = nullptr;

extern int memory_pc_alloc;
extern int memory_npc_alloc;
extern int memory_obj_alloc;
extern int memory_room_alloc;

/*  Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 *  Unit is NOT inserted in unit_list
 *
 * whom is an error message to be printed when something goes wrong.
 * bSwapin is TRUE if the swap information should be read.
 */
auto read_unit_string(CByteBuffer *pBuf, int type, int len, int bSwapin, char *whom) -> unit_data *
{
   void            *ptr;
   unit_data       *u;
   file_index_type *fi;
   char             zone[FI_MAX_ZONENAME + 1];
   char             name[FI_MAX_UNITNAME + 1];
   char            *tmp;
   int              i;
   int              j;
   uint8_t          unit_version;
   uint8_t          t8;
   uint16_t         t16;
   uint32_t         t32;
   uint32_t         nStart;

#ifdef MEMORY_DEBUG
   int memory_start;
#endif

   void start_all_special(unit_data * u);

   g_nCorrupt = 0;

#ifdef MEMORY_DEBUG
   memory_start = memory_total_alloc;
#endif

   if(type != UNIT_ST_NPC && type != UNIT_ST_PC && type != UNIT_ST_ROOM && type != UNIT_ST_OBJ)
   {
      g_nCorrupt = TRUE;
      return nullptr;
   }

   u = new(unit_data)(type);

   nStart = pBuf->GetReadPosition();
   g_nCorrupt += pBuf->Read8(&unit_version);

   assert(unit_version >= 37);

   if(unit_version > 57)
   {
      slog(LOG_EXTENSIVE, 0,
           "FATAL: Attempted to read '%s' but found "
           "version number %d which was NEWER than this implementation "
           "can handle! Aborting server.",
           whom, unit_version);
      assert(FALSE);
   }

   g_nCorrupt += UNIT_NAMES(u).ReadBuffer(pBuf);

   if(unit_version < 47 && type == UNIT_ST_PC)
   {
      char buf[256];

      strcpy(buf, UNIT_NAME(u));
      CAP(buf);
      UNIT_NAMES(u).Substitute(0, buf);
   }

   g_nCorrupt += bread_swap(pBuf, u);

   /* Read Key Zone, Name */
   g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
   g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

   UNIT_KEY(u) = find_file_index(zone, name);

   if(unit_version < 46)
   {
      g_nCorrupt += pBuf->Read16(&t16);
      UNIT_MANIPULATE(u) = t16;
   }
   else
   {
      g_nCorrupt += pBuf->Read32(&UNIT_MANIPULATE(u));
   }

   g_nCorrupt += pBuf->Read16(&UNIT_FLAGS(u));
   g_nCorrupt += pBuf->Read16(&UNIT_BASE_WEIGHT(u));
   g_nCorrupt += pBuf->Read16(&UNIT_WEIGHT(u));
   g_nCorrupt += pBuf->Read16(&UNIT_CAPACITY(u));

   g_nCorrupt += pBuf->Read32(&UNIT_MAX_HIT(u));
   g_nCorrupt += pBuf->Read32(&UNIT_HIT(u));

   if(unit_version <= 54)
   {
      if(UNIT_MAX_HIT(u) <= 0)
      {
         UNIT_HIT(u) = UNIT_MAX_HIT(u) = 1000;
      }
   }

   g_nCorrupt += pBuf->Read16(&UNIT_ALIGNMENT(u));

   g_nCorrupt += pBuf->Read8(&UNIT_OPEN_FLAGS(u));
   g_nCorrupt += pBuf->Read8(&UNIT_LIGHTS(u));
   g_nCorrupt += pBuf->Read8(&UNIT_BRIGHT(u));
   g_nCorrupt += pBuf->Read8(&UNIT_ILLUM(u));
   g_nCorrupt += pBuf->Read8(&UNIT_CHARS(u));
   g_nCorrupt += pBuf->Read8(&UNIT_MINV(u));

   if(unit_version >= 53)
   {
      g_nCorrupt += pBuf->Read16(&UNIT_SIZE(u));
   }
   else
   {
      UNIT_SIZE(u) = 180;
   }

   if(unit_version >= 51)
   {
      g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
      g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

      file_index_type *tmpfi = find_file_index(zone, name);

      if(tmpfi != nullptr)
      {
         if(UNIT_TYPE(u) == UNIT_ST_ROOM)
         {
            UNIT_IN(u) = (unit_data *)tmpfi; /* To be normalized! */
         }
         else
         {
            if(IS_PC(u))
            {
               CHAR_LAST_ROOM(u) = tmpfi->room_ptr;
            }
            else
            {
               UNIT_IN(u) = tmpfi->room_ptr;
            }
         }
      }
   }

   switch(UNIT_TYPE(u))
   {
      case UNIT_ST_NPC:
         g_nCorrupt += pBuf->ReadStringAlloc(&CHAR_MONEY(u));
         /* fallthru */

      case UNIT_ST_PC:
         g_nCorrupt += pBuf->Read32(&CHAR_EXP(u));
         g_nCorrupt += pBuf->Read32(&CHAR_FLAGS(u));

         g_nCorrupt += pBuf->Read16(&CHAR_MANA(u));
         g_nCorrupt += pBuf->Read16(&CHAR_ENDURANCE(u));

         g_nCorrupt += pBuf->Read8(&CHAR_NATURAL_ARMOUR(u));

         if(unit_version >= 39)
         {
            g_nCorrupt += pBuf->Read8(&CHAR_SPEED(u));
            if(IS_PC(u))
            {
               if(CHAR_SPEED(u) < SPEED_MIN)
               {
                  CHAR_SPEED(u) = SPEED_DEFAULT;
               }
            }
         }
         else
         {
            CHAR_SPEED(u) = SPEED_DEFAULT;
         }

         g_nCorrupt += pBuf->Read16(&t16);
         CHAR_ATTACK_TYPE(u) = t16;

         if(unit_version <= 52)
         {
            g_nCorrupt += pBuf->Read16(&UNIT_SIZE(u));
         }
         g_nCorrupt += pBuf->Read16(&CHAR_RACE(u));

         g_nCorrupt += pBuf->Read16(&CHAR_OFFENSIVE(u));
         g_nCorrupt += pBuf->Read16(&CHAR_DEFENSIVE(u));

         g_nCorrupt += pBuf->Read8(&CHAR_SEX(u));
         g_nCorrupt += pBuf->Read8(&CHAR_LEVEL(u));
         g_nCorrupt += pBuf->Read8(&CHAR_POS(u));

         g_nCorrupt += pBuf->Read8(&t8);
         j = t8;

         for(i = 0; i < j; i++)
         {
            g_nCorrupt += pBuf->Read8(&CHAR_ABILITY(u, i));

            if(IS_PC(u))
            {
               g_nCorrupt += pBuf->Read8(&PC_ABI_LVL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_ABI_COST(u, i));
               if(unit_version < 49)
               {
                  PC_ABI_COST(u, i) /= 5;
               }
            }
         }

         if(IS_PC(u))
         {
            g_nCorrupt += pBuf->ReadFloat(&PC_ACCOUNT(u).credit);
            g_nCorrupt += pBuf->Read32(&PC_ACCOUNT(u).credit_limit);
            g_nCorrupt += pBuf->Read32(&PC_ACCOUNT(u).total_credit);

            if(unit_version >= 44)
            {
               g_nCorrupt += pBuf->Read16(&PC_ACCOUNT(u).last4);
            }
            else
            {
               if(unit_version >= 41)
               {
                  g_nCorrupt += pBuf->Skip32(); /* cc_time */
               }
               PC_ACCOUNT(u).last4 = -1;
            }

            if(unit_version >= 45)
            {
               g_nCorrupt += pBuf->Read8(&PC_ACCOUNT(u).discount);

               if(unit_version >= 52)
               {
                  g_nCorrupt += pBuf->Read32(&PC_ACCOUNT(u).flatrate);
               }
               else
               {
                  PC_ACCOUNT(u).flatrate = 0;
               }

               g_nCorrupt += pBuf->Read8(&PC_ACCOUNT(u).cracks);
            }
            else
            {
               PC_ACCOUNT(u).flatrate = 0;
               PC_ACCOUNT(u).discount = 0;
               PC_ACCOUNT(u).cracks   = 0;
            }

            if(unit_version >= 48)
            {
               g_nCorrupt += pBuf->Read16(&PC_LIFESPAN(u));
            }
            else
            {
               CHAR_RACE(u)--; /* spooky */

               struct base_race_info_type *sex_race;

               if(CHAR_SEX(u) == SEX_MALE)
               {
                  sex_race = &race_info[CHAR_RACE(u)].male;
               }
               else
               {
                  sex_race = &race_info[CHAR_RACE(u)].female;
               }

               PC_LIFESPAN(u) = sex_race->lifespan + dice(sex_race->lifespan_dice.reps, sex_race->lifespan_dice.size);
            }

            if(unit_version < 50)
            {
               g_nCorrupt += pBuf->SkipString();
            }

            g_nCorrupt += pBuf->Read8(&PC_SETUP_ECHO(u));
            g_nCorrupt += pBuf->Read8(&PC_SETUP_REDRAW(u));
            g_nCorrupt += pBuf->Read8(&PC_SETUP_WIDTH(u));
            g_nCorrupt += pBuf->Read8(&PC_SETUP_HEIGHT(u));
            g_nCorrupt += pBuf->Read8(&PC_SETUP_EMULATION(u));
            g_nCorrupt += pBuf->Read8(&PC_SETUP_TELNET(u));
            g_nCorrupt += pBuf->Read8(&PC_SETUP_COLOUR(u));

            if(unit_version < 51)
            {
               g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
               g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

               if((ptr = find_file_index(zone, name)) != nullptr)
               {
                  CHAR_LAST_ROOM(u) = ((file_index_type *)ptr)->room_ptr;
               }
            }

            if(unit_version >= 42)
            {
               g_nCorrupt += pBuf->ReadStringCopy(PC_FILENAME(u), PC_MAX_NAME);
               PC_FILENAME(u)[PC_MAX_NAME - 1] = 0;
            }
            else
            {
               // MS2020: odd warning strncpy(PC_FILENAME(u), UNIT_NAME(u), PC_MAX_NAME);
               memcpy(PC_FILENAME(u), UNIT_NAME(u), PC_MAX_NAME);
               PC_FILENAME(u)[PC_MAX_NAME - 1] = 0;
               str_lower(PC_FILENAME(u));
            }

            g_nCorrupt += pBuf->ReadStringCopy(PC_PWD(u), PC_MAX_PASSWORD);
            PC_PWD(u)[PC_MAX_PASSWORD - 1] = '\0';

            if(unit_version >= 54)
            {
               for(i = 0; i < 5; i++)
               {
                  g_nCorrupt += pBuf->Read32(&PC_LASTHOST(u)[i]);
               }
            }
            else
            {
               for(i = 0; i < 5; i++)
               {
                  PC_LASTHOST(u)[i] = 0;
               }
            }

            g_nCorrupt += pBuf->Read32(&PC_ID(u));

            if(unit_version >= 40)
            {
               g_nCorrupt += pBuf->Read16(&PC_CRACK_ATTEMPTS(u));
            }

            g_nCorrupt += pBuf->ReadStringAlloc(&PC_HOME(u));
            g_nCorrupt += pBuf->ReadStringAlloc(&PC_GUILD(u));

            g_nCorrupt += pBuf->Read32(&PC_GUILD_TIME(u));

            if(unit_version >= 38)
            {
               g_nCorrupt += pBuf->Read16(&PC_VIRTUAL_LEVEL(u));
            }
            else
            {
               PC_VIRTUAL_LEVEL(u) = CHAR_LEVEL(u);
            }

            g_nCorrupt += pBuf->Read32(&t32);
            PC_TIME(u).creation = t32;

            g_nCorrupt += pBuf->Read32(&t32);
            PC_TIME(u).connect = t32;
            g_nCorrupt += pBuf->Read32(&t32);
            PC_TIME(u).birth = t32;
            g_nCorrupt += pBuf->Read32(&t32);
            PC_TIME(u).played = t32;

            if(unit_version < 44)
            {
               void race_adjust(unit_data *);
               race_adjust(u);
            }

            g_nCorrupt += pBuf->ReadStringAlloc(&PC_BANK(u));

            g_nCorrupt += pBuf->Read32(&PC_SKILL_POINTS(u));
            g_nCorrupt += pBuf->Read32(&PC_ABILITY_POINTS(u));

            g_nCorrupt += pBuf->Read16(&t16);
            PC_FLAGS(u) = t16;

            g_nCorrupt += pBuf->Read8(&t8);
            j = t8;

            for(i = 0; i < j; i++)
            {
               g_nCorrupt += pBuf->Read8(&PC_SPL_SKILL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_SPL_LVL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_SPL_COST(u, i));

               if(unit_version < 49)
               {
                  PC_SPL_COST(u, i) /= 5;
               }
               if(unit_version < 46)
               {
                  if((i < SPL_GROUP_MAX) && (PC_SPL_SKILL(u, i) == 0))
                  {
                     PC_SPL_SKILL(u, i) = 1;
                  }
               }
            }

            g_nCorrupt += pBuf->Read8(&t8);
            j = t8;

            for(i = 0; i < j; i++)
            {
               g_nCorrupt += pBuf->Read8(&PC_SKI_SKILL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_SKI_LVL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_SKI_COST(u, i));
               if(unit_version < 49)
               {
                  PC_SKI_COST(u, i) /= 5;
               }
            }

            g_nCorrupt += pBuf->Read8(&t8);
            j = t8;

            for(i = 0; i < j; i++)
            {
               g_nCorrupt += pBuf->Read8(&PC_WPN_SKILL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_WPN_LVL(u, i));
               g_nCorrupt += pBuf->Read8(&PC_WPN_COST(u, i));
               if(unit_version < 49)
               {
                  PC_WPN_COST(u, i) /= 5;
               }
               if(unit_version < 46)
               {
                  if((i < WPN_GROUP_MAX) && (PC_WPN_SKILL(u, i) == 0))
                  {
                     PC_WPN_SKILL(u, i) = 1;
                  }
               }
            }

            if(unit_version < 47)
            {
               PC_WPN_SKILL(u, WPN_KICK) = PC_SKI_SKILL(u, SKI_KICK);
               PC_SKI_SKILL(u, SKI_KICK) = 0;
            }

            g_nCorrupt += pBuf->Read16(&PC_CRIMES(u));

            g_nCorrupt += pBuf->Read8(&t8);
            j = t8;

            for(i = 0; i < j; i++)
            {
               g_nCorrupt += pBuf->Read8(&PC_COND(u, i));
            }

            if(unit_version >= 56)
            {
               g_nCorrupt += pBuf->Read8(&PC_ACCESS_LEVEL(u));
            }
            else
            {
               PC_ACCESS_LEVEL(u) = 0;
            }

            g_nCorrupt += bread_extra(pBuf, &PC_QUEST(u));

            if(unit_version >= 50)
            {
               g_nCorrupt += bread_extra(pBuf, &PC_INFO(u));
            }
         }
         else
         {
            for(i = 0; i < WPN_GROUP_MAX; i++)
            {
               g_nCorrupt += pBuf->Read8(&NPC_WPN_SKILL(u, i));
            }

            for(i = 0; i < SPL_GROUP_MAX; i++)
            {
               g_nCorrupt += pBuf->Read8(&NPC_SPL_SKILL(u, i));
            }

            g_nCorrupt += pBuf->Read8(&NPC_DEFAULT(u));
            g_nCorrupt += pBuf->Read8(&NPC_FLAGS(u));
         }
         break;

      case UNIT_ST_OBJ:
         g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 0));
         g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 1));
         g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 2));
         g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 3));
         g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 4));

         g_nCorrupt += pBuf->Read32(&t32);
         OBJ_FLAGS(u) = t32;
         g_nCorrupt += pBuf->Read32(&OBJ_PRICE(u));
         g_nCorrupt += pBuf->Read32(&OBJ_PRICE_DAY(u));

         g_nCorrupt += pBuf->Read8(&OBJ_TYPE(u));
         OBJ_EQP_POS(u) = 0;

         g_nCorrupt += pBuf->Read8(&OBJ_RESISTANCE(u));

         if(unit_version < 49)
         {
            if(OBJ_TYPE(u) == ITEM_WEAPON && (OBJ_VALUE(u, 3) == 0))
            {
               OBJ_VALUE(u, 3) = RACE_DO_NOT_USE;
            }
         }
         break;

      case UNIT_ST_ROOM:
         if(unit_version < 51)
         {
            /* See if room is to be placed inside another room! */
            g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
            g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
            if((fi = find_file_index(zone, name)) != nullptr)
            {
               UNIT_IN(u) = (unit_data *)fi; /* A file index */
            }
            else
            {
               UNIT_IN(u) = nullptr;
            }
         }

         /* Read N, S, E, W, U and D directions */
         for(i = 0; i < 6; i++)
         {
            ROOM_EXIT(u, i) = nullptr;
            g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
            g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

            if((*zone != 0) && (*name != 0))
            {
               if((fi = find_file_index(zone, name)) != nullptr)
               {
                  ROOM_EXIT(u, i) = new(class room_direction_data);
                  g_nCorrupt += ROOM_EXIT(u, i)->open_name.ReadBuffer(pBuf);

                  g_nCorrupt += pBuf->Read16(&t16);
                  ROOM_EXIT(u, i)->exit_info = t16;

                  g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
                  g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

                  ROOM_EXIT(u, i)->key = find_file_index(zone, name);

                  /* NOT fi->room_ptr! Done later */
                  ROOM_EXIT(u, i)->to_room = (unit_data *)fi;
               }
               else
               { /* Exit not existing, skip the junk info! */
                  char **nb;

                  szonelog(unit_error_zone, "Unknown room direction '%s@%s'", name, zone);
                  g_nCorrupt += pBuf->SkipNames();
                  g_nCorrupt += pBuf->Skip16();
                  g_nCorrupt += pBuf->SkipString();
                  g_nCorrupt += pBuf->SkipString();
               }
            }
         }

         g_nCorrupt += pBuf->Read16(&t16);
         ROOM_FLAGS(u) = t16;
         g_nCorrupt += pBuf->Read8(&ROOM_LANDSCAPE(u));
         g_nCorrupt += pBuf->Read8(&ROOM_RESISTANCE(u));
         break;

      default:
         assert(FALSE);
         break;
   }

   g_nCorrupt += bread_affect(pBuf, u, unit_version);

   UNIT_FUNC(u) = bread_func(pBuf, unit_version, u);

   if(len != (int)(pBuf->GetReadPosition() - nStart))
   {
      slog(LOG_ALL, 0, "FATAL: Length read mismatch (%d / %d)", len, pBuf->GetReadPosition());
      g_nCorrupt++;
   }

   if(g_nCorrupt == 0)
   {
      if(IS_CHAR(u) && CHAR_MONEY(u))
      {
         long int val1;
         long int val2;
         char    *c;
         char    *prev = CHAR_MONEY(u);

         while((c = strchr(prev, '~')) != nullptr)
         {
            sscanf(prev, "%ld %ld", &val1, &val2);
            if(val1 > 0 && (is_in(val2, DEF_CURRENCY, MAX_MONEY) != 0))
            {
               coins_to_unit(u, val1, val2);
            }

            prev = c + 1;
         }

         free(CHAR_MONEY(u));
      }

      /* We dare not start if unit is corrupt! */
      start_all_special(u);
      start_affect(u);
   }
   else
   {
      extern file_index_type *slime_fi;

      slog(LOG_ALL, 0, "FATAL: UNIT CORRUPT: %s", u->names.Name());

      if((type != UNIT_ST_PC) && (type != UNIT_ST_ROOM) && (slime_fi != nullptr))
      {
         return read_unit(slime_fi); /* Slime it! */
      }
   }

#ifdef MEMORY_DEBUG
   switch(type)
   {
      case UNIT_ST_PC:
         memory_pc_alloc += memory_total_alloc - memory_start;
         break;
      case UNIT_ST_NPC:
         memory_npc_alloc += memory_total_alloc - memory_start;
         break;
      case UNIT_ST_OBJ:
         memory_obj_alloc += memory_total_alloc - memory_start;
         break;
      case UNIT_ST_ROOM:
         memory_room_alloc += memory_total_alloc - memory_start;
         break;
   }
#endif

   return u;
}

/*  Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 */
void read_unit_file(file_index_type *org_fi, CByteBuffer *pBuf)
{
   FILE *f;
   char  buf[256];

   sprintf(buf, "%s%s.data", zondir, org_fi->zone->filename);

   if((f = fopen_cache(buf, "rb")) == nullptr)
   {
      error(HERE, "Couldn't open %s for reading.", buf);
   }

   pBuf->FileRead(f, org_fi->filepos, org_fi->length);

   // bread_block(f, org_fi->filepos, org_fi->length, buffer);

   /* was fclose(f) */
}

/*  Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 */
auto read_unit(file_index_type *org_fi) -> unit_data *
{
   auto is_slimed(file_index_type * sp)->int;

   unit_data *u;

   if(org_fi == nullptr)
   {
      return nullptr;
   }

   if(is_slimed(org_fi) != 0)
   {
      org_fi = slime_fi;
   }

   read_unit_file(org_fi, &g_FileBuffer);

   unit_error_zone = org_fi->zone;

   u = read_unit_string(&g_FileBuffer, org_fi->type, org_fi->length, FALSE, str_cc(org_fi->name, org_fi->zone->name));

   UNIT_FILE_INDEX(u) = org_fi;

   if(!IS_ROOM(u))
   {
      assert(UNIT_IN(u) == nullptr);
   }

   unit_error_zone = nullptr;

   org_fi->no_in_mem++;

   if(IS_ROOM(u))
   {
      org_fi->room_ptr = u;
   }

   insert_in_unit_list(u); /* Put unit into the unit_list      */

   apply_affect(u); /* Set all affects that modify      */

   /* void dil_loadtime_activate(unit_data *u);

   dil_loadtime_activate(u); */

   return u;
}

void read_all_rooms()
{
   struct zone_type *z;
   file_index_type  *fi;

   extern struct zone_type *boot_zone;

   for(z = zone_info.zone_list; z != nullptr; z = z->next)
   {
      boot_zone = z;

      for(fi = z->fi; fi != nullptr; fi = fi->next)
      {
         if(fi->type == UNIT_ST_ROOM)
         {
            read_unit(fi);
         }
      }
   }

   boot_zone = nullptr;
}

/* After boot time, normalize all room exits */
void normalize_world()
{
   file_index_type *fi;
   unit_data       *u;
   unit_data       *tmpu;
   int              i;

   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_ROOM(u))
      {
         /* Place room inside another room? */
         if(UNIT_IN(u))
         {
            fi = (file_index_type *)UNIT_IN(u);

            assert(fi->room_ptr);

            UNIT_IN(u) = fi->room_ptr;
         }

         /* Change directions into unit_data points from file_index_type */
         for(i = 0; i < 6; i++)
         {
            if(ROOM_EXIT(u, i))
            {
               ROOM_EXIT(u, i)->to_room = ((file_index_type *)ROOM_EXIT(u, i)->to_room)->room_ptr;
            }
         }
      }
   }

   for(u = unit_list; u != nullptr; u = u->gnext)
   {
      if(IS_ROOM(u) && UNIT_IN(u))
      {
         tmpu       = UNIT_IN(u);
         UNIT_IN(u) = nullptr;

         if(unit_recursive(u, tmpu) != 0)
         {
            slog(LOG_ALL, 0, "Error: %s@%s was recursively in %s@%s!", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), UNIT_FI_NAME(tmpu),
                 UNIT_FI_ZONENAME(tmpu));
         }
         else
         {
            unit_to_unit(u, tmpu);
         }
      }
   }
}

#define ZON_DIR_CONT   0
#define ZON_DIR_NEST   1
#define ZON_DIR_UNNEST 2

/* For local error purposes */
static struct zone_type *read_zone_error = nullptr;

auto read_zone(FILE *f, struct zone_reset_cmd *cmd_list) -> struct zone_reset_cmd *
{
   struct zone_reset_cmd *cmd;
   struct zone_reset_cmd *tmp_cmd;
   file_index_type       *fi;
   uint8_t                cmdno;
   uint8_t                direction;
   char                   zonename[FI_MAX_ZONENAME + 1];
   char                   name[FI_MAX_UNITNAME + 1];
   CByteBuffer            cBuf(100);

   tmp_cmd = cmd_list;

   while(((cmdno = (uint8_t)fgetc(f)) != 255) && (feof(f) == 0))
   {
      CREATE(cmd, struct zone_reset_cmd, 1);
      cmd->cmd_no = cmdno;

      fstrcpy(&cBuf, f);

      if(feof(f) != 0)
      {
         break;
      }

      strcpy(zonename, (char *)cBuf.GetData());

      fstrcpy(&cBuf, f);

      if(feof(f) != 0)
      {
         break;
      }

      strcpy(name, (char *)cBuf.GetData());

      if((*zonename != 0) && (*name != 0))
      {
         if((fi = find_file_index(zonename, name)) != nullptr)
         {
            cmd->fi[0] = fi;
         }
         else
         {
            szonelog(read_zone_error, "Slimed: Illegal ref.: %s@%s", name, zonename);
            cmd->fi[0] = slime_fi;
         }
      }
      else
      {
         cmd->fi[0] = nullptr;
      }

      fstrcpy(&cBuf, f);

      if(feof(f) != 0)
      {
         break;
      }

      strcpy(zonename, (char *)cBuf.GetData());

      fstrcpy(&cBuf, f);

      if(feof(f) != 0)
      {
         break;
      }

      strcpy(name, (char *)cBuf.GetData());

      if((*zonename != 0) && (*name != 0))
      {
         if((fi = find_file_index(zonename, name)) != nullptr)
         {
            cmd->fi[1] = fi;
         }
         else
         {
            szonelog(read_zone_error, "Illegal ref.: %s@%s", name, zonename);
            cmd->fi[1] = slime_fi;
         }
      }
      else
      {
         cmd->fi[1] = nullptr;
      }

      if(fread(&(cmd->num[0]), sizeof(cmd->num[0]), 1, f) != 1)
      {
         error(HERE, "Failed to fread() cmd->num[0]");
      }
      if(fread(&(cmd->num[1]), sizeof(cmd->num[1]), 1, f) != 1)
      {
         error(HERE, "Failed to fread() cmd->num[1]");
      }
      if(fread(&(cmd->num[2]), sizeof(cmd->num[2]), 1, f) != 1)
      {
         error(HERE, "Failed to fread() cmd->num[2]");
      }
      if(fread(&(cmd->cmpl), sizeof(uint8_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() cmd->cmpl");
      }

      /* Link into list of next command */
      if(cmd_list == nullptr)
      {
         cmd_list = cmd;
         tmp_cmd  = cmd;
      }
      else
      {
         tmp_cmd->next = cmd;
         tmp_cmd       = cmd;
      }

      direction = (uint8_t)fgetc(f);

      switch(direction)
      {
         case ZON_DIR_CONT:
            break;

         case ZON_DIR_NEST:
            cmd->nested = read_zone(f, nullptr);
            break;

         case ZON_DIR_UNNEST:
            return cmd_list;
            break;

         default:
            szonelog(read_zone_error, "Serious Error: Unknown zone direction: %d", direction);
            break;
      }
   }

   return cmd_list;
}

void read_all_zones()
{
   struct zone_type *zone;
   char              filename[FI_MAX_ZONENAME + 41];
   FILE             *f;

   for(zone = zone_info.zone_list; zone != nullptr; zone = zone->next)
   {
      read_zone_error = zone;

      sprintf(filename, "%s%s.reset", zondir, zone->filename);

      if((f = fopen(filename, "rb")) == nullptr)
      {
         slog(LOG_OFF, 0, "Could not open zone file: %s", zone->filename);
         exit(1);
      }

      if(fread(&(zone->zone_time), sizeof(uint16_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() zone->zone_time");
      }

      if(fread(&(zone->reset_mode), sizeof(uint8_t), 1, f) != 1)
      {
         error(HERE, "Failed to fread() zone->reset_mode");
      }

      zone->zri = read_zone(f, nullptr);

      fclose(f);
   }
}

auto read_info_file(char *name, char *oldstr) -> char *
{
   char tmp[2 * MAX_STRING_LENGTH];
   char buf[2 * MAX_STRING_LENGTH];

   if(oldstr != nullptr)
   {
      free(oldstr);
   }

   file_to_string(name, (char *)tmp, sizeof(tmp) - 1);
   str_escape_format(tmp, buf, sizeof(buf));

   return str_dup(buf);
}

extern int memory_roomread_alloc;
extern int memory_zoneidx_alloc;
extern int memory_zonereset_alloc;

void boot_db()
{
   void competition_boot();
   void mail_boot();
   void create_dijkstra();
   void player_file_index();
   void reception_boot();
   void load_messages();
   void boot_social_messages();
   void boot_pose_messages();
   void assign_command_pointers();
   void assign_spell_pointers();
   void reset_all_zones();
   void boot_justice();
   void load_ban();
   void boot_money();
   void zone_boot();
   void slime_boot();
   void boot_help();
   void boot_swap();
   void boot_spell();
   void boot_skill();
   void boot_weapon();
   void boot_ability();
   void boot_race();
   void boot_interpreter();
   void interpreter_dil_check();
   void persist_boot();

   void       cleanup_playerfile(int argc, char *argv[]);
   extern int player_convert;

   slog(LOG_OFF, 0, "Boot DB -- BEGIN.");
   slog(LOG_OFF, 0, "Copyright (C) 1994 - 1996 by Valhalla.");

   slog(LOG_OFF, 0, "Generating indexes from list of zone-filenames.");
#ifdef MEMORY_DEBUG
   memory_zoneidx_alloc = memory_total_alloc;
#endif
   generate_zone_indexes();
#ifdef MEMORY_DEBUG
   memory_zoneidx_alloc = memory_total_alloc - memory_zoneidx_alloc;
#endif

   slog(LOG_OFF, 0, "Generating player index.");
   player_file_index();

   slog(LOG_OFF, 0, "Booting postoffice.");
   mail_boot();

   slog(LOG_OFF, 0, "Booting reception.");
   reception_boot();

   slog(LOG_OFF, 0, "Booting the money-system.");
   boot_money();

   slog(LOG_OFF, 0, "Resetting the game time:");
   boot_time_and_weather();

   boot_interpreter(); /* Must do to clear cmd_info->tmpl before skill boot */

   slog(LOG_OFF, 0, "Spell, skill, weapon and ability boot.");
   boot_race();
   boot_spell();
   boot_skill();
   boot_weapon();
   boot_ability();

   if(player_convert != 0)
   {
      cleanup_playerfile(0, nullptr);
      exit(0);
   }

   slog(LOG_OFF, 0, "Reading all rooms into memory.");
#ifdef MEMORY_DEBUG
   memory_roomread_alloc = memory_total_alloc;
#endif

   read_all_rooms();

#ifdef MEMORY_DEBUG
   memory_roomread_alloc = memory_total_alloc - memory_roomread_alloc;
#endif
   slog(LOG_OFF, 0, "Normalizing file index ref. and replacing rooms.");
   normalize_world();

   slog(LOG_OFF, 0, "Creating shortest path information for zones.");
   create_dijkstra();

   slog(LOG_OFF, 0, "Reading Zone Reset information.");
   read_all_zones();

   slog(LOG_OFF, 0, "Opening help file.");
   boot_help();

   ConnectionBoot();

   touch_file(str_cc(libdir, STATISTICS_FILE));

   g_cAccountConfig.Boot();
   competition_boot();

   slog(LOG_OFF, 0, "Loading social messages.");
   boot_social_messages();

   slog(LOG_OFF, 0, "Loading pose messages.");
   boot_pose_messages();

   slog(LOG_OFF, 0, "Building interpreter trie.");
   assign_command_pointers();

   slog(LOG_OFF, 0, "DIL check."); // MS2020
   interpreter_dil_check();

   slog(LOG_OFF, 0, "Loading fight messages.");
   load_messages();

   slog(LOG_OFF, 0, "Calling boot-sequences of all zones.");
   zone_boot();

   slog(LOG_OFF, 0, "Booting Justice Routines.");
   boot_justice();

   slog(LOG_OFF, 0, "Booting ban-file.");
   load_ban();

   slog(LOG_OFF, 0, "Booting slime system.");
   slime_boot();

   slog(LOG_OFF, 0, "Performing boot time reset.");
#ifdef MEMORY_DEBUG
   memory_zonereset_alloc = memory_total_alloc;
#endif
   reset_all_zones();
#ifdef MEMORY_DEBUG
   memory_zonereset_alloc = memory_total_alloc - memory_zonereset_alloc;
#endif

   slog(LOG_OFF, 0, "Booting swap system.");
   boot_swap();

   slog(LOG_OFF, 0, "Reloading persistent objects.");
   persist_boot();

   touch_file(str_cc(libdir, STATISTICS_FILE));

   slog(LOG_OFF, 0, "Boot db -- DONE (%d bytes allocated).", memory_total_alloc);
}

void db_shutdown()
{
   return;

   unit_data *u;
   unit_data *tmpu;

   slog(LOG_OFF, 0, "Destroying unit list.");

   void clear_destructed();
   void register_destruct(int i, void *ptr);

   while(!IS_ROOM(unit_list))
   {
      tmpu = unit_list;
      extract_unit(tmpu);
      clear_destructed();
   }

   void stop_all_special(unit_data * u);

   while((tmpu = unit_list) != nullptr)
   {
      DeactivateDil(tmpu);
      stop_all_special(tmpu);
      stop_affect(tmpu);
      unit_from_unit(tmpu);
      remove_from_unit_list(tmpu);

      delete tmpu;

      clear_destructed();
   }

   slog(LOG_OFF, 0, "Destroying zone list.");

   class zone_type *z;
   class zone_type *nextz;

   for(z = zone_info.zone_list; z != nullptr; z = nextz)
   {
      nextz = z->next;
      delete z;
   }
}
