#pragma once

#include "essential.h"
#include "flatrate_type.h"

#define MAX_FLATRATE 2

class CAccountConfig
{
public:
   CAccountConfig();

   void Boot();

   char *m_pCoinName;
   char *m_pOverdueMessage;
   char *m_pClosedMessage;
   char *m_pPaypointMessage;

   int m_nMaxCharge;
   int m_nMinCharge;
   int m_nFreeFromLevel;
   int m_nAccountFree;
   int m_nAccountLimit;
   int m_nHourlyRate;
   int m_bCreditCard;

   flatrate_type m_flatrate[MAX_FLATRATE]{};
};

extern CAccountConfig g_cAccountConfig;
