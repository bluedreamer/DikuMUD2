#pragma once

#include "structs.h"

void enter_game(unit_data *ch);
void nanny_menu(descriptor_data *d, const char *arg);
void nanny_close(descriptor_data *d, const char *arg);
void set_descriptor_fptr(descriptor_data *d, void (*fptr)(descriptor_data *, const char *), bool call);
void nanny_get_name(descriptor_data *d, char *arg);
