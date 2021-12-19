#pragma once
#include "essential.h"
#include "structs.h"
#include "unit_data.h"

#define MINUTE_GRANULARITY (15)
#define TIME_GRANULARITY   ((24 * 60) / MINUTE_GRANULARITY)
extern int32_t day_charge[7][TIME_GRANULARITY];

auto           account_is_closed(unit_data *ch) -> int;
auto           account_is_overdue(const unit_data *ch) -> int;
auto           flatrate_sanity(int *numlist, int numlen) -> int;
void           account_cclog(unit_data *ch, int amount);
void           account_closed(unit_data *ch);
void           account_defaults(unit_data *pc);
void           account_flatrate_change(unit_data *god, unit_data *whom, int32_t days);
void           account_global_stat(const unit_data *ch);
void           account_insert(unit_data *god, unit_data *whom, uint32_t amount);
void           account_local_stat(const unit_data *ch, unit_data *u);
void           account_overdue(const unit_data *ch);
void           account_paypoint(unit_data *ch);
void           account_subtract(unit_data *pc, time_t from, time_t to);
void           account_withdraw(unit_data *god, unit_data *whom, uint32_t amount);
void           numlist_sanity(int *numlist, int numlen);
void           numlist_to_charge(int *numlist, int len, int *day_charge);
