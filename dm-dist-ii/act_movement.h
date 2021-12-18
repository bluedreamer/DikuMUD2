#pragma once
#include "unit_data.h"
auto locate_lock(unit_data *ch, char *arg) -> struct door_data *;
auto has_found_door(unit_data *pc, int dir) -> int;
