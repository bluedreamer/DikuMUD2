#pragma once
#include <values.h>

#define dilend dilend ^
#define pause  wait(SFB_TICK | SFB_RANTIME, TRUE)

#define IS_GOOD(a)    ((a) >= 350)
#define IS_EVIL(a)    ((a) <= -350)
#define IS_NEUTRAL(a) (((a) > -350) and ((a) < 350))

#define RACE_IS_HUMANOID(race) ((race) <= RACE_OTHER_HUMANOID)

#define RACE_IS_MAMMAL(race) (((race) > RACE_OTHER_HUMANOID) and ((race) <= RACE_OTHER_MAMMAL))

#define RACE_IS_UNDEAD(race) (((race) > RACE_OTHER_CREATURE) and ((race) <= RACE_OTHER_UNDEAD))

/* ====================================================================== */
/*                   R O O M     D E F I N I T I O N S                    */
/*                                                                        */
/*                       Only to be used on rooms!                        */
/* ====================================================================== */

#define north exit[NORTH]
#define east  exit[EAST]
#define south exit[SOUTH]
#define west  exit[WEST]
#define up    exit[UP]
#define down  exit[DOWN]

/* Used to set the diffculty accociated with searching for hidden doors.   */
/*   'dir'   : The direction of the door (NORTH, SOUTH, EAST, etc.)        */
/*   'skill' : How difficult it is to spot the door (0-100 (200))          */
#define SECRET_DOOR_DIFFICULTY(dir, skill)                                                                                                 \
   affect id ID_HIDDEN_DIFFICULTY duration - 1 data[0] dir data[1] skill firstf TIF_NONE tickf TIF_NONE lastf TIF_NONE applyf APF_NONE;

/* LIGHTS: */
/* This is how the 'day' in the world affects light in outside rooms      */
/* Default is 0 light sources (natural light outside, always lit inside)  */
/* Time     Light            */
/* 05         0 light source */
/* 06-20     +1 light source */
/* 21         0 light source */
/* 22-04     -1 light source */

/* Always a dark outside room, no matter what time of day */
#define OUT_ALWAYS_DARK light - 2

/* Always a dark room, except when it is high noon */
#define OUT_DARK_NON_NOON light - 1

/* When a room is always light - both for inside and outside rooms */
#define ALWAYS_LIGHT light 1

/* When a inside room is always dark - both inside and outside */
#define IN_ALWAYS_DARK light - 1

/* ====================================================================== */
/*               O B J E C T     D E F I N I T I O N S                    */
/*                                                                        */
/*                       Only to be used on objects!                      */
/* ====================================================================== */

/* weapon_category: One of the WPN_XXX macros.       */
/* craftsmanship:   [-25..25] Material used & Smity  */
/* magic_bonus:     [-25..25] Magic modification     */
#define WEAPON_DEF(weapon_category, craftsmanship, magic_bonus)                                                                            \
   manipulate{                                                                                                                             \
      MANIPULATE_TAKE,                                                                                                                     \
      MANIPULATE_WIELD} type ITEM_WEAPON value[0] weapon_category value[1] craftsmanship value[2] magic_bonus value[3] RACE_DO_NOT_USE

#define WEAPONSZ_DEF(weapon_category, craftsmanship, magic_bonus, hgt)                                                                     \
   WEAPON_DEF(weapon_category, craftsmanship, magic_bonus)                                                                                 \
   height hgt

#define SHIELD_DEF(shield_type, craftsmanship, magic_bonus)                                                                                \
   manipulate{MANIPULATE_TAKE, MANIPULATE_WEAR_SHIELD} type ITEM_SHIELD value[0] shield_type value[1] craftsmanship value[2] magic_bonus

#define SHIELDSZ_DEF(shield_type, craftsmanship, magic_bonus, hgt)                                                                         \
   SHIELD_DEF(shield_type, craftsmanship, magic_bonus)                                                                                     \
   height hgt

