/* *********************************************************************** *
 * File   : account.h                                 Part of Valhalla MUD *
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

#pragma once

#define MAX_FLATRATE 2
#include "essential.h"
#include "structs.h"

struct flatrate_type
{
   char    *pMessage;
   uint16_t days;
   uint32_t price;
};

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

extern class CAccountConfig g_cAccountConfig;

void account_flatrate_change(unit_data *god, unit_data *whom, int32_t days);

void account_cclog(unit_data *ch, int amount);

void account_insert(unit_data *god, unit_data *whom, uint32_t amount);
void account_withdraw(unit_data *god, unit_data *whom, uint32_t amount);
void account_global_stat(const unit_data *ch);
void account_local_stat(const unit_data *ch, unit_data *u);

void account_defaults(unit_data *pc);

void account_subtract(unit_data *pc, time_t from, time_t to);

auto account_is_overdue(const unit_data *ch) -> int;
void account_overdue(const unit_data *ch);

void account_paypoint(unit_data *ch);
void account_closed(unit_data *ch);
auto account_is_closed(unit_data *ch) -> int;
