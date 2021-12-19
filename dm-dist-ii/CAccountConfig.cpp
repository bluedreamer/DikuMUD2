#include "CAccountConfig.h"
#include "CServerConfiguration.h"
#include "account.h"
#include "common.h"
#include "config.h"
#include "db.h"
#include "essential.h"
#include "files.h"
#include "str_parse.h"
#include "textutil.h"
#include <cstring>
#include <ctime>

CAccountConfig g_cAccountConfig;
extern int     next_crc;

CAccountConfig::CAccountConfig()
{
   m_pOverdueMessage  = nullptr;
   m_pClosedMessage   = nullptr;
   m_pPaypointMessage = nullptr;
   m_pCoinName        = nullptr;
   m_nFreeFromLevel   = 200;
   m_nAccountLimit    = 1500;
   m_nAccountFree     = 1000;
   m_nMaxCharge       = 10000;
   m_nMinCharge       = 1000;
   m_nHourlyRate      = 1000;
   m_bCreditCard      = 0;

   // MS2020: memset(&m_flatrate, sizeof(m_flatrate), 0); warning
   //  Looks like an error :-)
   memset(&m_flatrate, 0, sizeof(m_flatrate));
}

void CAccountConfig::Boot()
{
   char  Buf[2 * MAX_STRING_LENGTH];
   char *c;
   int   len;
   int   i;
   int   j;
   int  *numlist;
   FILE *f;

   if(g_cServerConfig.m_bAccounting == 0)
   {
      return;
   }

   slog(LOG_OFF, 0, "Booting account system.");

   if(static_cast<unsigned int>(file_exists(str_cc(libdir, ACCOUNT_LOG))) == 0U)
   {
      time_t now = time(nullptr);

      f = fopen(str_cc(libdir, ACCOUNT_LOG), "wb");

      if(f == nullptr)
      {
         slog(LOG_ALL, 0, "Can't create account log file.");
         exit(0);
      }

      slog(LOG_ALL, 0, "No account log existed - a new was created.");

      now ^= 0xAF876162;
      fprintf(f, "%08x%08x\n", (uint32_t)now, (uint32_t)now);

      fclose(f);
   }

   f = fopen_cache(str_cc(libdir, ACCOUNT_LOG), "rb");

   // MS2020: fscanf(f, "%*08x%08x", &next_crc); warning
   int m_err = fscanf(f, "%*08x%08x", &next_crc);

   touch_file(str_cc(libdir, ACCOUNT_FILE));

   config_file_to_string(str_cc(libdir, ACCOUNT_FILE), Buf, sizeof(Buf));

   c = Buf;

   m_pCoinName = parse_match_name(&c, "Coinage Name");

   if(m_pCoinName == nullptr)
   {
      slog(LOG_ALL, 0, "Error reading coin name.");
      exit(0);
   }

   m_pOverdueMessage = parse_match_name(&c, "Account Overdue");

   if(m_pOverdueMessage == nullptr)
   {
      slog(LOG_ALL, 0, "Error reading overdue message.");
      exit(0);
   }
   else
   {
      str_escape_format(m_pOverdueMessage, Buf, sizeof(Buf));
      free(m_pOverdueMessage);
      m_pOverdueMessage = str_dup((char *)Buf);
   }

   m_pClosedMessage = parse_match_name(&c, "Account Closed");
   if(m_pClosedMessage == nullptr)
   {
      slog(LOG_ALL, 0, "Error reading closed message.");
      exit(0);
   }
   else
   {
      str_escape_format(m_pClosedMessage, Buf, sizeof(Buf));
      free(m_pClosedMessage);
      m_pClosedMessage = str_dup((char *)Buf);
   }

   if(parse_match_num(&c, "Min Charge", &m_nMinCharge) == 0)
   {
      slog(LOG_ALL, 0, "Error reading 'Min Charge'.");
      exit(0);
   }

   if(parse_match_num(&c, "Max Charge", &m_nMaxCharge) == 0)
   {
      slog(LOG_ALL, 0, "Error reading 'Max Charge'.");
      exit(0);
   }

   if(parse_match_num(&c, "Charge Level", &m_nFreeFromLevel) == 0)
   {
      slog(LOG_ALL, 0, "Error reading 'Charge Level'.");
      exit(0);
   }

   if(parse_match_num(&c, "Account Free", &m_nAccountFree) == 0)
   {
      slog(LOG_ALL, 0, "Error reading account free.");
      exit(0);
   }

   if(parse_match_num(&c, "Account Limit", &m_nAccountLimit) == 0)
   {
      slog(LOG_ALL, 0, "Error reading account limit");
      exit(0);
   }

   if(parse_match_num(&c, "Credit Card", &m_bCreditCard) == 0)
   {
      slog(LOG_ALL, 0, "Error reading Credit Card usage.");
      exit(0);
   }

   if(parse_match_num(&c, "Base Charge", &m_nHourlyRate) == 0)
   {
      slog(LOG_ALL, 0, "Error reading base charge.");
      exit(0);
   }

   for(i = 0; i < 7; i++)
   {
      for(j = 0; j < TIME_GRANULARITY; j++)
      {
         day_charge[i][j] = m_nHourlyRate;
      }
   }

   numlist = parse_match_numlist(&c, "Base Range", &len);

   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      for(i = 0; i < 7; i++)
      {
         numlist_to_charge(numlist, len, day_charge[i]);
      }
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeSun", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[0]);
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeMon", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[1]);
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeTue", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[2]);
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeWed", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[3]);
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeThu", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[4]);
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeFri", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[5]);
      free(numlist);
   }

   numlist = parse_match_numlist(&c, "ChargeSat", &len);
   if(numlist != nullptr)
   {
      numlist_sanity(numlist, len);
      numlist_to_charge(numlist, len, day_charge[6]);
      free(numlist);
   }

   memset(m_flatrate, 0, sizeof(m_flatrate));

   for(i = 0; i < MAX_FLATRATE; i++)
   {
      sprintf(Buf, "Flatrate%d", i + 1);

      numlist = parse_match_numlist(&c, Buf, &len);
      if(numlist != nullptr)
      {
         if(flatrate_sanity(numlist, len) != 0)
         {
            m_flatrate[i].days  = numlist[0];
            m_flatrate[i].price = numlist[1];
         }

         free(numlist);

         strcat(Buf, " Message");

         m_flatrate[i].pMessage = parse_match_name(&c, Buf);
         if(m_flatrate[i].pMessage == nullptr)
         {
            slog(LOG_ALL, 0, "Error reading flatrate message.");
            exit(0);
         }
         else
         {
            str_escape_format(m_flatrate[i].pMessage, Buf, sizeof(Buf));
            free(m_flatrate[i].pMessage);
            m_flatrate[i].pMessage = str_dup((char *)Buf);
         }
      }
   }

   numlist = parse_match_numlist(&c, "Flatrate2", &len);
   if(numlist != nullptr)
   {
      if(flatrate_sanity(numlist, len) != 0)
      {
         m_flatrate[1].days  = numlist[0];
         m_flatrate[1].price = numlist[1];
      }

      free(numlist);
   }

   m_pPaypointMessage = parse_match_name(&c, "Account Paypoint");
   if(m_pPaypointMessage == nullptr)
   {
      slog(LOG_ALL, 0, "Error reading paypoint message.");
      exit(0);
   }
   else
   {
      str_escape_format(m_pPaypointMessage, Buf, sizeof(Buf));
      free(m_pPaypointMessage);
      m_pPaypointMessage = str_dup((char *)Buf);
   }
}
