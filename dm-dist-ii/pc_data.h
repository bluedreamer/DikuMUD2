#pragma once

#include "extra.h"
#include "pc_account_data.h"
#include "pc_time_data.h"
#include "protocol.h"
#include "values.h"

constexpr size_t PC_MAX_PASSWORD = 12; /* Max length of any pc-password  */
constexpr size_t PC_MAX_NAME     = 15; /* 14 Characters + Null           */

class pc_data
{
public:
   pc_data();
   ~pc_data();

   terminal_setup_type setup;
   pc_time_data        time;                        /* PCs time info  */
   pc_account_data     account;                     /* Accounting     */
   char               *guild;                       /* Which guild is the player a member of?  */
   char               *bank;                        /* How much money in bank?                 */
   char               *hometown;                    /* PCs Hometown (symbolic reference)       */
   extra_descr_data   *info;                        /* For saving INFO information             */
   extra_descr_data   *quest;                       /* For saving QUEST information            */
   uint32_t            guild_time;                  /* When (playing secs) player entered      */
   uint16_t            vlvl;                        /* Virtual Level for player                */
   int32_t             id;                          /* Unique identifier for each player (-1 guest) */
   int32_t             skill_points;                /* No of practice points left              */
   int32_t             ability_points;              /* No of practice points left              */
   uint16_t            flags;                       /* flags for PC setup (brief, noshout...)  */
   uint16_t            nr_of_crimes;                /* Number of crimes committed              */
   uint16_t            crack_attempts;              /* Number of wrong passwords entered       */
   uint16_t            lifespan;                    /* How many year to live....               */
   uint8_t             spells[SPL_TREE_MAX];        /* The spells learned                  */
   uint8_t             spell_lvl[SPL_TREE_MAX];     /* Practiced within that level         */
   int8_t              spell_cost[SPL_TREE_MAX];    /* Cost modifier                       */
   uint8_t             skills[SKI_TREE_MAX];        /* The skills learned                  */
   uint8_t             skill_lvl[SKI_TREE_MAX];     /* The skills practiced within level   */
   int8_t              skill_cost[SKI_TREE_MAX];    /* Cost modifier                       */
   uint8_t             weapons[WPN_TREE_MAX];       /* The weapons learned                 */
   uint8_t             weapon_lvl[WPN_TREE_MAX];    /* The spells learned                  */
   int8_t              weapon_cost[WPN_TREE_MAX];   /* Cost modifier                       */
   uint8_t             ability_lvl[ABIL_TREE_MAX];  /* The spells learned                  */
   int8_t              ability_cost[ABIL_TREE_MAX]; /* Cost modifier                       */
   int8_t              conditions[3];               /* Drunk full etc.                     */
   uint8_t             nAccessLevel;                /* Access Level for BBS use            */
   char                pwd[PC_MAX_PASSWORD];        /* Needed when loaded w/o descriptor   */
   char                filename[PC_MAX_NAME];       /* The name on disk...                 */
   uint32_t            lasthosts[5];                /* last 5 different IPs                */
};
