#include "account.h"
#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "competition.h"
#include "connectionlog.h"
#include "db.h"
#include "db_file.h"
#include "dilrun.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>

int32_t            player_id       = 1;
static const char *tmp_player_name = PLAY_DIR "player.tmp";

extern char libdir[];
extern char plydir[];

auto PlayerFileName(const char *pName) -> char *
{
   static char Buf[MAX_INPUT_LENGTH + 1];
   char        TmpBuf[MAX_INPUT_LENGTH + 1];

   strcpy(TmpBuf, pName);
   str_lower(TmpBuf);
   sprintf(Buf, "%s%c/%s", plydir, *TmpBuf, TmpBuf);

   return Buf;
}

/* Return TRUE if exists */
auto player_exists(const char *pName) -> int
{
   return static_cast<int>(file_exists(PlayerFileName(pName)));
}

auto find_player(char *name) -> unit_data *
{
   descriptor_data *d;

   d = find_descriptor(name, nullptr);

   if((d != nullptr) && (d->fptr == descriptor_interpreter) && (d->character != nullptr))
   {
      return d->character;
   }
   return nullptr;
}

/* Return TRUE if deleted */
auto delete_inventory(const char *pName) -> int
{
   auto ContentsFileName(const char *pName)->char *;

   if(remove(ContentsFileName(pName)) != 0)
   {
      return static_cast<int>(FALSE);
   }

   return static_cast<int>(TRUE);
}

/* Return TRUE if deleted */
auto delete_player(const char *pName) -> int
{
   if(remove(PlayerFileName(pName)) != 0)
   {
      return static_cast<int>(FALSE);
   }

   delete_inventory(pName);

   return static_cast<int>(TRUE);
}

/* Given a name, return pointer to player-idx blk, or BLK_NULL if non exist */
auto find_player_id(char *pName) -> int
{
   FILE *pFile;
   int   id;

   if(static_cast<unsigned int>(str_is_empty(pName)) != 0U)
   {
      slog(LOG_ALL, 0, "Empty string in find_player_id.");
      return -1;
   }

   pFile = fopen(PlayerFileName(pName), "rb");

   if(pFile == nullptr)
   {
      return -1;
   }

   rewind(pFile);

   if(fread(&id, sizeof(int), 1, pFile) != 1)
   {
      error(HERE, "Unable to read ID for player: '%s'", pName);
   }

   fclose(pFile);

   return id;
}

/* Call to read current id from file*/
auto read_player_id() -> int32_t
{
   int32_t tmp_sl;
   FILE   *pFile;

   /* By using r+ we are sure that we don't erase it accidentially
      if the host crashes just after opening the file. */

   pFile = fopen_cache(str_cc(libdir, PLAYER_ID_NAME), "r+");
   assert(pFile);
   fseek(pFile, 0, SEEK_SET);
   int ms2020 = fscanf(pFile, " %d ", &tmp_sl);

   return tmp_sl;
}

/* Call to generate new id */
auto new_player_id() -> int
{
   FILE *pFile;

   /* By using r+ we are sure that we don't erase it accidentially
      if the host crashes just after opening the file. */

   pFile = fopen_cache(str_cc(libdir, PLAYER_ID_NAME), "r+");
   assert(pFile);
   fseek(pFile, 0, SEEK_SET);

   fprintf(pFile, " %d ", player_id + 1);

   // fflush(pFile);

   return player_id++;
}

void save_player_disk(const char *pName, char *pPassword, int id, int nPlyLen, const uint8_t *pPlyBuf)
{
   int   n;
   FILE *pPlayerFile;

   /* Fucking shiting pissing lort! This marcel is driving me mad! */
   assert(!file_exists(tmp_player_name));
   pPlayerFile = fopen(tmp_player_name, "wb");
   assert(pPlayerFile);

   n = fwrite(&id, sizeof(id), 1, pPlayerFile);
   assert(n == 1);

   n = fwrite(&nPlyLen, sizeof(nPlyLen), 1, pPlayerFile);
   assert(n == 1);

   n = fwrite(pPlyBuf, sizeof(uint8_t), nPlyLen, pPlayerFile);
   assert(n == nPlyLen);

   /* This is a small test to see if the file was actually written to the
      disk... apparently sometimes this is not the case on marcel (disk
      full?). Anyway if that is a problem it should have been caught by
      the n == nPlyLen */

   if(fseek(pPlayerFile, 0L, SEEK_END) != 0)
   {
      assert(FALSE);
   }

   assert(ftell(pPlayerFile) == (long int)(nPlyLen + sizeof(nPlyLen) + sizeof(id)));

   fclose(pPlayerFile);

   /* Unfortunately this must be done to ensure that when the host
      crashes, it doesn't garble the player. At least then, the
      old file will remain intact. */

   n = rename(tmp_player_name, PlayerFileName(pName));
   assert(n == 0);
}

