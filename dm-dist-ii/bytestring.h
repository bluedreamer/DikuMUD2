#pragma once

#include "essential.h"

auto bread_uint8_t(uint8_t **buf) -> uint8_t;
auto bread_uint16_t(uint8_t **buf) -> uint16_t;
auto bread_uint32_t(uint8_t **buf) -> uint32_t;
auto bread_float(uint8_t **buf) -> float;
auto bread_data(uint8_t **b, uint32_t *len) -> uint8_t *;
void bwrite_data(uint8_t **b, uint8_t *data, uint32_t len);
void bread_strcpy(uint8_t **b, char *str);
auto bread_str_alloc(uint8_t **buf) -> char *;
auto bread_str_skip(uint8_t **b) -> char *;
auto bread_nameblock(uint8_t **b) -> char **;

void bwrite_uint8_t(uint8_t **b, uint8_t i);
void bwrite_uint16_t(uint8_t **b, uint16_t i);
void bwrite_uint32_t(uint8_t **b, uint32_t i);
void bwrite_float(uint8_t **b, float f);
void bwrite_string(uint8_t **b, const char *str);
void bwrite_double_string(uint8_t **b, char *str);
void bwrite_nameblock(uint8_t **b, char **nb);
