#pragma once
#include "unit_data.h"
#include "diltemplate.h"
#include <cstdint>

struct command_info
{
   uint8_t combat_speed;  /* The speed of a combat command         */
   uint8_t combat_buffer; /* Use the combat speed / buffer system? */

   const char *cmd_str;

   int32_t no;

   uint8_t minimum_position;

   void (*cmd_fptr)(unit_data *ch, char *arg, const struct command_info *c);

   uint8_t minimum_level;
   uint8_t log_level; /* For logging certain immortal commands */

   diltemplate *tmpl; /* Perhaps a DIL template...         */

   /* Also put noise/provokation and impact values here            */

   /* Example: if ((cmd->noise > 50) || (cmd->impact > 7))         */
   /*          { wake(dragon); .... }                              */
   /* Example: if (cmd->provokation < 0) { do_kiss(maid, ch); .. } */
};

extern command_info  cmd_auto_enter;
extern command_info  cmd_auto_tick;
extern command_info  cmd_auto_extract;
extern command_info  cmd_auto_death;
extern command_info *cmd_follow;
extern command_info  cmd_auto_combat;
extern command_info  cmd_auto_unknown;
extern command_info  cmd_auto_save;
extern command_info  cmd_auto_msg;
extern command_info  cmd_auto_damage;
extern command_info  cmd_a_social;
