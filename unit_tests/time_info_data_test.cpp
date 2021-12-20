#include "../dm-dist-ii/constants.h"
#include "../dm-dist-ii/structs.h"
#include "../dm-dist-ii/weather.h"
#include <boost/test/unit_test.hpp>
#include <vector>

struct Fixture
{
   Fixture()
   {
      populate_days();
      populate_months();
   }

   static void LogTimeInfoData(const time_info_data &result)
   {
      BOOST_TEST_MESSAGE("hours=" << (int)result.hours << " day=" << (int)result.day << " month=" << (int)result.month
                                  << " year=" << (int)result.year);
   }
   void populate_months()
   {
      months.push_back("Month of Winter");
      months.push_back("Month of the Winter Wolf");
      months.push_back("Month of the Frost Giant");
      months.push_back("Month of the Spring");
      months.push_back("Month of Futility");
      months.push_back("Month of the Sun");
      months.push_back("Month of the Heat");
      months.push_back("Month of the Long Shadows");
      months.push_back("Month of the Ancient Darkness");
   }

   void populate_days()
   {
      days.push_back("the Day of the Moon");
      days.push_back("the Day of the Bull");
      days.push_back("the Day of the Deception");
      days.push_back("the Day of Thunder");
      days.push_back("the Day of Freedom");
      days.push_back("the Day of the Great Gods");
      days.push_back("the Day of the Sun");
   }
   auto day_prefix(int day) -> std::string
   {
      if(day == 1)
      {
         return "st";
      }
      else if(day == 2)
      {
         return "nd";
      }
      else if(day == 3)
      {
         return "rd";
      }
      else if(day < 20)
      {
         return "th";
      }
      else if((day % 10) == 1)
      {
         return "st";
      }
      else if((day % 10) == 2)
      {
         return "nd";
      }
      else if((day % 10) == 3)
      {
         return "rd";
      }
      return "th";
   }
   std::vector<std::string> days;
   std::vector<std::string> months;
};

BOOST_AUTO_TEST_CASE(time_info_data_defines)
{
   BOOST_TEST(SECS_PER_REAL_MIN == 60);
   BOOST_TEST(SECS_PER_REAL_HOUR == 3600);
   BOOST_TEST(SECS_PER_REAL_DAY == 86400);
   BOOST_TEST(SECS_PER_REAL_YEAR == 31536000);

   BOOST_TEST(MUD_DAY == 24);
   BOOST_TEST(MUD_WEEK == 7);
   BOOST_TEST(MUD_MONTH == 14);
   BOOST_TEST(MUD_YEAR == 9);

   BOOST_TEST(SECS_PER_MUD_HOUR == 300);
   BOOST_TEST(SECS_PER_MUD_DAY == 7200);
   BOOST_TEST(SECS_PER_MUD_MONTH == 100800);
   BOOST_TEST(SECS_PER_MUD_YEAR == 907200);

   BOOST_TEST(beginning_of_time == 650336715);
}

BOOST_AUTO_TEST_CASE(time_info_data_constants_tests)
{
   Fixture f;
   for(auto i = 0; i < MUD_WEEK; ++i)
   {
      BOOST_TEST(std::string(weekdays[i]) == f.days[i]);
   }
}

BOOST_AUTO_TEST_CASE(real_time_passed_test)
{
   time_t now         = time(nullptr);
   time_t hour_future = now + (60 * 60);
   time_t day_future  = now + (60 * 60 * 24);
   time_t year_future = now + (60 * 60 * 24 * 365);

   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = real_time_passed(now, now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == -1);
      BOOST_TEST(result.year == -1);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = real_time_passed(now, hour_future);
      BOOST_TEST(result.hours == -1);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == -1);
      BOOST_TEST(result.year == -1);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = real_time_passed(hour_future, now);
      BOOST_TEST(result.hours == 1);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == -1);
      BOOST_TEST(result.year == -1);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = real_time_passed(now, day_future);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == -1);
      BOOST_TEST(result.month == -1);
      BOOST_TEST(result.year == -1);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = real_time_passed(day_future, now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 1);
      BOOST_TEST(result.month == -1);
      BOOST_TEST(result.year == -1);
   }
}

BOOST_AUTO_TEST_CASE(mud_time_passed_test)
{
   time_t now          = time(nullptr);
   time_t hour_future  = now + (60 * 60);
   time_t day_future   = now + (60 * 60 * 24);
   time_t month_future = now + (60 * 60 * 24 * 30);
   time_t year_future  = now + (60 * 60 * 24 * 365);

   //   BOOST_TEST_MESSAGE("hours=" << (int)result.hours << " day=" << (int)result.day << " month=" << (int)result.month
   //                               << " year=" << (int)result.year);
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(now, now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == 0);
      BOOST_TEST(result.year == 0);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(now, hour_future);
      BOOST_TEST(result.hours == -12);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == 0);
      BOOST_TEST(result.year == 0);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(hour_future, now);
      BOOST_TEST(result.hours == 12);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == 0);
      BOOST_TEST(result.year == 0);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(now, day_future);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == -12);
      BOOST_TEST(result.month == 0);
      BOOST_TEST(result.year == 0);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(day_future, now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 12);
      BOOST_TEST(result.month == 0);
      BOOST_TEST(result.year == 0);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(now, month_future);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == -10);
      BOOST_TEST(result.month == -7);
      BOOST_TEST(result.year == -2);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(month_future, now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 10);
      BOOST_TEST(result.month == 7);
      BOOST_TEST(result.year == 2);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(now, year_future);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == -12);
      BOOST_TEST(result.month == -6);
      BOOST_TEST(result.year == -34);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   {
      time_info_data result = mud_time_passed(year_future, now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 12);
      BOOST_TEST(result.month == 6);
      BOOST_TEST(result.year == 34);
   }
}

