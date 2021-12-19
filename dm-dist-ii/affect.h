#pragma once

#include "essential.h"
#include "structs.h"
#include "unit_data.h"

void apply_affect(unit_data *unit);
void start_affect(unit_data *unit);
void stop_affect(unit_data *unit);
auto affected_by_spell(const unit_data *unit, int16_t id) -> unit_affected_type *;
void unlink_affect(unit_data *u, unit_affected_type *af);

void create_affect(unit_data *unit, unit_affected_type *orgaf);
void destroy_affect(unit_affected_type *af);
void affect_clear_unit(unit_data *unit);
