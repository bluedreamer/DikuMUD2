#pragma once

#include <ctime>
#include <iostream>
#include <string>

class BanEntry
{
public:
   enum class ban_type_e
   {
      NO_BAN        = '0',
      BAN_NEW_CHARS = 'n', /* if(until == 0) ban is forever, otherwise      */
      BAN_TOTAL     = 't'  /* it will be lifted when time(0) passes "until" */
   };

   [[nodiscard]] auto getType() const -> ban_type_e;
   void               setType(ban_type_e type);
   [[nodiscard]] auto getUntil() const -> time_t;
   void               setUntil(time_t until);
   [[nodiscard]] auto getTextfile() const -> const std::string &;
   void               setTextfile(const std::string &textfile);
   [[nodiscard]] auto getSite() const -> const std::string &;
   void               setSite(const std::string &site);

   static auto to_string(ban_type_e e) -> char;
   static auto from_string(char c) -> ban_type_e;
private:
   std::string     site_{};
   enum ban_type_e type_
   {
      ban_type_e::NO_BAN
   };
   time_t      until_{0};
   std::string textfile_{};
};

auto operator<<(std::ostream &os, const BanEntry &entry) -> std::ostream &;
auto operator>>(std::istream &is, BanEntry &entry) -> std::istream &;