BOOST_AUTO_TEST_CASE(mud_date_test)
{
   time_t         now    = 1640014705; // Mon Dec 20 08:38:25 2021
   time_info_data result = mud_date(now);
   BOOST_TEST(result.hours == 6);
   BOOST_TEST(result.day == 3);
   BOOST_TEST(result.month == 8);
   BOOST_TEST(result.year == 1090);
}

BOOST_AUTO_TEST_CASE(mudtime_strcpy_test)
{
   {
      // Test different hours
      const std::string msg{", on the Day of the Bull,\n\r   the 1st Day of the Month of Winter, Year 0.\n\r"};

      time_info_data time{};
      for(time.hours = -24; time.hours <= 24; ++time.hours)
      {
         time.day   = 0;
         time.month = 0;
         time.year  = 0;
         char buffer[4096]{};
         mudtime_strcpy(&time, buffer);
         std::ostringstream strm;
         int                display_hour = ((time.hours % 12 == 0) ? 12 : (time.hours % 12));
         strm << display_hour << " o'clock ";
         if((time.hours > 12 || time.hours == 0))
         {
            strm << "after Noon";
         }
         else
         {
            strm << "past Midnight";
         }
         strm << msg;
         BOOST_TEST(std::string(buffer) == strm.str());
      }
   }
   ///////////////////////////////////////////////////////////////////////////
   {
      // Test different days
      Fixture           f;
      const std::string start_msg{"12 o'clock after Noon, on "};
      const std::string mid_msg{",\n\r   the "};
      const std::string end_msg{" Day of the Month of Winter, Year 0.\n\r"};
      time_info_data    time{};
      for(time.day = 0; time.day < MUD_WEEK; ++time.day)
      {
         time.hours = 0;
         time.month = 0;
         time.year  = 0;
         char buffer[4096]{};
         mudtime_strcpy(&time, buffer);

         std::ostringstream strm;
         int                real_day = time.day + 1;
         int                week_day = ((MUD_MONTH * time.month) + time.day + 1) % MUD_WEEK;
         strm << start_msg << f.days[week_day] << mid_msg << real_day << f.day_prefix(real_day) << end_msg;
         BOOST_TEST(std::string(buffer) == strm.str());
      }
   }
   ///////////////////////////////////////////////////////////////////////////
   {
      // Test different months
      Fixture           f;
      const std::string start_msg{"12 o'clock after Noon, on "};
      const std::string mid_msg{",\n\r   the "};
      time_info_data    time{};
      for(time.month = 0; time.month < MUD_YEAR; ++time.month)
      {
         time.day   = 0;
         time.hours = 0;
         time.year  = 0;
         char buffer[4096]{};
         mudtime_strcpy(&time, buffer);

         std::ostringstream strm;
         int                real_day = time.day + 1;
         int                week_day = ((MUD_MONTH * time.month) + time.day + 1) % MUD_WEEK;
         strm << start_msg << f.days[week_day] << mid_msg << real_day << f.day_prefix(real_day) << " Day of the " << f.months[time.month]
              << ", Year 0.\n\r";
         BOOST_TEST(std::string(buffer) == strm.str());
      }
   }
}

BOOST_AUTO_TEST_CASE(age_test)
{
   { // Isn't a PC
      unit_data unit(UNIT_ST_NPC);
      unit.next                             = nullptr;
      unit.gnext                            = nullptr;
      unit.gprevious                        = nullptr;
      unit.data.ch->specific.pc->time.birth = 690399843;

      auto result = age(&unit);
      //      Fixture::LogTimeInfoData(result);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 0);
      BOOST_TEST(result.month == 0);
      BOOST_TEST(result.year == 0);
   }
   { // Is a PC
      unit_data unit(UNIT_ST_PC);
      unit.next                             = nullptr;
      unit.gnext                            = nullptr;
      unit.gprevious                        = nullptr;
      unit.data.ch->specific.pc->time.birth = 690399843;
      auto result                           = age(&unit);
      Fixture::LogTimeInfoData(result);
      BOOST_TEST(result.hours == 23);
      BOOST_TEST(result.day == 11);
      BOOST_TEST(result.month == 6);
      BOOST_TEST(result.year == 1046);
   }
}
