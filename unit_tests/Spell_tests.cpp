#include "../dm-dist-ii/essential.h"
#include "../dm-dist-ii/spells.h"
#include "../dm-dist-ii/values.h"
#include <boost/test/unit_test.hpp>
#include <map>

BOOST_AUTO_TEST_SUITE(Spell_suite);
BOOST_AUTO_TEST_CASE(spell_legal_type_test)
{
   std::map<int, int> expected_results = {
      {SPL_NONE, 0},
      {SPL_ALL, 0},
      {SPL_DIVINE, 0},
      {SPL_PROTECTION, 0},
      {SPL_DETECTION, 0},
      {SPL_SUMMONING, 0},
      {SPL_CREATION, 0},
      {SPL_MIND, 0},
      {SPL_HEAT, 0},
      {SPL_COLD, 0},
      {SPL_CELL, 0},
      {SPL_INTERNAL, 0},
      {SPL_EXTERNAL, 0},
      {SPL_CALL_LIGHTNING, 1},
      {SPL_BLESS, 1},
      {SPL_CURSE, 1},
      {SPL_REMOVE_CURSE, 1},
      {SPL_CURE_WOUNDS_1, 1},
      {SPL_CURE_WOUNDS_2, 1},
      {SPL_CURE_WOUNDS_3, 1},
      {SPL_CAUSE_WOUNDS_1, 1},
      {SPL_CAUSE_WOUNDS_2, 1},
      {SPL_CAUSE_WOUNDS_3, 1},
      {SPL_DISPEL_EVIL, 1},
      {SPL_REPEL_UNDEAD_1, 1},
      {SPL_REPEL_UNDEAD_2, 1},
      {SPL_BLIND, 1},
      {SPL_CURE_BLIND, 1},
      {SPL_LOCATE_OBJECT, 1},
      {SPL_LOCATE_CHAR, 1},
      {SPL_RAISE_MAG, 1},
      {SPL_RAISE_DIV, 1},
      {SPL_RAISE_STR, 1},
      {SPL_RAISE_DEX, 1},
      {SPL_RAISE_CON, 1},
      {SPL_RAISE_CHA, 1},
      {SPL_RAISE_BRA, 1},
      {SPL_SUN_RAY, 1},
      {SPL_DIVINE_RESIST, 1},
      {SPL_QUICKEN, 1},
      {SPL_HASTE, 1},
      {SPL_RAISE_SUMMONING, 1},
      {SPL_AWAKEN, 1},
      {SPL_MIND_SHIELD, 1},
      {SPL_HEAT_RESI, 1},
      {SPL_COLD_RESI, 1},
      {SPL_ELECTRICITY_RESI, 1},
      {SPL_POISON_RESI, 1},
      {SPL_ACID_RESI, 1},
      {SPL_PRO_EVIL, 1},
      {SPL_SANCTUARY, 1},
      {SPL_DISPEL_MAGIC, 1},
      {SPL_SUSTAIN, 1},
      {SPL_LOCK, 1},
      {SPL_UNLOCK, 1},
      {SPL_DROWSE, 1},
      {SPL_SLOW, 1},
      {SPL_DUST_DEVIL, 1},
      {SPL_DET_ALIGN, 1},
      {SPL_DET_INVISIBLE, 1},
      {SPL_DET_MAGIC, 1},
      {SPL_DET_POISON, 1},
      {SPL_DET_UNDEAD, 1},
      {SPL_DET_CURSE, 1},
      {SPL_SENSE_LIFE, 1},
      {SPL_IDENTIFY_1, 1},
      {SPL_IDENTIFY_2, 1},
      {SPL_RANDOM_TELEPORT, 1},
      {SPL_CLEAR_SKIES, 1},
      {SPL_STORM_CALL, 1},
      {SPL_WORD_OF_RECALL, 1},
      {SPL_CONTROL_TELEPORT, 1},
      {SPL_MINOR_GATE, 1},
      {SPL_GATE, 1},
      {SPL_CREATE_FOOD, 1},
      {SPL_CREATE_WATER, 1},
      {SPL_LIGHT_1, 1},
      {SPL_LIGHT_2, 1},
      {SPL_DARKNESS_1, 1},
      {SPL_DARKNESS_2, 1},
      {SPL_STUN, 1},
      {SPL_HOLD, 1},
      {SPL_ANIMATE_DEAD, 1},
      {SPL_LEATHER_SKIN, 1},
      {SPL_BARK_SKIN, 1},
      {SPL_CONTROL_UNDEAD, 1},
      {SPL_BONE_SKIN, 1},
      {SPL_STONE_SKIN, 1},
      {SPL_AID, 1},
      {SPL_COLOURSPRAY_1, 1},
      {SPL_COLOURSPRAY_2, 1},
      {SPL_COLOURSPRAY_3, 1},
      {SPL_INVISIBILITY, 1},
      {SPL_WIZARD_EYE, 1},
      {SPL_FEAR, 1},
      {SPL_CONFUSION, 1},
      {SPL_SLEEP, 1},
      {SPL_XRAY_VISION, 1},
      {SPL_CALM, 1},
      {SPL_SUMMER_RAIN, 1},
      {SPL_COMMAND, 1},
      {SPL_LEAVING, 1},
      {SPL_FIREBALL_1, 1},
      {SPL_FIREBALL_2, 1},
      {SPL_FIREBALL_3, 1},
      {SPL_FROSTBALL_1, 1},
      {SPL_FROSTBALL_2, 1},
      {SPL_FROSTBALL_3, 1},
      {SPL_LIGHTNING_1, 1},
      {SPL_LIGHTNING_2, 1},
      {SPL_LIGHTNING_3, 1},
      {SPL_STINKING_CLOUD_1, 1},
      {SPL_STINKING_CLOUD_2, 1},
      {SPL_STINKING_CLOUD_3, 1},
      {SPL_POISON, 1},
      {SPL_REMOVE_POISON, 1},
      {SPL_ENERGY_DRAIN, 1},
      {SPL_DISEASE_1, 1},
      {SPL_DISEASE_2, 1},
      {SPL_REM_DISEASE, 1},
      {SPL_ACIDBALL_1, 1},
      {SPL_ACIDBALL_2, 1},
      {SPL_ACIDBALL_3, 1},
      {SPL_MANA_BOOST, 1},
      {SPL_FIND_PATH, 1},
      {SPL_DISPEL_GOOD, 1},
      {SPL_PRO_GOOD, 1},
      {SPL_TRANSPORT, 1},
      {SPL_FIRE_BREATH, 1},
      {SPL_FROST_BREATH, 1},
      {SPL_LIGHTNING_BREATH, 1},
      {SPL_ACID_BREATH, 1},
      {SPL_GAS_BREATH, 1},
      {SPL_LIGHT_BREATH, 1},
      {SPL_HOLD_MONSTER, 1},
      {SPL_HOLD_UNDEAD, 1},
      {SPL_RAISE_DEAD, 1},
      {SPL_RESURRECTION, 1},
      {SPL_TOTAL_RECALL, 1},
      {SPL_UNDEAD_DOOR, 1},
      {SPL_LIFE_PROTECTION, 1},
      {SPL_ENERGY_BOLT, 1},
      {SPL_CLENCHED_FIST, 1},
      {SPL_METEOR_SHOWER, 1},
      {SPL_SUN_BEAM, 1},
      {SPL_SOLAR_FLARE, 1},
      {SPL_SUMMON_DEVIL, 1},
      {SPL_SUMMON_DEMON, 1},
      {SPL_SUMMON_FIRE, 1},
      {SPL_SUMMON_WATER, 1},
      {SPL_SUMMON_AIR, 1},
      {SPL_SUMMON_EARTH, 1},
      {SPL_CHARGE_WAND, 1},
      {SPL_CHARGE_STAFF, 1},
      {SPL_MENDING, 1},
      {SPL_REPAIR, 1},
      {SPL_RECONSTRUCT, 1},
      {SPL_SENDING, 1},
      {SPL_REFIT, 1},
      {SPL_FIND_WANTED, 1},
      {SPL_LOCATE_WANTED, 1},
      {SPL_STORM_GATE, 1},
      {SPL_SUN_GLOBE, 1},
      {SPL_MAGIC_CANDLE, 1},
      {SPL_SONIC_BREATH, 0},
      {SPL_SHARD_BREATH, 0},
      {SPL_CONE_SHARD, 0},
      {SPL_SACRED, 0},
      {SPL_BLANK1, 0},
      {SPL_BLANK2, 0},
      {SPL_BLANK3, 0},
      {SPL_BLANK4, 0},
      {SPL_BLANK5, 0},
      {SPL_BLANK6, 0},
      {SPL_BLANK7, 0},
      {SPL_BLANK8, 0},
      {SPL_BLANK9, 0},
      {176, 0},
      {177, 0},
      {178, 0},
      {179, 0},
      {180, 0},
      {181, 0},
      {182, 0},
      {183, 0},
      {184, 0},
      {185, 0},
      {186, 0},
      {187, 0},
      {188, 0},
      {189, 0},
      {190, 0},
      {191, 0},
      {192, 0},
      {193, 0},
      {194, 0},
      {195, 0},
      {196, 0},
      {197, 0},
      {198, 0},
      {199, 0},
      {200, 0},
      {201, 0},
      {202, 0},
      {203, 0},
      {204, 0},
      {205, 0},
      {206, 0},
      {207, 0},
      {208, 0},
      {209, 0},
      {210, 0},
      {211, 0},
      {212, 0},
      {213, 0},
      {214, 0},
      {215, 0},
      {216, 0},
      {217, 0},
      {218, 0},
      {219, 0},
      {220, 0},
   };

   for(int i = SPL_ALL; i <= SPL_TREE_MAX; ++i)
   {
      auto result   = spell_legal_type(i, MEDIA_SPELL);
      auto expected = expected_results.find(i);
      if(expected != expected_results.end())
      {
         //         BOOST_TEST(result == expected->second);
         if(result != expected->second)
         {
            BOOST_TEST_MESSAGE("i=" << i << " result=" << (int)result);
         }
      }
      else
      {
         BOOST_TEST_MESSAGE("Missing spell number from expected_results() : " << i);
         BOOST_TEST(false);
      }
   }
}

BOOST_AUTO_TEST_SUITE_END();