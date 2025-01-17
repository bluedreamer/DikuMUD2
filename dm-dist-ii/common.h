#pragma once

#include "essential.h"
#include "unit_data.h"
#include <string>

extern const char *fillwords[];

extern const char *drinks[];
extern const char *dirs[];

extern const std::string  libdir;

auto               required_xp(int level) -> int;
auto               level_xp(int level) -> int;

auto               is_in(int a, int from, int to) -> int;
void               set_hits(unit_data *obj, int craftsmanship);

auto               skill_point_gain() -> int;
auto               ability_point_gain() -> int;
auto               ability_point_total(int level) -> int;
auto               skill_point_total(int level) -> int;
auto               damage_str(int strength) -> double;

auto               distribute_points(uint8_t *skills, int max, int points, int level) -> int;
auto               buy_points(int points, int level, int *error) -> int;

auto               apply_quality(int num, int quality) -> int;

auto               hitpoint_total(int hpp) -> int;

void               set_weapon(unit_data *);
void               set_shield(unit_data *);
void               set_armour(unit_data *);

/* ..................................................................... */
/*                           A B I L I T I E S                           */
/* ..................................................................... */

#define SKILL_MAX            250 /* Maximum of any skill / ability */
#define AVERAGE_SKILL_COST   10
#define HITPOINTS_BASE       125
#define ABILITY_POINT_FACTOR (4) /* Abilitypoints multiplied by this */
#define SKILL_POINT_FACTOR   (8) /* Skillpoints   multiplied by this */
