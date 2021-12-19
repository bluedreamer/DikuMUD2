#include "connectionlog.h"

#include "CAccountConfig.h"
#include "common.h"
#include "CServerConfiguration.h"
#include "db.h"
#include "essential.h"
#include "files.h"
#include "textutil.h"
#include "utils.h"

#include <cstdio>
#include <ctime>

static void ConnectionWrite(uint32_t nId, uint32_t nLogon, uint32_t nLogoff, uint32_t nLanPay)
{
   FILE *f = fopen_cache(str_cc(libdir, STATISTIC_FILE.c_str()), "ab+");

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

void ConnectionBegin(unit_data *pc)
{
   ConnectionWrite(PC_ID(pc), CHAR_DESCRIPTOR(pc)->logon, 0, 0);
}

void ConnectionLog(unit_data *pc)
{
   time_t   t0      = time(nullptr);

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
