#pragma once
#include "Mplex/mplex.h"

#include <cstdint>

void protocol_translate(cConHook *con, uint8_t code, char **b);
void translate_init();
