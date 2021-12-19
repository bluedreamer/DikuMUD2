#pragma once

#include "descriptor_data.h"
#include "diltemplate.h"

auto find_descriptor(const char *name, descriptor_data *except) -> descriptor_data *;
auto find_zone(const char *zonename) -> struct zone_type *;
auto find_file_index(const char *zonename, const char *name) -> file_index_type *;
auto find_dil_index(char *zonename, char *name) -> diltemplate *;
auto world_room(const char *zone, const char *name) -> unit_data *;
auto find_dil_template(const char *name) -> diltemplate *;

auto str_to_file_index(const char *str) -> file_index_type *;
auto pc_str_to_file_index(const unit_data *ch, const char *str) -> file_index_type *;
