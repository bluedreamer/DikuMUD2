#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "nanny.h"
#include "skills.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "trie.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

#define UT_ROOM (1 << 0)
#define UT_OBJ  (1 << 1)
#define UT_PC   (1 << 2)
#define UT_NPC  (1 << 3)
#define UT_CHAR (UT_NPC | UT_PC)
#define UT_UNIT (UT_CHAR | UT_OBJ | UT_ROOM)

#define AT_VAL    1  /* A value is expected as argument */
#define AT_BIT    2  /* A list of bits is expected as argument */
#define AT_TYP    3  /* A type (char) is expected as argument */
#define AT_STR    4  /* A single line string is expected as argument */
#define AT_DES    5  /* A multi line string is expected as argument */
#define AT_UNT    6  /* A unit-path is expected as argument */
#define AT_KEYDES 7  /* A keyword and a multiline descr. is expected */
#define AT_TYPVAL 8  /* A type followed by a value is expected */
#define AT_DIRBIT 9  /* A direction then a list of bits is expected */
#define AT_TYPDES 10 /* A type then a description is expected */
#define AT_DIRSTR 11 /* A direction then a string is expected */
#define AT_DIRUNT 12 /* A direction then a unit-path is expected */
#define AT_DIRDES 13 /* A direction then a description is expected */

#define MAX_SET_FIELDS 68

struct field_type
{
   uint8_t      utype;     /* type of unit to work on           */
   uint8_t      atype;     /* type of argument to expect        */
   const char **structure; /* structure of bit/type recognition */
   uint8_t      minself;   /* minimum level to modify self      */
   uint8_t      minother;  /* minimum level to modify other     */
   uint8_t      minplayer; /* minimum level to modify player    */
};

static const char *unit_field_names[MAX_SET_FIELDS + 1] = {
   "add-name",   "del-name",   "title",   "descr",        "add-extra",      "del-extra",     "manipulate",  "flags",     "weight",
   "capacity",   "max-hit",    "hit",     "key",          "alignment",      "open-flags",    "toughness",   "lights",    "bright",
   "room-flags", "movement",   "ccinfo",  "add-dir-name", "del-dir-name",   "dir-flags",     "dir-key",     "value0",    "value1",
   "value2",     "value3",     "value4",  "obj-flags",    "cost",           "rent",          "type",        "equip",     "guild-name",
   "pwd",        "pc-flags",   "crimes",  "drunk",        "full",           "thirsty",       "default-pos", "npc-flags", "hometown",
   "exp",        "char-flags", "mana",    "endurance",    "attack-type",    "hand-quality",  "size",        "race",      "sex",
   "level",      "position",   "ability", "skill-points", "ability-points", "remove-affect", "add-quest",   "del-quest", "speed",
   "add-info",   "del-info",   "access",  "age",          "lifespan",       nullptr};

