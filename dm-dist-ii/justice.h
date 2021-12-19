#pragma once
#include "unit_data.h"

#define CRIME_LIFE 12 /* Remember crime for (5 mins * CRIME_LIFE)    */

#define CRIME_NONPRO 8  /* Person will be non-protected                */
#define CRIME_OUTLAW 32 /* Person is wanted                            */
#define CRIME_REWARD 64 /* Person will be wanted dead for reward.      */

/* STRUCTS ---------------------------------------------------------------*/

void offend_legal_state(unit_data *ch, unit_data *victim);
void log_crime(unit_data *criminal, unit_data *victim, uint8_t crime_type, bool active = true);
void set_reward_char(unit_data *ch, int crimes);
