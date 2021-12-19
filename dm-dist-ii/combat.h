#pragma once

#include "unit_data.h"

void set_fighting(unit_data *ch, unit_data *vict, int bMelee = static_cast<int>(FALSE));
void stop_fighting(unit_data *ch, unit_data *victim = nullptr);

void stat_combat(const unit_data *ch, unit_data *u);
