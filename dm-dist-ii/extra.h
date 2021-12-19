#pragma once
#include "essential.h"
#include "extra_descr_data.h"
#include "unit_data.h"

auto unit_find_extra_string(unit_data *ch, char *word, unit_data *list) -> const char *;
auto char_unit_find_extra(unit_data *ch, unit_data **target, char *word, unit_data *list) -> extra_descr_data *;
auto unit_find_extra(const char *word, unit_data *unit) -> extra_descr_data *;
