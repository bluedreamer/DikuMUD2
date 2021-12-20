#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "CServerConfiguration.h"
#include "db.h"
#include "db_file.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

auto        write_unit_string(uint8_t *b, unit_data *u) -> int;

/* *************************************************************************
 * Routines for calculating rent                                           *
 ************************************************************************* */

static int  rent_info;

static void show_items(unit_data *ch, unit_data *item, uint32_t price)
{
   if(price > 0)
   {
      price = money_round_up(price, local_currency(ch), 2);

      act("$2t for $3n", A_ALWAYS, ch, money_string(price, local_currency(ch), FALSE), item, TO_CHAR);
      rent_info = static_cast<int>(TRUE);
   }
}

/* ----------------------------------------------------------------- */

static void subtract_rent(unit_data *ch, unit_data *item, uint32_t price)
{
   if(price > 0)
   {
      if(static_cast<unsigned int>(char_can_afford(ch, price, DEF_CURRENCY)) == 0U)
      {
         act(COLOUR_ATTN "You couldn't afford to keep $2n." COLOUR_NORMAL, A_ALWAYS, ch, item, nullptr, TO_CHAR);
         extract_unit(item);
      }
      else
      {
         money_transfer(ch, nullptr, price, DEF_CURRENCY);
      }
   }
}

/* ----------------------------------------------------------------- */

static auto subtract_recurse(unit_data *ch, unit_data *item, uint32_t seconds, void (*fptr)(unit_data *ch, unit_data *obj, uint32_t price))
   -> uint32_t
{
   uint32_t sum = 0;

   if(IS_IMMORTAL(ch))
   {
      return 0;
   }

   if(item == nullptr)
   {
      return 0;
   }

   if(UNIT_MINV(item) == 0u)
   {
      sum += subtract_recurse(ch, UNIT_CONTAINS(item), seconds, fptr);
   }

   sum += subtract_recurse(ch, item->next, seconds, fptr);

   if(IS_OBJ(item) && (UNIT_MINV(item) == 0u))
   {
      uint32_t price = 0;

      if(OBJ_PRICE_DAY(item) > 0)
      {
         price = (int)((float)OBJ_PRICE_DAY(item) * (float)seconds / (float)SECS_PER_REAL_DAY);

         price = (price * g_cServerConfig.m_nRentModifier) / 100;

         sum += price;
      }

      if(fptr != nullptr)
      {
         (*fptr)(ch, item, price);
      }
   }

   return sum;
}

/* ----------------------------------------------------------------- */

auto rent_calc(unit_data *ch, time_t savetime) -> uint32_t
{
   uint32_t sum = 0;

   assert(IS_PC(ch));

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   { /* If loading or similar, dont subtract! */
      return 0;
   }

   if(IS_MORTAL(ch))
   {
      time_t t = time(nullptr);

      if((t > savetime) && (savetime > 0))
      {
         t -= savetime;

         if(t > SECS_PER_REAL_MIN * 10)
         {
            sum = subtract_recurse(ch, UNIT_CONTAINS(ch), t, subtract_rent);
         }
      }
   }

   return sum;
}

void do_rent(unit_data *ch, char *arg, const command_info *cmd)
{
   uint32_t sum;

   rent_info = static_cast<int>(FALSE);

   sum       = subtract_recurse(ch, UNIT_CONTAINS(ch), SECS_PER_REAL_DAY, show_items);

   if(rent_info == 0)
   {
      send_to_char("You are charged no rent.\n\r", ch);
   }
   else
   {
      act("Your inventory costs $2t per day to rent.", A_ALWAYS, ch, money_string(sum, local_currency(ch), FALSE), nullptr, TO_CHAR);
   }
}

/* *************************************************************************
 * Routines for loading and saving inventory                               *
 ************************************************************************* */

/* Header of compressed byte-string
   note: sice the length of the entire diff-set is often needed even when
   the head-struct may not be available, the diff-length is not stored in
   the header. */
struct diffhead
{
   uint32_t  reflen; /* length of reference (for checking) */
   uint32_t  crc;    /* extra check - maybe one enough? */
   short int start;  /* offset to first different byte */
   short int end;    /* offset to first similar byte after start */
};

/* Per-object header */
struct objheader
{
   int16_t length; /* length of data */
   char    zone[FI_MAX_ZONENAME + 1];
   char    unit[FI_MAX_UNITNAME + 1];
   uint8_t level;      /* level of 'containment' (depth) */
   uint8_t equip;      /* equipment position */
   uint8_t compressed; /* compressed? */
   uint8_t type;       /* NPC or OBJ? */
};

