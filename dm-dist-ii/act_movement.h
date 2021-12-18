#pragma once
auto locate_lock(struct unit_data *ch, char *arg) -> struct door_data *;
auto has_found_door(struct unit_data *pc, int dir) -> int;
