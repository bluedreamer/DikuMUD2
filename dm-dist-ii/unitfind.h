#pragma once
#include "unit_data.h"

auto same_surroundings(unit_data *u1, unit_data *u2) -> bool;
void scan4_unit_room(unit_data *room, uint8_t type);
void scan4_unit(unit_data *ch, uint8_t type);
auto scan4_ref(unit_data *ch, unit_data *fu) -> unit_data *;

auto random_direction(unit_data *ch) -> int;
auto find_unit(const unit_data *ch, char **arg, const unit_data *list, uint32_t bitvector) -> unit_data *;
auto find_unit_general(const unit_data *viewer, const unit_data *ch, char **arg, const unit_data *list, uint32_t bitvector) -> unit_data *;

auto find_symbolic_instance(file_index_type *fi) -> unit_data *;
auto find_symbolic(char *zone, char *name) -> unit_data *;
auto find_symbolic_instance_ref(unit_data *ref, file_index_type *fi, uint16_t bitvector) -> unit_data *;
auto random_unit(unit_data *ref, int sflags, int tflags) -> unit_data *;
