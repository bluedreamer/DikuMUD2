#pragma once

#include "structs.h"

void enter_game(struct unit_data *ch);
void nanny_menu(struct descriptor_data *d, const char *arg);
void nanny_close(struct descriptor_data *d, const char *arg);
void set_descriptor_fptr(struct descriptor_data *d, void (*fptr)(struct descriptor_data *, const char *), ubit1 call);
