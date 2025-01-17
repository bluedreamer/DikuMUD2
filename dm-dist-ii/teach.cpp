#include "comm.h"
#include "common.h"
#include "guild.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "skills.h"
#include "spell_info_type.h"
#include "spells.h"
#include "textutil.h"
#include "tree_type.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>

#define PRACTICE_COST_LEVEL (START_LEVEL + 5)

#define TEACH_ABILITIES 0
#define TEACH_SPELLS    1
#define TEACH_SKILLS    2
#define TEACH_WEAPONS   3

/* Use -1 in node to indicate end of teach_type */
struct skill_teach_type
{
   uint8_t max_skill;          /* Maximum skill that can be taught            */
   uint8_t min_level;          /* What level do you have to be to learn this? */
   int    *costs;              /* The point cost (0 ends)                     */
   int     node;               /* A node in a tree               */
   int     min_cost_per_point; /* The gold point cost per point  */
   int     max_cost_per_point; /* The gold point cost per point  */
};

struct teacher_msg
{
   char *unknown_skill;
   char *no_teaching;
   char *not_enough_gold;
   char *not_enough_points;
   char *teacher_not_good_enough;
   char *not_pure;
   char *remove_inventory;
};

struct teach_packet
{
   uint8_t                  type;       /* Ability, spell, skill, weapon */
   uint8_t                  level_type; /* 0 for guild-level, 1 for ordinary level */
   struct teacher_msg       msgs;
   struct skill_teach_type *teaches; /* Array of skills */
   struct tree_type        *tree;
   const char             **text;
};

static auto gold_cost(struct skill_teach_type *s, int level) -> int
{
   if(level < 1)
   {
      return s->min_cost_per_point;
   }
   if(level >= s->max_skill)
   {
      return s->max_cost_per_point;
   }
   int i = s->max_cost_per_point - s->min_cost_per_point;

   i /= s->max_skill;

   i = i * level + s->min_cost_per_point;

   return i;
}

auto actual_training_level(int lvl, const int costs[]) -> int
{
   int i;

   for(i = 0; i < lvl; i++)
   {
      if(costs[i] == 0)
      {
         return i;
      }
   }

   return i;
}

auto actual_cost(int cost, int8_t modifier) -> int
{
   if(cost <= 0)
   {
      return 0;
   }

   /*  +5 gives a 1 point reduction */
   /* +10 gives a 2 point reduction */
   /* +15 gives a 3 point reduction */

   switch(modifier)
   {
      case 1:
         return cost - 1;

      case 2:
         return cost - 2;

      case 3:
         return cost - 4;

      case -1:
         return cost + 1;

      case -2:
         return cost + 2;

      case -3:
         return cost + 4;
   }

   return cost;
}

void clear_training_level(unit_data *ch)
{
   int i;

   assert(IS_PC(ch));

   for(i = 0; i < SPL_TREE_MAX; i++)
   {
      PC_SPL_LVL(ch, i) = 0;
   }

   for(i = 0; i < WPN_TREE_MAX; i++)
   {
      PC_WPN_LVL(ch, i) = 0;
   }

   for(i = 0; i < SKI_TREE_MAX; i++)
   {
      PC_SKI_LVL(ch, i) = 0;
   }

   for(i = 0; i < ABIL_TREE_MAX; i++)
   {
      PC_ABI_LVL(ch, i) = 0;
   }
}

auto teaches_index(struct skill_teach_type *teaches_skills, int node) -> int
{
   int i;

   for(i = 0; teaches_skills[i].node != -1; i++)
   {
      if(teaches_skills[i].node == node)
      {
         return i;
      }
   }

   return -1;
}