#define ARMOUR_CLOTHES(craftsmanship, magic_bonus)                                                                                         \
   manipulate{MANIPULATE_TAKE} type ITEM_ARMOR value[0] ARM_CLOTHES value[1] craftsmanship value[2] magic_bonus

#define ARMOURSZ_CLOTHES(craftsmanship, magic_bonus, hgt)                                                                                  \
   ARMOUR_CLOTHES(craftsmanship, magic_bonus)                                                                                              \
   height hgt

#define ARMOUR_LEATHER(craftsmanship, magic_bonus)                                                                                         \
   manipulate{MANIPULATE_TAKE} type ITEM_ARMOR value[0] ARM_LEATHER value[1] craftsmanship value[2] magic_bonus

#define ARMOURSZ_LEATHER(craftsmanship, magic_bonus, hgt)                                                                                  \
   ARMOUR_LEATHER(craftsmanship, magic_bonus)                                                                                              \
   height hgt

#define ARMOUR_HRD_LEATHER(craftsmanship, magic_bonus)                                                                                     \
   manipulate{MANIPULATE_TAKE} type ITEM_ARMOR value[0] ARM_HLEATHER value[1] craftsmanship value[2] magic_bonus

#define ARMOURSZ_HRD_LEATHER(craftsmanship, magic_bonus, hgt)                                                                              \
   ARMOUR_HRD_LEATHER(craftsmanship, magic_bonus)                                                                                          \
   height hgt

#define ARMOUR_CHAIN(craftsmanship, magic_bonus)                                                                                           \
   manipulate{MANIPULATE_TAKE} type ITEM_ARMOR value[0] ARM_CHAIN value[1] craftsmanship value[2] magic_bonus

#define ARMOURSZ_CHAIN(craftsmanship, magic_bonus, hgt)                                                                                    \
   ARMOUR_CHAIN(craftsmanship, magic_bonus)                                                                                                \
   height hgt

#define ARMOUR_PLATE(craftsmanship, magic_bonus)                                                                                           \
   manipulate{MANIPULATE_TAKE} type ITEM_ARMOR value[0] ARM_PLATE value[1] craftsmanship value[2] magic_bonus

#define ARMOURSZ_PLATE(craftsmanship, magic_bonus, hgt)                                                                                    \
   ARMOUR_PLATE(craftsmanship, magic_bonus)                                                                                                \
   height hgt

/* Material Types
 *  All objects must have a material type!!  Please select one that
 *  best fits your items material.  If your item is composed of
 *  several types of materials list all of them.
 *  Use the MATERIAL_XXX to put on all objects,  ie
 *
 * MATERIAL_WOOD("A very hard wood handle");
 * MATERIAL_METAL("A heavy cast iron spike");
 * MATERIAL_LEATHER("Leather bindings");
 *
 * All of the descriptions will be shown to the players when they id
 * thier items.
 *
 * The plan is to make spells that affect certain types of weapons and
 * materials differently.  Ie acid will affect most anything except glass.
 * Electricty should give more dammage to a person that is wearing metal
 * than someone that is wearing leather.  Organics might be susceptable to
 * rot, and decay spells, while metal might rust.
 */
#define MATERIAL_WOOD(DESCR)    extra{"$material", "$mat_wood"} DESCR
#define MATERIAL_METAL(DESCR)   extra{"$material", "$mat_metal"} DESCR
#define MATERIAL_STONE(DESCR)   extra{"$material", "$mat_stone"} DESCR
#define MATERIAL_CLOTH(DESCR)   extra{"$material", "$mat_cloth"} DESCR
#define MATERIAL_LEATHER(DESCR) extra{"$material", "$mat_leather"} DESCR
#define MATERIAL_SKIN(DESCR)    extra{"$material", "$mat_skin"} DESCR
#define MATERIAL_ORGANIC(DESCR) extra{"$material", "$mat_organic"} DESCR
#define MATERIAL_GLASS(DESCR)   extra{"$material", "$mat_glass"} DESCR
#define MATERIAL_FIRE(DESCR)    extra{"$material", "$mat_fire"} DESCR
#define MATERIAL_WATER(DESCR)   extra{"$material", "$mat_water"} DESCR
#define MATERIAL_EARTH(DESCR)   extra{"$material", "$mat_earth"} DESCR
#define MATERIAL_MAGIC(DESCR)   extra{"$material", "$mat_magic"} DESCR

