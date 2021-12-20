#pragma once

#include "BanEntry.h"
#include "descriptor_data.h"

#include <string>
#include <map>

class command_info;
class descriptor_data;
class unit_data;

class BanFile
{
public:

   const std::string BAN_FILE{"ban_list"};
   BanFile();

private:
   void save_ban();
   void load_ban();
   auto ban_timer(char *arg) -> time_t;
   auto site_banned(char *cur_site) -> char;
   void show_ban_text(char *site, descriptor_data *d);
   void add_ban(unit_data *ch, const std::string &site, char type, time_t until, const std::string &textfile);
   void kill_entry(const BanEntry &entry);
   void del_ban(unit_data *ch, char *site);
   void show_site(unit_data *ch, BanEntry *entry);
   void do_ban(unit_data *ch, char *arg, const command_info *cmd);
   auto ban_check(char *ban, char *site) -> bool;

   std::string filename_;
   std::map<std::string, BanEntry> bans_;
};

#if 0
auto site_banned(char *cur_site) -> char;
void show_ban_text(char *site, descriptor_data *d);

   #define NO_BAN        '0'
   #define BAN_NEW_CHARS 'n' /* if(until == 0) ban is forever, otherwise      */
   #define BAN_TOTAL     't' /* it will be lifted when time(0) passes "until" */
#endif
