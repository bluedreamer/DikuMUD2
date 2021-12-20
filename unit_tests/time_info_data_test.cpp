#include "../dm-dist-ii/structs.h"
#include "../dm-dist-ii/weather.h"
#include <boost/test/unit_test.hpp>

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
      time_info_data result = real_time_passed(hour_future,now);
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
      time_info_data result = real_time_passed(day_future,now);
      BOOST_TEST(result.hours == 0);
      BOOST_TEST(result.day == 1);
      BOOST_TEST(result.month == -1);
      BOOST_TEST(result.year == -1);
   }
}
