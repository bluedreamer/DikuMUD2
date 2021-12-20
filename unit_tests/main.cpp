#define BOOST_TEST_MODULE "Diku MUD 2 Tests"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

void boot_db(void);
struct GlobalFixture
{
   GlobalFixture() { boot_db(); }
   void setup() {}
   void teardown() {}
   ~GlobalFixture() {}
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalFixture);