/* Save the player 'pc' (no inventory) */
void save_player_file(unit_data *pc)
{
   static bool      locked = FALSE;
   blk_length       nPlyLen;
   int              tmp_i;
   unit_data       *tmp_u;
   unit_data       *list = nullptr;
   descriptor_data *tmp_descr;
   CByteBuffer     *pBuf = &g_FileBuffer;

   pBuf->Clear();

   assert(IS_PC(pc));
   assert(strlen(PC_FILENAME(pc)) < PC_MAX_NAME);
   assert(!is_destructed(DR_UNIT, pc));

   if(is_destructed(DR_UNIT, pc) != 0)
   {
      return;
   }

   if(locked)
   {
      slog(LOG_ALL, 0, "ERROR: INFORM PAPI OF A RECURSIVE CALL OF SAVE PLAYER!");
      return;
   }

   locked = TRUE;

   if(PC_IS_UNSAVED(pc))
   {
      PC_TIME(pc).played++;
   }

   /* PRIMITIVE SANITY CHECK */
   assert(PC_ID(pc) >= 0 && PC_ID(pc) <= 1000000);

   if((UNIT_IN(pc) != nullptr) && !IS_SET(UNIT_FLAGS(unit_room(pc)), UNIT_FL_NOSAVE))
   {
      CHAR_LAST_ROOM(pc) = unit_room(pc);
   }

   tmp_descr           = CHAR_DESCRIPTOR(pc);
   CHAR_DESCRIPTOR(pc) = nullptr; /* Do this to turn off all messages! */

   /* Remove all inventory and equipment in order to make a CLEAN save */
   while((tmp_u = UNIT_CONTAINS(pc)) != nullptr)
   {
      if(IS_OBJ(tmp_u))
      {
         if((tmp_i = OBJ_EQP_POS(tmp_u)) != 0)
         {
            unequip_object(tmp_u);
         }
         OBJ_EQP_POS(tmp_u) = tmp_i;
      }
      unit_from_unit(tmp_u);
      tmp_u->next = list;
      list        = tmp_u;
   }

   send_save_to(pc, pc); // Tell funcs on PC he is saved now...

   /* Player is now clean (empty and unequipped) */
   nPlyLen = write_unit_string(pBuf, pc);

   save_player_disk(PC_FILENAME(pc), PC_PWD(pc), PC_ID(pc), nPlyLen, pBuf->GetData());

   /* Restore all inventory and equipment */
   while((tmp_u = list) != nullptr)
   {
      list        = list->next;
      tmp_u->next = nullptr;

      unit_to_unit(tmp_u, pc);

      if(IS_OBJ(tmp_u))
      {
         tmp_i              = OBJ_EQP_POS(tmp_u);
         OBJ_EQP_POS(tmp_u) = 0;
         if(tmp_i != 0)
         {
            equip_char(pc, tmp_u, tmp_i);
         }
      }
   }

   CHAR_DESCRIPTOR(pc) = tmp_descr; /* Turn msgs back on */

   locked = FALSE;
}

/* If 'fast' is false compression is used for inventory.       */
void save_player_contents(unit_data *pc, int fast)
{
   static bool locked = FALSE;
   time_t      t0;
   time_t      keep_period;
   amount_t    daily_cost;
   currency_t  cur = local_currency(pc);

   auto save_contents(const char *pFileName, unit_data *unit, int fast, int bContainer)->int;

   assert(IS_PC(pc));

   if(locked)
   {
      slog(LOG_ALL, 0, "ERROR: INFORM PAPI OF A RECURSIVE SAVE INVENTORY!");
      return;
   }

   locked = TRUE;

   /* Too much log in convert
   slog(LOG_ALL, "Saving %s [len %d of %d].", PC_FILENAME(pc), len,
        filbuffer_length); */

   /* Calculate for how long player may keep objects until erased */
   t0          = time(nullptr);
   keep_period = t0;

   daily_cost = save_contents(PC_FILENAME(pc), pc, fast, static_cast<int>(FALSE));

   if(daily_cost <= 0)
   {
      keep_period += SECS_PER_REAL_DAY * 30;
   }
   else
   {
      amount_t amount = char_holds_amount(pc, DEF_CURRENCY);

      if(amount > 0)
      {
         int    tmp_i;
         time_t tdiff;

         /* No of days items may be kept (Maximum of 30!) */
         tmp_i = std::min(30, amount / daily_cost);

         if(tmp_i >= 1)
         {
            keep_period += tmp_i * SECS_PER_REAL_DAY;
            amount -= tmp_i * daily_cost;
         }

         if(tmp_i < 30)
         {
            keep_period += (int)(((float)SECS_PER_REAL_DAY * (float)amount) / (float)daily_cost);
         }

         tdiff = (keep_period - t0) / SECS_PER_REAL_HOUR;
         act("Inventory expires in $2d hours ($3t daily).", A_ALWAYS, pc, (int *)&tdiff, money_string(daily_cost, cur, FALSE), TO_CHAR);
      }
      else
      {
         act(
            "You can't afford to keep your inventory (cost is $3t).", A_ALWAYS, pc, nullptr, money_string(daily_cost, cur, FALSE), TO_CHAR);
      }
   }

   locked = FALSE;
}