struct field_type unit_field_data[MAX_SET_FIELDS + 1] = {
   {UT_UNIT, AT_STR, nullptr, 200, 200, 253},            /* add-name        */
   {UT_UNIT, AT_STR, nullptr, 200, 200, 253},            /* del-name        */
   {UT_UNIT, AT_STR, nullptr, 200, 200, 200},            /* title           */
   {UT_UNIT, AT_DES, nullptr, 200, 200, 253},            /* long-description*/
   {UT_UNIT, AT_KEYDES, nullptr, 200, 200, 200},         /* add-extra       */
   {UT_UNIT, AT_STR, nullptr, 200, 200, 200},            /* del-extra       */
   {UT_UNIT, AT_BIT, unit_manipulate, 200, 200, 250},    /* manipulate      */
   {UT_UNIT, AT_BIT, unit_flags, 200, 200, 250},         /* unit-flags      */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 230},            /* weight          */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 230},            /* capacity        */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 253},            /* max-hp          */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 253},            /* hp              */
   {UT_UNIT, AT_UNT, nullptr, 200, 200, 253},            /* key             */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 249},            /* alignment       */
   {UT_UNIT, AT_BIT, unit_open_flags, 200, 200, 253},    /* open-flags      */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 253},            /* tgh             */
   {UT_UNIT, AT_VAL, nullptr, 253, 253, 253},            /* lights          */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 253},            /* bright          */
   {UT_ROOM, AT_BIT, room_flags, 200, 200, 200},         /* room-flags      */
   {UT_ROOM, AT_TYP, room_landscape, 200, 200, 200},     /* movement        */
   {UT_PC, AT_VAL, nullptr, 255, 255, 255},              /* cc-info         */
   {UT_ROOM, AT_DIRSTR, nullptr, 200, 200, 200},         /* add-dir-name    */
   {UT_ROOM, AT_DIRSTR, nullptr, 200, 200, 200},         /* del-dir-name    */
   {UT_ROOM, AT_DIRBIT, unit_open_flags, 200, 200, 200}, /* dir-flags       */
   {UT_ROOM, AT_DIRUNT, nullptr, 200, 200, 200},         /* dir-key         */
   {UT_OBJ, AT_VAL, nullptr, 200, 240, 200},             /* value0          */
   {UT_OBJ, AT_VAL, nullptr, 200, 240, 200},             /* value1          */
   {UT_OBJ, AT_VAL, nullptr, 200, 240, 200},             /* value2          */
   {UT_OBJ, AT_VAL, nullptr, 200, 240, 200},             /* value3          */
   {UT_OBJ, AT_VAL, nullptr, 200, 240, 200},             /* value4          */
   {UT_OBJ, AT_BIT, obj_flags, 200, 200, 200},           /* obj-flags       */
   {UT_OBJ, AT_VAL, nullptr, 200, 200, 200},             /* cost            */
   {UT_OBJ, AT_VAL, nullptr, 200, 200, 200},             /* rent            */
   {UT_OBJ, AT_TYP, obj_types, 200, 240, 200},           /* type            */
   {UT_OBJ, AT_TYP, obj_pos, 200, 240, 200},             /* equip           */
   {UT_PC, AT_STR, nullptr, 200, 253, 253},              /* guild-name      */
   {UT_PC, AT_STR, nullptr, 240, 253, 230},              /* pwd             */
   {UT_PC, AT_BIT, pc_flags, 200, 200, 253},             /* pc-flags        */
   {UT_PC, AT_VAL, nullptr, 200, 200, 253},              /* crimes          */
   {UT_PC, AT_VAL, nullptr, 200, 200, 200},              /* drunk           */
   {UT_PC, AT_VAL, nullptr, 200, 200, 200},              /* full            */
   {UT_PC, AT_VAL, nullptr, 200, 200, 200},              /* thirsty         */
   {UT_NPC, AT_VAL, nullptr, 200, 200, 253},             /* default-pos     */
   {UT_NPC, AT_BIT, npc_flags, 200, 200, 200},           /* npc-flags       */
   {UT_PC, AT_STR, nullptr, 200, 230, 230},              /* hometown        */
   {UT_CHAR, AT_VAL, nullptr, 253, 253, 253},            /* exp             */
   {UT_CHAR, AT_BIT, char_flags, 200, 200, 253},         /* char-flags      */
   {UT_CHAR, AT_VAL, nullptr, 200, 200, 240},            /* mana            */
   {UT_CHAR, AT_VAL, nullptr, 200, 200, 240},            /* endurance       */
   {UT_CHAR, AT_TYP, wpn_text, 200, 200, 253},           /* attack-type     */
   {UT_CHAR, AT_VAL, nullptr, 200, 200, 253},            /* hand-quality    */
   {UT_UNIT, AT_VAL, nullptr, 200, 200, 230},            /* height          */
   {UT_CHAR, AT_TYP, pc_races, 200, 200, 253},           /* race            */
   {UT_CHAR, AT_TYP, char_sex, 200, 200, 253},           /* sex             */
   {UT_NPC, AT_VAL, nullptr, 255, 255, 255},             /* level           */
   {UT_CHAR, AT_TYP, char_pos, 200, 253, 253},           /* position        */
   {UT_CHAR, AT_TYPVAL, abil_text, 240, 253, 253},       /* ability         */
   {UT_PC, AT_VAL, nullptr, 230, 253, 253},              /* skill-points    */
   {UT_PC, AT_VAL, nullptr, 230, 253, 253},              /* ability-points  */
   {UT_UNIT, AT_VAL, nullptr, 200, 230, 200},            /* remove affects  */
   {UT_PC, AT_STR, nullptr, 200, 200, 230},              /* add-quest       */
   {UT_PC, AT_STR, nullptr, 200, 200, 230},              /* del-quest       */
   {UT_CHAR, AT_VAL, nullptr, 200, 253, 230},            /* speed           */
   {UT_PC, AT_STR, nullptr, 255, 254, 254},              /* add-info        */
   {UT_PC, AT_STR, nullptr, 255, 254, 254},              /* del-info        */
   {UT_PC, AT_VAL, nullptr, 255, 254, 254},              /* access          */
   {UT_PC, AT_VAL, nullptr, 255, 254, 254},              /* age             */
   {UT_PC, AT_VAL, nullptr, 255, 254, 254}               /* lifespan        */
};

// Post-porcessing of adding-extra descriptions.
void edit_extra(descriptor_data *d)
{
   extra_descr_data *exd;

   for(exd = UNIT_EXTRA_DESCR(d->editing); exd != nullptr; exd = exd->next)
   {
      if(exd == d->editref)
      {
         break; // It still exists!
      }
   }

   if(exd != nullptr)
   {
      exd->descr.Reassign(d->localstr);
   }
}

// Post-porcessing of adding-extra descriptions.
void edit_info(descriptor_data *d)
{
   extra_descr_data *exd;

   for(exd = PC_INFO(d->editing); exd != nullptr; exd = exd->next)
   {
      if(exd == d->editref)
      {
         break; // It still exists!
      }
   }

   if(exd != nullptr)
   {
      exd->descr.Reassign(d->localstr);
   }
}

void edit_outside_descr(descriptor_data *d)
{
   UNIT_OUT_DESCR(d->editing).Reassign(d->localstr);
}

void edit_inside_descr(descriptor_data *d)
{
   UNIT_IN_DESCR(d->editing).Reassign(d->localstr);
}

auto search_block_set(char *arg, const char **list, bool exact) -> int
{
   int i;
   int l;

   /* Substitute '_' and get length of string */
   for(l = 0; arg[l] != 0; l++)
   {
      if(arg[l] == '_')
      {
         arg[l] = ' ';
      }
   }

   if(exact)
   {
      for(i = 0; list[i] != nullptr; i++)
      {
         if(strcmp(arg, list[i]) == 0)
         {
            return i;
         }
      }
   }
   else
   {
      if(l == 0)
      {
         l = 1; /* Avoid "" to match the first available string */
      }

      for(i = 0; list[i] != nullptr; i++)
      {
         if(str_nccmp(arg, list[i], l) == 0)
         { /* no regard of case */
            return i;
         }
      }
   }

   return -1;
}

