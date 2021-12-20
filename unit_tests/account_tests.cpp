#include "../dm-dist-ii/account.h"
#include "../dm-dist-ii/values.h"
#include "../dm-dist-ii/structs.h"
#include "../dm-dist-ii/config.h"
#include <boost/test/unit_test.hpp>

struct Fixture {
   unit_data room_data{UNIT_ST_ROOM};
   unit_data obj_data{UNIT_ST_OBJ};
   unit_data npc_data{UNIT_ST_NPC};
   unit_data pc_data{UNIT_ST_PC};
};

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
   Fixture f;
   g_cServerConfig.m_bAccounting=1;
   BOOST_TEST(account_is_overdue(&f.npc_data)==0);
   BOOST_TEST(account_is_overdue(&f.room_data)==0);
   BOOST_TEST(account_is_overdue(&f.obj_data)==0);
   BOOST_TEST(account_is_overdue(&f.pc_data)==1);
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
   Fixture f;
   BOOST_TEST(account_is_closed(&f.npc_data)==0);
   BOOST_TEST(account_is_closed(&f.room_data)==0);
   BOOST_TEST(account_is_closed(&f.obj_data)==0);

   BOOST_TEST(account_is_closed(&f.pc_data)==0);
}