/* Local global variables */

/*
static char *membuf = 0;
static int membuflen = 0, mempos;
*/

/* Global variables */
file_index_type *slime_fi = nullptr;

/* save object */
void             enlist(CByteBuffer *pBuf, unit_data *unit, int level, int fast)
{
   int              len;
   int              diflen;
   struct objheader h;
   char            *buf;
   CByteBuffer      TmpBuf;

   auto             diff(char *ref, uint32_t reflen, char *obj, int objlen, char *dif, int diflen, uint32_t crc)->int;

   assert(IS_SET(UNIT_TYPE(unit), UNIT_ST_NPC | UNIT_ST_OBJ));
   assert(!is_destructed(DR_UNIT, unit));

   len = write_unit_string(&TmpBuf, unit);

   if((fast != 0) || (UNIT_FILE_INDEX(unit) == nullptr))
   {
      h.compressed = 0;
   }
   else
   {
      assert(FALSE);

#ifdef SUSPEKT
      CByteBuffer TmpBuf;

      read_unit_file(UNIT_FILE_INDEX(unit), &TmpBuf);

      if((diflen = diff((char *)TmpBuf.GetData(), UNIT_FILE_INDEX(unit)->length, pBuf, len, tmpdif, TMPMEM, UNIT_FILE_INDEX(unit)->crc)) <
         0)
      {
         slog(LOG_ALL, 0, "Out of memory for diff-generation.");
         abort();
      }

      h.compressed = 1;
      buf          = tmpdif;
      len          = diflen;
#endif
   }

   strcpy(h.zone, UNIT_FI_ZONENAME(unit));
   strcpy(h.unit, UNIT_FI_NAME(unit));

   h.type  = UNIT_TYPE(unit);
   h.level = level;

   if(IS_OBJ(unit))
   {
      h.equip = OBJ_EQP_POS(unit);
   }
   else
   {
      h.equip = 0;
   }

   h.length = len;

   pBuf->Append((uint8_t *)&h, sizeof(h));
   pBuf->Append(&TmpBuf);
}

/* Arguments:                                                    */
/*    unit   - The container to be saved (with contents)         */
/*    level  - 0 for contents only, 1 for contents & container   */
/*    fast   - TRUE for compression, FALSE for no compression.   */

void add_units(CByteBuffer *pBuf, unit_data *parent, unit_data *unit, int level, int fast)
{
   int        tmp_i = 0;
   unit_data *tmp_u;

   if(IS_ROOM(unit))
   {
      slog(LOG_ALL, 0, "BAD ERROR: Room attempted saved as inventory!");
      return;
   }

   if((tmp_u = UNIT_CONTAINS(unit)) != nullptr)
   {
      if(IS_OBJ(tmp_u) && ((tmp_i = OBJ_EQP_POS(tmp_u)) != 0))
      {
         unequip_object(tmp_u);
         OBJ_EQP_POS(tmp_u) = tmp_i;
      }

      unit_from_unit(tmp_u);

      add_units(pBuf, parent, unit, level, fast);

      if(IS_OBJ(tmp_u) || IS_NPC(tmp_u))
      {
         add_units(pBuf, parent, tmp_u, level + 1, fast);
      }

      unit_to_unit(tmp_u, unit);

      if(IS_OBJ(tmp_u) && (tmp_i != 0))
      {
         OBJ_EQP_POS(tmp_u) = 0;
         equip_char(unit, tmp_u, tmp_i);
      }
   }
   else /* UNIT CONTAINS NOTHING */
      if((level != 0) && (IS_OBJ(unit) || IS_NPC(unit)) && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_NOSAVE))
      {
         enlist(pBuf, unit, level, fast);
      }
}

void send_saves(unit_data *parent, unit_data *unit)
{
   unit_data *tmp_u;

   if(unit == nullptr)
   {
      return;
   }

   send_saves(parent, UNIT_CONTAINS(unit));
   send_saves(parent, unit->next);

   if((IS_OBJ(unit) || IS_NPC(unit)) && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_NOSAVE))
   {
      send_save_to(parent, unit);
   }
}

auto ContentsFileName(const char *pName) -> char *
{
   static char Buf[MAX_INPUT_LENGTH + 1];

   auto        PlayerFileName(const char *)->char *;

   sprintf(Buf, "%s.inv", PlayerFileName(pName));

   return Buf;
}