/* show possible fields */
#define GET_FIELD_UT(c)                                                                                                                    \
   (c) == UT_CHAR   ? "Char"                                                                                                               \
   : (c) == UT_PC   ? "Pc"                                                                                                                 \
   : (c) == UT_NPC  ? "Npc"                                                                                                                \
   : (c) == UT_OBJ  ? "Obj"                                                                                                                \
   : (c) == UT_UNIT ? "Unit"                                                                                                               \
   : (c) == UT_ROOM ? "Room"                                                                                                               \
                    : "Not Used"

#define GET_FIELD_AT(c)                                                                                                                    \
   (c) == AT_VAL      ? "<value>"                                                                                                          \
   : (c) == AT_BIT    ? "<bitlist>"                                                                                                        \
   : (c) == AT_TYP    ? "<type>"                                                                                                           \
   : (c) == AT_STR    ? "<string>"                                                                                                         \
   : (c) == AT_DES    ? "(enter description)"                                                                                              \
   : (c) == AT_UNT    ? "<unitpath>"                                                                                                       \
   : (c) == AT_KEYDES ? "<keyword> (enter description)"                                                                                    \
   : (c) == AT_TYPVAL ? "<type> <value>"                                                                                                   \
   : (c) == AT_DIRBIT ? "<direction> <bitlist>"                                                                                            \
   : (c) == AT_TYPDES ? "<type> (enter description)"                                                                                       \
   : (c) == AT_DIRSTR ? "<direction> <string>"                                                                                             \
   : (c) == AT_DIRUNT ? "<direction> <unitpath>"                                                                                           \
   : (c) == AT_DIRDES ? "<direction> (enter description)"                                                                                  \
                      : "Not usable"

void show_fields(unit_data *ch)
{
   char  string[MAX_STRING_LENGTH];
   char *c;
   int   i;

   for(*string = '\0', i = 0, c = string; i < MAX_SET_FIELDS; i++)
   {
      sprintf(c,
              "%18s : on %5s. : %21s\n\r",
              unit_field_names[i],
              GET_FIELD_UT(unit_field_data[i].utype),
              GET_FIELD_AT(unit_field_data[i].atype));
      TAIL(c);
   }
   send_to_char(string, ch);
}

#undef GET_FIELDT_UT
#undef GET_FIELD_AT

void show_structure(const char *structure[], unit_data *ch)
{
   char **c;
   char  *cc;
   char   buf[MAX_STRING_LENGTH];

   strcpy(buf, "Not defined yet.\n\r");

   for(cc = buf, c = (char **)structure; *c != nullptr; c++)
   {
      sprintf(cc, "%s\n\r", *c);
      TAIL(cc);
   }
   send_to_char(buf, ch);
}

auto get_bit(char *bitlst, const char *structure[]) -> long int
{
   char    *l;
   char    *s;
   char     bit[MAX_STRING_LENGTH];
   long int bitval = 0;
   long int tmpval;

   /* walk through bitlist */
   for(l = bitlst; *l != 0;)
   {
      /* copy bitname */
      for(s = bit; (*l != 0) && *l != '|'; *(s++) = *(l++))
      {
         ;
      }

      *s = 0;
      if(*l != 0)
      {
         l++;
      }

      if(static_cast<unsigned int>(str_is_empty(bit)) == 0U)
      {
         if((tmpval = search_block_set(bit, structure, FALSE)) == -1)
         {
            return -1;
         }
         SET_BIT(bitval, 1 << tmpval);
      }
   }
   return bitval;
}

auto get_type(char *typdef, const char *structure[]) -> int
{
   return search_block_set(typdef, structure, FALSE);
}

