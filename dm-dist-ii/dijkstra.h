#pragma once

#include "unit_data.h"

auto npc_move(const unit_data *npc, const unit_data *to) -> int;
auto move_to(const unit_data *from, const unit_data *to) -> int;
