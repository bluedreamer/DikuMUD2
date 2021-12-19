/* *********************************************************************** *
 * File   : account.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: To implement accounting commands, status, etc.                 *
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

/* #define ACCOUNT_DEBUG */
#include "account.h"

#include "CAccountConfig.h"
#include "comm.h"
#include "common.h"
#include "CServerConfiguration.h"
#include "db.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "str_parse.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

/* This is a LOT nicer than using structs. Trust me, I tried to program it! */

int32_t day_charge[7][TIME_GRANULARITY];

int     next_crc = 0;

void    account_cclog(unit_data *ch, int amount)
{
   FILE *f;

   f = fopen(str_cc(libdir, CREDITFILE_LOG), "a+b");

   fprintf(f, "%-16s %6.2f %s\n", UNIT_NAME(ch), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

   fclose(f);
}

static void account_log(char action, unit_data *god, unit_data *pc, int amount)
{
   time_t   now = time(nullptr);
   char    *c;
   char     buf[1024];
   uint32_t gid;
   uint32_t pid;
   uint32_t total;
   uint32_t crc;
   uint32_t m_xor;
   FILE    *f;

   m_xor = number(0x10000000, 0xFFFFFF00 >> 1);

   next_crc ^= m_xor;

   f = fopen_cache(str_cc(libdir, ACCOUNT_LOG), "r+b");

   if(fseek(f, 8L, SEEK_SET) != 0)
   {
      error(HERE, "Unable to seek in account log.");
   }
   sprintf(buf, "%08x", next_crc);

   if(fwrite(buf, sizeof(char), 8, f) != 8)
   {
      error(HERE, "Unable to write 1 in account log.");
   }

   c = buf;
   sprintf(c, "%c %-15s %-15s %8d ", action, UNIT_NAME(god), UNIT_NAME(pc), amount);

   if(IS_PC(god))
   {
      gid = PC_ID(god) ^ (m_xor);
   }
   else
   {
      gid = 0 ^ (m_xor);
   }

   pid   = PC_ID(pc) ^ (m_xor << 1);
   total = PC_ACCOUNT(pc).total_credit;

   amount += 13;
   amount ^= m_xor << 2;
   total += 17;
   total ^= m_xor << 3;

   crc = gid + pid + total + amount + now;
   crc ^= m_xor << 4;

   TAIL(c);
   sprintf(c, "%01x%08x%08x%08x%08x%08x%08x%08x%08x\n", number(0, 15), ~m_xor, gid, crc, pid, amount, total, next_crc, (uint32_t)now);

   fseek(f, 0L, SEEK_END);

   // MS2020:  fprintf(f, buf); warning
   fprintf(f, "%s", buf);

   fflush(f);
}

/* Hours in 0..23, minutes in 0..59 */
auto index_to_time(int index) -> int
{
   return 100 * ((index * MINUTE_GRANULARITY) / 60) + (index * MINUTE_GRANULARITY) % 60;
}

/* Hours in 0..23, minutes in 0..59 */
auto time_to_index(uint8_t hours, uint8_t minutes) -> int
{
   return ((hours * 60) + minutes) / MINUTE_GRANULARITY;
}

void account_local_stat(const unit_data *ch, unit_data *u)
{
   if(g_cServerConfig.m_bAccounting == 0)
   {
      send_to_char("Game is not in accounting mode.\n\r", ch);
      return;
   }

   if(!IS_PC(u))
   {
      send_to_char("Only players have accounts.\n\r", ch);
      return;
   }

   char  *pTmstr = ctime((time_t *)&PC_ACCOUNT(u).flatrate);
   char   buf[MAX_STRING_LENGTH];
   time_t now = time(nullptr);

   if(IS_ADMINISTRATOR(ch))
   {
      sprintf(buf,
              "Credit         : %5.2f\n\r"
              "Credit Limit   : %5.2f\n\r"
              "Credit to date : %5.2f\n\r"
              "Credit Card    : %s\n\r"
              "Discount       : %3d%%\n\r"
              "Flat Rate      : %s%s"
              "Crack counter  : %3d\n\r",
              (float)PC_ACCOUNT(u).credit / 100.0,
              (float)PC_ACCOUNT(u).credit_limit / 100.0,
              (float)PC_ACCOUNT(u).total_credit / 100.0,
              PC_ACCOUNT(u).last4 == -1 ? "NONE" : "REGISTERED",
              PC_ACCOUNT(u).discount,
              PC_ACCOUNT(u).flatrate < (uint32_t)now ? "Expired" : "Expires on ",
              PC_ACCOUNT(u).flatrate < (uint32_t)now ? " (none)\n\r" : pTmstr,
              PC_ACCOUNT(u).cracks);
   }
   else
   {
      if(PC_ACCOUNT(u).total_credit > 0)
      {
         sprintf(buf, "Has paid for playing.\n\r");
      }
      else
      {
         sprintf(buf, "Has NOT yet paid for playing.\n\r");
      }
   }

   send_to_char(buf, ch);
}

void account_global_stat(const unit_data *ch)
{
   char  buf[100 * TIME_GRANULARITY];
   char *b;
   int   i;
   int   j;

   if(g_cServerConfig.m_bAccounting == 0)
   {
      return;
   }

   sprintf(buf,
           "\n\rAccounting mode:\n\r"
           "  Free from level : %d\n\r"
           "  Currency Name   : %s\n\r"
           "  Default limit   : %.2f\n\r"
           "  Default start   : %.2f\n\r\n\r",
           g_cAccountConfig.m_nFreeFromLevel,
           g_cAccountConfig.m_pCoinName,
           (float)g_cAccountConfig.m_nAccountLimit / 100.0,
           (float)g_cAccountConfig.m_nAccountFree / 100.0);

   send_to_char(buf, ch);

   b = buf;
   sprintf(buf, "    Time    Sun  Mon  Tir  Wed  Thu  Fri  Sat\n\r");
   TAIL(b);

   for(i = 0; i < TIME_GRANULARITY; i++)
   {
      int st;
      int et;

      st = index_to_time(i);
      et = st + MINUTE_GRANULARITY - 1;

      sprintf(b, "%4d-%4d", st, et);
      TAIL(b);

      for(j = 0; j < 7; j++)
      {
         sprintf(b, "%5d", day_charge[j][i]);
         TAIL(b);
      }
      strcat(b, "\n\r");
      TAIL(b);
   }

   page_string(CHAR_DESCRIPTOR(ch), buf);
}

void account_overdue(const unit_data *ch)
{
   int i;
   int j;

   if(g_cServerConfig.m_bAccounting != 0)
   {
      char     Buf[256];

      uint32_t discount = PC_ACCOUNT(ch).discount;
      uint32_t lcharge  = ((100 - discount) * g_cAccountConfig.m_nHourlyRate) / 100;

      if(lcharge == 0)
      {
         i = 0;
         j = 0;
      }
      else
      {
         i = (int)(PC_ACCOUNT(ch).credit_limit + PC_ACCOUNT(ch).credit);
         j = (int)(((float)(i % lcharge) / (float)((float)lcharge / 60.0)));
         i = i / lcharge;
      }

      sprintf(Buf,
              "Your account is overdue by %.2f %s with a "
              "limit of %.2f %s.\n\r"
              "The account will expire in %d hours and %d minutes.\n\r\n\r",
              (float)-PC_ACCOUNT(ch).credit / 100.0,
              g_cAccountConfig.m_pCoinName,
              (float)PC_ACCOUNT(ch).credit_limit / 100.0,
              g_cAccountConfig.m_pCoinName,
              i,
              j);

      send_to_char(Buf, ch);
      send_to_char(g_cAccountConfig.m_pOverdueMessage, ch);
   }
}

void account_paypoint(unit_data *ch)
{
   send_to_char(g_cAccountConfig.m_pPaypointMessage, ch);
}

void account_closed(unit_data *ch)
{
   if(g_cServerConfig.m_bAccounting != 0)
   {
      send_to_char(g_cAccountConfig.m_pClosedMessage, ch);
   }
}

static auto seconds_used(uint8_t bhr, uint8_t bmi, uint8_t bse, uint8_t ehr, uint8_t emi, uint8_t ese) -> uint32_t
{
   uint32_t secs;

   secs = (ese - bse);

   secs += 60 * (emi - bmi);

   secs += 60 * 60 * (ehr - bhr);

   return secs;
}

static auto tm_less_than(tm *b, tm *e) -> int
{
   if(b->tm_wday != e->tm_wday)
   {
      return static_cast<int>(TRUE);
   }

   if(b->tm_hour > e->tm_hour)
   {
      return static_cast<int>(FALSE);
   }

   if(b->tm_hour == e->tm_hour)
   {
      if(b->tm_min > e->tm_min)
      {
         return static_cast<int>(FALSE);
      }
      if(b->tm_min == e->tm_min)
      {
         if(b->tm_sec > e->tm_sec)
         {
            return static_cast<int>(FALSE);
         }
         return static_cast<int>(TRUE);
      }
   }

   return static_cast<int>(TRUE);
}

static void account_calc(unit_data *pc, tm *b, tm *e)
{
   int      bidx;
   int      eidx;
   tm       t;
   uint32_t secs;

   if(PC_ACCOUNT(pc).flatrate > (uint32_t)time(nullptr))
   {
      return;
   }

   bidx = time_to_index(b->tm_hour, b->tm_min);
   assert(bidx < TIME_GRANULARITY);

   eidx = time_to_index(e->tm_hour, e->tm_min);
   assert(eidx < TIME_GRANULARITY);

   if(bidx != eidx)
   {
      int ntime = index_to_time(bidx);
      t.tm_hour = ntime / 100;
      t.tm_min  = ntime % 100 + 14;
      t.tm_sec  = 59;
      t.tm_wday = b->tm_wday;
      secs      = 1;
   }
   else
   {
      t    = *e;
      secs = 0;
   }

   secs += seconds_used(b->tm_hour, b->tm_min, b->tm_sec, t.tm_hour, t.tm_min, t.tm_sec);

   float amt = (((float)secs) * ((float)day_charge[b->tm_wday][bidx]) / 3600.0);

   if(is_in(PC_ACCOUNT(pc).discount, 1, 99) != 0)
   {
      PC_ACCOUNT(pc).credit -= (((float)(100 - PC_ACCOUNT(pc).discount)) * amt) / 100.0;
   }
   else
   {
      PC_ACCOUNT(pc).credit -= amt;
   }

#ifdef ACCOUNT_DEBUG
   {
      char buf[500];
      sprintf(buf,
              "%d:%d.%d (%d) to %d:%d.%d (%d) = %d -- "
              "charge %d / %.2f\n\r",
              b->tm_hour,
              b->tm_min,
              b->tm_sec,
              b->tm_wday,
              t.tm_hour,
              t.tm_min,
              t.tm_sec,
              e->tm_wday,
              secs,
              day_charge[b->tm_wday][bidx],
              (((float)secs) * ((float)day_charge[b->tm_wday][bidx]) / 3600.0));
      send_to_char(buf, pc);
   }
#endif

   *b        = t;
   b->tm_sec = 0;
   b->tm_min++;
   if(b->tm_min > 59)
   {
      b->tm_min = 0;
      b->tm_hour++;
      if(b->tm_hour > 23)
      {
         if(b->tm_wday != e->tm_wday) /* To make <= to work */
         {
            b->tm_wday = (b->tm_wday + 1) % 7;
            b->tm_hour = 0;
         }
      }
   }

   if(tm_less_than(b, e) != 0)
   {
      account_calc(pc, b, e);
   }
}

void account_subtract(unit_data *pc, time_t from, time_t to)
{
   struct tm bt;
   struct tm et;

   assert(IS_PC(pc));

   if(g_cServerConfig.m_bAccounting == 0)
   {
      return;
   }

   if(CHAR_LEVEL(pc) >= g_cAccountConfig.m_nFreeFromLevel)
   {
      return;
   }

   bt = *localtime(&from);
   et = *localtime(&to);

   /* DEBUG:
      sprintf(buf, "%d:%d.%d (%d) to %d:%d.%d (%d)\n\r",
      bt.tm_hour, bt.tm_min, bt.tm_sec, bt.tm_wday,
      et.tm_hour, et.tm_min, et.tm_sec, et.tm_wday);
      send_to_char(buf, pc); */

   account_calc(pc, &bt, &et);
}

auto account_is_overdue(const unit_data *ch) -> int
{
   if((g_cServerConfig.m_bAccounting != 0) && (CHAR_LEVEL(ch) < g_cAccountConfig.m_nFreeFromLevel))
   {
      if(PC_ACCOUNT(ch).flatrate > (uint32_t)time(nullptr))
      {
         return static_cast<int>(FALSE);
      }

      return static_cast<int>(PC_ACCOUNT(ch).credit < 0.0);
   }

   return static_cast<int>(FALSE);
}

static void account_status(const unit_data *ch)
{
   char     Buf[256];
   int      j;
   int      i;
   char    *pTmstr;
   uint32_t discount = PC_ACCOUNT(ch).discount;
   uint32_t lcharge  = ((100 - discount) * g_cAccountConfig.m_nHourlyRate) / 100;

   if(account_is_overdue(ch) != 0)
   {
      send_to_char("Your account is overdue.\n\r", ch);
      send_to_char(g_cAccountConfig.m_pOverdueMessage, ch);
   }

   if(discount > 0)
   {
      sprintf(Buf, "You have an overall discount of %d%%.\n\r", discount);
      send_to_char(Buf, ch);
   }

   if(PC_ACCOUNT(ch).flatrate > (uint32_t)time(nullptr))
   {
      pTmstr = ctime((time_t *)&PC_ACCOUNT(ch).flatrate);
      sprintf(Buf, "Your account is on a flat rate until %s", pTmstr);
      send_to_char(Buf, ch);

      if(PC_ACCOUNT(ch).credit >= 0.0)
      {
         sprintf(Buf, "You have a positive balance of %.2f %s.\n\r", PC_ACCOUNT(ch).credit / 100.0, g_cAccountConfig.m_pCoinName);

         send_to_char(Buf, ch);
      }
      return;
   }

   if(PC_ACCOUNT(ch).credit >= 0.0)
   {
      sprintf(Buf, "You have a positive balance of %.2f %s.\n\r", PC_ACCOUNT(ch).credit / 100.0, g_cAccountConfig.m_pCoinName);

      send_to_char(Buf, ch);

      if(lcharge > 0)
      {
         i = (int)PC_ACCOUNT(ch).credit;
         j = (int)(((float)(i % lcharge) / (float)((float)lcharge / 60.0)));
         i = i / lcharge;

         act("At the current rate that is $2d hours and $3d minutes.", A_ALWAYS, ch, &i, &j, TO_CHAR);

         i = (int)(((float)PC_ACCOUNT(ch).credit_limit / (float)(lcharge)));

         sprintf(Buf,
                 "Your credit limit is %d hours (%.2f %s).\n\r",
                 i,
                 (float)PC_ACCOUNT(ch).credit_limit / 100.0,
                 g_cAccountConfig.m_pCoinName);
         send_to_char(Buf, ch);
      }
   }
   else
   {
      if(lcharge > 0)
      {
         i = (int)(PC_ACCOUNT(ch).credit_limit + PC_ACCOUNT(ch).credit);
         j = (int)(((float)(i % lcharge) / (float)((float)lcharge / 60.0)));
         i = i / lcharge;

         sprintf(Buf,
                 "Your account is overdue by %.2f %s with a "
                 "limit of %.2f %s.\n\r"
                 "The account will expire in %d hours and %d minutes.\n\r",
                 (float)-PC_ACCOUNT(ch).credit / 100.0,
                 g_cAccountConfig.m_pCoinName,
                 (float)PC_ACCOUNT(ch).credit_limit / 100.0,
                 g_cAccountConfig.m_pCoinName,
                 i,
                 j);
         send_to_char(Buf, ch);
      }
      else
      {
         sprintf(Buf, "You have a negative balance of %.2f %s.\n\r", PC_ACCOUNT(ch).credit / 100.0, g_cAccountConfig.m_pCoinName);
      }
   }
}

auto account_is_closed(unit_data *ch) -> int
{
   int i;
   int j;

   if((g_cServerConfig.m_bAccounting != 0) && (CHAR_LEVEL(ch) < g_cAccountConfig.m_nFreeFromLevel))
   {
      if(PC_ACCOUNT(ch).flatrate > (uint32_t)time(nullptr))
      {
         return static_cast<int>(FALSE);
      }

      i = (int)PC_ACCOUNT(ch).credit;
      j = PC_ACCOUNT(ch).credit_limit;

      return static_cast<int>(i < -j);
   }

   return static_cast<int>(FALSE);
}

void account_insert(unit_data *god, unit_data *whom, uint32_t amount)
{
   PC_ACCOUNT(whom).credit += (float)amount;
   PC_ACCOUNT(whom).total_credit += amount;

   slog(LOG_ALL, 255, "%s inserted %d on account %s.", UNIT_NAME(god), amount, UNIT_NAME(whom));
   account_log('I', god, whom, amount);
}

void account_withdraw(unit_data *god, unit_data *whom, uint32_t amount)
{
   PC_ACCOUNT(whom).credit -= (float)amount;
   if((uint32_t)amount > PC_ACCOUNT(whom).total_credit)
   {
      PC_ACCOUNT(whom).total_credit = 0;
   }
   else
   {
      PC_ACCOUNT(whom).total_credit -= amount;
   }

   slog(LOG_ALL, 255, "%s withdrew %d from account %s.", UNIT_NAME(god), amount, UNIT_NAME(whom));

   account_log('W', god, whom, amount);
}

void account_flatrate_change(unit_data *god, unit_data *whom, int32_t days)
{
   char    Buf[256];
   int32_t add = days * SECS_PER_REAL_DAY;

   time_t  now = time(nullptr);

   if(days > 0)
   {
      if(PC_ACCOUNT(whom).flatrate > (uint32_t)now)
      {
         sprintf(Buf, "\n\rAdding %d days to the flatrate.\n\r\n\r", days);
         PC_ACCOUNT(whom).flatrate += add;
      }
      else
      {
         assert(add > 0);
         sprintf(Buf, "\n\rSetting flatrate to %d days.\n\r\n\r", days);
         PC_ACCOUNT(whom).flatrate = now + add;
      }
   }
   else /* days < 0 */
   {
      if((int32_t)PC_ACCOUNT(whom).flatrate + add < now)
      {
         sprintf(Buf, "\n\rDisabling flatrate, enabling measure rate.\n\r\n\r");
         PC_ACCOUNT(whom).flatrate = 0;
      }
      else
      {
         sprintf(Buf, "\n\rSubtracting %d days from the flatrate.\n\r\n\r", days);
         PC_ACCOUNT(whom).flatrate += add;
      }
   }

   slog(LOG_ALL, 255, "%s change flatrate with %d on account %s.", UNIT_NAME(god), days, UNIT_NAME(whom));
   account_log('F', god, whom, days);

   send_to_char(Buf, god);
}

void do_account(unit_data *ch, char *arg, const command_info *cmd)
{
   char                    Buf[256];
   char                    word[MAX_INPUT_LENGTH];
   unit_data              *u;
   unit_data              *note;
   char                   *c            = (char *)arg;

   const char             *operations[] = {"insert", "withdraw", "limit", "discount", "flatrate", nullptr};
   int                     i;
   int                     amount;

   extern file_index_type *letter_fi;

   if((g_cServerConfig.m_bAccounting == 0) || !IS_PC(ch))
   {
      send_to_char("That command is not available.\n\r", ch);
      return;
   }

   if((static_cast<unsigned int>(str_is_empty(arg)) != 0U) || !IS_ADMINISTRATOR(ch))
   {
      account_status(ch);
      return;
   }

   c = skip_spaces(c);

#ifdef ACCOUNT_DEBUG

   if(isdigit(*c))
   {
      struct tm btm, etm;

      char      bhr[200], bmi[200], bse[200], bda[200];
      char      ehr[200], emi[200], ese[200], eda[200];

      c           = str_next_word_copy(c, bhr);
      c           = str_next_word_copy(c, bmi);
      c           = str_next_word_copy(c, bse);
      c           = str_next_word_copy(c, bda);

      c           = str_next_word_copy(c, ehr);
      c           = str_next_word_copy(c, emi);
      c           = str_next_word_copy(c, ese);
      c           = str_next_word_copy(c, eda);

      btm.tm_hour = atoi(bhr);
      btm.tm_min  = atoi(bmi);
      btm.tm_sec  = atoi(bse);
      btm.tm_wday = atoi(bda);

      etm.tm_hour = atoi(ehr);
      etm.tm_min  = atoi(emi);
      etm.tm_sec  = atoi(ese);
      etm.tm_wday = atoi(eda);

      account_calc(ch, &btm, &etm);

      return;
   }

#endif

   assert(IS_ADMINISTRATOR(ch));

   u = find_unit(ch, &c, nullptr, FIND_UNIT_SURRO | FIND_UNIT_WORLD);

   if((u == nullptr) || !IS_PC(u))
   {
      send_to_char("No such player found.\n\r", ch);
      return;
   }

   c = str_next_word(c, word);

   i = search_block(word, operations, TRUE);

   if(i == -1)
   {
      send_to_char("No such operation available.\n\r", ch);
      return;
   }

   c      = str_next_word(c, word);

   amount = atoi(word);

   if(is_in(i, 0, 2) != 0)
   {
      if(is_in(amount, 0, g_cAccountConfig.m_nMaxCharge) == 0)
      {
         send_to_char("Invalid amount.\n\r", ch);
         return;
      }
   }

   switch(i)
   {
      case 0: /* Insert amount   */
         account_local_stat(ch, u);

         sprintf(Buf, "\n\rInserting %.2f %s.\n\r\n\r", (float)amount / 100.0, g_cAccountConfig.m_pCoinName);
         send_to_char(Buf, ch);

         account_insert(ch, u, amount);

         account_local_stat(ch, u);

         sprintf(Buf, "%s inserted %.2f %s on your account.\n\r", UNIT_NAME(ch), (float)amount / 100.0, g_cAccountConfig.m_pCoinName);
         note                   = read_unit(letter_fi);
         UNIT_EXTRA_DESCR(note) = UNIT_EXTRA_DESCR(note)->add((char *)nullptr, Buf);
         unit_to_unit(note, u);
         break;

      case 1: /* Withdraw amount */
         account_local_stat(ch, u);

         sprintf(Buf, "\n\rWithdrawing %.2f %s.\n\r\n\r", ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);
         send_to_char(Buf, ch);

         account_withdraw(ch, u, amount);

         account_local_stat(ch, u);
         break;

      case 2: /* Change limit    */
         account_local_stat(ch, u);

         sprintf(Buf, "\n\rSetting limit to %.2f %s.\n\r\n\r", (float)amount / 100.0, g_cAccountConfig.m_pCoinName);

         send_to_char(Buf, ch);

         PC_ACCOUNT(u).credit_limit = amount;

         account_local_stat(ch, u);

         slog(LOG_ALL, 255, "%s changed limit of %s to %d.", UNIT_NAME(ch), UNIT_NAME(u), amount);
         account_log('L', ch, u, amount);
         break;

      case 3: /* Discount */
         if(is_in(amount, 0, 100) == 0)
         {
            send_to_char("Invalid discount, select 0% to 100%.\n\r", ch);
            return;
         }

         account_local_stat(ch, u);

         sprintf(Buf, "\n\rSetting discount to %3d%%.\n\r\n\r", amount);

         send_to_char(Buf, ch);

         PC_ACCOUNT(u).discount = amount;

         account_local_stat(ch, u);

         slog(LOG_ALL, 255, "%s changed discount of %s to %d.", UNIT_NAME(ch), UNIT_NAME(u), amount);
         account_log('D', ch, u, amount);
         break;

      case 4: /* Flatrate: account papi flatrate 30 add, 20 remove, etc. */
         if(is_in(amount, 1, 365) == 0)
         {
            send_to_char("Invalid number of days, select 1 to 365.\n\r", ch);
            return;
         }

         c = str_next_word(c, word);
         if(static_cast<unsigned int>(is_abbrev(word, "add")) != 0U)
         {
            ;
         }
         else if(static_cast<unsigned int>(is_abbrev(word, "remove")) != 0U)
         {
            amount = -amount;
         }
         else
         {
            send_to_char("You must either add or remove.\n\r", ch);
            return;
         }

         account_local_stat(ch, u);

         account_flatrate_change(ch, u, amount);

         account_local_stat(ch, u);
         break;

      default:
         abort();
   }
}

void account_defaults(unit_data *pc)
{
   PC_ACCOUNT(pc).credit       = g_cAccountConfig.m_nAccountFree;
   PC_ACCOUNT(pc).credit_limit = (int)g_cAccountConfig.m_nAccountLimit;
   PC_ACCOUNT(pc).total_credit = 0;
   PC_ACCOUNT(pc).last4        = -1;
   PC_ACCOUNT(pc).discount     = 0;
   PC_ACCOUNT(pc).cracks       = 0;
   PC_ACCOUNT(pc).flatrate     = 0;
}

void charge_sanity(uint8_t b_hr, uint8_t b_min, uint8_t e_hr, uint8_t e_min, int charge)
{
   if(charge < 0)
   {
      slog(LOG_ALL, 0, "Account error: Charge %d is < 0.", charge);
      exit(0);
   }

   if((b_hr > e_hr) || ((b_hr >= e_hr) && (b_min >= e_min)))
   {
      slog(LOG_ALL, 0, "Account error: %d:%d >= %d:%d.", b_hr, b_min, e_hr, e_min);
      exit(0);
   }

   if(b_min % MINUTE_GRANULARITY != 0)
   {
      slog(LOG_ALL,
           0,
           "Account error: Begin time (%d:%d), minute not "
           "divisible by %d.",
           b_hr,
           b_min,
           MINUTE_GRANULARITY);
      exit(0);
   }

   if(e_min % MINUTE_GRANULARITY != (MINUTE_GRANULARITY - 1))
   {
      slog(LOG_ALL,
           0,
           "Account error: End time (%d:%d), minute not "
           "at border to %d.",
           e_hr,
           e_min,
           MINUTE_GRANULARITY);
      exit(0);
   }
}

void numlist_to_charge(int *numlist, int len, int *day_charge)
{
   int i;
   int j;
   int start_idx;
   int end_idx;

   for(i = 0; i < len; i += 3)
   {
      start_idx = time_to_index(numlist[i + 1] / 100, numlist[i + 1] % 100);
      end_idx   = time_to_index(numlist[i + 2] / 100, numlist[i + 2] % 100);

      if(start_idx >= end_idx)
      {
         slog(LOG_ALL, 0, "Illegal time range %d - %d.", numlist[i + 1], numlist[i + 2]);
         exit(0);
      }

      if(end_idx >= TIME_GRANULARITY)
      {
         slog(LOG_ALL, 0, "Illegal time range %d - %d.", numlist[i + 1], numlist[i + 2]);
         exit(0);
      }

      for(j = start_idx; j <= end_idx; j++)
      {
         day_charge[j] = numlist[i + 0];
      }
   }
}

/* exits if something is wrong */
void numlist_sanity(int *numlist, int numlen)
{
   int i;

   if(numlist == nullptr)
   {
      return;
   }

   if(numlen % 3 != 0)
   {
      slog(LOG_ALL, 0, "Account range illegal length - not divisible by 3.");
      exit(0);
   }

   for(i = 0; i < numlen; i += 3)
   {
      charge_sanity(numlist[i + 1] / 100, numlist[i + 1] % 100, numlist[i + 2] / 100, numlist[i + 2] % 100, numlist[i + 0]);
   }
}

auto flatrate_sanity(int *numlist, int numlen) -> int
{
   if(numlist == nullptr)
   {
      return static_cast<int>(FALSE);
   }

   if(numlen != 2)
   {
      slog(LOG_ALL, 0, "Account flatrate illegal length, must be 2.");
      exit(0);
   }

   if(is_in(numlist[0], 30, 365) == 0)
   {
      slog(LOG_ALL, 0, "Account flatrate illegal days, must be 30..365.");
      exit(0);
   }

   if(is_in(numlist[1], 1, 1000000) == 0)
   {
      slog(LOG_ALL, 0, "Account flatrate illegal amount.");
      exit(0);
   }

   return static_cast<int>(TRUE);
}