/* modification of anything in units */
void do_set(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char                arg[MAX_STRING_LENGTH];
   char                buf[MAX_STRING_LENGTH];
   int                 type;

   char                strarg[MAX_STRING_LENGTH];
   int                 typarg = 0;
   long int            valarg = 0;
   long int            bitarg = 0;

   file_index_type    *untarg = nullptr;
   extra_descr_data   *ed;
   unit_data          *unt;
   unit_affected_type *aff;

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   /* Check argument */
   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Syntax: set <name> <field> <arguments>.\n\r", ch);
      return;
   }

   str_next_word(argument, arg);

   if(str_ccmp(arg, "room") == 0)
   {
      argument = str_next_word(argument, arg);
      unt      = UNIT_IN(ch);
   }
   else
   {
      unt = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD | FIND_UNIT_SURRO | FIND_UNIT_INVEN | FIND_UNIT_EQUIP);
      if(unt == nullptr)
      {
         send_to_char("No such thing.\n\r", ch);
         return;
      }
      /* Temporary fix, as modifying money CAN crash the game!   /gnort */
      if(IS_MONEY(unt) && CHAR_LEVEL(ch) < 255)
      {
         send_to_char("You can't modify money yet, sorry.\n\r", ch);
         return;
      }
   }

   /* find field to change */
   argument = str_next_word(argument, arg);

   /* Partial match on fields */
   if((type = search_block(arg, unit_field_names, FALSE)) == -1)
   {
      send_to_char("Invalid field.\n\r", ch);
      show_fields(ch);
      return;
   }

   /* check if level of users is ok */
   if(CHAR_LEVEL(ch) < unit_field_data[type].minplayer && unt != ch && IS_PC(unt))
   {
      send_to_char("Authority to set field for OTHER PLAYERS denied!\n\r", ch);
      return;
   }

   /* check if level of users is ok */
   if((CHAR_LEVEL(ch) < unit_field_data[type].minother && unt != ch && IS_PC(unt)) ||
      (CHAR_LEVEL(ch) < unit_field_data[type].minself && unt == ch))
   {
      send_to_char("Authority to set field denied!\n\r", ch);
      return;
   }

   /* see if field is valid for unit */
   if((unit_field_data[type].utype == UT_ROOM && !IS_ROOM(unt)) || (unit_field_data[type].utype == UT_OBJ && !IS_OBJ(unt)) ||
      (unit_field_data[type].utype == UT_NPC && !IS_NPC(unt)) || (unit_field_data[type].utype == UT_PC && !IS_PC(unt)) ||
      (unit_field_data[type].utype == UT_CHAR && !IS_CHAR(unt)))
   {
      send_to_char("Field invalid for type of unit.\n\r", ch);
      return;
   }

   argument                    = skip_spaces(argument);

   char mbuf[MAX_INPUT_LENGTH] = {0};

   /* read in field parameters */
   switch(unit_field_data[type].atype)
   {
      case AT_STR:
         if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
         {
            send_to_char("Argument expected.\n\r", ch);
            return;
         }
         break;

      case AT_DES:
         if(unit_is_edited(unt) != nullptr)
         {
            send_to_char("Unit is already being edited.\n\r", ch);
            return;
         }
         CHAR_DESCRIPTOR(ch)->editing = unt;
         /* handle this later */
         break;

      case AT_VAL:
         send_to_char("Arg:<value>\n\r", ch);
         argument = str_next_word(argument, arg);
         if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
         {
            send_to_char("Numeric argument expected.\n\r", ch);
            return;
         }
         valarg = atoi(arg);
         sprintf(buf, "Value is %ld\n\r", valarg);
         send_to_char(buf, ch);
         break;

      case AT_BIT:
         send_to_char("Arg:<bitlist>\n\r", ch);
         argument = str_next_word(argument, arg);
         if((bitarg = get_bit(arg, unit_field_data[type].structure)) == -1)
         {
            send_to_char("Invalid or missing bit for field\n\r\n\r", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
         }
         sprintf(buf, "Bit found is %ld\n\r", bitarg);
         send_to_char(buf, ch);
         break;

      case AT_TYP:
         send_to_char("Arg:<type>\n\r", ch);
         argument = str_next_word(argument, arg);
         if((typarg = get_type(arg, unit_field_data[type].structure)) == -1)
         {
            send_to_char("Invalid or missing type for field.\n\r", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
         }
         sprintf(buf, "Type found is %s [%d]\n\r", unit_field_data[type].structure[typarg], typarg);
         send_to_char(buf, ch);
         break;

      case AT_UNT:
         send_to_char("Arg:<unitpath>\n\r", ch);
         argument = str_next_word(argument, strarg);
         /* find unit by 'path' name in arg */
         if((untarg = str_to_file_index(strarg)) == nullptr)
         {
            send_to_char("Invalid or missing unit path for field.\n\r", ch);
            return;
         }
         sprintf(buf, "Unit pointer is [%s@%s]\n\r", untarg->name, untarg->zone->name);
         send_to_char(buf, ch);
         break;

      case AT_KEYDES:
         send_to_char("Arg:<string> (description)\n\r", ch);
         if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
         {
            send_to_char("Missing string argument.\n\r", ch);
            return;
         }
         if(unit_is_edited(unt) != nullptr)
         {
            send_to_char("Unit is already being edited.\n\r", ch);
            return;
         }
         CHAR_DESCRIPTOR(ch)->editing = unt;
         /* handle takes care of the rest */
         break;

      case AT_TYPVAL:
         send_to_char("Arg:<type> <value>\n\r", ch);
         argument = str_next_word(argument, arg);
         if((typarg = get_type(arg, unit_field_data[type].structure)) == -1)
         {
            send_to_char("Invalid or missing type for field.\n\r", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
         }
         sprintf(buf, "Type found is %s [%d]\n\r", unit_field_data[type].structure[typarg], typarg);
         send_to_char(buf, ch);
         argument = str_next_word(argument, arg);
         if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
         {
            send_to_char("Numeric argument expected.\n\r", ch);
            return;
         }
         valarg = atoi(arg);
         sprintf(buf, "Value is %ld\n\r", valarg);
         send_to_char(buf, ch);
         break;

      case AT_DIRBIT:
         send_to_char("Arg:<direction> <bitlist>\n\r", ch);
         argument = str_next_word(argument, arg);
         if((typarg = get_type(arg, dirs)) == -1)
         {
            send_to_char("Invalid direction.\n\r", ch);
            show_structure(dirs, ch);
            return;
         }
         sprintf(buf, "Direction found is %s [%d]\n\r", dirs[typarg], typarg);
         send_to_char(buf, ch);
         argument = str_next_word(argument, arg);
         if((bitarg = get_bit(arg, unit_field_data[type].structure)) == -1)
         {
            send_to_char("Invalid or missing bit for field.\n\r", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
         }
         sprintf(buf, "Bit found is %ld\n\r", bitarg);
         send_to_char(buf, ch);
         break;

      case AT_DIRSTR:
         send_to_char("Arg:<direction> <string>\n\r", ch);
         argument = str_next_word(argument, arg);
         if((typarg = get_type(arg, dirs)) == -1)
         {
            send_to_char("Invalid direction.\n\r", ch);
            show_structure(dirs, ch);
            return;
         }
         sprintf(buf, "Direction found is %s [%d]\n\r", dirs[typarg], typarg);
         send_to_char(buf, ch);
         if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
         {
            send_to_char("Missing string argument.\n\r", ch);
            return;
         }
         argument = skip_spaces(argument);
         break;

      case AT_DIRUNT:
         send_to_char("Arg:<direction> <unitpath>\n\r", ch);
         argument = str_next_word(argument, arg);
         if((typarg = get_type(arg, dirs)) == -1)
         {
            send_to_char("Invalid or missing direction.\n\r", ch);
            show_structure(dirs, ch);
            return;
         }
         sprintf(buf, "Direction found is %s [%d]\n\r", dirs[typarg], typarg);
         send_to_char(buf, ch);
         argument = str_next_word(argument, arg);
         if((untarg = str_to_file_index(arg)) == nullptr)
         {
            send_to_char("Invalid or missing unit path for field.\n\r", ch);
            return;
         }
         sprintf(buf, "Unit pointer is [%s@%s]\n\r", untarg->name, untarg->zone->name);
         send_to_char(buf, ch);
         break;

      case AT_DIRDES:
         send_to_char("Arg:<direction> (description)\n\r", ch);
         argument = str_next_word(argument, arg);
         if((typarg = get_type(arg, dirs)) == -1)
         {
            send_to_char("Invalid or missing direction.\n\r", ch);
            show_structure(dirs, ch);
            return;
         }
         sprintf(buf, "Direction found is %s [%d]\n\r", dirs[typarg], typarg);
         send_to_char(buf, ch);
         if(unit_is_edited(unt) != nullptr)
         {
            send_to_char("Unit is already being edited.\n\r", ch);
            return;
         }
         CHAR_DESCRIPTOR(ch)->editing = unt;
         /* handle rest later */
         break;

      default:
         send_to_char("Forbidden argument type for field, please "
                      "contact implementators.\n\r",
                      ch);
         return;
   }

   argument = skip_spaces(argument);
   strip_trailing_spaces(argument);

   /* insert data read in argument */
   switch(type)
   {
      case 0: /* "add-name" */
         if(IS_PC(unt) && CHAR_LEVEL(ch) < 255)
         {
            send_to_char("Not allowed to modify PC's.\n\r", ch);
            return;
         }
         argument = skip_spaces(argument);
         strip_trailing_blanks(argument);
         str_remspc(argument);

         UNIT_NAMES(unt).AppendName(argument);
         send_to_char("The extra name was added.\n\r", ch);
         return;

      case 1: /* "del-name" */
         if((UNIT_NAMES(unt).Name(0) == nullptr) || (UNIT_NAMES(unt).Name(1) == nullptr))
         {
            send_to_char("Must have minimum of two names\n\r", ch);
            return;
         }
         argument = skip_spaces(argument);
         UNIT_NAMES(unt).RemoveName(argument);
         send_to_char("Name may have been deleted.\n\r", ch);
         return;

      case 2: /* "title",   */
         UNIT_TITLE(unt).Reassign(argument);

         send_to_char("Title modified.\n\r", ch);
         return;

      case 3: /* "outside description" */
         send_to_char("Modifying long description.\n\r", ch);
         CHAR_DESCRIPTOR(ch)->postedit = edit_outside_descr;
         set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add, TRUE);
         return;

      case 4: /* "add-extra" */
         argument = str_next_word(argument, strarg);
         act("Searching for $2t.", A_ALWAYS, ch, strarg, nullptr, TO_CHAR);

         if((ed = unit_find_extra(strarg, unt)) == nullptr)
         {
            /* the field was not found. create a new one. */
            ed                    = new(class extra_descr_data);
            ed->next              = UNIT_EXTRA_DESCR(unt);
            UNIT_EXTRA_DESCR(unt) = ed;

            while(*strarg != 0) /* insert names */
            {
               ed->names.AppendName(strarg);
               argument = str_next_word(argument, strarg);
            }

            send_to_char("New field.\n\r", ch);
         }
         else
         {
            /* add the rest of the names if they do not exist */
            argument = str_next_word(argument, strarg);
            while(*strarg != 0)
            {
               if(ed->names.IsName(strarg) != nullptr)
               {
                  ed->names.AppendName(strarg);
               }

               argument = str_next_word(argument, strarg);
            }

            send_to_char("Modifying existing description.\n\r", ch);
         }

         CHAR_DESCRIPTOR(ch)->editref  = ed;
         CHAR_DESCRIPTOR(ch)->postedit = edit_extra;
         CHAR_DESCRIPTOR(ch)->editing  = unt;
         set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add, TRUE);
         return;

      case 5: /* "del-extra" */
         if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
         {
            send_to_char("You must supply a field name.\n\r", ch);
            return;
         }

         if((ed = unit_find_extra(argument, unt)) == nullptr)
         {
            send_to_char("No field with that keyword.\n\r", ch);
            return;
         }

         UNIT_EXTRA_DESCR(unt) = UNIT_EXTRA_DESCR(unt)->remove(ed);
         send_to_char("Field deleted.\n\r", ch);
         return;

      case 6: /* "manipulate-flags" */
         UNIT_MANIPULATE(unt) = bitarg;
         return;

      case 7: /* "unit-flags" */
         if(IS_SET(UNIT_FLAGS(unt), UNIT_FL_TRANS) && !IS_SET(bitarg, UNIT_FL_TRANS))
         {
            trans_unset(unt);
         }
         else if(!IS_SET(UNIT_FLAGS(unt), UNIT_FL_TRANS) && IS_SET(bitarg, UNIT_FL_TRANS))
         {
            trans_set(unt);
         }

         UNIT_FLAGS(unt) = bitarg;
         return;

      case 8: /* "weight" */
         UNIT_BASE_WEIGHT(unt) = UNIT_WEIGHT(unt) = valarg;
         return;

      case 9: /* "capacity" */
         UNIT_CAPACITY(unt) = valarg;
         return;

      case 10: /* "max_hp" */
         UNIT_MAX_HIT(unt) = valarg;
         return;

      case 11: /* "hp" */
         UNIT_HIT(unt) = valarg;
         return;

      case 12: /* "key" */
         UNIT_KEY(unt) = untarg;
         return;

      case 13: /* "alignment" */
         if(is_in(valarg, -1000, 1000) != 0)
         {
            UNIT_ALIGNMENT(unt) = valarg;
         }
         else
         {
            send_to_char("Shame on you: Value must be in -1000..+1000!\n\r", ch);
         }
         return;

      case 14: /* "open_flags" */
         UNIT_OPEN_FLAGS(unt) = bitarg;
         return;

      case 15: /* "tgh" OBSOLETE */
         /* UNIT_TGH(unt) = valarg; */
         return;

      case 16: /* "lights" */
         UNIT_LIGHTS(unt) = valarg;
         UNIT_BRIGHT(unt) = valarg;
         UNIT_ILLUM(unt)  = valarg;
         send_to_char("WARNING: This value is absolute and will cause 'darkness' "
                      "bugs if not used properly! Only use this to fix darkness "
                      "- use bright for changing the illumination!\n\r",
                      ch);
         return;

      case 17: /* "bright" */
         if(is_in(valarg, -6, 6) == 0)
         {
            send_to_char("Expected -6..+6\n\r", ch);
            return;
         }
         modify_bright(unt, valarg - UNIT_BRIGHT(unt));
         return;

      case 18: /* "room_flags" */
         ROOM_FLAGS(unt) = bitarg;
         return;

      case 19: /* "movement" */
         ROOM_LANDSCAPE(unt) = typarg;
         return;

      case 20: /* "ccinfo" */
         if(valarg == -1)
         {
            send_to_char("Erasing CC information.\n\r", ch);
            PC_ACCOUNT(unt).last4  = -1;
            PC_ACCOUNT(unt).cracks = 0;
         }
         else
         {
            if(is_in(valarg, 0, 9999) != 0)
            {
               send_to_char("Setting CC information.\n\r", ch);
               PC_ACCOUNT(unt).last4  = valarg;
               PC_ACCOUNT(unt).cracks = 0;
            }
            else
            {
               send_to_char("Illegal value, expected -1 or 0..9999.\n\r", ch);
            }
         }
         return;

      case 21: /* "add-dir-name" */
         if(ROOM_EXIT(unt, typarg) == nullptr)
         {
            send_to_char("No such exit.\n\r", ch);
            return;
         }

         argument = str_next_word(argument, strarg);
         ROOM_EXIT(unt, typarg)->open_name.AppendName(strarg);
         return;

      case 22: /* "del-dir-name" */
         if(ROOM_EXIT(unt, typarg) == nullptr)
         {
            send_to_char("No such exit.\n\r", ch);
            return;
         }

         argument = str_next_word(argument, strarg);
         ROOM_EXIT(unt, typarg)->open_name.AppendName(strarg);
         return;

      case 23: /* "dir-flags" */
         if(ROOM_EXIT(unt, typarg) == nullptr)
         {
            send_to_char("No such exit.\n\r", ch);
            return;
         }

         ROOM_EXIT(unt, typarg)->exit_info = bitarg;
         return;

      case 24: /* "dir-key" */
         if(ROOM_EXIT(unt, typarg) == nullptr)
         {
            send_to_char("No such exit.\n\r", ch);
            return;
         }

         ROOM_EXIT(unt, typarg)->key = untarg;
         return;

      case 25: /* "value0" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }

         /* Should be expanded to handle the different object-types */
         OBJ_VALUE(unt, 0) = valarg;
         return;

      case 26: /* "value1" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }
         /* Should be expanded to handle the different object-types */
         OBJ_VALUE(unt, 1) = valarg;
         return;

      case 27: /* "value2" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }
         /* Should be expanded to handle the different object-types */
         OBJ_VALUE(unt, 2) = valarg;
         return;

      case 28: /* "value3" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }
         /* Should be expanded to handle the different object-types */
         OBJ_VALUE(unt, 3) = valarg;
         return;

      case 29: /* "value4" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }
         /* Should be expanded to handle the different object-types */
         OBJ_VALUE(unt, 4) = valarg;
         return;

      case 30: /* "obj-flags" */
         OBJ_FLAGS(unt) = bitarg;
         return;

      case 31: /* "cost" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }

         OBJ_PRICE(unt) = valarg;
         return;

      case 32: /* "rent" */
         OBJ_PRICE_DAY(unt) = valarg;
         return;

      case 33: /* "type" */
         switch(OBJ_TYPE(unt))
         {
            case ITEM_MONEY:
               send_to_char("Operation not allowed on this type of object.\n\r", ch);
               return;
         }

         if(typarg == ITEM_MONEY)
         {
            send_to_char("Operation not allowed to that kind of object.\n\r", ch);
            return;
         }

         OBJ_TYPE(unt) = typarg;
         return;

      case 34: /* "equip" */
         OBJ_EQP_POS(unt) = typarg;
         return;

      case 35: /* "guild-name" */
         if(str_ccmp_next_word(argument, "none") != nullptr)
         {
            if(PC_GUILD(unt))
            {
               free(PC_GUILD(unt));
               PC_GUILD(unt) = nullptr;
            }
            send_to_char("Changed.\n\r", ch);
            return;
         }
         if(PC_GUILD(unt) == nullptr)
         {
            CREATE(PC_GUILD(unt), char, strlen(argument) + 1);
         }
         else
         {
            RECREATE(PC_GUILD(unt), char, strlen(argument) + 1);
         }
         strcpy(PC_GUILD(unt), argument);
         send_to_char("Changed.\n\r", ch);
         return;

      case 36: /* "pwd" */
         argument = str_next_word(argument, strarg);
         strncpy(PC_PWD(unt), crypt(strarg, UNIT_NAME(unt)), 10);
         return;

      case 37: /* "setup-flags" */
         PC_FLAGS(unt) = bitarg;
         return;

      case 38: /* "crimes" */
         PC_CRIMES(unt) = valarg;
         return;

      case 39: /* "drunk" */
         PC_COND(unt, DRUNK) = valarg;
         return;

      case 40: /* "full" */
         PC_COND(unt, FULL) = valarg;
         return;

      case 41: /* "thirsty" */
         PC_COND(unt, THIRST) = valarg;
         return;

      case 42: /* "default-pos" */
         NPC_DEFAULT(unt) = valarg;
         return;

      case 43: /* "act-flags" */
         NPC_FLAGS(unt) = bitarg;
         return;

      case 44: /* hometown */
         if(PC_HOME(unt) == nullptr)
         {
            CREATE(PC_HOME(unt), char, strlen(argument) + 1);
         }
         else
         {
            RECREATE(PC_HOME(unt), char, strlen(argument) + 1);
         }

         strcpy(PC_HOME(unt), argument);
         send_to_char("Changed.\n\r", ch);
         return;

      case 45: /* "exp" */
         if(CHAR_LEVEL(unt) < MORTAL_MAX_LEVEL && valarg > required_xp(1000))
         {
            send_to_char("You are not allowed to set exp that high for "
                         "mortals\n\r",
                         ch);
         }
         else
         {
            CHAR_EXP(unt) = valarg;
         }
         return;

      case 46: /* "affected-by" */
         CHAR_FLAGS(unt) = bitarg;
         return;

      case 47: /* "mana" */
         CHAR_MANA(unt) = valarg;
         return;

      case 48: /* "endurance" */
         CHAR_ENDURANCE(unt) = valarg;
         return;

      case 49: /* "attack-type" */
         CHAR_ATTACK_TYPE(unt) = typarg;
         return;

      case 50: /* "hand-quality" OBSOLETE */
         /*
            if (is_in(valarg, 0, 200))
            CHAR_HAND_QUALITY(unt) = valarg;
            else
            send_to_char("Shame on you: Value must be in 0%..200%!\n\r", ch); */
         return;

      case 51: /* "height" */
         UNIT_SIZE(unt) = valarg;
         return;

      case 52: /* "race" */
         CHAR_RACE(unt) = typarg;
         return;

      case 53: /* "sex" */
         CHAR_SEX(unt) = typarg;
         return;

      case 54: /* "level" */
         if(is_in(valarg, 0, 199) != 0)
         {
            CHAR_LEVEL(unt) = valarg;
         }
         else
         {
            send_to_char("Shame on you: Value must be in 0..199!\n\r", ch);
         }
         return;

      case 55: /* "position" */
         CHAR_POS(unt) = typarg;
         return;

      case 56: /* "ability" */
         if(is_in(valarg, 0, 200) != 0)
         {
            CHAR_ABILITY(unt, typarg) = valarg;
         }
         else
         {
            send_to_char("Shame on you: Value must be in 0%..200%!\n\r", ch);
         }
         return;

      case 57: /* "skill-points" */
         PC_SKILL_POINTS(unt) = valarg;
         return;

      case 58: /* "ability-points" */
         PC_ABILITY_POINTS(unt) = valarg;
         return;

      case 59: /* "remove-affect" */
         if((aff = affected_by_spell(unt, valarg)) != nullptr)
         {
            destroy_affect(aff);
            send_to_char("Affect attempted removed.\n\r", ch);
         }
         else
         {
            send_to_char("No such affect.\n\r", ch);
         }
         return;

      case 60: /* "add-quest" */
         quest_add(unt, argument, mbuf);

         send_to_char("New quest.\n\r", ch);
         return;

      case 61: /* "del-quest" */
         if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
         {
            send_to_char("You must supply a field name.\n\r", ch);
            return;
         }

         send_to_char("Attempting to remove such a quest.\n\r", ch);

         PC_QUEST(unt) = PC_QUEST(unt)->remove(argument);
         return;

      case 62: /* "speed" */
         if(is_in(valarg, SPEED_MIN, SPEED_MAX) != 0)
         {
            CHAR_SPEED(unt) = valarg;
         }
         else
         {
            send_to_char("Speed must be in [12..200]!\n\r", ch);
         }
         return;

      case 63: /* "add-info" */
         argument = str_next_word(argument, strarg);
         act("Searching for $2t.", A_ALWAYS, ch, strarg, nullptr, TO_CHAR);

         if((ed = PC_INFO(unt)->find_raw(strarg)) == nullptr)
         {
            /* the field was not found. create a new one. */
            ed           = new(class extra_descr_data);
            ed->next     = PC_INFO(unt);
            PC_INFO(unt) = ed;

            while(*strarg != 0) /* insert names */
            {
               ed->names.AppendName(strarg);
               argument = str_next_word(argument, strarg);
            }

            send_to_char("New field.\n\r", ch);
         }
         else
         {
            /* add the rest of the names if they do not exist */
            argument = str_next_word(argument, strarg);
            while(*strarg != 0)
            {
               if(ed->names.IsName(strarg) != nullptr)
               {
                  ed->names.AppendName(strarg);
               }

               argument = str_next_word(argument, strarg);
            }

            send_to_char("Modifying existing description.\n\r", ch);
         }

         CHAR_DESCRIPTOR(ch)->editref  = ed;
         CHAR_DESCRIPTOR(ch)->postedit = edit_info;
         CHAR_DESCRIPTOR(ch)->editing  = unt;
         set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add, TRUE);
         return;

      case 64: /* "del-info" */
         if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
         {
            send_to_char("You must supply a field name.\n\r", ch);
            return;
         }

         send_to_char("Attempting to remove such an info.\n\r", ch);

         PC_INFO(unt) = PC_INFO(unt)->remove(argument);
         return;

      case 65: /* "access" */
         PC_ACCESS_LEVEL(unt) = valarg;
         return;

      case 66: /* "age" */
         /* This sets a time_t so that the PC's age
          * will be as many years as valarg
          */
         if(is_in(valarg, 10, 1000) != 0)
         {
            PC_TIME(unt).birth = time(nullptr) - (valarg * SECS_PER_MUD_YEAR);
         }
         else
         {
            send_to_char("Value must be in 10..1000!\n\r", ch);
         }
         return;

      case 67: /* "lifespan" */
         /* This sets a time_t so that the PC's age
          * will be as many years as valarg
          */
         if(is_in(valarg, 10, 1000) != 0)
         {
            PC_LIFESPAN(unt) = valarg;
         }
         else
         {
            send_to_char("Value must be in 10..1000!\n\r", ch);
         }
         return;

      default: /* undefined field type */
         send_to_char("Field-type is undefined, please contact implementor\n\r", ch);
         return;
   }
}

