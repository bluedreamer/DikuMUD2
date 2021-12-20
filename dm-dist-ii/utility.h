#pragma once

#include "essential.h"
#include "unit_data.h"

auto number(int from, int to) -> int;
auto dice(int number, int size) -> int;

auto sprintbit(char *buf, uint32_t vektor, const char *names[]) -> char *;
auto sprinttype(char *buf, int type, const char *names[]) -> const char *;

/* in game log stuff below */

#define MAXLOG 10

#define HERE __FILE__, __LINE__

/* Use like this:  error(HERE, "Something went wrong: %s", buf); */
void error(const char *file, int line, const char *fmt, ...);

auto hometown_unit(char *str) -> unit_data *;

class log_buffer
{
public:
   log_buffer() { str[0] = 0; }

   char           str[MAX_INPUT_LENGTH + 50];
   enum log_level level;
   uint8_t        wizinv_level;
};

/* For the printing of obj_type information, as used by the identify spell and
 * wstat command.  Look in act_stat.c for details.
 */
class obj_type_t
{
public:
   const char *fmt;
   uint8_t     v[5];
};

auto stat_obj_data(unit_data *u, obj_type_t *obj_data) -> char *;
