/* *********************************************************************** *
 * File   : connectionlog.c                           Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Connection log.                                                *
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

#include <cstdio>
#include <ctime>

#include "essential.h"
#include "utils.h"
#include "values.h"

#include "account.h"
#include "common.h"
#include "connectionlog.h"
#include "db.h"
#include "files.h"
#include "textutil.h"

static void ConnectionWrite(uint32_t nId, uint32_t nLogon, uint32_t nLogoff, uint32_t nLanPay)
{
   FILE *f = fopen_cache(str_cc(libdir, STATISTIC_FILE), "ab+");

   assert(f);

   uint8_t  buf[40];
   uint8_t *b = buf;

   bwrite_uint32_t(&b, nId);
   bwrite_uint32_t(&b, nLogon);
   bwrite_uint32_t(&b, nLogoff);
   bwrite_uint32_t(&b, nLanPay);

   fwrite(buf, sizeof(uint8_t), b - buf, f);

   fflush(f);
}

void ConnectionBegin(class unit_data *pc)
{
   ConnectionWrite(PC_ID(pc), CHAR_DESCRIPTOR(pc)->logon, 0, 0);
}

void ConnectionLog(class unit_data *pc)
{
   time_t t0 = time(nullptr);

   uint32_t nLanPay = 0;

   if(g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(pc)->host) != 0)
   {
      nLanPay |= 0x0001;
   }

   if(g_cServerConfig.m_bAccounting != 0)
   {
      nLanPay |= 0x0002;
   }

   if(CHAR_LEVEL(pc) < g_cAccountConfig.m_nFreeFromLevel)
   {
      nLanPay |= 0x0004;
   }

   if(PC_ACCOUNT(pc).flatrate > (uint32_t)time(nullptr))
   {
      nLanPay |= 0x0008;
   }

   if(PC_ACCOUNT(pc).discount >= 100)
   {
      nLanPay |= 0x0010;
   }

   if(PC_ACCOUNT(pc).credit > 0.0)
   {
      nLanPay |= 0x0020;
   }

   ConnectionWrite(PC_ID(pc), CHAR_DESCRIPTOR(pc)->logon, t0, nLanPay);
}

void ConnectionBoot()
{
   touch_file(str_cc(libdir, STATISTIC_FILE));
   ConnectionWrite(0xFFFFFFFF, time(nullptr), 0xFFFFFFFF, 0xFFFFFFFF);
}