/*
 *  Modification of character skills
 */

#define SET_SKILL  0
#define SET_SPELL  1
#define SET_WEAPON 2

static const char *skill_field_names[] = {"skill", "spell", "weapon", nullptr};

void               do_setskill(unit_data *ch, char *argument, const struct command_info *cmd)
{
   int        type;
   int        skillarg;
   int        valarg = 0;
   char       buf[MAX_STRING_LENGTH];
   char       arg[MAX_STRING_LENGTH];
   unit_data *unt;

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   /* Check argument */
   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Syntax: setskill <name> (skill|spell|weapon)"
                   " <field> <value>.\n\r",
                   ch);
      return;
   }

   /* find unit to set */
   if((unt = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD)) == nullptr)
   {
      send_to_char("No such person or thing\n\r", ch);
      return;
   }

   if(!IS_CHAR(unt))
   {
      send_to_char("Unit-type must be char\n\r", ch);
      return;
   }

   /* find field to change */
   argument = str_next_word(argument, arg);
   /* Partial match on fields */
   if((type = search_block(arg, skill_field_names, TRUE)) == -1)
   {
      send_to_char("Missing or invalid skill-type, \n\r"
                   "Use 'skill' 'spell' or 'weapon'.\n\r",
                   ch);
      return;
   }

   argument = str_next_word(argument, arg);

   switch(type)
   {
      case SET_SKILL:
         if(!IS_PC(unt))
         {
            send_to_char("Skills are only for PC's\n\r", ch);
            return;
         }
         if((skillarg = get_type(arg, ski_text)) == -1)
         {
            send_to_char("Invalid or missing skill\n\r", ch);
            show_structure(ski_text, ch);
            return;
         }
         argument                    = str_next_word(argument, arg);
         valarg                      = atoi(arg);
         PC_SKI_SKILL(unt, skillarg) = valarg;
         break;

      case SET_SPELL:
         if((skillarg = get_type(arg, spl_text)) == -1)
         {
            send_to_char("Invalid or missing spell\n\r", ch);
            show_structure(spl_text, ch);
            return;
         }
         if(skillarg > SPL_EXTERNAL && IS_NPC(unt))
         {
            send_to_char("Only spell-groups for NPC\n\r", ch);
            return;
         }
         argument = str_next_word(argument, arg);
         valarg   = atoi(arg);

         if(IS_PC(unt))
         {
            PC_SPL_SKILL(unt, skillarg) = valarg;
         }
         else
         {
            NPC_SPL_SKILL(unt, skillarg) = valarg;
         }
         break;

      case SET_WEAPON:
         if((skillarg = get_type(arg, wpn_text)) == -1)
         {
            send_to_char("Invalid or missing weaponskill\n\r", ch);
            show_structure(wpn_text, ch);
            return;
         }
         if(skillarg > WPN_SPECIAL && IS_NPC(unt))
         {
            send_to_char("Only weapon-groups for NPC\n\r", ch);
            return;
         }
         argument = str_next_word(argument, arg);
         valarg   = atoi(arg);

         if(IS_PC(unt))
         {
            PC_WPN_SKILL(unt, skillarg) = valarg;
         }
         else
         {
            NPC_WPN_SKILL(unt, skillarg) = valarg;
         }
   }

   sprintf(buf, "New value: %d\n\rOk.\n\r", valarg);
   send_to_char(buf, ch);
}
