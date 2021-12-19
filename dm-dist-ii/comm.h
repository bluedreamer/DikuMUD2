#pragma once
#include "descriptor_data.h"
#include "unit_data.h"

void page_string(descriptor_data *d, const char *);

void send_to_outdoor(const char *messg);
void send_to_zone_outdoor(const struct zone_type *z, const char *messg);
void send_to_descriptor(const char *messg, descriptor_data *d);
void send_to_all(const char *messg);
void send_to_char(const char *messg, const unit_data *ch);
void send_to_room(const char *messg, unit_data *room);

/*  Please note that act() does NOT accept TRUE or FALSE as second argument
 *  anymore...
 */
void act(const char *str, int hide_invisible, const void *arg1, const void *arg2, const void *arg3, int type);
void act_generate(char *buf, const char *str, int show_type, const void *arg1, const void *arg2, const void *arg3, int type, unit_data *to);

auto process_output(descriptor_data *t) -> int;
auto process_input(descriptor_data *t) -> int;

#define PROMPT_SENT   0
#define PROMPT_EXPECT 1
#define PROMPT_IGNORE 2
