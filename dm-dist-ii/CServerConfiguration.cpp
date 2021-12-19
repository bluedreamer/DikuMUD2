#include "CServerConfiguration.h"

#include "common.h"
#include "db.h"
#include "essential.h"
#include "files.h"
#include "str_parse.h"
#include "textutil.h"
#include <cstring>

CServerConfiguration g_cServerConfig;
CServerConfiguration::CServerConfiguration()
{
   m_bAccounting   = 0;
   m_bBBS          = static_cast<int>(FALSE);
   m_bLawful       = static_cast<int>(FALSE);
   m_bAliasShout   = static_cast<int>(TRUE);
   m_bNoSpecials   = static_cast<int>(FALSE);
   m_bBOB          = static_cast<int>(FALSE);
   m_nShout        = 1;
   m_nRentModifier = 10;
   m_nMotherPort   = 4999;

   // MS2020 memset(&m_sSubnetMask, sizeof(m_sSubnetMask), 0);
   // MS2020 memset(&m_sLocalhost, sizeof(m_sLocalhost), 0);
   memset(&m_sSubnetMask, 0, sizeof(m_sSubnetMask));
   memset(&m_sLocalhost, 0, sizeof(m_sLocalhost));

   m_pCredits = nullptr;
   m_pNews    = nullptr;
   m_pMotd    = nullptr;
   m_pWizlist = nullptr;
   m_pLogo    = nullptr;
   m_pWelcome = nullptr;
   m_pGoodbye = nullptr;
   m_pNewbie  = nullptr;
}

auto CServerConfiguration::FromLAN(char *pFromHost) const -> int
{
   struct in_addr sTmp;

   if(inet_aton(pFromHost, &sTmp) == 0)
   {
      slog(LOG_ALL, 0, "Localhost invalid.");
      return static_cast<int>(FALSE);
   }

   return static_cast<int>((m_sSubnetMask.s_addr & m_sLocalhost.s_addr) == (m_sSubnetMask.s_addr & sTmp.s_addr));
}

auto CServerConfiguration::ValidMplex(struct sockaddr_in *isa) -> int
{
   struct in_addr sTmp;

   int i;

   for(i = 0; i < 10; i++)
   {
      if(isa->sin_addr.s_addr == m_aMplexHosts[i].s_addr)
      {
         return static_cast<int>(TRUE);
      }
   }

   return static_cast<int>(FALSE);
}

void CServerConfiguration::TripleColorFormat(struct SFightColorSet *pSet, char **ppList)
{
   if(ppList == nullptr)
   {
      slog(LOG_ALL, 0, "Color triple non existant.");
      exit(0);
   }

   int i = 0;

   while(ppList[i] != nullptr)
   {
      i++;
   }

   if(i != 3)
   {
      slog(LOG_ALL, 0, "Color triple does not have three elements.");
      exit(0);
   }

   pSet->pAttacker = str_escape_format(ppList[0]);
   pSet->pDefender = str_escape_format(ppList[1]);
   pSet->pOthers   = str_escape_format(ppList[2]);

   free_namelist(ppList);
}