/* Save the player 'pc'. Update logon and playing time.        */
void save_player(unit_data *pc)
{
   if(CHAR_DESCRIPTOR(pc) != nullptr)
   {
      time_t   t0;
      uint32_t used;

      t0 = time(nullptr);
      if(t0 < CHAR_DESCRIPTOR(pc)->logon)
      {
         slog(LOG_ALL, 0, "PCSAVE: Current less than last logon");
         CHAR_DESCRIPTOR(pc)->logon = t0;
      }

      account_subtract(pc, CHAR_DESCRIPTOR(pc)->logon, t0);

      used = t0 - CHAR_DESCRIPTOR(pc)->logon;

      ConnectionLog(pc);

      PC_TIME(pc).played += used;
      CHAR_DESCRIPTOR(pc)->logon = t0;

      if(account_is_closed(pc) != 0)
      {
         account_closed(pc);
      }
      else if(account_is_overdue(pc) != 0)
      {
         account_overdue(pc);
      }

      competition_update(pc);
   }

   /* Too much log...
   slog(LOG_ALL,"Saving %s %s",UNIT_NAME(pc),UNIT_CONTAINS(pc) ? "(INV)" : "");
   */

   save_player_file(pc);
}

/* Read player from file, starting at index "index" */
/* String is allocated                              */
auto load_player_file(FILE *pFile) -> unit_data *
{
   unit_data   *pc;
   int          nPlyLen;
   int          n;
   int          id;
   CByteBuffer *pBuf;

   assert(pFile);

   n = fread(&id, sizeof(int), 1, pFile);
   if(n != 1)
   {
      return nullptr;
   }

   n = fread(&nPlyLen, sizeof(nPlyLen), 1, pFile);
   if(n != 1)
   {
      return nullptr;
   }

   pBuf = &g_FileBuffer;
   n    = pBuf->FileRead(pFile, nPlyLen);

   if(n != nPlyLen)
   {
      slog(LOG_ALL, 0, "ERROR: PC FILE LENGTH MISMATCHED RECORDED LENGTH!");
   }

   char mbuf[MAX_INPUT_LENGTH];
   strcpy(mbuf, "Player");
   pc = read_unit_string(pBuf, UNIT_ST_PC, nPlyLen, static_cast<int>(TRUE), mbuf);

   if(pc == nullptr)
   {
      return nullptr;
   }

   if(g_nCorrupt != 0)
   {
      return nullptr;
   }

   return pc;
}

/* Read player from file, starting at index "index"   */
/* String is allocated                                */
/* Is neither inserted in unit_list not into anything */
auto load_player(const char *pName) -> unit_data *
{
   FILE      *pFile;
   unit_data *pc;

   void stop_all_special(unit_data * u);

   if(static_cast<unsigned int>(str_is_empty(pName)) != 0U)
   {
      return nullptr;
   }

   pFile = fopen(PlayerFileName(pName), "rb");
   if(pFile == nullptr)
   {
      return nullptr;
   }

   pc = load_player_file(pFile);

   fclose(pFile);

   if(pc == nullptr)
   {
      slog(LOG_ALL, 0, "Corrupted player %s.", pName);
      return nullptr;
   }

   stop_affect(pc);
   stop_all_special(pc);
   DeactivateDil(pc);

   if(str_ccmp(pName, PC_FILENAME(pc)) != 0)
   {
      slog(LOG_ALL, 0, "Mismatching player name %s / %s.", pName, PC_FILENAME(pc));
      extract_unit(pc);
      return nullptr;
   }

   if(PC_IS_UNSAVED(pc))
   {
      slog(LOG_ALL, 0, "PC loaded with unsaved set!", pName);
      PC_TIME(pc).played++;
   }

   return pc;
}

/* Call at boot time to index file */
void player_file_index()
{
   FILE   *pFile;
   int32_t tmp_sl;
   int     n;

   /* Get rid of any temporary player save file */
   while(static_cast<unsigned int>(file_exists(tmp_player_name)) != 0U)
   {
      n = remove(tmp_player_name);
      if(n != 0)
      {
         slog(LOG_ALL, 0, "Remove failed");
      }
      if(static_cast<unsigned int>(file_exists(tmp_player_name)) != 0U)
      {
         n = rename(tmp_player_name, "./playingfuck");
         if(n != 0)
         {
            error(HERE, "Rename failed too - going down :-(");
         }
      }
   }

   if(static_cast<unsigned int>(file_exists(str_cc(libdir, PLAYER_ID_NAME))) == 0U)
   {
      touch_file(str_cc(libdir, PLAYER_ID_NAME));
      player_id = -7;
      return;
   }

   pFile = fopen_cache(str_cc(libdir, PLAYER_ID_NAME), "r+");
   assert(pFile);

   int ms2020 = fscanf(pFile, " %d ", &tmp_sl);

   if((player_id = tmp_sl) <= 0)
   {
      slog(LOG_ALL, 0, "WARNING: Player ID is %d", player_id);
   }
}