/*  These are for use in dill to check if a item is a certain type of
 * material.
 *
 * ie.
 *
 * if (MAT_WOOD in item.extra) {
 *	 ...
 *	 ...
 * }
 *
 * if the item has a wooden material it will evualate to true. */

#define MATERIAL    "$material"
#define MAT_WOOD    "$mat_wood"
#define MAT_METAL   "$mat_wood"
#define MAT_STONE   "$mat_stone"
#define MAT_CLOTH   "$mat_cloth"
#define MAT_LEATHER "$mat_leather"
#define MAT_ORGANIC "$mat_organic"
#define MAT_GLASS   "$mat_glass"
#define MAT_FIRE    "$mat_fire"
#define MAT_WATER   "$mat_water"
#define MAT_MAGIC   "$mat_magic"

/* liquid_type:  Must be one of LIQ_XXX from values.h                */
/* wgt:          Is the containers weight when it is empty           */
/* max_capacity: Is the maximum amount of liquid it can contain.     */
/*               -1 means infinite amount of liquid.                 */
/* liquid_inside:Is how much liquid there actually is inside (>=0).  */
/* poison fact:  Any value > 0 makes poison. Larger the more lethal  */
#define LIQ_DEF(liquid_type, wgt, max_capacity, liquid_inside, poison_factor)                                                              \
   type ITEM_DRINKCON                      weight(wgt) +                                                                                   \
      (liquid_inside)capacity max_capacity value[0] max_capacity value[1] liquid_inside value[2] liquid_type value[3] poison_factor

#define FOOD_DEF(food_amount, poison_factor) type ITEM_FOOD value[0] food_amount value[3] poison_factor

/* Hours     : How many hours (mud time) that the item may burn for.        */
/* How_bright: The amount of light sources that the item shines with.       */
/*             small torch = 1, torch = 2, lantern = 3...                   */
#define LIGHT_DEF(hours, how_bright) type ITEM_LIGHT value[0] hours value[1] how_bright

#define CONTAINER_DEF(max_capacity) type ITEM_CONTAINER capacity max_capacity

/* This is ALL you need to define money, rest is inserted at runtime. */
#define MONEY(coin_type, coins) type ITEM_MONEY manipulate MANIPULATE_TAKE title coin_type value[0] coins

/* ---------------------------------- MAGICK ---------------------------- */

/* race : Match one of the RACE_XXX values. */
#define WEAPON_SLAYER(race) flags{UNIT_FL_MAGIC} value[3] race

/* To be used when a object is cursed so that it can't be unequipped */
#define CURSED_OBJECT                                                                                                                      \
   affect id ID_CURSE duration - 1 firstf TIF_NONE tickf TIF_NONE lastf TIF_NONE applyf APF_MOD_OBJ_FLAGS data[0] OBJ_NO_UNEQUIP;

#define CHAR_FLAG_TRANSFER(_MFLAGS)                                                                                                        \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_CHARFLAGS                                    duration -                                      \
      1 data[0] _MFLAGS firstf TIF_EYES_TINGLE tickf TIF_NONE lastf TIF_EYES_TINGLE applyf APF_MOD_CHAR_FLAGS;

/* skill MUST be one of SKI_XXX, amount in -10 to +10 */
#define SKILL_TRANSFER(skill, amount)                                                                                                      \
   flags{UNIT_FL_MAGIC} affect id ID_SKILL_TRANSFER duration -                                                                             \
      1 data[0] skill data[1] amount firstf TIF_SKI_INC tickf TIF_NONE lastf TIF_SKI_DEC applyf APF_SKILL_ADJ;

