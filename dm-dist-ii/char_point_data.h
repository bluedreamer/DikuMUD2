#pragma once
#include <cstdint>
#include <values.h>

struct char_point_data
{
   uint32_t flags; /* Char flags                               */
   int32_t  exp;   /* The experience of the player             */

   int16_t  mana;      /* How many mana points are left?           */
   int16_t  endurance; /* How many endurance points are left?      */
   uint16_t race;      /* PC/NPC race, Humanoid, Animal, etc.     */

   int16_t offensive; /* The OB of a character.                   */
   int16_t defensive; /* The DB of a character.                   */

   uint8_t speed;          /* The default speed for natural combat     */
   uint8_t natural_armour; /* The natural built-in armour (ARM_)       */
   uint8_t attack_type;    /* PC/NPC Attack Type for bare hands (WPN_) */

   uint8_t dex_reduction;            /* For speed of armour calculations only    */
   uint8_t sex;                      /* PC / NPC s sex                           */
   uint8_t level;                    /* PC / NPC s level                         */
   uint8_t position;                 /* Standing, sitting, fighting...           */
   uint8_t abilities[ABIL_TREE_MAX]; /* Str/dex etc.                 */
};
