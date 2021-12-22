#pragma once

void  stat_bank(const struct unit_data *ch, struct unit_data *u);
char *in_string(struct unit_data *ch, struct unit_data *u);
int   pay_point_charlie(struct unit_data *ch, struct unit_data *to);
void  tax_player(struct unit_data *ch);
int   lose_exp(struct unit_data *ch);