/* weapon MUST be one of WPN_XXX, amount in -10 to +10 */
#define WEAPON_TRANSFER(weapon, amount)                                                                                                    \
   flags{UNIT_FL_MAGIC} affect id ID_WEAPON_TRANSFER duration -                                                                            \
      1 data[0] weapon data[1] amount firstf TIF_WPN_INC tickf TIF_NONE lastf TIF_WPN_DEC applyf APF_WEAPON_ADJ;

/* spell MUST be one of SPL_XXX, amount in -10 to +10 */
#define SPELL_TRANSFER(spell, amount)                                                                                                      \
   flags{UNIT_FL_MAGIC} affect id ID_SPELL_TRANSFER duration - 1                      /* Must be permanent in the object */                \
      data[0] spell                                                                   /* It is a spell SPL_XXX transfer  */                \
                                                                       data[1] amount /* Amount of better spell skill    */                \
            firstf TIF_SPL_INC tickf TIF_NONE lastf TIF_SPL_DEC applyf APF_SPELL_ADJ;

#define POTION_DEF(power, spell1, spell2, spell3)                                                                                          \
   manipulate{MANIPULATE_TAKE, MANIPULATE_HOLD} flags{                                                                                     \
      UNIT_FL_MAGIC} spell power type ITEM_POTION value[0] power value[1] spell1 value[2] spell2 value[3] spell3

#define SCROLL_DEF(power, spell1, spell2, spell3)                                                                                          \
   manipulate{MANIPULATE_TAKE, MANIPULATE_HOLD} flags{                                                                                     \
      UNIT_FL_MAGIC} spell power type ITEM_SCROLL value[0] power value[1] spell1 value[2] spell2 value[3] spell3

#define WAND_DEF(power, charge, spell1, spell2)                                                                                            \
   manipulate{MANIPULATE_TAKE, MANIPULATE_HOLD} flags{UNIT_FL_MAGIC} spell power type ITEM_WAND                                            \
      value[0] power value[1] charge value[2] spell1 value[3] spell2 value[4] charge /* The max charge */

#define STAFF_DEF(power, charge, spell1, spell2)                                                                                           \
   manipulate{MANIPULATE_TAKE, MANIPULATE_HOLD} flags{UNIT_FL_MAGIC} spell power type ITEM_STAFF                                           \
      value[0] power value[1] charge value[2] spell1 value[3] spell2 value[4] charge /* The max charge */

#define STR_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_STR duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_STR                                                                /* It is a strength function       */                \
                                                                       data[1] amount /* Amount of better strength       */                \
            firstf TIF_STR_INC tickf TIF_NONE lastf TIF_STR_DEC applyf APF_ABILITY;

#define DEX_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_DEX duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_DEX                                                                /* It is a dex function            */                \
                                                                       data[1] amount /* Amount of better dex            */                \
            firstf TIF_DEX_INC tickf TIF_NONE lastf TIF_DEX_DEC applyf APF_ABILITY;

#define CON_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_CON duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_CON                                                                /* It is a con function            */                \
                                                                       data[1] amount /* Amount of better con            */                \
            firstf TIF_CON_INC tickf TIF_NONE lastf TIF_CON_DEC applyf APF_ABILITY;

#define CHA_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_CHA duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_CHA                                                                /* It is a cha function            */                \
                                                                       data[1] amount /* Amount of better cha            */                \
            firstf TIF_CHA_INC tickf TIF_NONE lastf TIF_CHA_DEC applyf APF_ABILITY;

#define BRA_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_BRA duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_BRA                                                                /* It is a bra function            */                \
                                                                       data[1] amount /* Amount of better bra            */                \
            firstf TIF_BRA_INC tickf TIF_NONE lastf TIF_BRA_DEC applyf APF_ABILITY;

#define MAG_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_MAG duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_MAG                                                                /* It is a mag function            */                \
                                                                       data[1] amount /* Amount of better mag            */                \
            firstf TIF_MAG_INC tickf TIF_NONE lastf TIF_MAG_DEC applyf APF_ABILITY;