/* Save all units inside 'unit' in the blk_file 'bf' as uncompressed  */
/* if fast == 1 or compressed if fast == 0. Only OBJ's and NPC's will */
/* be saved!                                                          */
/* Container = 1 if container should be saved also                    */
void basic_save_contents(const char *pFileName, unit_data *unit, int fast, int bContainer)
{
   descriptor_data *tmp_descr = nullptr;
   FILE            *pFile;
   char             TmpName[MAX_INPUT_LENGTH + 1];

   fast = 1; /* MAJOR BUG IN DIFF CAUSING BAD PLAYERS! WITH TOO MUCH */
   /* INVENTORY, THE PLAYER INDEX WILL GET FUCKED UP!      */

   if(IS_CHAR(unit))
   {
      tmp_descr             = CHAR_DESCRIPTOR(unit);
      CHAR_DESCRIPTOR(unit) = nullptr;
   }

   CByteBuffer *pBuf = &g_FileBuffer;
   pBuf->Clear();

   if(bContainer != 0)
   {
      send_save_to(unit, unit);
   }

   send_saves(unit, UNIT_CONTAINS(unit));

   add_units(pBuf, unit, unit, bContainer != 0 ? 1 : 0, fast);

   if(IS_CHAR(unit))
   {
      CHAR_DESCRIPTOR(unit) = tmp_descr;
   }

   if(pBuf->GetLength() > 0)
   {
      strcpy(TmpName, ContentsFileName("aaa-inv.tmp"));
      pFile = fopen(TmpName, "wb");
      assert(pFile);
      pBuf->FileWrite(pFile);
      fclose(pFile);

      if(rename(TmpName, pFileName) != 0)
      {
         perror("rename:");
         exit(1);
      }
   }
}

/* Save all units inside 'unit' in the blk_file 'bf' as uncompressed  */
/* if fast == 1 or compressed if fast == 0. Only OBJ's and NPC's will */
/* be saved!                                                          */
/* Container = 1 if container should be saved also                    */
auto save_contents(const char *pFileName, unit_data *unit, int fast, int bContainer) -> int
{
   char name[MAX_INPUT_LENGTH + 1];

   fast = 1; /* MAJOR BUG IN DIFF CAUSING BAD PLAYERS! WITH TOO MUCH */
   /* INVENTORY, THE PLAYER INDEX WILL GET FUCKED UP!      */

   strcpy(name, ContentsFileName(pFileName));

   if(UNIT_CONTAINS(unit) == nullptr)
   {
      remove(name);
      return 0;
   }

   basic_save_contents(name, unit, fast, bContainer);

   return subtract_recurse(unit, UNIT_CONTAINS(unit), SECS_PER_REAL_DAY, nullptr);
}

/* From the block_file 'bf' at index 'blk_idx' load the objects    */
/* and place them inside 'unit' by unit_to_unit and possibly equip */
/* Return the top level unit loaded                                */

