#pragma once

#include "ban_t.h"
#include "descriptor_data.h"

#include <string>

class command_info;
class descriptor_data;
class unit_data;

class BanFile
{
public:
   enum class ban_type_e
   {
      NO_BAN        = '0',
      BAN_NEW_CHARS = 'n', /* if(until == 0) ban is forever, otherwise      */
      BAN_TOTAL     = 't'
   }; /* it will be lifted when time(0) passes "until" */
   const std::string BAN_FILE{"ban_list"};
   BanFile();
private:
   void save_ban();
   void load_ban();
   auto ban_timer(char *arg) -> time_t;
   auto site_banned(char *cur_site) -> char;
   void show_ban_text(char *site, descriptor_data *d);
   void add_ban(unit_data *ch, char *site, char type, time_t *until, char *textfile);
   void kill_entry(ban_t *entry);
   void del_ban(unit_data *ch, char *site);
   void show_site(unit_data *ch, ban_t *entry);
   void do_ban(unit_data *ch, char *arg, const command_info *cmd);
   auto ban_check(char *ban, char *site) -> bool;

   std::string filename_;
};