#define DIV_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_DIV duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_DIV                                                                /* It is a div function            */                \
                                                                       data[1] amount /* Amount of better div            */                \
            firstf TIF_DIV_INC tickf TIF_NONE lastf TIF_DIV_DEC applyf APF_ABILITY;

#define HIT_TRANSFER(amount)                                                                                                               \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_HPP duration - 1                        /* Must be permanent in the object */                \
      data[0] ABIL_HP                                                                 /* It is a hitpoint function       */                \
                                                                       data[1] amount /* Amount of better strength       */                \
            firstf TIF_HIT_INC tickf TIF_NONE lastf TIF_HIT_DEC applyf APF_ABILITY;

#define SPEED_TRANSFER(newspeed)                                                                                                           \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_SPEED duration - 1 /* Must be permanent in the object */                                     \
      data[0] newspeed firstf TIF_SPEED_BETTER tickf TIF_NONE lastf TIF_SPEED_WORSE applyf APF_SPEED;

#define SLOW_TRANSFER(amount)                                                                                                              \
   flags{UNIT_FL_MAGIC} affect id ID_TRANSFER_SPEED                                          duration -                                    \
      1 data[0] newspeed firstf TIF_SPEED_WORSE tickf TIF_NONE lastf TIF_SPEED_BETTER applyf APF_SPEED;

/* ====================================================================== */
/*                M O N S T E R     D E F I N I T I O N S                 */
/*                                                                        */
/*                       Only to be used on mobiles!                      */
/* ====================================================================== */

/* Use when making objects which protect from good when used */
/* Has replaced CHAR_REVERSE_...                             */

#define PROTECTION_FROM_GOOD                                                                                                               \
   affect id ID_PROT_GOOD_TRANSFER duration - 1 firstf TIF_PROT_GOOD_ON tickf TIF_NONE lastf TIF_PROT_GOOD_OFF applyf APF_NONE;

/* Use when making objects which protect from evil when used */

#define PROTECTION_FROM_EVIL                                                                                                               \
   affect id ID_PROT_EVIL_TRANSFER duration - 1 firstf TIF_PROT_EVIL_ON tickf TIF_NONE lastf TIF_PROT_EVIL_OFF applyf APF_NONE;

#define NATURAL_DEF(weapon_category, armour_category) armour armour_category attack weapon_category

#define ATTACK_DEFENSE(attack, defense) offensive attack defensive defense

#define MSET_ABILITY(str, dex, con, hpp, bra, cha, mag, div)                                                                               \
   ability[ABIL_STR] str ability[ABIL_DEX] dex ability[ABIL_CON] con ability[ABIL_HP] hpp ability[ABIL_BRA] bra ability[ABIL_MAG] mag      \
      ability[ABIL_DIV] div ability[ABIL_CHA] cha

/* axe_ham is all axes and hammers.
   sword is all swords.
   club_mace is all Clubs, Maces, Flails, Morning stars
   Pole is all quarterstaff, spear, halberd, bardiche, sickle scythe trident
   unarmed is all natural attacks (crush, claw, fist, etc)
   special is whip & unused */

#define MSET_WEAPON(axe_ham, sword, club_mace, pole, unarmed, special)                                                                     \
   weapon[WPN_AXE_HAM] axe_ham weapon[WPN_SWORD] sword weapon[WPN_CLUB_MACE] club_mace weapon[WPN_POLEARM] pole                            \
      weapon[WPN_UNARMED] unarmed weapon[WPN_SPECIAL] special

#define MSET_SPELL(div, pro, det, sum, cre, min, hea, col, cel, int, ext)                                                                  \
   spell[SPL_DIVINE] div spell[SPL_PROTECTION] pro spell[SPL_DETECTION] det spell[SPL_SUMMONING] sum spell[SPL_CREATION] cre               \
      spell[SPL_MIND] min spell[SPL_HEAT] hea spell[SPL_COLD] col spell[SPL_CELL] cel spell[SPL_INTERNAL] int spell[SPL_EXTERNAL] ext
