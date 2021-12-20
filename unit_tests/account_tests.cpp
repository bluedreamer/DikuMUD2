#include "../dm-dist-ii/account.h"
#include "../dm-dist-ii/config.h"
#include "../dm-dist-ii/structs.h"
#include "../dm-dist-ii/values.h"
#include <boost/test/unit_test.hpp>
#include <memory>

struct AccountsFixture
{
   AccountsFixture();
   ~AccountsFixture();
   //   std::unique_ptr<unit_data> room_data;
   //   unit_data *obj_data{nullptr};
   std::unique_ptr<unit_data> npc_data;
   std::unique_ptr<unit_data> pc_data;
};

AccountsFixture::AccountsFixture()
   : //   room_data(std::make_unique<unit_data>(UNIT_ST_ROOM))
   npc_data(std::make_unique<unit_data>(UNIT_ST_NPC))
   , pc_data(std::make_unique<unit_data>(UNIT_ST_PC))
{
   void init_char(struct unit_data * ch);
   init_char(pc_data.get());
}

AccountsFixture::~AccountsFixture()
{
}

BOOST_AUTO_TEST_SUITE(account_suite);
BOOST_AUTO_TEST_CASE(account_flatrate_change_test)
{
}
BOOST_AUTO_TEST_CASE(account_cclog_test)
{
}
BOOST_AUTO_TEST_CASE(account_insert_test)
{
}
BOOST_AUTO_TEST_CASE(account_withdraw_test)
{
}
BOOST_AUTO_TEST_CASE(account_global_stat_test)
{
}
BOOST_AUTO_TEST_CASE(account_local_stat_test)
{
}
BOOST_AUTO_TEST_CASE(account_defaults_test)
{
}
BOOST_AUTO_TEST_CASE(account_subtract_test)
{
}
BOOST_AUTO_TEST_CASE(account_is_overdue_test)
{
   AccountsFixture f;
   g_cServerConfig.m_bAccounting = 1;
   BOOST_TEST(account_is_overdue(f.pc_data.get()) == 0);
}
BOOST_AUTO_TEST_CASE(account_overdue_test)
{
}
BOOST_AUTO_TEST_CASE(account_paypoint_test)
{
}
BOOST_AUTO_TEST_CASE(account_closed_test)
{
}
BOOST_AUTO_TEST_CASE(account_is_closed_test)
{
   AccountsFixture f;
//   BOOST_TEST(account_is_closed(f.npc_data.get()) == 0);
   //BOOST_TEST(account_is_closed(f.room_data.get()) == 0);
   BOOST_TEST(account_is_closed(f.pc_data.get()) == 0);
}
BOOST_AUTO_TEST_SUITE_END();
