#pragma once

#include "descriptor_data.h"

auto site_banned(char *cur_site) -> char;
void show_ban_text(char *site, descriptor_data *d);

#define NO_BAN        '0'
#define BAN_NEW_CHARS 'n' /* if(until == 0) ban is forever, otherwise      */
#define BAN_TOTAL     't' /* it will be lifted when time(0) passes "until" */

#define BAN_FILE "ban_list"
