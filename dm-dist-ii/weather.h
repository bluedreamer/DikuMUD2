#pragma once
/* *********************************************************************** *
 * File   : weather.h                                 Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : noop@diku.dk                                                   *
 *                                                                         *
 * Purpose:  Weather handling.                                             *
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

#include "time_info_data.h"
#include "unit_data.h"
#include <ctime>

extern int sunlight;

extern const time_t beginning_of_time;

auto timetodate(time_t t) -> char *;
auto real_time_passed(time_t t2, time_t t1) -> time_info_data;
auto mud_date(time_t t) -> time_info_data;
auto mud_time_passed(time_t t2, time_t t1) -> time_info_data;
auto age(unit_data *ch) -> time_info_data;
void weather_change(time_info_data time_data);
void mudtime_strcpy(time_info_data *time, char *str);

void update_time_and_weather();
void boot_time_and_weather();
