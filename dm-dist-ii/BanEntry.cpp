#include "BanEntry.h"

auto operator<<(std::ostream &os, const BanEntry &entry) -> std::ostream &
{
   os << entry.getSite() << ' ' << BanEntry::to_string(entry.getType()) << ' ' << entry.getUntil() << ' ' << entry.getTextfile();
   return os;
}

auto operator>>(std::istream &is, BanEntry &entry) -> std::istream &
{
   std::string site{};
   char        type{};
   time_t      until{0};
   std::string textfile{};

   is >> site >> type >> until >> textfile;
   if(is.good())
   {
      entry.setSite(site);
      entry.setType(BanEntry::from_string(type));
      entry.setUntil(until);
      entry.setTextfile(textfile);
   }

   return is;
}

auto BanEntry::getSite() const -> const std::string &
{
   return site_;
}

void BanEntry::setSite(const std::string &site)
{
   site_ = site;
   for(auto &c: site_)
   {
      c=std::tolower(c);
   }
}

auto BanEntry::getType() const -> BanEntry::ban_type_e
{
   return type_;
}

void BanEntry::setType(BanEntry::ban_type_e type)
{
   type_ = type;
}

auto BanEntry::getUntil() const -> time_t
{
   return until_;
}

void BanEntry::setUntil(time_t until)
{
   until_ = until;
}

auto BanEntry::getTextfile() const -> const std::string &
{
   return textfile_;
}

void BanEntry::setTextfile(const std::string &textfile)
{
   textfile_ = textfile;
}

auto BanEntry::to_string(BanEntry::ban_type_e e) -> char
{
   switch(e)
   {
      case ban_type_e::BAN_NEW_CHARS:
         return 'n';
      case ban_type_e::BAN_TOTAL:
         return 't';
      case ban_type_e::NO_BAN:
      default:
         return '0';
   }
}

auto BanEntry::from_string(char c) -> BanEntry::ban_type_e
{
   switch(c)
   {
      case 'n':
         return ban_type_e::BAN_NEW_CHARS;
      case 't':
         return ban_type_e::BAN_TOTAL;
      case '0':
      default:
         return ban_type_e::NO_BAN;
   }
}