void CServerConfiguration::Boot()
{
   char        Buf[2 * MAX_STRING_LENGTH];
   char       *c;
   char      **list;
   const char *d;
   int         i;

   slog(LOG_OFF, 0, "Booting server.");

   if(static_cast<unsigned int>(file_exists(str_cc(libdir, SERVER_CONFIG))) == 0U)
   {
      slog(LOG_ALL, 0, "No server configuration file.");
      return;
   }

   config_file_to_string(str_cc(libdir, SERVER_CONFIG), Buf, sizeof(Buf));

   c = Buf;

   if(parse_match_num(&c, "Port", &i) != 0)
   {
      m_nMotherPort = i;
   }

   if(is_in(m_nMotherPort, 2000, 8000) == 0)
   {
      slog(LOG_ALL, 0, "Mother port not in [2000..8000].");
      exit(0);
   }

   if(parse_match_num(&c, "Rent", &i) != 0)
   {
      m_nRentModifier = i;
   }

   if(is_in(m_nRentModifier, 0, 100) == 0)
   {
      slog(LOG_ALL, 0, "Rent modifier not in [0..100].");
      exit(0);
   }

   if(parse_match_num(&c, "BOB", &i) != 0)
   {
      m_bBOB = i;
   }

   if(is_in(m_bBOB, 0, 1) == 0)
   {
      slog(LOG_ALL, 0, "BOB not 0 or 1");
      exit(0);
   }

   if(parse_match_num(&c, "Alias Shout", &i) != 0)
   {
      m_bAliasShout = i;
   }

   if(is_in(m_bAliasShout, 0, 1) == 0)
   {
      slog(LOG_ALL, 0, "Alias Shout not 0 or 1");
      exit(0);
   }

   if(parse_match_num(&c, "No Specials", &i) != 0)
   {
      m_bNoSpecials = i;
   }

   if(is_in(m_bNoSpecials, 0, 1) == 0)
   {
      slog(LOG_ALL, 0, "Specials not 0 or 1");
      exit(0);
   }

   if(parse_match_num(&c, "Lawful", &i) != 0)
   {
      m_bLawful = i;
   }

   if(is_in(m_bLawful, 0, 1) == 0)
   {
      slog(LOG_ALL, 0, "Lawful not 0 or 1");
      exit(0);
   }

   if(parse_match_num(&c, "BBS", &i) != 0)
   {
      m_bBBS = i;
   }

   if(is_in(m_bBBS, 0, 1) == 0)
   {
      slog(LOG_ALL, 0, "BBS not 0 or 1");
      exit(0);
   }

   if(parse_match_num(&c, "Accounting", &i) != 0)
   {
      m_bAccounting = i;
   }

   if(is_in(m_bAccounting, 0, 1) == 0)
   {
      slog(LOG_ALL, 0, "Accounting not 0 or 1");
      exit(0);
   }

   if(parse_match_num(&c, "Shout", &i) != 0)
   {
      m_nShout = i;
   }

   if(is_in(m_nShout, 0, 255) == 0)
   {
      slog(LOG_ALL, 0, "Shout level not [0..255]");
      exit(0);
   }

   d = parse_match_name(&c, "subnetmask");

   if(d == nullptr)
   {
      d = "255.255.255.255";
   }

   if(inet_aton(d, &m_sSubnetMask) == 0)
   {
      slog(LOG_ALL, 0, "SubnetMask invalid.");
      exit(0);
   }

   d = parse_match_name(&c, "localhost");

   if(d == nullptr)
   {
      d = "127.0.0.1";
   }

   if(inet_aton(d, &m_sLocalhost) == 0)
   {
      slog(LOG_ALL, 0, "Localhost invalid.");
      exit(0);
   }

   char **ppNames;

   ppNames = parse_match_namelist(&c, "mplex hosts");

   if(ppNames == nullptr)
   {
      slog(LOG_ALL, 0, "Mplex hosts must be specified.");
      exit(0);
   }

   int l = 0;

   while(ppNames[l] != nullptr)
   {
      l++;
   }

   if(l < 1)
   {
      slog(LOG_ALL, 0, "Mplex hosts must have at least one entry.");
      exit(0);
   }

   if(l > 10)
   {
      slog(LOG_ALL, 0, "10 is maximum number of mplex hosts.");
      exit(0);
   }

   for(i = 0; i < 10; i++)
   {
      if(i < l)
      {
         if(inet_aton(ppNames[i], &m_aMplexHosts[i]) == 0)
         {
            slog(LOG_ALL, 0, "Mplex host invalid IP.");
            exit(0);
         }
      }
      else
      {
         m_aMplexHosts[i] = m_aMplexHosts[i - 1];
      }
   }

   m_sColor.pDefault = str_escape_format("&cw");

   d                = parse_match_name(&c, "color prompt");
   m_sColor.pPrompt = d != nullptr ? str_escape_format(d) : "";
   free(d);

   d                  = parse_match_name(&c, "color who");
   m_sColor.pWhoTitle = d != nullptr ? str_escape_format(d) : "";
   free(d);

   d                 = parse_match_name(&c, "color who elem");
   m_sColor.pWhoElem = d != nullptr ? str_escape_format(d) : "";
   free(d);

   d                   = parse_match_name(&c, "color room title");
   m_sColor.pRoomTitle = d != nullptr ? str_escape_format(d) : "";
   free(d);

   d                  = parse_match_name(&c, "color room text");
   m_sColor.pRoomText = d != nullptr ? str_escape_format(d) : "";
   free(d);

   d                    = parse_match_name(&c, "color exits begin");
   m_sColor.pExitsBegin = d != nullptr ? str_escape_format(d) : "";
   free(d);

   d                  = parse_match_name(&c, "color exits end");
   m_sColor.pExitsEnd = d != nullptr ? str_escape_format(d) : "";
   free(d);

   /* MELEE STUFF */

   list = parse_match_namelist(&c, "color melee hit");
   TripleColorFormat(&m_sColor.sMeleeHit, list);

   list = parse_match_namelist(&c, "color melee miss");
   TripleColorFormat(&m_sColor.sMeleeMiss, list);

   list = parse_match_namelist(&c, "color melee nodam");
   TripleColorFormat(&m_sColor.sMeleeNodam, list);

   list = parse_match_namelist(&c, "color melee shield");
   TripleColorFormat(&m_sColor.sMeleeShield, list);

   list = parse_match_namelist(&c, "color melee death");
   TripleColorFormat(&m_sColor.sMeleeDeath, list);

   /* News files, etc. */

   slog(LOG_OFF, 0, "Reading newsfile, credits, wizlist and motd.");

   auto read_info_file(char *name, char *oldstr)->char *;

   touch_file(str_cc(libdir, WIZLIST_FILE));
   m_pWizlist = read_info_file(str_cc(libdir, WIZLIST_FILE), m_pWizlist);

   touch_file(str_cc(libdir, NEWS_FILE));
   m_pNews = read_info_file(str_cc(libdir, NEWS_FILE), m_pNews);

   touch_file(str_cc(libdir, CREDITS_FILE));
   m_pCredits = read_info_file(str_cc(libdir, CREDITS_FILE), m_pCredits);

   touch_file(str_cc(libdir, MOTD_FILE));
   m_pMotd = read_info_file(str_cc(libdir, MOTD_FILE), m_pMotd);

   touch_file(str_cc(libdir, LOGO_FILE));
   m_pLogo = read_info_file(str_cc(libdir, LOGO_FILE), m_pLogo);

   touch_file(str_cc(libdir, WELCOME_FILE));
   m_pWelcome = read_info_file(str_cc(libdir, WELCOME_FILE), m_pWelcome);

   touch_file(str_cc(libdir, GOODBYE_FILE));
   m_pGoodbye = read_info_file(str_cc(libdir, GOODBYE_FILE), m_pGoodbye);

   touch_file(str_cc(libdir, NEWBIE_FILE));
   m_pNewbie = read_info_file(str_cc(libdir, NEWBIE_FILE), m_pNewbie);
}
