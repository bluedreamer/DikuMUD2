#include "pc_data.h"

#include <cstring>
extern int world_nopc;
pc_data::pc_data()
{
   world_nopc++;

   info     = nullptr;
   bank     = nullptr;
   guild    = nullptr;
   hometown = nullptr;
   quest    = nullptr;

   memset(&setup, 0, sizeof(setup));
   memset(&time, 0, sizeof(setup));
   memset(&account, 0, sizeof(setup));

   guild_time = 0;
   vlvl       = 0;
   id         = -1;

   skill_points   = 0;
   ability_points = 0;
   flags          = 0;

   nr_of_crimes   = 0;
   crack_attempts = 0;

   memset(spells, 0, sizeof(spells));
   memset(spell_lvl, 0, sizeof(spell_lvl));
   memset(spell_cost, 0, sizeof(spell_cost));

   memset(skills, 0, sizeof(skills));
   memset(skill_lvl, 0, sizeof(skill_lvl));
   memset(skill_cost, 0, sizeof(skill_cost));

   memset(weapons, 0, sizeof(weapons));
   memset(weapon_lvl, 0, sizeof(weapon_lvl));
   memset(weapon_cost, 0, sizeof(weapon_cost));

   memset(ability_lvl, 0, sizeof(ability_lvl));
   memset(ability_cost, 0, sizeof(ability_cost));

   memset(conditions, 0, sizeof(conditions));

   nAccessLevel = 0;
   pwd[0]       = 0;
   filename[0]  = 0;
}

pc_data::~pc_data()
{
   world_nopc--;

   if(guild != nullptr)
   {
      free(guild);
   }

   if(hometown != nullptr)
   {
      free(hometown);
   }

   if(bank != nullptr)
   {
      free(bank);
   }

   info->free_list();
   quest->free_list();
}
