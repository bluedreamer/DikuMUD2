#pragma once

#include "SFightColorSet.h"

#include <arpa/inet.h>

class CServerConfiguration
{
public:
   CServerConfiguration();

   void        Boot();

   auto        FromLAN(char *pFromHost) const -> int;
   auto        ValidMplex(struct sockaddr_in *isa) -> int;
   static void TripleColorFormat(struct SFightColorSet *pSet, char **ppList);

   int         m_nMotherPort;
   int         m_nRentModifier;
   int         m_bAccounting;
   int         m_bAliasShout;
   int         m_bBBS;
   int         m_bLawful;
   int         m_bNoSpecials;
   int         m_bBOB;
   int         m_nShout;

   struct
   {
      SFightColorSet sMeleeHit;
      SFightColorSet sMeleeMiss;
      SFightColorSet sMeleeNodam;
      SFightColorSet sMeleeShield;
      SFightColorSet sMeleeDeath;

      const char    *pDefault;
      const char    *pPrompt;
      const char    *pWhoTitle;
      const char    *pWhoElem;
      const char    *pRoomTitle;
      const char    *pRoomText;
      const char    *pExitsBegin;
      const char    *pExitsEnd;
   } m_sColor;

   struct in_addr m_sSubnetMask;
   struct in_addr m_sLocalhost;

   struct in_addr m_aMplexHosts[10];

   char          *m_pCredits; /* the Credits List                */
   char          *m_pNews;    /* the news                        */
   char          *m_pMotd;    /* the messages of today           */
   char          *m_pWizlist; /* the wizlist                     */
   char          *m_pLogo;    /* Intro screen                    */
   char          *m_pWelcome; /* Welcome Message                 */
   char          *m_pGoodbye; /* Goodbye message                 */
   char          *m_pNewbie;  /* Newbie message                 */
};

extern class CServerConfiguration g_cServerConfig;
