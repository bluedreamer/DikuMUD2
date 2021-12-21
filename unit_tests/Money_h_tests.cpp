#include "../dm-dist-ii/money.h"

#include <boost/test/unit_test.hpp>
#include <memory>
BOOST_AUTO_TEST_SUITE(money_h_suite);

BOOST_AUTO_TEST_CASE(money_externs_test)
{
   BOOST_TEST(std::string(money_types[0].abbrev) == std::string(IRON_PIECE));
   BOOST_TEST(std::string(money_types[1].abbrev) == std::string(COPPER_PIECE));
   BOOST_TEST(std::string(money_types[2].abbrev) == std::string(SILVER_PIECE));
   BOOST_TEST(std::string(money_types[3].abbrev) == std::string(GOLD_PIECE));
   BOOST_TEST(std::string(money_types[4].abbrev) == std::string(PLATINUM_PIECE));
}

BOOST_AUTO_TEST_CASE(coins_to_unit_test)
{
   BOOST_TEST_MESSAGE("Testing function coins_to_unit()");
   auto pc = std::make_unique<unit_data>(UNIT_ST_PC);

   coins_to_unit(pc.get(), 11, 0);
}

BOOST_AUTO_TEST_SUITE_END()
