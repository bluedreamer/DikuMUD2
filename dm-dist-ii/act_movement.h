#pragma once
#include "door_data.h"
#include "unit_data.h"
auto locate_lock(unit_data *ch, char *arg) -> door_data *;
auto has_found_door(unit_data *pc, int dir) -> int;
