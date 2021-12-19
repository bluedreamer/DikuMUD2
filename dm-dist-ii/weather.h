#pragma once
#include "time_info_data.h"
#include "unit_data.h"

#include <ctime>

extern int          sunlight;

extern const time_t beginning_of_time;

auto                timetodate(time_t t) -> char *;
auto                real_time_passed(time_t t2, time_t t1) -> time_info_data;
auto                mud_date(time_t t) -> time_info_data;
auto                mud_time_passed(time_t t2, time_t t1) -> time_info_data;
auto                age(unit_data *ch) -> time_info_data;
void                weather_change(time_info_data time_data);
void                mudtime_strcpy(time_info_data *time, char *str);

void                update_time_and_weather();
void                boot_time_and_weather();