auto base_load_contents(const char *pFileName, const unit_data *unit) -> unit_data *
{
   struct objheader h;
   file_index_type *fi;
   unit_data       *pnew;
   unit_data       *pstack[25];
   int              len;
   int              init;
   int              frame;
   int              plen;
   int              n;
   descriptor_data *tmp_descr = nullptr;
   int              equip_ok;
   FILE            *pFile;
   unit_data       *topu = nullptr;

   CByteBuffer      InvBuf;
   InvBuf.Clear();

   extern unit_data *void_room;

   auto              is_slimed(file_index_type * sp)->int;
   auto              patch(char *ref, uint32_t reflen, char *dif, int diflen, char *res, int reslen, uint32_t crc)->int;

   assert(slime_fi != nullptr);

   pFile = fopen(pFileName, "rb");

   if(pFile == nullptr)
   {
      return nullptr;
   }

   len = fsize(pFile);
   if(len == 0)
   {
      fclose(pFile);
      return nullptr;
   }

   n = InvBuf.FileRead(pFile, len);
   fclose(pFile);

   if(n != len)
   {
      slog(LOG_ALL, 0, "Corrupted inventory: %s", pFileName);
      return nullptr;
   }

   frame         = 0;
   pstack[frame] = (unit_data *)unit;

   if((unit != nullptr) && IS_CHAR(unit))
   {
      tmp_descr                                      = CHAR_DESCRIPTOR(unit);
      CHAR_DESCRIPTOR(const_cast<unit_data *>(unit)) = nullptr;
   }

   for(init = static_cast<int>(TRUE); InvBuf.GetReadPosition() < InvBuf.GetLength();)
   {
      if(InvBuf.Read((uint8_t *)&h, sizeof(h)) != 0)
      {
         break;
      }

      fi       = find_file_index(h.zone, h.unit);

      pnew     = nullptr;

      equip_ok = static_cast<int>(TRUE);

      if(h.compressed != 0U)
      {
         assert(FALSE);

#ifdef SUSPEKT
         if(fi == NULL)
         {
            /* Too much log
              slog(LOG_ALL,0,"1. patch failed in load_contents, sliming!.");*/
            pnew     = read_unit(slime_fi); /* Unit lost - Slime it! */
            equip_ok = FALSE;
         }
         else
         {
            read_unit_file(fi, &InvBuf);

            if((plen = patch((char *)InvBuf.GetData(), fi->length, p, h.length, tmpmem, TMPMEM, fi->crc)) < 0)
            {
               /* TEST FOR IS SLIMED HERE! */
               /* slog(LOG_ALL,0,"2. Using orig."); Too much log*/
               pnew = read_unit_string(&InvBuf, h.type, 0, TRUE, "Uncompress"); /* Load NEW item */
               if(g_nCorrupt)
               {
                  slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
                  break;
               }
               insert_in_unit_list(pnew);
               UNIT_FILE_INDEX(pnew) = fi;
               fi->no_in_mem++;
               equip_ok = FALSE;
            }
            else
            {
               pbuf = tmpmem;
               pnew = read_unit_string(&InvBuf, h.type, 0, TRUE, "Uncompress 2");
               if(g_nCorrupt)
               {
                  slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
                  break;
               }
               insert_in_unit_list(pnew);
               UNIT_FILE_INDEX(pnew) = fi;
               fi->no_in_mem++;
            }
         }
#endif
      }
      else /* uncompressed */
      {
         if((fi == nullptr) || (is_slimed(fi) != 0))
         {
            pnew = read_unit(slime_fi);
            InvBuf.Skip(h.length);
         }
         else
         {
            pnew = read_unit_string(&InvBuf, h.type, h.length, static_cast<int>(TRUE), str_cc(fi->name, fi->zone->name));
            if(g_nCorrupt != 0)
            {
               slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
               break;
            }
            insert_in_unit_list(pnew);
            UNIT_FILE_INDEX(pnew) = fi;
            fi->no_in_mem++;
         }
      }

      if(init != 0)
      {
         topu = pnew;
      }

      if(pstack[frame] == nullptr)
      {
         if(UNIT_IN(pnew) != nullptr)
         {
            pstack[frame] = UNIT_IN(pnew);
         }
         else
         {
            pstack[frame] = void_room;
         }
      }

      UNIT_IN(pnew) = nullptr;

      if(h.level > frame)
      {
         unit_to_unit(pnew, pstack[frame]);
         frame = h.level;
      }
      else
      {
         frame = h.level;
         unit_to_unit(pnew, UNIT_IN(pstack[frame]));
      }

      /* IS_CHAR() needed, since a potential char may have been slimed! */
      if((h.equip != 0U) && (equip_ok != 0) && IS_CHAR(UNIT_IN(pnew)))
      {
         equip_char(UNIT_IN(pnew), pnew, h.equip);
      }

      pstack[frame] = pnew;
   }

   if((unit != nullptr) && IS_CHAR(unit))
   {
      CHAR_DESCRIPTOR(const_cast<unit_data *>(unit)) = tmp_descr;
   }

   return topu;
}

/* From the block_file 'bf' at index 'blk_idx' load the objects    */
/* and place them inside 'unit' by unit_to_unit and possibly equip */
/* Return the daily cost                                           */
void load_contents(const char *pFileName, unit_data *unit)
{
   base_load_contents(ContentsFileName(pFileName), unit);
}

void reception_boot()
{
   slime_fi = find_file_index("basis", "slime");
   assert(slime_fi);
}

/* ************************************************************************
 * Low-level routines for handling diff/patch                              *
 ************************************************************************* */

