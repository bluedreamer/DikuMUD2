#include "../dm-dist-ii/structs.h"
#include "../dm-dist-ii/weather.h"
#include <boost/test/unit_test.hpp>

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
}

BOOST_AUTO_TEST_CASE(real_time_passed_test)
{
   time_t now         = time(nullptr);
   time_t hour_future = now + (60 * 60);
   time_t day_future  = now + (60 * 60 * 24);
   time_t year_future = now + (60 * 60 * 24 * 365);

   //   BOOST_TEST_MESSAGE("hours=" << (int)result.hours << " day=" << (int)result.day << " month=" << (int)result.month
   //                               << " year=" << (int)result.year);
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