void info_show_one(unit_data  *teacher,
                   unit_data  *pupil,
                   uint8_t     current_points,
                   uint8_t     max_level,
                   int         next_point,
                   int         gold,
                   const char *text,
                   int         indent,
                   uint8_t     isleaf,
                   uint8_t     level_type,
                   int         min_level)
{
   char buf[256];

   if(isleaf != 0U)
   {
      if(level_type == 0) /* Guild Level */
      {
         if(!IS_SET(PC_FLAGS(pupil), PC_EXPERT) && char_guild_level(pupil) < min_level)
         {
            sprintf(buf, "%s%-20s [Guild Level %d]\n\r", spc(4 * indent), text, min_level);
            send_to_char(buf, pupil);
            return;
         }
      }
      else
      {
         if(CHAR_LEVEL(pupil) < min_level)
         {
            sprintf(buf, "%s%-20s [Level %d]\n\r", spc(4 * indent), text, min_level);
            send_to_char(buf, pupil);
            return;
         }
      }

      if(next_point == 0)
      {
         sprintf(buf, "%s%-20s [Now %3d%%, Practice next level]\n\r", spc(4 * indent), text, current_points);
      }
      else
      {
         currency_t currency = local_currency(teacher);

         if(IS_SET(PC_FLAGS(pupil), PC_EXPERT))
         {
            sprintf(buf,
                    "%s%-20s [%3d%% of %3d%%, points %2d, guild level %2d, %s]\n\r",
                    spc(4 * indent),
                    text,
                    current_points,
                    max_level,
                    next_point,
                    min_level,
                    money_string(money_round(TRUE, gold, currency, 1), currency, FALSE));
         }
         else
         {
            sprintf(buf, "%s%-20s [Now %3d%%, practice points %3d]\n\r", spc(4 * indent), text, current_points, next_point);
         }
      }
   }
   else
   {
      sprintf(buf, "%s%-20s\n\r", spc(4 * indent), text);
   }

   send_to_char(buf, pupil);
}

void info_show_roots(unit_data               *teacher,
                     unit_data               *pupil,
                     struct skill_teach_type *teaches_skills,
                     struct tree_type        *tree,
                     const char              *text[],
                     uint8_t                  level_type,
                     uint8_t                  pc_values[],
                     uint8_t                  pc_lvl[],
                     int8_t                   pc_cost[])
{
   int i;
   int cost;
   int lvl;

   for(i = 0; teaches_skills[i].node != -1; i++)
   {
      if((!TREE_ISROOT(tree, teaches_skills[i].node) && !TREE_ISLEAF(tree, teaches_skills[i].node)) ||
         ((TREE_ISROOT(tree, teaches_skills[i].node) && TREE_ISLEAF(tree, teaches_skills[i].node))))
      {
         lvl  = actual_training_level(pc_lvl[teaches_skills[i].node], teaches_skills[i].costs);

         cost = actual_cost(teaches_skills[i].costs[lvl], pc_cost[teaches_skills[i].node]);

         info_show_one(teacher,
                       pupil,
                       pc_values[teaches_skills[i].node],
                       teaches_skills[i].max_skill,
                       cost,
                       gold_cost(&teaches_skills[i], pc_values[teaches_skills[i].node]),
                       text[teaches_skills[i].node],
                       0,
                       TREE_ISLEAF(tree, teaches_skills[i].node),
                       level_type,
                       teaches_skills[i].min_level);
      }
   }
}

void info_one_skill(unit_data               *teacher,
                    unit_data               *pupil,
                    struct skill_teach_type *teaches_skills,
                    struct tree_type        *tree,
                    const char              *text[],
                    uint8_t                  pc_values[],
                    uint8_t                  pc_lvl[],
                    int8_t                   pc_cost[],
                    int                      teach_index,
                    uint8_t                  level_type,
                    struct teacher_msg      *msgs)

{
   int indent;
   int i;
   int j;
   int lvl;
   int cost;
   indent = 0;

   /* Find category if index is a leaf with a category parent */

   if(TREE_ISLEAF(tree, teaches_skills[teach_index].node) && !TREE_ISROOT(tree, teaches_skills[teach_index].node))
   {
      i           = TREE_PARENT(tree, teaches_skills[teach_index].node);

      teach_index = teaches_index(teaches_skills, i);
   }

   if(teach_index == -1)
   {
      send_to_char("Something is fishy about this teacher! "
                   "Please contact a god!\n\r",
                   pupil);
      return;
   }

