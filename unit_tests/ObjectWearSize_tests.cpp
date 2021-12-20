#include "../dm-dist-ii/act.h"
#include "../dm-dist-ii/structs.h"
#include "../dm-dist-ii/values.h"
#include <boost/test/unit_test.hpp>
#include <memory>

void init_char(struct unit_data *ch);

BOOST_AUTO_TEST_SUITE(obj_wear_size_suite);
BOOST_AUTO_TEST_CASE(obj_wear_size_test)
{
   BOOST_TEST_MESSAGE("obj_wear_size tests");

   auto ch = std::make_unique<unit_data>(UNIT_ST_PC);
   init_char(ch.get());
   ch->size = 180;

   auto obj = std::make_unique<unit_data>(UNIT_ST_OBJ);

   {
      BOOST_TEST_MESSAGE("Plain call");
      auto result = obj_wear_size(ch.get(), obj.get());
      if(result != nullptr)
      {
         BOOST_TEST_MESSAGE(result);
      }
      BOOST_TEST(result == nullptr);
   }
   {
      BOOST_TEST_MESSAGE("Trying with all manipulate versions set on object");
      std::vector<int> manipulate = {
         MANIPULATE_TAKE,       MANIPULATE_WEAR_FINGER, MANIPULATE_WEAR_NECK,  MANIPULATE_WEAR_BODY,  MANIPULATE_WEAR_HEAD,
         MANIPULATE_WEAR_LEGS,  MANIPULATE_WEAR_FEET,   MANIPULATE_WEAR_HANDS, MANIPULATE_WEAR_ARMS,  MANIPULATE_WEAR_SHIELD,
         MANIPULATE_WEAR_ABOUT, MANIPULATE_WEAR_WAIST,  MANIPULATE_WEAR_WRIST, MANIPULATE_WIELD,      MANIPULATE_HOLD,
         MANIPULATE_ENTER,      MANIPULATE_WEAR_EAR,    MANIPULATE_WEAR_BACK,  MANIPULATE_WEAR_CHEST, MANIPULATE_WEAR_ANKLE,
      };
      for(auto manip : manipulate)
      {
         obj->manipulate = manip;
         obj->size       = 180;
         auto result     = obj_wear_size(ch.get(), obj.get());
         if(result != nullptr)
         {
            BOOST_TEST_MESSAGE(result);
         }
         BOOST_TEST(result == nullptr);
      }
   }
   {
      BOOST_TEST_MESSAGE("Trying with all wear keywords passed in");
      std::vector<int> keywords = {
         WEAR_FINGER_R, WEAR_FINGER_L, WEAR_NECK_1, WEAR_NECK_2, WEAR_BODY,    WEAR_HEAD,    WEAR_LEGS,    WEAR_FEET,
         WEAR_HANDS,    WEAR_ARMS,     WEAR_ABOUT,  WEAR_WAIST,  WEAR_WRIST_L, WEAR_WRIST_R, WEAR_WIELD,   WEAR_HOLD,
         WEAR_SHIELD,   WEAR_CHEST,    WEAR_BACK,   WEAR_EAR_L,  WEAR_EAR_R,   WEAR_ANKLE_L, WEAR_ANKLE_R,
      };
      for(auto k : keywords)
      {
         auto result = obj_wear_size(ch.get(), obj.get(), k);
         if(result != nullptr)
         {
            BOOST_TEST_MESSAGE(result);
         }
         BOOST_TEST(result == nullptr);
      }
   }
}
BOOST_AUTO_TEST_SUITE_END();