/* Create difference-data (patch can reconstruct obj based on ref & dif) */
auto diff(char *ref, uint32_t reflen, char *obj, int objlen, char *dif, int diflen, uint32_t crc) -> int
{
   int             dstart;
   int             dend;
   int             len;
   int             rlen;
   char           *oend;
   char           *rend;
   struct diffhead head;

   rend = ref + reflen - 1;
   oend = obj + objlen - 1;
   len  = objlen;
   rlen = reflen;

   /* find start of difference */
   for(dstart = 0; (len != 0) && (rlen != 0); dstart++, len--, rlen--)
   {
      if(*(ref + dstart) != *(obj + dstart))
      {
         break;
      }
   }

   /* find end of difference */
   for(dend = 0; (len != 0) && (rlen != 0); dend++, len--, rlen--)
   {
      if(*(rend - dend) != *(oend - dend))
      {
         break;
      }
   }

   if((int)(sizeof(head) + len) > diflen)
   {
      return -1;
   }
   diflen      = sizeof(head) + len;
   head.start  = dstart;

   head.end    = reflen - dend;
   head.reflen = reflen;
   head.crc    = crc;

   memcpy(dif, (char *)&head, sizeof(head));
   dif += sizeof(head);
   if(len != 0)
   {
      memcpy(dif, (char *)obj + dstart, len);
   }
   return (diflen);
}

/* reconstruct obj based on ref and diff */
auto patch(char *ref, uint32_t reflen, char *dif, int diflen, char *res, int reslen, uint32_t crc) -> int
{
   struct diffhead head;

   if(diflen < (int)sizeof(head))
   {
      return -1;
   }

   memcpy((char *)&head, dif, sizeof(head));
   dif += sizeof(head);
   diflen -= sizeof(head);

   if((int)(head.start + diflen + reflen - head.end) > reslen)
   {
      return -1;
   }

   if(head.reflen != reflen)
   {
      return -1;
   }

   if(head.crc != crc)
   {
      return -1;
   }

   if(head.start != 0)
   {
      memcpy(res, ref, head.start);
   }
   if(diflen != 0)
   {
      memcpy(res + head.start, dif, diflen);
   }
   if(head.end < (int)reflen)
   {
      memcpy(res + head.start + diflen, ref + head.end, reflen - head.end);
   }

   return (head.start + diflen + reflen - head.end);
}

/* ========================= DIL STORE / RESTORE ======================= */

void store_unit(unit_data *u)
{
   if(UNIT_FILE_INDEX(u) == nullptr)
   {
      return;
   }

   FILE *f;
   char  buf[MAX_INPUT_LENGTH + 1];

   sprintf(buf, DFLT_DIR UNIT_DIR "%s.%s", UNIT_FI_ZONENAME(u), UNIT_FI_NAME(u));

   f = fopen(buf, "wb");

   if(f == nullptr)
   {
      slog(LOG_ALL, 0, "Unable to store: %s", buf);
      return;
   }

   send_save_to(u, u); // Tell funcs on 'u' it is saved now...

   CByteBuffer *pBuf = &g_FileBuffer;
   pBuf->Clear();

   pBuf->Append8(UNIT_TYPE(u));
   int len = write_unit_string(pBuf, u);

   len     = pBuf->FileWrite(f);
   assert(len = pBuf->GetLength());

   fclose(f);
}

auto restore_unit(char *zonename, char *unitname) -> unit_data *
{
   file_index_type *fi   = find_file_index(zonename, unitname);
   CByteBuffer     *pBuf = &g_FileBuffer;
   pBuf->Clear();

   if(fi == nullptr)
   {
      return nullptr;
   }

   FILE *f;
   char  buf[MAX_INPUT_LENGTH + 1];

   sprintf(buf, DFLT_DIR UNIT_DIR "%s.%s", zonename, unitname);

   f = fopen(buf, "rb");

   if(f == nullptr)
   {
      slog(LOG_ALL, 0, "Unable to restore: %s", buf);
      return nullptr;
   }

   int len = fsize(f);
   int n   = pBuf->FileRead(f, len);
   assert(n == len);

   fclose(f);

   if(n == 0)
   {
      slog(LOG_ALL, 0, "Unable to restore, file empty: %s", buf);
      return nullptr;
   }

   uint8_t nType;

   pBuf->Read8(&nType);

   char mbuf[MAX_INPUT_LENGTH];
   strcpy(mbuf, "RESTORE");
   unit_data *u = read_unit_string(pBuf, nType, len - 1, static_cast<int>(TRUE), mbuf);

   if(u == nullptr)
   {
      slog(LOG_ALL, 0, "Unable to restore, corrupt: %s", buf);
      return nullptr;
   }

   UNIT_IN(u)         = nullptr;
   UNIT_FILE_INDEX(u) = fi;
   fi->no_in_mem++;
   insert_in_unit_list(u); /* Put unit into the unit_list      */
   apply_affect(u);        /* Set all affects that modify      */

   if(IS_MONEY(u))
   {
      set_money(u, MONEY_AMOUNT(u));
   }

   return u;
}