   /* Show teach_index in case it is a category */
   if(!TREE_ISLEAF(tree, teaches_skills[teach_index].node))
   {
      i    = teaches_skills[teach_index].node;
      lvl  = actual_training_level(pc_lvl[i], teaches_skills[teach_index].costs);
      cost = actual_cost(teaches_skills[teach_index].costs[lvl], pc_cost[i]);

      info_show_one(teacher,
                    pupil,
                    pc_values[i],
                    teaches_skills[teach_index].max_skill,
                    cost,
                    gold_cost(&teaches_skills[teach_index], pc_values[i]),
                    text[i],
                    indent++,
                    TREE_ISLEAF(tree, teaches_skills[teach_index].node),
                    level_type,
                    teaches_skills[teach_index].min_level);

      /* Show children of teach_index category */
      for(j = 0; teaches_skills[j].node != -1; j++)
      {
         if(TREE_ISLEAF(tree, teaches_skills[j].node) && (TREE_PARENT(tree, teaches_skills[j].node) == teaches_skills[teach_index].node))
         {
            /* It is a child */
            i    = teaches_skills[j].node;
            lvl  = actual_training_level(pc_lvl[i], teaches_skills[j].costs);
            cost = actual_cost(teaches_skills[j].costs[lvl], pc_cost[i]);
            info_show_one(teacher,
                          pupil,
                          pc_values[i],
                          teaches_skills[j].max_skill,
                          cost,
                          gold_cost(&teaches_skills[j], pc_values[i]),
                          text[i],
                          indent,
                          TREE_ISLEAF(tree, teaches_skills[j].node),
                          level_type,
                          teaches_skills[j].min_level);
         }
      }
   }
   else
   {
      /* Show all leaves, no category above */
      for(j = 0; teaches_skills[j].node != -1; j++)
      {
         if(TREE_ISLEAF(tree, teaches_skills[j].node))
         {
            /* It is a child */
            i    = teaches_skills[j].node;
            lvl  = actual_training_level(pc_lvl[i], teaches_skills[j].costs);
            cost = actual_cost(teaches_skills[j].costs[lvl], pc_cost[i]);

            info_show_one(teacher,
                          pupil,
                          pc_values[i],
                          teaches_skills[j].max_skill,
                          cost,
                          gold_cost(&teaches_skills[j], pc_values[i]),
                          text[i],
                          indent,
                          TREE_ISLEAF(tree, teaches_skills[j].node),
                          level_type,
                          teaches_skills[j].min_level);
         }
      }
   }
}

auto pupil_magic(unit_data *pupil) -> int
{
   unit_affected_type *af;

   for(af = UNIT_AFFECTED(pupil); af != nullptr; af = af->next)
   {
      switch(af->id)
      {
         case ID_BLESS:
         case ID_CURSE:
         case ID_ENERGY_DRAIN:
         case ID_SPL_RAISE_MAG:
         case ID_SPL_RAISE_DIV:
         case ID_SPL_RAISE_STR:
         case ID_SPL_RAISE_DEX:
         case ID_SPL_RAISE_CON:
         case ID_SPL_RAISE_CHA:
         case ID_SPL_RAISE_BRA:
         case ID_SPL_RAISE_HPP:
         case ID_SPL_RAISE_DIVINE:
         case ID_SPL_RAISE_SUMMONING:
         case ID_SPL_RAISE_MIND:
         case ID_SPL_RAISE_HEAT:
         case ID_SPL_RAISE_COLD:
         case ID_SPL_RAISE_CELL:
         case ID_SPL_RAISE_INTERNAL:
         case ID_SPL_RAISE_EXTERNAL:
         case ID_RAISE_MAG:
         case ID_RAISE_DIV:
         case ID_RAISE_STR:
         case ID_RAISE_DEX:
         case ID_RAISE_CON:
         case ID_RAISE_CHA:
         case ID_RAISE_BRA:
         case ID_RAISE_HPP:
         case ID_RAISE_DIVINE:
         case ID_RAISE_MIND:
         case ID_RAISE_HEAT:
         case ID_RAISE_COLD:
         case ID_RAISE_CELL:
         case ID_RAISE_INTERNAL:
         case ID_RAISE_EXTERNAL:

         case ID_SPELL_TRANSFER:
         case ID_SKILL_TRANSFER:
         case ID_WEAPON_TRANSFER:

            return static_cast<int>(TRUE);
      }
   }

   return static_cast<int>(FALSE);
}

