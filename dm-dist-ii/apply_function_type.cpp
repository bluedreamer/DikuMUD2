#include "apply_function_type.h"

#include "apf_affect.h"

apply_function_type apf[] = {{"Ability Adjustment", apf_ability_adj},
                             {"Spell skill adjustment", apf_spell_adj},
                             {"Modify bright/light", apf_light},
                             {"Modify CHAR_FLAGS()", apf_mod_char_flags},
                             {"Modify UNIT_FLAGS()", apf_mod_unit_flags},
                             {"Modify OBJ_FLAGS()", apf_mod_obj_flags},
                             {"Skill adjustment", apf_skill_adj},
                             {"Weapon adjustment", apf_weapon_adj},
                             {"Natural Armour Modification", apf_natural_armour},
                             {"Speed Adjustment", apf_speed}};
