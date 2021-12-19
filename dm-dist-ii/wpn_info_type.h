#pragma once
#include <cstdint>
/* Tree has a pointer to parent for each node. 0 pointer from root */
struct wpn_info_type
{
   int     hands;  /* 0=N/A, 1 = 1, 2 = 1.5, 3 = 2          */
   int     speed;  /* Speed modification by weapon 0..      */
   int     type;   /* Is the weapon slashing/piercing...    */
   uint8_t shield; /* Shield method SHIELD_M_XXX            */
};
extern wpn_info_type wpn_info[];