auto practice(struct spec_arg     *sarg,
              struct teach_packet *pckt,
              struct tree_type    *tree,
              const char          *text[],
              uint8_t              pc_values[],
              uint8_t              pc_lvl[],
              int8_t               pc_cost[],
              int32_t             *practice_points,
              int                  teach_index) -> int
{
   int        current_points;
   int        cost;
   int        lvl;
   char       buf[512];
   currency_t currency = local_currency(sarg->owner);
   amount_t   amt;

   assert(teach_index != -1);

   if(!TREE_ISLEAF(tree, pckt->teaches[teach_index].node))
   {
      sprintf(buf,
              "It is not possible to practice the category '%s'.\n\r"
              "The category is there to prevent you from being flooded with information.\n\r"
              "Try the command: 'info %s' on the category itself,\n\r"
              "to see which skills it contains.\n\r",
              text[pckt->teaches[teach_index].node],
              text[pckt->teaches[teach_index].node]);
      send_to_char(buf, sarg->activator);
      return static_cast<int>(TRUE);
   }

   if(pckt->teaches[teach_index].max_skill <= pc_values[pckt->teaches[teach_index].node])
   {
      act(pckt->msgs.teacher_not_good_enough, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      return static_cast<int>(TRUE);
   }

   if(pckt->level_type == 0) /* Guild Level */
   {
      if(pckt->teaches[teach_index].min_level > char_guild_level(sarg->activator))
      {
         sprintf(buf,
                 "You need to be at least guild level %d to practice "
                 "%s.\n\r",
                 pckt->teaches[teach_index].min_level,
                 text[pckt->teaches[teach_index].node]);
         send_to_char(buf, sarg->activator);
         return static_cast<int>(TRUE);
      }
   }
   else
   {
      if(pckt->teaches[teach_index].min_level > CHAR_LEVEL(sarg->activator))
      {
         sprintf(buf,
                 "You need to be at least level %d to practice %s.\n\r",
                 pckt->teaches[teach_index].min_level,
                 text[pckt->teaches[teach_index].node]);
         send_to_char(buf, sarg->activator);
         return static_cast<int>(TRUE);
      }
   }

   current_points = pc_values[pckt->teaches[teach_index].node];

   lvl            = actual_training_level(pc_lvl[pckt->teaches[teach_index].node], pckt->teaches[teach_index].costs);

   cost           = actual_cost(pckt->teaches[teach_index].costs[lvl], pc_cost[pckt->teaches[teach_index].node]);

   if(cost == 0)
   {
      act("$1n tells you, 'You've learned all you can about $2t at "
          "this level.'",
          A_ALWAYS,
          sarg->owner,
          text[pckt->teaches[teach_index].node],
          sarg->activator,
          TO_VICT);
      return static_cast<int>(TRUE);
   }

   assert(cost > 0);

   if(*practice_points < cost)
   {
      sprintf(buf, pckt->msgs.not_enough_points, cost);
      act(buf, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      if(CHAR_LEVEL(sarg->activator) == START_LEVEL)
      {
         send_to_char("Beginners note: Go on adventure and gain a level.\n\r"
                      "Then come back and practice afterwards.\n\r",
                      sarg->activator);
      }
      return static_cast<int>(TRUE);
   }

   if(pupil_magic(sarg->activator) != 0)
   {
      act(pckt->msgs.not_pure, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      return static_cast<int>(TRUE);
   }

   /* Ok, now we can teach */

   amt = money_round(TRUE, gold_cost(&pckt->teaches[teach_index], pc_values[pckt->teaches[teach_index].node]), currency, 1);

   if(CHAR_LEVEL(sarg->activator) > PRACTICE_COST_LEVEL &&
      (static_cast<unsigned int>(char_can_afford(sarg->activator, amt, currency)) == 0U))
   {
      sprintf(buf, pckt->msgs.not_enough_gold, money_string(amt, local_currency(sarg->activator), TRUE));
      act(buf, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      return static_cast<int>(TRUE);
   }

   current_points = pc_values[pckt->teaches[teach_index].node];

   *practice_points -= (int)cost;

   pc_lvl[pckt->teaches[teach_index].node]++;

   if(CHAR_LEVEL(sarg->activator) > PRACTICE_COST_LEVEL)
   {
      money_from_unit(sarg->activator, amt, currency);
   }

   if(pckt->type == TEACH_ABILITIES)
   {
      pc_values[pckt->teaches[teach_index].node] += 2;
   }
   else
   {
      pc_values[pckt->teaches[teach_index].node] += 5;
   }

   int idx = pckt->teaches[teach_index].node;

   while(pc_values[idx] > 2 * pc_values[TREE_PARENT(tree, idx)])
   {
      int pidx = TREE_PARENT(tree, idx);

      pc_lvl[pidx]++;
      pc_values[pidx] = pc_values[idx] / 2;

      if(TREE_ISROOT(tree, pidx))
      {
         break;
      }

      idx = TREE_PARENT(tree, idx);
   }

   act("You finish training $2t with $1n.", A_ALWAYS, sarg->owner, text[pckt->teaches[teach_index].node], sarg->activator, TO_VICT);

   return static_cast<int>(FALSE);
}

auto teach_basis(struct spec_arg *sarg, struct teach_packet *pckt) -> int
{
   int      index;
   int      stop;
   uint8_t *pc_values       = nullptr;
   uint8_t *pc_lvl          = nullptr;
   int8_t  *pc_cost         = nullptr;
   int32_t *practice_points = nullptr;
   char     buf[MAX_INPUT_LENGTH];
   char    *arg;

   if(sarg->cmd->no != CMD_INFO && sarg->cmd->no != CMD_PRACTICE)
   {
      return SFR_SHARE;
   }

   if(!IS_PC(sarg->activator))
   {
      send_to_char("That wouldn't be wise.\n\r", sarg->activator);
      return SFR_BLOCK;
   }

   if(!CHAR_IS_READY(sarg->owner))
   {
      act("$1n is not capable of teaching now.", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      return SFR_BLOCK;
   }

   switch(pckt->type)
   {
      case TEACH_ABILITIES:
         pc_values       = &CHAR_ABILITY(sarg->activator, 0);
         pc_lvl          = &PC_ABI_LVL(sarg->activator, 0);
         pc_cost         = &PC_ABI_COST(sarg->activator, 0);
         practice_points = &PC_ABILITY_POINTS(sarg->activator);
         break;

      case TEACH_SKILLS:
         pc_values       = &PC_SKI_SKILL(sarg->activator, 0);
         pc_lvl          = &PC_SKI_LVL(sarg->activator, 0);
         pc_cost         = &PC_SKI_COST(sarg->activator, 0);
         practice_points = &PC_SKILL_POINTS(sarg->activator);
         break;

      case TEACH_SPELLS:
         pc_values       = &PC_SPL_SKILL(sarg->activator, 0);
         pc_lvl          = &PC_SPL_LVL(sarg->activator, 0);
         pc_cost         = &PC_SPL_COST(sarg->activator, 0);
         practice_points = &PC_SKILL_POINTS(sarg->activator);
         break;

      case TEACH_WEAPONS:
         pc_values       = &PC_WPN_SKILL(sarg->activator, 0);
         pc_lvl          = &PC_WPN_LVL(sarg->activator, 0);
         pc_cost         = &PC_SPL_COST(sarg->activator, 0);
         practice_points = &PC_SKILL_POINTS(sarg->activator);
         break;

      default:
         assert(FALSE);
   }

   if(static_cast<unsigned int>(str_is_empty(sarg->arg)) != 0U)
   {
      if(sarg->cmd->no == CMD_INFO)
      {
         info_show_roots(sarg->owner, sarg->activator, pckt->teaches, pckt->tree, pckt->text, pckt->level_type, pc_values, pc_lvl, pc_cost);
         sprintf(buf, "\n\rYou have %lu practice points left.\n\r", (unsigned long)*practice_points);
         send_to_char(buf, sarg->activator);
      }
      else
      {
         act("$1n asks, 'What do you wish to practice, $3n?'", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
         if(CHAR_LEVEL(sarg->activator) == START_LEVEL)
         {
            send_to_char("Beginners note: Try the 'info' command NOW.\n\r", sarg->activator);
         }
      }

      return SFR_BLOCK;
   }

   arg   = skip_spaces(sarg->arg);
   index = search_block_abbrevs(arg, pckt->text, (const char **)&arg);

   if(index == -1)
   {
      act(pckt->msgs.unknown_skill, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      return SFR_BLOCK;
   }

   index = teaches_index(pckt->teaches, index);
   if(index == -1)
   {
      act(pckt->msgs.no_teaching, A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_VICT);
      return SFR_BLOCK;
   }

   if(sarg->cmd->no == CMD_INFO)
   {
      info_one_skill(sarg->owner,
                     sarg->activator,
                     pckt->teaches,
                     pckt->tree,
                     pckt->text,
                     pc_values,
                     pc_lvl,
                     pc_cost,
                     index,
                     pckt->level_type,
                     &pckt->msgs);
      sprintf(buf, "\n\rYou have %lu practice points left.\n\r", (unsigned long)*practice_points);
      send_to_char(buf, sarg->activator);
   }
   else /* Practice! */
   {
      stop                          = practice(sarg, pckt, pckt->tree, pckt->text, pc_values, pc_lvl, pc_cost, practice_points, index);

      /* If hpp changed, then update no of maximum hitpoints */
      UNIT_MAX_HIT(sarg->activator) = hit_limit(sarg->activator);

      info_one_skill(sarg->owner,
                     sarg->activator,
                     pckt->teaches,
                     pckt->tree,
                     pckt->text,
                     pc_values,
                     pc_lvl,
                     pc_cost,
                     index,
                     pckt->level_type,
                     &pckt->msgs);
      sprintf(buf, "\n\rYou have %lu practice points left.\n\r", (unsigned long)*practice_points);
      send_to_char(buf, sarg->activator);
   }

   return SFR_BLOCK;
}

auto teaching(struct spec_arg *sarg) -> int
{
   struct teach_packet *packet;
   int                  i;

   packet = (struct teach_packet *)sarg->fptr->data;
   assert(packet);

   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if(packet->msgs.unknown_skill != nullptr)
      {
         free(packet->msgs.unknown_skill);
      }

      if(packet->msgs.no_teaching != nullptr)
      {
         free(packet->msgs.no_teaching);
      }

      if(packet->msgs.not_enough_gold != nullptr)
      {
         free(packet->msgs.not_enough_gold);
      }

      if(packet->msgs.not_enough_points != nullptr)
      {
         free(packet->msgs.not_enough_points);
      }

      if(packet->msgs.teacher_not_good_enough != nullptr)
      {
         free(packet->msgs.teacher_not_good_enough);
      }

      if(packet->msgs.not_pure != nullptr)
      {
         free(packet->msgs.not_pure);
      }

      if(packet->msgs.remove_inventory != nullptr)
      {
         free(packet->msgs.remove_inventory);
      }

      for(i = 0; packet->teaches[i].node != -1; i++)
      {
         if(packet->teaches[i].costs != nullptr)
         {
            free(packet->teaches[i].costs);
         }
      }

      if(packet->teaches != nullptr)
      {
         free(packet->teaches);
      }

      free(packet);

      sarg->fptr->data = nullptr;
      return SFR_BLOCK;
   }

   return teach_basis(sarg, packet);
}

auto get_next_str(char *data, char *dest) -> char *
{
   while(isspace(*data) != 0)
   {
      data++;
   }

   while((*data != 0) && *data != ';')
   {
      *dest++ = *data++;
   }

   if(*data != 0)
   { /* == ';' */
      data++;
   }

   *dest = 0;

   return data;
}

auto teach_init(struct spec_arg *sarg) -> int
{
   char                   *c;
   int                     i;
   int                     count;
   int                     n;
   int                     realm = -1;
   char                    buf[MAX_STRING_LENGTH];
   int                     points[20];
   struct teach_packet    *packet;
   struct skill_teach_type a_skill;

   static const char      *teach_types[] = {"abilities", "spells", "skills", "weapons", nullptr};

   if(sarg->cmd->no < CMD_NORTH)
   {
      return SFR_SHARE;
   }

   if((c = (char *)sarg->fptr->data) == nullptr)
   {
      szonelog(
         UNIT_FI_ZONE(sarg->owner), "%s@%s: No text data for teacher-init!", UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   c = get_next_str(c, buf); /* Get type */

   if((i = search_block(buf, teach_types, TRUE)) == -1)
   {
      szonelog(UNIT_FI_ZONE(sarg->owner),
               "%s@%s: Illegal teach type in "
               "teacher init!",
               UNIT_FI_NAME(sarg->owner),
               UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   CREATE(packet, struct teach_packet, 1);
   packet->type = i;

   switch(i)
   {
      case TEACH_ABILITIES:
         packet->tree = abil_tree;
         packet->text = abil_text;
         break;

      case TEACH_SKILLS:
         packet->tree = ski_tree;
         packet->text = ski_text;
         break;

      case TEACH_SPELLS:
         packet->tree = spl_tree;
         packet->text = spl_text;
         break;

      case TEACH_WEAPONS:
         packet->tree = wpn_tree;
         packet->text = wpn_text;
         break;

      default:
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Illegal type in "
                  "teacher-init",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         break;
   }

   c = get_next_str(c, buf);
   if(static_cast<unsigned int>(str_is_number(buf)) != 0U)
   {
      packet->level_type = atoi(buf);
   }
   else
   {
      szonelog(UNIT_FI_ZONE(sarg->owner),
               "%s@%s: Illegal level-type in "
               "teacher-init.",
               UNIT_FI_NAME(sarg->owner),
               UNIT_FI_ZONENAME(sarg->owner));
      packet->level_type = 0;
   }

   c                                    = get_next_str(c, buf);
   packet->msgs.unknown_skill           = str_dup(buf);
   c                                    = get_next_str(c, buf);
   packet->msgs.no_teaching             = str_dup(buf);
   c                                    = get_next_str(c, buf);
   packet->msgs.not_enough_gold         = str_dup(buf);
   c                                    = get_next_str(c, buf);
   packet->msgs.not_enough_points       = str_dup(buf);
   c                                    = get_next_str(c, buf);
   packet->msgs.teacher_not_good_enough = str_dup(buf);
   c                                    = get_next_str(c, buf);
   packet->msgs.not_pure                = str_dup(buf);
   c                                    = get_next_str(c, buf);
   packet->msgs.remove_inventory        = str_dup(buf);

   count                                = 1;
   CREATE(packet->teaches, struct skill_teach_type, 1);

   for(;;)
   {
      c = get_next_str(c, buf);
      if(*buf == 0)
      {
         break;
      }

      a_skill.min_level = atoi(buf);
      if(is_in(a_skill.min_level, 0, 150) == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Minimum level expected "
                  "between 1 - 150 (is %s)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner),
                  buf);
         c = str_line(c, buf);
         continue;
      }

      c = get_next_str(c, buf);
      if(*buf == 0)
      {
         break;
      }

      a_skill.max_skill = atoi(buf);

      if(is_in(a_skill.max_skill, 1, 100) == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Maximum skill expected "
                  "between 1 - 100 (is %s)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner),
                  buf);
         c = str_line(c, buf);
         continue;
      }

      c = get_next_str(c, buf);
      if(*buf == 0)
      {
         break;
      }

      strip_trailing_spaces(buf);

      if((i = search_block(buf, packet->text, TRUE)) == -1)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Illegal teacher-init "
                  "skill: %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner),
                  buf);
         c = str_line(c, buf);
         continue;
      }
      a_skill.node = i;

      if(packet->text == spl_text)
      {
         if(realm == -1)
         {
            realm = spell_info[i].realm;
         }
         else if(realm != spell_info[i].realm)
         {
            szonelog(UNIT_FI_ZONE(sarg->owner),
                     "%s@%s: Differing realms in "
                     "%s",
                     UNIT_FI_NAME(sarg->owner),
                     UNIT_FI_ZONENAME(sarg->owner),
                     spl_text[i]);
            c = str_line(c, buf);
            continue;
         }
      }

      /* Get cost */

      c = get_next_str(c, buf);
      if(*buf == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Premature ending of "
                  "teacher-init! (expected cost)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         c = str_line(c, buf);
         continue;
      }

      i = atoi(buf);
      if(i == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Illegal min cost amount "
                  "in teacher init (be > 0): %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner),
                  buf);
         c = str_line(c, buf);
         continue;
      }
      a_skill.min_cost_per_point = i;

      c                          = get_next_str(c, buf);
      if(*buf == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Premature ending of "
                  "teacher-init! (expected cost)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         break;
      }

      i = atoi(buf);
      if(i == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Illegal max cost amount "
                  "in teacher init (be > 0): %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner),
                  buf);
         c = str_line(c, buf);
         continue;
      }
      a_skill.max_cost_per_point = i;

      if(a_skill.max_cost_per_point <= a_skill.min_cost_per_point)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Max amount is <= "
                  "min amount... must be error!: %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner),
                  buf);
         c = str_line(c, buf);
         continue;
      }

      n = 0;

      do
      {
         c = get_next_str(c, buf);
         if(*buf == 0)
         {
            szonelog(UNIT_FI_ZONE(sarg->owner),
                     "%s@%s: Premature ending of "
                     "teacher-init! (expected point)",
                     UNIT_FI_NAME(sarg->owner),
                     UNIT_FI_ZONENAME(sarg->owner));
            c = str_line(c, buf);
            continue;
         }

         i           = atoi(buf);
         points[n++] = i;
      } while((i != 0) && (n < 15));

      if((i != 0) || (*buf == 0))
      {
         break;
      }

      if(n < 1)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Premature ending of "
                  "teacher-init! (expected points cost)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         c = str_line(c, buf);
         continue;
      }

      /* Spheres are inserted automagically */
      if(!TREE_ISLEAF(packet->tree, a_skill.node))
      {
         continue;
      }

      packet->teaches[count - 1] = a_skill;
      CREATE(packet->teaches[count - 1].costs, int, n);
      memcpy(packet->teaches[count - 1].costs, points, n * sizeof(int));
      count++;
      RECREATE(packet->teaches, struct skill_teach_type, count);
   }

   packet->teaches[count - 1].node               = -1;
   packet->teaches[count - 1].min_cost_per_point = -1;
   packet->teaches[count - 1].max_cost_per_point = -1;

   for(i = 0; packet->teaches[i].node != -1; i++)
   {
      if(TREE_ISLEAF(packet->tree, packet->teaches[i].node) && !TREE_ISROOT(packet->tree, packet->teaches[i].node))
      {
         if(teaches_index(packet->teaches, TREE_PARENT(packet->tree, packet->teaches[i].node)) == -1)
         {
            packet->teaches[count - 1].max_skill = 100;
            packet->teaches[count - 1].min_level = 0;
            CREATE(packet->teaches[count - 1].costs, int, 1);
            packet->teaches[count - 1].costs[0]           = 0;
            packet->teaches[count - 1].node               = TREE_PARENT(packet->tree, packet->teaches[i].node);
            packet->teaches[count - 1].min_cost_per_point = 0;
            packet->teaches[count - 1].max_cost_per_point = 0;

            count++;
            RECREATE(packet->teaches, struct skill_teach_type, count);

            packet->teaches[count - 1].node               = -1;
            packet->teaches[count - 1].min_cost_per_point = -1;
            packet->teaches[count - 1].max_cost_per_point = -1;
         }
      }
   }
   packet->teaches[count - 1].node               = -1;
   packet->teaches[count - 1].min_cost_per_point = -1;
   packet->teaches[count - 1].max_cost_per_point = -1;

   sarg->fptr->index                             = SFUN_TEACHING;
   sarg->fptr->heart_beat                        = 0;
   free(sarg->fptr->data); /* Free the text string! */
   sarg->fptr->data = packet;

   /* Call teaching in case initialization occurs with first command! */
   return teaching(sarg);
}

void do_practice(unit_data *ch, char *arg, const struct command_info *cmd)
{
   send_to_char("You can only practice at a teacher.\n\r", ch);
}
