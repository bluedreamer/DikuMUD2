/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* First part of user prologue.  */
#line 39 "dmcpar.y"

#include <ctype.h>
#include <stdio.h>

#include "db_file.h"
#include "dil.h"
#include "dmc.h"
#include "money.h"
#include "textutil.h"
#include "utils.h"

extern char *yytext; /*  MUST be changed to
                      *  `extern char yytext[]'
                      *  if you use lex instead of flex.
                      */
int yylex(void);

#define ZON_DIR_CONT   0
#define ZON_DIR_NEST   1
#define ZON_DIR_UNNEST 2

#define yyerror(dum) syntax(dum)

extern char                cur_filename[];
extern int                 linenum;
extern struct zone_info    zone;
extern int                 nooutput;
struct unit_data          *cur;
struct extra_descr_data   *cur_extra;
struct reset_command      *cur_cmd;
struct unit_affected_type *cur_aff;
struct unit_fptr          *cur_func;
struct diltemplate        *cur_tmpl;
struct dilprg             *cur_prg;

int myi, cur_ex, errcon = 0;

/* Temporary data for stringlists */
int    str_top = 0; // MS2020
char  *str_list[50];
char   empty_ref[] = {'\0', '\0'};
int    istemplate;
char **tmplnames = NULL;

struct dilargstype dilargs;
int                dilarg_top;

/* Temporary data for moneylists */
int mon_top, mon_list[50][2];

struct unit_fptr *mcreate_func(void);
void              dumpdiltemplate(struct diltemplate *tmpl);
void              dumpdil(struct dilprg *prg);

void syntax(const char *str);
void fatal(const char *str);
void warning(const char *str);

#line 132 "y.tab.c"

#ifndef YY_CAST
   #ifdef __cplusplus
      #define YY_CAST(Type, Val)             static_cast<Type>(Val)
      #define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type>(Val)
   #else
      #define YY_CAST(Type, Val)             ((Type)(Val))
      #define YY_REINTERPRET_CAST(Type, Val) ((Type)(Val))
   #endif
#endif
#ifndef YY_NULLPTR
   #if defined __cplusplus
      #if 201103L <= __cplusplus
         #define YY_NULLPTR nullptr
      #else
         #define YY_NULLPTR 0
      #endif
   #else
      #define YY_NULLPTR ((void *)0)
   #endif
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
   #define YY_YY_Y_TAB_H_INCLUDED
   /* Debug traces.  */
   #ifndef YYDEBUG
      #define YYDEBUG 0
   #endif
   #if YYDEBUG
extern int yydebug;
   #endif

   /* Token kinds.  */
   #ifndef YYTOKENTYPE
      #define YYTOKENTYPE
enum yytokentype
{
   YYEMPTY    = -2,
   YYEOF      = 0,   /* "end of file"  */
   YYerror    = 256, /* error  */
   YYUNDEF    = 257, /* "invalid token"  */
   FLAGS      = 258, /* FLAGS  */
   END        = 259, /* END  */
   UNKNOWN    = 260, /* UNKNOWN  */
   OBJECTS    = 261, /* OBJECTS  */
   ZONE       = 262, /* ZONE  */
   RESET      = 263, /* RESET  */
   ROOMS      = 264, /* ROOMS  */
   MOBILES    = 265, /* MOBILES  */
   ENDMARK    = 266, /* ENDMARK  */
   DIL        = 267, /* DIL  */
   DILPRG     = 268, /* DILPRG  */
   STRING     = 269, /* STRING  */
   SYMBOL     = 270, /* SYMBOL  */
   PNUM       = 271, /* PNUM  */
   NAMES      = 272, /* NAMES  */
   TITLE      = 273, /* TITLE  */
   DESCR      = 274, /* DESCR  */
   OUT_DESCR  = 275, /* OUT_DESCR  */
   IN_DESCR   = 276, /* IN_DESCR  */
   EXTRA      = 277, /* EXTRA  */
   KEY        = 278, /* KEY  */
   OPEN       = 279, /* OPEN  */
   MANIPULATE = 280, /* MANIPULATE  */
   UFLAGS     = 281, /* UFLAGS  */
   WEIGHT     = 282, /* WEIGHT  */
   CAPACITY   = 283, /* CAPACITY  */
   SPECIAL    = 284, /* SPECIAL  */
   LIGHT      = 285, /* LIGHT  */
   BRIGHT     = 286, /* BRIGHT  */
   MINV       = 287, /* MINV  */
   RANDOM     = 288, /* RANDOM  */
   DILCOPY    = 289, /* DILCOPY  */
   ID         = 290, /* ID  */
   DATA       = 291, /* DATA  */
   DURATION   = 292, /* DURATION  */
   FIRSTF     = 293, /* FIRSTF  */
   TICKF      = 294, /* TICKF  */
   LASTF      = 295, /* LASTF  */
   APPLYF     = 296, /* APPLYF  */
   TIME       = 297, /* TIME  */
   BITS       = 298, /* BITS  */
   STRINGT    = 299, /* STRINGT  */
   EXIT       = 300, /* EXIT  */
   MOVEMENT   = 301, /* MOVEMENT  */
   IN         = 302, /* IN  */
   SPELL      = 303, /* SPELL  */
   LINK       = 304, /* LINK  */
   TO         = 305, /* TO  */
   KEYWORD    = 306, /* KEYWORD  */
   VALUE      = 307, /* VALUE  */
   COST       = 308, /* COST  */
   RENT       = 309, /* RENT  */
   TYPE       = 310, /* TYPE  */
   AFFECT     = 311, /* AFFECT  */
   MANA       = 312, /* MANA  */
   HIT        = 313, /* HIT  */
   MONEY      = 314, /* MONEY  */
   EXP        = 315, /* EXP  */
   ALIGNMENT  = 316, /* ALIGNMENT  */
   SEX        = 317, /* SEX  */
   LEVEL      = 318, /* LEVEL  */
   HEIGHT     = 319, /* HEIGHT  */
   RACE       = 320, /* RACE  */
   POSITION   = 321, /* POSITION  */
   ABILITY    = 322, /* ABILITY  */
   WEAPON     = 323, /* WEAPON  */
   ATTACK     = 324, /* ATTACK  */
   ARMOUR     = 325, /* ARMOUR  */
   DEFENSIVE  = 326, /* DEFENSIVE  */
   OFFENSIVE  = 327, /* OFFENSIVE  */
   SPEED      = 328, /* SPEED  */
   DEFAULT    = 329, /* DEFAULT  */
   ACT        = 330, /* ACT  */
   RESET_F    = 331, /* RESET_F  */
   LIFESPAN   = 332, /* LIFESPAN  */
   CREATORS   = 333, /* CREATORS  */
   NOTES      = 334, /* NOTES  */
   HELP       = 335, /* HELP  */
   WEATHER    = 336, /* WEATHER  */
   LOAD       = 337, /* LOAD  */
   FOLLOW     = 338, /* FOLLOW  */
   MAX        = 339, /* MAX  */
   ZONEMAX    = 340, /* ZONEMAX  */
   LOCAL      = 341, /* LOCAL  */
   INTO       = 342, /* INTO  */
   NOP        = 343, /* NOP  */
   EQUIP      = 344, /* EQUIP  */
   DOOR       = 345, /* DOOR  */
   PURGE      = 346, /* PURGE  */
   REMOVE     = 347, /* REMOVE  */
   COMPLETE   = 348, /* COMPLETE  */
   SPLUS      = 349, /* SPLUS  */
   UMINUS     = 350, /* UMINUS  */
   UPLUS      = 351  /* UPLUS  */
};
typedef enum yytokentype yytoken_kind_t;
   #endif
   /* Token kinds.  */
   #define YYEMPTY    -2
   #define YYEOF      0
   #define YYerror    256
   #define YYUNDEF    257
   #define FLAGS      258
   #define END        259
   #define UNKNOWN    260
   #define OBJECTS    261
   #define ZONE       262
   #define RESET      263
   #define ROOMS      264
   #define MOBILES    265
   #define ENDMARK    266
   #define DIL        267
   #define DILPRG     268
   #define STRING     269
   #define SYMBOL     270
   #define PNUM       271
   #define NAMES      272
   #define TITLE      273
   #define DESCR      274
   #define OUT_DESCR  275
   #define IN_DESCR   276
   #define EXTRA      277
   #define KEY        278
   #define OPEN       279
   #define MANIPULATE 280
   #define UFLAGS     281
   #define WEIGHT     282
   #define CAPACITY   283
   #define SPECIAL    284
   #define LIGHT      285
   #define BRIGHT     286
   #define MINV       287
   #define RANDOM     288
   #define DILCOPY    289
   #define ID         290
   #define DATA       291
   #define DURATION   292
   #define FIRSTF     293
   #define TICKF      294
   #define LASTF      295
   #define APPLYF     296
   #define TIME       297
   #define BITS       298
   #define STRINGT    299
   #define EXIT       300
   #define MOVEMENT   301
   #define IN         302
   #define SPELL      303
   #define LINK       304
   #define TO         305
   #define KEYWORD    306
   #define VALUE      307
   #define COST       308
   #define RENT       309
   #define TYPE       310
   #define AFFECT     311
   #define MANA       312
   #define HIT        313
   #define MONEY      314
   #define EXP        315
   #define ALIGNMENT  316
   #define SEX        317
   #define LEVEL      318
   #define HEIGHT     319
   #define RACE       320
   #define POSITION   321
   #define ABILITY    322
   #define WEAPON     323
   #define ATTACK     324
   #define ARMOUR     325
   #define DEFENSIVE  326
   #define OFFENSIVE  327
   #define SPEED      328
   #define DEFAULT    329
   #define ACT        330
   #define RESET_F    331
   #define LIFESPAN   332
   #define CREATORS   333
   #define NOTES      334
   #define HELP       335
   #define WEATHER    336
   #define LOAD       337
   #define FOLLOW     338
   #define MAX        339
   #define ZONEMAX    340
   #define LOCAL      341
   #define INTO       342
   #define NOP        343
   #define EQUIP      344
   #define DOOR       345
   #define PURGE      346
   #define REMOVE     347
   #define COMPLETE   348
   #define SPLUS      349
   #define UMINUS     350
   #define UPLUS      351

   /* Value type.  */
   #if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
      #line 102 "dmcpar.y"

   char               *str;
   char                sym[SYMSIZE + 1];
   char              **str_list;
   struct dilargtype  *dilarg;
   struct dilargstype *dilargs;
   int                 num;

      #line 386 "y.tab.c"
};
typedef union YYSTYPE YYSTYPE;
      #define YYSTYPE_IS_TRIVIAL  1
      #define YYSTYPE_IS_DECLARED 1
   #endif

extern YYSTYPE yylval;

int yyparse(void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
   YYSYMBOL_YYEMPTY        = -2,
   YYSYMBOL_YYEOF          = 0,   /* "end of file"  */
   YYSYMBOL_YYerror        = 1,   /* error  */
   YYSYMBOL_YYUNDEF        = 2,   /* "invalid token"  */
   YYSYMBOL_FLAGS          = 3,   /* FLAGS  */
   YYSYMBOL_END            = 4,   /* END  */
   YYSYMBOL_UNKNOWN        = 5,   /* UNKNOWN  */
   YYSYMBOL_OBJECTS        = 6,   /* OBJECTS  */
   YYSYMBOL_ZONE           = 7,   /* ZONE  */
   YYSYMBOL_RESET          = 8,   /* RESET  */
   YYSYMBOL_ROOMS          = 9,   /* ROOMS  */
   YYSYMBOL_MOBILES        = 10,  /* MOBILES  */
   YYSYMBOL_ENDMARK        = 11,  /* ENDMARK  */
   YYSYMBOL_DIL            = 12,  /* DIL  */
   YYSYMBOL_DILPRG         = 13,  /* DILPRG  */
   YYSYMBOL_14_            = 14,  /* '{'  */
   YYSYMBOL_15_            = 15,  /* '}'  */
   YYSYMBOL_16_            = 16,  /* '['  */
   YYSYMBOL_17_            = 17,  /* ']'  */
   YYSYMBOL_18_            = 18,  /* ','  */
   YYSYMBOL_19_            = 19,  /* ';'  */
   YYSYMBOL_STRING         = 20,  /* STRING  */
   YYSYMBOL_SYMBOL         = 21,  /* SYMBOL  */
   YYSYMBOL_PNUM           = 22,  /* PNUM  */
   YYSYMBOL_NAMES          = 23,  /* NAMES  */
   YYSYMBOL_TITLE          = 24,  /* TITLE  */
   YYSYMBOL_DESCR          = 25,  /* DESCR  */
   YYSYMBOL_OUT_DESCR      = 26,  /* OUT_DESCR  */
   YYSYMBOL_IN_DESCR       = 27,  /* IN_DESCR  */
   YYSYMBOL_EXTRA          = 28,  /* EXTRA  */
   YYSYMBOL_KEY            = 29,  /* KEY  */
   YYSYMBOL_OPEN           = 30,  /* OPEN  */
   YYSYMBOL_MANIPULATE     = 31,  /* MANIPULATE  */
   YYSYMBOL_UFLAGS         = 32,  /* UFLAGS  */
   YYSYMBOL_WEIGHT         = 33,  /* WEIGHT  */
   YYSYMBOL_CAPACITY       = 34,  /* CAPACITY  */
   YYSYMBOL_SPECIAL        = 35,  /* SPECIAL  */
   YYSYMBOL_LIGHT          = 36,  /* LIGHT  */
   YYSYMBOL_BRIGHT         = 37,  /* BRIGHT  */
   YYSYMBOL_MINV           = 38,  /* MINV  */
   YYSYMBOL_RANDOM         = 39,  /* RANDOM  */
   YYSYMBOL_DILCOPY        = 40,  /* DILCOPY  */
   YYSYMBOL_ID             = 41,  /* ID  */
   YYSYMBOL_DATA           = 42,  /* DATA  */
   YYSYMBOL_DURATION       = 43,  /* DURATION  */
   YYSYMBOL_FIRSTF         = 44,  /* FIRSTF  */
   YYSYMBOL_TICKF          = 45,  /* TICKF  */
   YYSYMBOL_LASTF          = 46,  /* LASTF  */
   YYSYMBOL_APPLYF         = 47,  /* APPLYF  */
   YYSYMBOL_TIME           = 48,  /* TIME  */
   YYSYMBOL_BITS           = 49,  /* BITS  */
   YYSYMBOL_STRINGT        = 50,  /* STRINGT  */
   YYSYMBOL_EXIT           = 51,  /* EXIT  */
   YYSYMBOL_MOVEMENT       = 52,  /* MOVEMENT  */
   YYSYMBOL_IN             = 53,  /* IN  */
   YYSYMBOL_SPELL          = 54,  /* SPELL  */
   YYSYMBOL_LINK           = 55,  /* LINK  */
   YYSYMBOL_TO             = 56,  /* TO  */
   YYSYMBOL_KEYWORD        = 57,  /* KEYWORD  */
   YYSYMBOL_VALUE          = 58,  /* VALUE  */
   YYSYMBOL_COST           = 59,  /* COST  */
   YYSYMBOL_RENT           = 60,  /* RENT  */
   YYSYMBOL_TYPE           = 61,  /* TYPE  */
   YYSYMBOL_AFFECT         = 62,  /* AFFECT  */
   YYSYMBOL_MANA           = 63,  /* MANA  */
   YYSYMBOL_HIT            = 64,  /* HIT  */
   YYSYMBOL_MONEY          = 65,  /* MONEY  */
   YYSYMBOL_EXP            = 66,  /* EXP  */
   YYSYMBOL_ALIGNMENT      = 67,  /* ALIGNMENT  */
   YYSYMBOL_SEX            = 68,  /* SEX  */
   YYSYMBOL_LEVEL          = 69,  /* LEVEL  */
   YYSYMBOL_HEIGHT         = 70,  /* HEIGHT  */
   YYSYMBOL_RACE           = 71,  /* RACE  */
   YYSYMBOL_POSITION       = 72,  /* POSITION  */
   YYSYMBOL_ABILITY        = 73,  /* ABILITY  */
   YYSYMBOL_WEAPON         = 74,  /* WEAPON  */
   YYSYMBOL_ATTACK         = 75,  /* ATTACK  */
   YYSYMBOL_ARMOUR         = 76,  /* ARMOUR  */
   YYSYMBOL_DEFENSIVE      = 77,  /* DEFENSIVE  */
   YYSYMBOL_OFFENSIVE      = 78,  /* OFFENSIVE  */
   YYSYMBOL_SPEED          = 79,  /* SPEED  */
   YYSYMBOL_DEFAULT        = 80,  /* DEFAULT  */
   YYSYMBOL_ACT            = 81,  /* ACT  */
   YYSYMBOL_RESET_F        = 82,  /* RESET_F  */
   YYSYMBOL_LIFESPAN       = 83,  /* LIFESPAN  */
   YYSYMBOL_CREATORS       = 84,  /* CREATORS  */
   YYSYMBOL_NOTES          = 85,  /* NOTES  */
   YYSYMBOL_HELP           = 86,  /* HELP  */
   YYSYMBOL_WEATHER        = 87,  /* WEATHER  */
   YYSYMBOL_LOAD           = 88,  /* LOAD  */
   YYSYMBOL_FOLLOW         = 89,  /* FOLLOW  */
   YYSYMBOL_MAX            = 90,  /* MAX  */
   YYSYMBOL_ZONEMAX        = 91,  /* ZONEMAX  */
   YYSYMBOL_LOCAL          = 92,  /* LOCAL  */
   YYSYMBOL_INTO           = 93,  /* INTO  */
   YYSYMBOL_NOP            = 94,  /* NOP  */
   YYSYMBOL_EQUIP          = 95,  /* EQUIP  */
   YYSYMBOL_DOOR           = 96,  /* DOOR  */
   YYSYMBOL_PURGE          = 97,  /* PURGE  */
   YYSYMBOL_REMOVE         = 98,  /* REMOVE  */
   YYSYMBOL_COMPLETE       = 99,  /* COMPLETE  */
   YYSYMBOL_100_           = 100, /* '+'  */
   YYSYMBOL_101_           = 101, /* '-'  */
   YYSYMBOL_102_           = 102, /* '*'  */
   YYSYMBOL_103_           = 103, /* '/'  */
   YYSYMBOL_SPLUS          = 104, /* SPLUS  */
   YYSYMBOL_UMINUS         = 105, /* UMINUS  */
   YYSYMBOL_UPLUS          = 106, /* UPLUS  */
   YYSYMBOL_107_           = 107, /* '('  */
   YYSYMBOL_108_           = 108, /* ')'  */
   YYSYMBOL_109_           = 109, /* '@'  */
   YYSYMBOL_YYACCEPT       = 110, /* $accept  */
   YYSYMBOL_file           = 111, /* file  */
   YYSYMBOL_112_1          = 112, /* $@1  */
   YYSYMBOL_sections       = 113, /* sections  */
   YYSYMBOL_uglykludge     = 114, /* uglykludge  */
   YYSYMBOL_dil_section    = 115, /* dil_section  */
   YYSYMBOL_116_2          = 116, /* $@2  */
   YYSYMBOL_dils           = 117, /* dils  */
   YYSYMBOL_dil            = 118, /* dil  */
   YYSYMBOL_room_section   = 119, /* room_section  */
   YYSYMBOL_120_3          = 120, /* $@3  */
   YYSYMBOL_rooms          = 121, /* rooms  */
   YYSYMBOL_122_4          = 122, /* $@4  */
   YYSYMBOL_room           = 123, /* room  */
   YYSYMBOL_room_fields    = 124, /* room_fields  */
   YYSYMBOL_room_field     = 125, /* room_field  */
   YYSYMBOL_oroom_field    = 126, /* oroom_field  */
   YYSYMBOL_exitindex      = 127, /* exitindex  */
   YYSYMBOL_exit_fields    = 128, /* exit_fields  */
   YYSYMBOL_exit_field     = 129, /* exit_field  */
   YYSYMBOL_object_section = 130, /* object_section  */
   YYSYMBOL_objects        = 131, /* objects  */
   YYSYMBOL_132_5          = 132, /* $@5  */
   YYSYMBOL_object         = 133, /* object  */
   YYSYMBOL_object_fields  = 134, /* object_fields  */
   YYSYMBOL_object_field   = 135, /* object_field  */
   YYSYMBOL_oobject_field  = 136, /* oobject_field  */
   YYSYMBOL_mobile_section = 137, /* mobile_section  */
   YYSYMBOL_mobiles        = 138, /* mobiles  */
   YYSYMBOL_139_6          = 139, /* $@6  */
   YYSYMBOL_mobile         = 140, /* mobile  */
   YYSYMBOL_mobile_fields  = 141, /* mobile_fields  */
   YYSYMBOL_mobile_field   = 142, /* mobile_field  */
   YYSYMBOL_omobile_field  = 143, /* omobile_field  */
   YYSYMBOL_unit_field     = 144, /* unit_field  */
   YYSYMBOL_145_7          = 145, /* $@7  */
   YYSYMBOL_146_8          = 146, /* $@8  */
   YYSYMBOL_147_9          = 147, /* $@9  */
   YYSYMBOL_optfuncargs    = 148, /* optfuncargs  */
   YYSYMBOL_optfuncarg     = 149, /* optfuncarg  */
   YYSYMBOL_affect_fields  = 150, /* affect_fields  */
   YYSYMBOL_affect_field   = 151, /* affect_field  */
   YYSYMBOL_dilargs        = 152, /* dilargs  */
   YYSYMBOL_dilarg         = 153, /* dilarg  */
   YYSYMBOL_zone_section   = 154, /* zone_section  */
   YYSYMBOL_zone_fields    = 155, /* zone_fields  */
   YYSYMBOL_zone_field     = 156, /* zone_field  */
   YYSYMBOL_reset_section  = 157, /* reset_section  */
   YYSYMBOL_158_10         = 158, /* $@10  */
   YYSYMBOL_reset_commands = 159, /* reset_commands  */
   YYSYMBOL_reset_command  = 160, /* reset_command  */
   YYSYMBOL_161_11         = 161, /* $@11  */
   YYSYMBOL_block          = 162, /* block  */
   YYSYMBOL_alloc          = 163, /* alloc  */
   YYSYMBOL_command        = 164, /* command  */
   YYSYMBOL_loadfields     = 165, /* loadfields  */
   YYSYMBOL_loadfield      = 166, /* loadfield  */
   YYSYMBOL_followfields   = 167, /* followfields  */
   YYSYMBOL_followfield    = 168, /* followfield  */
   YYSYMBOL_equipfields    = 169, /* equipfields  */
   YYSYMBOL_equipfield     = 170, /* equipfield  */
   YYSYMBOL_local          = 171, /* local  */
   YYSYMBOL_zonemax        = 172, /* zonemax  */
   YYSYMBOL_max            = 173, /* max  */
   YYSYMBOL_position       = 174, /* position  */
   YYSYMBOL_moneylist      = 175, /* moneylist  */
   YYSYMBOL_moneytype      = 176, /* moneytype  */
   YYSYMBOL_stringlist     = 177, /* stringlist  */
   YYSYMBOL_hardstringlist = 178, /* hardstringlist  */
   YYSYMBOL_strings        = 179, /* strings  */
   YYSYMBOL_stringcomp     = 180, /* stringcomp  */
   YYSYMBOL_flags          = 181, /* flags  */
   YYSYMBOL_numbers        = 182, /* numbers  */
   YYSYMBOL_number         = 183, /* number  */
   YYSYMBOL_expr           = 184, /* expr  */
   YYSYMBOL_term           = 185, /* term  */
   YYSYMBOL_factor         = 186, /* factor  */
   YYSYMBOL_zonename       = 187, /* zonename  */
   YYSYMBOL_czonename      = 188, /* czonename  */
   YYSYMBOL_unitname       = 189, /* unitname  */
   YYSYMBOL_cunitname      = 190, /* cunitname  */
   YYSYMBOL_reference      = 191, /* reference  */
   YYSYMBOL_index          = 192  /* index  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;

#ifdef short
   #undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
   #include <limits.h> /* INFRINGES ON USER NAME SPACE */
   #if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
      #include <stdint.h> /* INFRINGES ON USER NAME SPACE */
      #define YY_STDINT_H
   #endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined               YY_STDINT_H
typedef int_least8_t   yytype_int8;
#else
typedef signed char    yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined                YY_STDINT_H
typedef int_least16_t  yytype_int16;
#else
typedef short          yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
   #undef UINT_LEAST8_MAX
   #undef UINT_LEAST16_MAX
   #define UINT_LEAST8_MAX  255
   #define UINT_LEAST16_MAX 65535
#endif

#if defined                  __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif(!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t  yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char  yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined                   __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif(!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int   yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
   #if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
      #define YYPTRDIFF_T       __PTRDIFF_TYPE__
      #define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
   #elif defined PTRDIFF_MAX
      #ifndef ptrdiff_t
         #include <stddef.h> /* INFRINGES ON USER NAME SPACE */
      #endif
      #define YYPTRDIFF_T       ptrdiff_t
      #define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
   #else
      #define YYPTRDIFF_T       long
      #define YYPTRDIFF_MAXIMUM LONG_MAX
   #endif
#endif

#ifndef YYSIZE_T
   #ifdef __SIZE_TYPE__
      #define YYSIZE_T __SIZE_TYPE__
   #elif defined size_t
      #define YYSIZE_T size_t
   #elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
      #include <stddef.h> /* INFRINGES ON USER NAME SPACE */
      #define YYSIZE_T size_t
   #else
      #define YYSIZE_T unsigned
   #endif
#endif

#define YYSIZE_MAXIMUM YY_CAST(YYPTRDIFF_T, (YYPTRDIFF_MAXIMUM < YY_CAST(YYSIZE_T, -1) ? YYPTRDIFF_MAXIMUM : YY_CAST(YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST(YYPTRDIFF_T, sizeof(X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
   #if defined YYENABLE_NLS && YYENABLE_NLS
      #if ENABLE_NLS
         #include <libintl.h> /* INFRINGES ON USER NAME SPACE */
         #define YY_(Msgid) dgettext("bison-runtime", Msgid)
      #endif
   #endif
   #ifndef YY_
      #define YY_(Msgid) Msgid
   #endif
#endif

#ifndef YY_ATTRIBUTE_PURE
   #if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
      #define YY_ATTRIBUTE_PURE __attribute__((__pure__))
   #else
      #define YY_ATTRIBUTE_PURE
   #endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
   #if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
      #define YY_ATTRIBUTE_UNUSED __attribute__((__unused__))
   #else
      #define YY_ATTRIBUTE_UNUSED
   #endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
   #define YY_USE(E) ((void)(E))
#else
   #define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && !defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
   /* Suppress an incorrect diagnostic about yylval being uninitialized.  */
   #define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                                                                             \
      _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")                                                 \
         _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
   #define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
   #define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   #define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   #define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
   #define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && !defined __ICC && 6 <= __GNUC__
   #define YY_IGNORE_USELESS_CAST_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")
   #define YY_IGNORE_USELESS_CAST_END   _Pragma("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
   #define YY_IGNORE_USELESS_CAST_BEGIN
   #define YY_IGNORE_USELESS_CAST_END
#endif

#define YY_ASSERT(E) ((void)(0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

   #ifdef YYSTACK_USE_ALLOCA
      #if YYSTACK_USE_ALLOCA
         #ifdef __GNUC__
            #define YYSTACK_ALLOC __builtin_alloca
         #elif defined __BUILTIN_VA_ARG_INCR
            #include <alloca.h> /* INFRINGES ON USER NAME SPACE */
         #elif defined _AIX
            #define YYSTACK_ALLOC __alloca
         #elif defined _MSC_VER
            #include <malloc.h> /* INFRINGES ON USER NAME SPACE */
            #define alloca _alloca
         #else
            #define YYSTACK_ALLOC alloca
            #if !defined _ALLOCA_H && !defined EXIT_SUCCESS
               #include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
            /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
               #ifndef EXIT_SUCCESS
                  #define EXIT_SUCCESS 0
               #endif
            #endif
         #endif
      #endif
   #endif

   #ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
      #define YYSTACK_FREE(Ptr)                                                                                                            \
         do                                                                                                                                \
         { /* empty */                                                                                                                     \
            ;                                                                                                                              \
         } while(0)
      #ifndef YYSTACK_ALLOC_MAXIMUM
      /* The OS might guarantee only one guard page at the bottom of the stack,
         and a page size can be as small as 4096 bytes.  So we cannot safely
         invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
         to allow for a few compiler-allocated temporary stack slots.  */
         #define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
      #endif
   #else
      #define YYSTACK_ALLOC YYMALLOC
      #define YYSTACK_FREE  YYFREE
      #ifndef YYSTACK_ALLOC_MAXIMUM
         #define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
      #endif
      #if(defined __cplusplus && !defined EXIT_SUCCESS && !((defined YYMALLOC || defined malloc) && (defined YYFREE || defined free)))
         #include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
         #ifndef EXIT_SUCCESS
            #define EXIT_SUCCESS 0
         #endif
      #endif
      #ifndef YYMALLOC
         #define YYMALLOC malloc
         #if !defined malloc && !defined EXIT_SUCCESS
void *malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
         #endif
      #endif
      #ifndef YYFREE
         #define YYFREE free
         #if !defined free && !defined EXIT_SUCCESS
void  free(void *);     /* INFRINGES ON USER NAME SPACE */
         #endif
      #endif
   #endif
#endif /* !defined yyoverflow */

#if(!defined yyoverflow && (!defined __cplusplus || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
   yy_state_t yyss_alloc;
   YYSTYPE    yyvs_alloc;
};

   /* The size of the maximum gap between one aligned stack and the next.  */
   #define YYSTACK_GAP_MAXIMUM (YYSIZEOF(union yyalloc) - 1)

   /* The size of an array large to enough to hold all stacks, each with
      N elements.  */
   #define YYSTACK_BYTES(N) ((N) * (YYSIZEOF(yy_state_t) + YYSIZEOF(YYSTYPE)) + YYSTACK_GAP_MAXIMUM)

   #define YYCOPY_NEEDED 1

   /* Relocate STACK from its old location to the new one.  The
      local variables YYSIZE and YYSTACKSIZE give the old and new number of
      elements in the stack, and YYPTR gives the new location of the
      stack.  Advance YYPTR to a properly aligned location for the next
      stack.  */
   #define YYSTACK_RELOCATE(Stack_alloc, Stack)                                                                                            \
      do                                                                                                                                   \
      {                                                                                                                                    \
         YYPTRDIFF_T yynewbytes;                                                                                                           \
         YYCOPY(&yyptr->Stack_alloc, Stack, yysize);                                                                                       \
         Stack      = &yyptr->Stack_alloc;                                                                                                 \
         yynewbytes = yystacksize * YYSIZEOF(*Stack) + YYSTACK_GAP_MAXIMUM;                                                                \
         yyptr += yynewbytes / YYSIZEOF(*yyptr);                                                                                           \
      } while(0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
   /* Copy COUNT objects from SRC to DST.  The source and destination do
      not overlap.  */
   #ifndef YYCOPY
      #if defined __GNUC__ && 1 < __GNUC__
         #define YYCOPY(Dst, Src, Count) __builtin_memcpy(Dst, Src, YY_CAST(YYSIZE_T, (Count)) * sizeof(*(Src)))
      #else
         #define YYCOPY(Dst, Src, Count)                                                                                                   \
            do                                                                                                                             \
            {                                                                                                                              \
               YYPTRDIFF_T yyi;                                                                                                            \
               for(yyi = 0; yyi < (Count); yyi++)                                                                                          \
                  (Dst)[yyi] = (Src)[yyi];                                                                                                 \
            } while(0)
      #endif
   #endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 479

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 110
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 83
/* YYNRULES -- Number of rules.  */
#define YYNRULES 218
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 355

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK 351

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX) (0 <= (YYX) && (YYX) <= YYMAXUTOK ? YY_CAST(yysymbol_kind_t, yytranslate[YYX]) : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] = {
   0,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  107, 108, 102, 100, 18, 101, 2,  103, 2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  19,
   2,  2,  2,  2,  109, 2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  16, 2,  17, 2,   2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  2,  2,  14, 2,   15, 2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   2,  2,   2,  2,   2,   2,  2,  2,  2,  2,  2,  2,  2,  2,
   2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,   2,   2,   2,   2,  2,   1,  2,   3,  4,   5,   6,  7,  8,  9,  10, 11, 12, 13, 20,
   21, 22, 23, 24, 25,  26, 27, 28, 29, 30, 31,  32,  33,  34,  35, 36,  37, 38,  39, 40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50,
   51, 52, 53, 54, 55,  56, 57, 58, 59, 60, 61,  62,  63,  64,  65, 66,  67, 68,  69, 70,  71,  72, 73, 74, 75, 76, 77, 78, 79, 80,
   81, 82, 83, 84, 85,  86, 87, 88, 89, 90, 91,  92,  93,  94,  95, 96,  97, 98,  99, 104, 105, 106};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] = {
   0,    167,  167,  167,  169,  173,  174,  175,  183,  182,  193,  196,  197,  200,  259,  259,  260,  263,  265,  264,  280,  285,
   286,  288,  289,  290,  292,  296,  300,  304,  308,  316,  321,  322,  324,  329,  334,  339,  343,  370,  373,  375,  374,  390,
   395,  396,  398,  399,  400,  402,  406,  410,  414,  418,  422,  431,  434,  436,  435,  451,  456,  457,  459,  460,  461,  463,
   467,  471,  488,  492,  496,  500,  504,  508,  512,  516,  520,  524,  528,  532,  536,  540,  544,  554,  560,  634,  658,  662,
   666,  679,  694,  698,  718,  722,  726,  730,  734,  738,  742,  746,  754,  758,  762,  767,  780,  766,  808,  807,  822,  823,
   825,  833,  839,  843,  848,  849,  851,  855,  859,  863,  867,  871,  875,  879,  889,  893,  899,  907,  917,  927,  939,  942,
   943,  945,  949,  953,  957,  961,  965,  969,  973,  977,  984,  983,  988,  990,  991,  993,  998,  997,  1003, 1020, 1040, 1049,
   1058, 1065, 1072, 1077, 1083, 1090, 1095, 1096, 1098, 1102, 1103, 1104, 1105, 1110, 1111, 1113, 1114, 1115, 1116, 1121, 1122, 1124,
   1125, 1129, 1134, 1139, 1144, 1149, 1150, 1159, 1167, 1178, 1195, 1199, 1207, 1219, 1225, 1229, 1236, 1240, 1251, 1266, 1283, 1287,
   1292, 1296, 1303, 1308, 1312, 1316, 1320, 1324, 1329, 1333, 1337, 1348, 1352, 1358, 1364, 1374, 1380, 1390, 1405, 1411, 1418};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST(yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name(yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] = {"\"end of file\"",
                                      "error",
                                      "\"invalid token\"",
                                      "FLAGS",
                                      "END",
                                      "UNKNOWN",
                                      "OBJECTS",
                                      "ZONE",
                                      "RESET",
                                      "ROOMS",
                                      "MOBILES",
                                      "ENDMARK",
                                      "DIL",
                                      "DILPRG",
                                      "'{'",
                                      "'}'",
                                      "'['",
                                      "']'",
                                      "','",
                                      "';'",
                                      "STRING",
                                      "SYMBOL",
                                      "PNUM",
                                      "NAMES",
                                      "TITLE",
                                      "DESCR",
                                      "OUT_DESCR",
                                      "IN_DESCR",
                                      "EXTRA",
                                      "KEY",
                                      "OPEN",
                                      "MANIPULATE",
                                      "UFLAGS",
                                      "WEIGHT",
                                      "CAPACITY",
                                      "SPECIAL",
                                      "LIGHT",
                                      "BRIGHT",
                                      "MINV",
                                      "RANDOM",
                                      "DILCOPY",
                                      "ID",
                                      "DATA",
                                      "DURATION",
                                      "FIRSTF",
                                      "TICKF",
                                      "LASTF",
                                      "APPLYF",
                                      "TIME",
                                      "BITS",
                                      "STRINGT",
                                      "EXIT",
                                      "MOVEMENT",
                                      "IN",
                                      "SPELL",
                                      "LINK",
                                      "TO",
                                      "KEYWORD",
                                      "VALUE",
                                      "COST",
                                      "RENT",
                                      "TYPE",
                                      "AFFECT",
                                      "MANA",
                                      "HIT",
                                      "MONEY",
                                      "EXP",
                                      "ALIGNMENT",
                                      "SEX",
                                      "LEVEL",
                                      "HEIGHT",
                                      "RACE",
                                      "POSITION",
                                      "ABILITY",
                                      "WEAPON",
                                      "ATTACK",
                                      "ARMOUR",
                                      "DEFENSIVE",
                                      "OFFENSIVE",
                                      "SPEED",
                                      "DEFAULT",
                                      "ACT",
                                      "RESET_F",
                                      "LIFESPAN",
                                      "CREATORS",
                                      "NOTES",
                                      "HELP",
                                      "WEATHER",
                                      "LOAD",
                                      "FOLLOW",
                                      "MAX",
                                      "ZONEMAX",
                                      "LOCAL",
                                      "INTO",
                                      "NOP",
                                      "EQUIP",
                                      "DOOR",
                                      "PURGE",
                                      "REMOVE",
                                      "COMPLETE",
                                      "'+'",
                                      "'-'",
                                      "'*'",
                                      "'/'",
                                      "SPLUS",
                                      "UMINUS",
                                      "UPLUS",
                                      "'('",
                                      "')'",
                                      "'@'",
                                      "$accept",
                                      "file",
                                      "$@1",
                                      "sections",
                                      "uglykludge",
                                      "dil_section",
                                      "$@2",
                                      "dils",
                                      "dil",
                                      "room_section",
                                      "$@3",
                                      "rooms",
                                      "$@4",
                                      "room",
                                      "room_fields",
                                      "room_field",
                                      "oroom_field",
                                      "exitindex",
                                      "exit_fields",
                                      "exit_field",
                                      "object_section",
                                      "objects",
                                      "$@5",
                                      "object",
                                      "object_fields",
                                      "object_field",
                                      "oobject_field",
                                      "mobile_section",
                                      "mobiles",
                                      "$@6",
                                      "mobile",
                                      "mobile_fields",
                                      "mobile_field",
                                      "omobile_field",
                                      "unit_field",
                                      "$@7",
                                      "$@8",
                                      "$@9",
                                      "optfuncargs",
                                      "optfuncarg",
                                      "affect_fields",
                                      "affect_field",
                                      "dilargs",
                                      "dilarg",
                                      "zone_section",
                                      "zone_fields",
                                      "zone_field",
                                      "reset_section",
                                      "$@10",
                                      "reset_commands",
                                      "reset_command",
                                      "$@11",
                                      "block",
                                      "alloc",
                                      "command",
                                      "loadfields",
                                      "loadfield",
                                      "followfields",
                                      "followfield",
                                      "equipfields",
                                      "equipfield",
                                      "local",
                                      "zonemax",
                                      "max",
                                      "position",
                                      "moneylist",
                                      "moneytype",
                                      "stringlist",
                                      "hardstringlist",
                                      "strings",
                                      "stringcomp",
                                      "flags",
                                      "numbers",
                                      "number",
                                      "expr",
                                      "term",
                                      "factor",
                                      "zonename",
                                      "czonename",
                                      "unitname",
                                      "cunitname",
                                      "reference",
                                      "index",
                                      YY_NULLPTR};

static const char *yysymbol_name(yysymbol_kind_t yysymbol)
{
   return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] = {0,   256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 123, 125, 91,  93,  44,
                                        59,  269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286,
                                        287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305,
                                        306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324,
                                        325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343,
                                        344, 345, 346, 347, 348, 43,  45,  42,  47,  349, 350, 351, 40,  41,  64};
#endif

#define YYPACT_NINF (-248)

#define yypact_value_is_default(Yyn) ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-213)

#define yytable_value_is_error(Yyn) 0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] = {
   -248, 21,   29,   -248, -248, 42,   39,   176,  -248, -248, 83,   -248, -248, 75,   67,   67,   20,   94,   96,   67,   -248, -248, -248,
   -248, -248, 63,   -248, -248, 5,    5,    67,   -248, -42,  18,   -248, -248, 17,   -248, -248, -248, -248, -248, -248, 111,  -248, -248,
   -248, 120,  63,   63,   18,   18,   -15,  5,    5,    5,    5,    -248, -248, 72,   -248, -248, 115,  117,  135,  -248, -248, -248, -248,
   -248, 18,   18,   -248, -248, -248, 142,  143,  143,  143,  -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, 160,  -248, -17,
   409,  357,  276,  -248, 67,   151,  151,  -248, 151,  151,  151,  151,  159,  -248, 54,   -248, -248, 20,   155,  156,  163,  169,  20,
   151,  54,   54,   54,   67,   67,   -248, 67,   67,   67,   151,  183,  171,  151,  67,   -248, 67,   67,   67,   -248, -248, -248, -248,
   -248, 54,   -248, 67,   183,  67,   67,   67,   -248, -248, -248, -248, 54,   -248, 183,  67,   67,   67,   67,   67,   67,   67,   183,
   183,  67,   67,   67,   67,   67,   67,   54,   -248, -248, -248, -248, 98,   99,   95,   -248, -248, -248, 67,   -248, 150,  191,  54,
   -248, -248, -248, -248, -248, -248, -248, 186,  -248, -248, -248, -248, -248, -248, 187,  -248, -248, -248, 101,  188,  -248, -248, -248,
   -248, 56,   -248, -248, -248, 27,   -248, -248, 54,   193,  192,  193,  -248, -248, 67,   -248, 193,  -248, -248, -248, -248, -248, 67,
   67,   -248, -248, -248, -248, -248, -248, -248, 143,  196,  -29,  -25,  91,   54,   151,  -248, -248, -248, 76,   -248, -248, 25,   209,
   67,   183,  67,   67,   67,   67,   67,   67,   177,  -248, -248, 207,  151,  54,   151,  20,   -248, -248, 67,   -248, -248, -248, -248,
   -248, -248, -248, 67,   67,   67,   151,  -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, 67,   -248,
   -248, 198,  -248, 54,   -248, -248, -12,  -248, -248, 128,  -248, -248, -248, 67,   -248, -248, -248, -248, -248, -248, -248, -248, -248,
   -248, -248, -248, -248, 192,  -248, -248, -248, -248, -248, -248, -248, 0,    25,   210,  4,    -248, -248, 67,   54,   211,  -248, 128,
   -248, -248, -248, -248, 67,   -248, -248, -248, 122,  -248};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] = {
   2,   0,   0,   1,   131, 0,   10,  0,   3,   8,   16,  141, 212, 0,   0,   0,   0,   0,   0,   0,   132, 140, 211, 11,  14,  7,
   138, 210, 0,   0,   0,   135, 200, 201, 206, 133, 0,   187, 136, 186, 137, 139, 134, 9,   17,  40,  56,  144, 7,   7,   203, 202,
   0,   0,   0,   0,   0,   189, 190, 0,   13,  12,  15,  39,  55,  142, 4,   5,   6,   209, 204, 205, 207, 208, 188, 0,   0,   0,
   0,   151, 191, 214, 19,  21,  213, 42,  44,  58,  60,  151, 145, 0,   0,   0,   0,   146, 0,   0,   0,   159, 0,   0,   0,   0,
   147, 25,  0,   20,  84,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   103, 0,   0,   0,   0,   0,   0,   0,   0,
   106, 0,   0,   0,   22,  24,  32,  23,  48,  0,   43,  0,   0,   0,   0,   0,   45,  47,  46,  64,  0,   59,  0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   61,  63,  62,  158, 214, 0,   215, 160, 167, 173, 0,   156, 0,
   0,   0,   27,  196, 83,  86,  89,  87,  88,  0,   92,  93,  94,  96,  97,  98,  0,   100, 101, 102, 0,   0,   31,  26,  28,  29,
   0,   95,  90,  99,  0,   53,  54,  0,   50,  0,   51,  52,  75,  0,   65,  67,  68,  69,  70,  71,  73,  0,   0,   72,  76,  78,
   79,  77,  74,  66,  0,   0,   152, 153, 154, 0,   0,   151, 149, 198, 0,   91,  104, 124, 0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   114, 30,  0,   0,   0,   0,   0,   33,  49,  0,   185, 183, 82,  80,  81,  217, 216, 0,   0,   0,   0,   166, 161, 165,
   164, 163, 172, 168, 171, 170, 169, 182, 177, 174, 175, 0,   155, 157, 151, 197, 0,   108, 192, 0,   125, 127, 128, 129, 218, 116,
   0,   117, 119, 120, 121, 122, 123, 107, 115, 38,  35,  37,  34,  36,  0,   180, 179, 178, 162, 176, 150, 199, 105, 0,   0,   0,
   118, 184, 0,   0,   0,   109, 111, 126, 85,  193, 195, 0,   112, 113, 110, 0,   194};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {-248, -248, -248, -248, 74,   -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248,
                                       -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -248,
                                       40,   -248, -248, -248, -248, -248, -248, -26,  -248, -103, -248, -248, -248, -248, -248, -9,   -86,
                                       -248, -248, -248, -248, -248, -248, -248, -248, -248, -248, -3,   -1,   -95,  -248, -121, -79,  -102,
                                       -247, -248, -85,  -108, -248, -14,  218,  1,    104,  -248, -5,   92,   -59,  -87,  -117};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {
   0,   1,   2,   5,   47,  10,  23,  43,  61,  25,  44,  62,  76,  82,  92,  134, 135, 136, 212, 269, 48, 63, 77,  85,  93,  146, 147, 49,
   64,  78,  87,  94,  169, 170, 137, 198, 303, 208, 334, 343, 261, 262, 305, 306, 6,   7,   20,  66,  79, 89, 90,  182, 246, 91,  104, 240,
   284, 241, 289, 242, 295, 285, 286, 287, 297, 216, 273, 38,  39,  59,  308, 184, 248, 185, 32,  33,  34, 21, 174, 83,  175, 176, 204};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] = {
   31,   35,  22,  95,   307, 42,   335,  186, 193, 194,  195, 177, 191, 178, 179,  180, 181,  84,   84,   84,   304,  3,   96,  218,
   347,  215, 348, 27,   192, 50,   51,   213, 57,  223,  36,  221, 4,   58,  202,  36,  37,   206,  220,  229,  230,  304, 263, 27,
   340,  341, 342, 9,    264, 8,    70,   71,  265, 266,  53,  54,  237, 279, 280,  281, 282,  279,  280,  281,  183,  45,  283, 97,
   98,   46,  288, 247,  27,  99,   100,  101, 102, 103,  172, 267, 268, 53,  54,   74,  307,  27,   75,   301,  24,   69,  302, 26,
   336,  253, 254, 255,  256, 257,  258,  259, 260, 196,  197, 270, 199, 200, 201,  349, 30,   -181, 40,   207,  41,   209, 210, 211,
   55,   56,  67,  68,   60,  28,   29,   214, 65,  217,  217, 219, 30,  148, 171,  298, -18,  312,  -41,  222,  217,  224, 225, 226,
   227,  228, 292, 296,  231, 232,  233,  234, 235, 236,  28,  29,  -57, 299, 323,  72,  73,   30,   80,   293,  81,   243, 325, 28,
   29,   86,  88,  -143, 173, -148, 30,   187, 188, 11,   322, 277, 324, 279, -130, 189, -130, -130, -130, -130, -130, 190, 294, -181,
   -181, 205, 333, 330,  319, 12,   -181, 203, 13,  -212, 238, 244, 239, 245, 249,  274, 251,  250,  252,  271,  272,  332, 95,  275,
   276,  12,  253, 254,  255, 256,  257,  258, 259, 260,  310, 321, 337, 346, 354,  352, 345,  351,  278,  320,  300,  309, 290, 311,
   291,  313, 314, 315,  316, 317,  318,  339, 52,  344,  0,   0,   0,   0,   0,    0,   0,    326,  14,   15,   16,   17,  18,  19,
   0,    327, 328, 329,  0,   0,    0,    0,   0,   0,    0,   0,   0,   149, 0,    150, 151,  0,    0,    331,  0,    0,   0,   0,
   0,    108, 0,   0,    0,   0,    0,    0,   0,   0,    338, 109, 110, 111, 112,  113, 114,  115,  116,  117,  118,  119, 120, 121,
   122,  123, 124, 0,    125, 0,    0,    0,   0,   309,  0,   0,   0,   0,   350,  0,   0,    0,    152,  0,    0,    0,   0,   353,
   0,    0,   130, 153,  131, 154,  155,  132, 156, 157,  133, 158, 159, 160, 161,  162, 163,  164,  165,  166,  167,  168, 138, 0,
   139,  140, 0,   0,    0,   0,    0,    0,   0,   0,    108, 0,   0,   0,   0,    0,   0,    0,    0,    0,    109,  110, 111, 112,
   113,  114, 115, 116,  117, 118,  119,  120, 121, 122,  123, 124, 0,   125, 0,    0,   0,    0,    0,    0,    0,    0,   0,   0,
   0,    0,   105, 141,  106, 107,  0,    142, 143, 144,  145, 130, 0,   131, 108,  0,   132,  0,    0,    133,  0,    0,   0,   0,
   109,  110, 111, 112,  113, 114,  115,  116, 117, 118,  119, 120, 121, 122, 123,  124, 0,    125,  0,    0,    0,    0,   0,   0,
   0,    0,   0,   0,    126, 127,  128,  129, 0,   0,    0,   0,   0,   0,   0,    130, 0,    131,  0,    0,    132,  0,   0,   133};

static const yytype_int16 yycheck[] = {
   14,  15,  7,   89,  251, 19,  18,  109, 116, 117, 118, 98,  114, 100, 101, 102, 103, 76,  77,  78,  20,  0,   39,  144, 20,  142, 22,
   22,  115, 28,  29,  139, 15,  154, 14,  152, 7,   20,  125, 14,  20,  128, 150, 160, 161, 20,  19,  22,  48,  49,  50,  12,  25,  11,
   53,  54,  29,  30,  100, 101, 168, 90,  91,  92,  93,  90,  91,  92,  14,  6,   99,  88,  89,  10,  99,  183, 22,  94,  95,  96,  97,
   98,  96,  56,  57,  100, 101, 15,  335, 22,  18,  15,  9,   108, 18,  20,  108, 41,  42,  43,  44,  45,  46,  47,  48,  119, 120, 215,
   122, 123, 124, 107, 107, 22,  20,  129, 20,  131, 132, 133, 102, 103, 48,  49,  13,  100, 101, 141, 8,   143, 144, 145, 107, 93,  94,
   243, 21,  254, 21,  153, 154, 155, 156, 157, 158, 159, 241, 242, 162, 163, 164, 165, 166, 167, 100, 101, 21,  244, 266, 55,  56,  107,
   20,  72,  21,  179, 268, 100, 101, 77,  78,  11,  21,  14,  107, 20,  20,  1,   265, 238, 267, 90,  6,   20,  8,   9,   10,  11,  12,
   20,  99,  100, 101, 22,  302, 282, 19,  21,  107, 16,  24,  103, 103, 53,  109, 14,  20,  221, 107, 22,  22,  18,  20,  15,  300, 229,
   230, 21,  41,  42,  43,  44,  45,  46,  47,  48,  17,  20,  100, 19,  108, 20,  335, 341, 239, 261, 245, 251, 241, 253, 241, 255, 256,
   257, 258, 259, 260, 326, 30,  334, -1,  -1,  -1,  -1,  -1,  -1,  -1,  271, 82,  83,  84,  85,  86,  87,  -1,  279, 280, 281, -1,  -1,
   -1,  -1,  -1,  -1,  -1,  -1,  -1,  1,   -1,  3,   4,   -1,  -1,  297, -1,  -1,  -1,  -1,  -1,  13,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
   -1,  312, 23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  -1,  40,  -1,  -1,  -1,  -1,  335, -1,  -1,
   -1,  -1,  340, -1,  -1,  -1,  54,  -1,  -1,  -1,  -1,  349, -1,  -1,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
   75,  76,  77,  78,  79,  80,  81,  1,   -1,  3,   4,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  13,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
   -1,  -1,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  -1,  40,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
   -1,  -1,  -1,  -1,  -1,  1,   54,  3,   4,   -1,  58,  59,  60,  61,  62,  -1,  64,  13,  -1,  67,  -1,  -1,  70,  -1,  -1,  -1,  -1,
   23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  -1,  40,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
   -1,  51,  52,  53,  54,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  62,  -1,  64,  -1,  -1,  67,  -1,  -1,  70};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] = {
   0,   111, 112, 0,   7,   113, 154, 155, 11,  12,  115, 1,   21,  24,  82,  83,  84,  85,  86,  87,  156, 187, 188, 116, 9,   119,
   20,  22,  100, 101, 107, 183, 184, 185, 186, 183, 14,  20,  177, 178, 20,  20,  183, 117, 120, 6,   10,  114, 130, 137, 185, 185,
   184, 100, 101, 102, 103, 15,  20,  179, 13,  118, 121, 131, 138, 8,   157, 114, 114, 108, 185, 185, 186, 186, 15,  18,  122, 132,
   139, 158, 20,  21,  123, 189, 190, 133, 189, 140, 189, 159, 160, 163, 124, 134, 141, 160, 39,  88,  89,  94,  95,  96,  97,  98,
   164, 1,   3,   4,   13,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  40,  51,  52,  53,  54,
   62,  64,  67,  70,  125, 126, 127, 144, 1,   3,   4,   54,  58,  59,  60,  61,  135, 136, 144, 1,   3,   4,   54,  63,  65,  66,
   68,  69,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  142, 143, 144, 183, 21,  188, 190, 191, 191, 191, 191, 191, 191,
   161, 14,  181, 183, 177, 20,  20,  20,  20,  177, 191, 181, 181, 181, 183, 183, 145, 183, 183, 183, 191, 16,  192, 22,  191, 183,
   147, 183, 183, 183, 128, 181, 183, 192, 175, 183, 175, 183, 181, 192, 183, 175, 183, 183, 183, 183, 183, 192, 192, 183, 183, 183,
   183, 183, 183, 181, 103, 109, 165, 167, 169, 183, 53,  14,  162, 181, 182, 20,  22,  107, 22,  41,  42,  43,  44,  45,  46,  47,
   48,  150, 151, 19,  25,  29,  30,  56,  57,  129, 181, 18,  20,  176, 183, 183, 183, 190, 188, 90,  91,  92,  93,  99,  166, 171,
   172, 173, 99,  168, 171, 172, 173, 72,  99,  170, 173, 174, 181, 191, 159, 15,  18,  146, 20,  152, 153, 178, 180, 183, 17,  183,
   192, 183, 183, 183, 183, 183, 183, 19,  151, 20,  191, 181, 191, 177, 183, 183, 183, 183, 191, 183, 15,  181, 148, 18,  108, 100,
   183, 176, 48,  49,  50,  149, 180, 153, 19,  20,  22,  107, 183, 181, 20,  183, 108};

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] = {
   0,   110, 112, 111, 113, 114, 114, 114, 116, 115, 115, 117, 117, 118, 120, 119, 119, 121, 122, 121, 123, 124, 124, 125, 125,
   125, 126, 126, 126, 126, 126, 127, 128, 128, 129, 129, 129, 129, 129, 130, 131, 132, 131, 133, 134, 134, 135, 135, 135, 136,
   136, 136, 136, 136, 136, 137, 138, 139, 138, 140, 141, 141, 142, 142, 142, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143,
   143, 143, 143, 143, 143, 143, 143, 143, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144,
   144, 144, 144, 145, 146, 144, 147, 144, 148, 148, 149, 149, 149, 149, 150, 150, 151, 151, 151, 151, 151, 151, 151, 151, 152,
   152, 152, 153, 153, 153, 154, 155, 155, 156, 156, 156, 156, 156, 156, 156, 156, 156, 158, 157, 157, 159, 159, 160, 161, 160,
   162, 163, 164, 164, 164, 164, 164, 164, 164, 164, 165, 165, 166, 166, 166, 166, 166, 167, 167, 168, 168, 168, 168, 169, 169,
   170, 170, 170, 171, 172, 173, 174, 174, 175, 175, 176, 177, 177, 178, 178, 179, 179, 180, 180, 180, 180, 181, 181, 182, 182,
   183, 184, 184, 184, 184, 184, 185, 185, 185, 186, 186, 187, 188, 189, 190, 191, 191, 191, 192};

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] = {
   0, 2, 0, 3, 5, 2, 2, 0, 0, 3, 0, 0, 2, 1, 0, 3, 0, 0, 0, 3, 3, 0, 2, 1, 1, 1, 2, 2, 2, 2, 3, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 3, 3,
   0, 2, 1, 1, 1, 3, 2, 2, 2, 2, 2, 2, 0, 0, 3, 3, 0, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 2, 1, 6, 2, 2,
   2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 5, 0, 4, 0, 2, 2, 1, 2, 2, 1, 2, 2, 2, 3, 2, 2, 2, 2, 2, 0, 1, 3, 1, 1, 1, 2, 0,
   2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 3, 0, 1, 2, 2, 0, 4, 3, 0, 3, 3, 3, 4, 2, 4, 2, 1, 0, 2, 2, 1, 1, 1, 1, 0, 2, 1, 1, 1, 1, 0, 2, 1,
   2, 1, 2, 2, 2, 0, 1, 2, 4, 1, 1, 1, 3, 2, 1, 3, 1, 3, 5, 3, 1, 3, 1, 3, 1, 1, 2, 2, 3, 3, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3};

enum
{
   YYENOMEM = -2
};

#define yyerrok   (yyerrstatus = 0)
#define yyclearin (yychar = YYEMPTY)

#define YYACCEPT goto yyacceptlab
#define YYABORT  goto yyabortlab
#define YYERROR  goto yyerrorlab

#define YYRECOVERING() (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                                                                                             \
   do                                                                                                                                      \
      if(yychar == YYEMPTY)                                                                                                                \
      {                                                                                                                                    \
         yychar = (Token);                                                                                                                 \
         yylval = (Value);                                                                                                                 \
         YYPOPSTACK(yylen);                                                                                                                \
         yystate = *yyssp;                                                                                                                 \
         goto yybackup;                                                                                                                    \
      }                                                                                                                                    \
      else                                                                                                                                 \
      {                                                                                                                                    \
         yyerror(YY_("syntax error: cannot back up"));                                                                                     \
         YYERROR;                                                                                                                          \
      }                                                                                                                                    \
   while(0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* Enable debugging if requested.  */
#if YYDEBUG

   #ifndef YYFPRINTF
      #include <stdio.h> /* INFRINGES ON USER NAME SPACE */
      #define YYFPRINTF fprintf
   #endif

   #define YYDPRINTF(Args)                                                                                                                 \
      do                                                                                                                                   \
      {                                                                                                                                    \
         if(yydebug)                                                                                                                       \
            YYFPRINTF Args;                                                                                                                \
      } while(0)

   /* This macro is provided for backward compatibility. */
   #ifndef YY_LOCATION_PRINT
      #define YY_LOCATION_PRINT(File, Loc) ((void)0)
   #endif

   #define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                                                                                   \
      do                                                                                                                                   \
      {                                                                                                                                    \
         if(yydebug)                                                                                                                       \
         {                                                                                                                                 \
            YYFPRINTF(stderr, "%s ", Title);                                                                                               \
            yy_symbol_print(stderr, Kind, Value);                                                                                          \
            YYFPRINTF(stderr, "\n");                                                                                                       \
         }                                                                                                                                 \
      } while(0)

/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void yy_symbol_value_print(FILE *yyo, yysymbol_kind_t yykind, YYSTYPE const *const yyvaluep)
{
   FILE *yyoutput = yyo;
   YY_USE(yyoutput);
   if(!yyvaluep)
      return;
   #ifdef YYPRINT
   if(yykind < YYNTOKENS)
      YYPRINT(yyo, yytoknum[yykind], *yyvaluep);
   #endif
   YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   YY_USE(yykind);
   YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void yy_symbol_print(FILE *yyo, yysymbol_kind_t yykind, YYSTYPE const *const yyvaluep)
{
   YYFPRINTF(yyo, "%s %s (", yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name(yykind));

   yy_symbol_value_print(yyo, yykind, yyvaluep);
   YYFPRINTF(yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void yy_stack_print(yy_state_t *yybottom, yy_state_t *yytop)
{
   YYFPRINTF(stderr, "Stack now");
   for(; yybottom <= yytop; yybottom++)
   {
      int yybot = *yybottom;
      YYFPRINTF(stderr, " %d", yybot);
   }
   YYFPRINTF(stderr, "\n");
}

   #define YY_STACK_PRINT(Bottom, Top)                                                                                                     \
      do                                                                                                                                   \
      {                                                                                                                                    \
         if(yydebug)                                                                                                                       \
            yy_stack_print((Bottom), (Top));                                                                                               \
      } while(0)

/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void yy_reduce_print(yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
   int yylno  = yyrline[yyrule];
   int yynrhs = yyr2[yyrule];
   int yyi;
   YYFPRINTF(stderr, "Reducing stack by rule %d (line %d):\n", yyrule - 1, yylno);
   /* The symbols being reduced.  */
   for(yyi = 0; yyi < yynrhs; yyi++)
   {
      YYFPRINTF(stderr, "   $%d = ", yyi + 1);
      yy_symbol_print(stderr, YY_ACCESSING_SYMBOL(+yyssp[yyi + 1 - yynrhs]), &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF(stderr, "\n");
   }
}

   #define YY_REDUCE_PRINT(Rule)                                                                                                           \
      do                                                                                                                                   \
      {                                                                                                                                    \
         if(yydebug)                                                                                                                       \
            yy_reduce_print(yyssp, yyvsp, Rule);                                                                                           \
      } while(0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
   #define YYDPRINTF(Args) ((void)0)
   #define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
   #define YY_STACK_PRINT(Bottom, Top)
   #define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
   #define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
   #define YYMAXDEPTH 10000
#endif

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void yydestruct(const char *yymsg, yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
   YY_USE(yyvaluep);
   if(!yymsg)
      yymsg = "Deleting";
   YY_SYMBOL_PRINT(yymsg, yykind, yyvaluep, yylocationp);

   YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   YY_USE(yykind);
   YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;

/*----------.
| yyparse.  |
`----------*/

int yyparse(void)
{
   yy_state_fast_t yystate = 0;
   /* Number of tokens to shift before error messages enabled.  */
   int yyerrstatus = 0;

   /* Refer to the stacks through separate pointers, to allow yyoverflow
      to reallocate them elsewhere.  */

   /* Their size.  */
   YYPTRDIFF_T yystacksize = YYINITDEPTH;

   /* The state stack: array, bottom, top.  */
   yy_state_t  yyssa[YYINITDEPTH];
   yy_state_t *yyss  = yyssa;
   yy_state_t *yyssp = yyss;

   /* The semantic value stack: array, bottom, top.  */
   YYSTYPE  yyvsa[YYINITDEPTH];
   YYSTYPE *yyvs  = yyvsa;
   YYSTYPE *yyvsp = yyvs;

   int yyn;
   /* The return value of yyparse.  */
   int yyresult;
   /* Lookahead symbol kind.  */
   yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
   /* The variables used to return semantic value and location from the
      action routines.  */
   YYSTYPE yyval;

#define YYPOPSTACK(N) (yyvsp -= (N), yyssp -= (N))

   /* The number of symbols on the RHS of the reduced rule.
      Keep to zero when no symbol should be popped.  */
   int yylen = 0;

   YYDPRINTF((stderr, "Starting parse\n"));

   yychar = YYEMPTY; /* Cause a token to be read.  */
   goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
   /* In all cases, when you get here, the value and location stacks
      have just been pushed.  So pushing a state here evens the stacks.  */
   yyssp++;

/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
   YYDPRINTF((stderr, "Entering state %d\n", yystate));
   YY_ASSERT(0 <= yystate && yystate < YYNSTATES);
   YY_IGNORE_USELESS_CAST_BEGIN
   *yyssp = YY_CAST(yy_state_t, yystate);
   YY_IGNORE_USELESS_CAST_END
   YY_STACK_PRINT(yyss, yyssp);

   if(yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
      goto yyexhaustedlab;
#else
   {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

   #if defined yyoverflow
      {
         /* Give user a chance to reallocate the stack.  Use copies of
            these so that the &'s don't force the real ones into
            memory.  */
         yy_state_t *yyss1 = yyss;
         YYSTYPE    *yyvs1 = yyvs;

         /* Each stack pointer address is followed by the size of the
            data in use in that stack, in bytes.  This used to be a
            conditional around just the two extra args, but that might
            be undefined if yyoverflow is a macro.  */
         yyoverflow(YY_("memory exhausted"), &yyss1, yysize * YYSIZEOF(*yyssp), &yyvs1, yysize * YYSIZEOF(*yyvsp), &yystacksize);
         yyss = yyss1;
         yyvs = yyvs1;
      }
   #else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if(YYMAXDEPTH <= yystacksize)
         goto yyexhaustedlab;
      yystacksize *= 2;
      if(YYMAXDEPTH < yystacksize)
         yystacksize = YYMAXDEPTH;

      {
         yy_state_t    *yyss1 = yyss;
         union yyalloc *yyptr = YY_CAST(union yyalloc *, YYSTACK_ALLOC(YY_CAST(YYSIZE_T, YYSTACK_BYTES(yystacksize))));
         if(!yyptr)
            goto yyexhaustedlab;
         YYSTACK_RELOCATE(yyss_alloc, yyss);
         YYSTACK_RELOCATE(yyvs_alloc, yyvs);
      #undef YYSTACK_RELOCATE
         if(yyss1 != yyssa)
            YYSTACK_FREE(yyss1);
      }
   #endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF((stderr, "Stack size increased to %ld\n", YY_CAST(long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if(yyss + yystacksize - 1 <= yyssp)
         YYABORT;
   }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

   if(yystate == YYFINAL)
      YYACCEPT;

   goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:
   /* Do appropriate processing given the current state.  Read a
      lookahead token if we need one and don't already have one.  */

   /* First try to decide what to do without reference to lookahead token.  */
   yyn = yypact[yystate];
   if(yypact_value_is_default(yyn))
      goto yydefault;

   /* Not known => get a lookahead token if don't already have one.  */

   /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
   if(yychar == YYEMPTY)
   {
      YYDPRINTF((stderr, "Reading a token\n"));
      yychar = yylex();
   }

   if(yychar <= YYEOF)
   {
      yychar  = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF((stderr, "Now at end of input.\n"));
   }
   else if(yychar == YYerror)
   {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar  = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
   }
   else
   {
      yytoken = YYTRANSLATE(yychar);
      YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
   }

   /* If the proper action on seeing token YYTOKEN is to reduce or to
      detect an error, take that action.  */
   yyn += yytoken;
   if(yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
      goto yydefault;
   yyn = yytable[yyn];
   if(yyn <= 0)
   {
      if(yytable_value_is_error(yyn))
         goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
   }

   /* Count tokens shifted since error; after three, turn off error
      status.  */
   if(yyerrstatus)
      yyerrstatus--;

   /* Shift the lookahead token.  */
   YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);
   yystate = yyn;
   YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   *++yyvsp = yylval;
   YY_IGNORE_MAYBE_UNINITIALIZED_END

   /* Discard the shifted token.  */
   yychar = YYEMPTY;
   goto yynewstate;

/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
   yyn = yydefact[yystate];
   if(yyn == 0)
      goto yyerrlab;
   goto yyreduce;

/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
   /* yyn is the number of a rule to reduce with.  */
   yylen = yyr2[yyn];

   /* If YYLEN is nonzero, implement the default value of the action:
      '$$ = $1'.

      Otherwise, the following line sets YYVAL to garbage.
      This behavior is undocumented and Bison
      users should not rely upon it.  Assigning to YYVAL
      unconditionally makes the parser a bit smaller, and it avoids a
      GCC warning that YYVAL may be used uninitialized.  */
   yyval = yyvsp[1 - yylen];

   YY_REDUCE_PRINT(yyn);
   switch(yyn)
   {
      case 2: /* $@1: %empty  */
#line 167 "dmcpar.y"
      {
         cur = NULL;
      }
#line 1874 "y.tab.c"
      break;

      case 8: /* $@2: %empty  */
#line 183 "dmcpar.y"
      {
         char fname[256];
         sprintf(fname, "%s.dh", zone.z_zone.name);
         remove(fname);
         istemplate = 1;
      }
#line 1885 "y.tab.c"
      break;

      case 9: /* dil_section: DIL $@2 dils  */
#line 190 "dmcpar.y"
      {
         istemplate = 0;
      }
#line 1893 "y.tab.c"
      break;

      case 13: /* dil: DILPRG  */
#line 201 "dmcpar.y"
      {
         /* read and register template */
         char        buf[255];
         FILE       *f;
         CByteBuffer Buf;

         if((f = fopen((yyvsp[0].str), "rb")) == NULL)
         {
            fprintf(stderr, "%s: ", (yyvsp[0].str));
            fatal("Couldn't open DIL template.");
         }
         else
         {
            /* read the template from temporary file */
            int i;

            while(!feof(f))
            {
               i = fgetc(f);
               if(i != -1)
                  Buf.Append8(i);
            }

            cur_tmpl = bread_diltemplate(&Buf);

            assert(Buf.GetReadPosition() == Buf.GetLength());

            /*fprintf(stderr,"DMC reading template (core %d==%d)\n",
                    Buf.GetLength(), cur_tmpl->coresz);*/
            /*		  dumpdiltemplate(cur_tmpl);*/

            fclose(f);
            remove((yyvsp[0].str));

            /* register DIL template in zone */
            if(search_block(cur_tmpl->prgname, (const char **)tmplnames, TRUE) != -1)
            {
               /* redifinition of template */
               sprintf(buf, "Redefinition of template named '%s'", cur_tmpl->prgname);
               fatal(buf);
            }
            else
            {
               if(!zone.z_tmpl)
               {
                  zone.z_tmpl = cur_tmpl;
               }
               else
               {
                  cur_tmpl->next = zone.z_tmpl;
                  zone.z_tmpl    = cur_tmpl;
               }
               tmplnames = add_name(cur_tmpl->prgname, tmplnames);
            }
         }
      }
#line 1950 "y.tab.c"
      break;

      case 14: /* $@3: %empty  */
#line 259 "dmcpar.y"
      {
         cur = 0;
      }
#line 1956 "y.tab.c"
      break;

      case 18: /* $@4: %empty  */
#line 265 "dmcpar.y"
      {
         if(cur)
         {
            cur->next = mcreate_unit(UNIT_ST_ROOM);
            cur       = cur->next;
         }
         else
         {
            zone.z_rooms = mcreate_unit(UNIT_ST_ROOM);
            cur          = zone.z_rooms;
         }
         cur_extra = 0;
      }
#line 1974 "y.tab.c"
      break;

      case 20: /* room: unitname room_fields END  */
#line 281 "dmcpar.y"
      {
         UNIT_IDENT_ASSIGN(cur, (yyvsp[-2].str));
      }
#line 1982 "y.tab.c"
      break;

      case 26: /* oroom_field: MOVEMENT PNUM  */
#line 293 "dmcpar.y"
      {
         ROOM_LANDSCAPE(cur) = (yyvsp[0].num);
      }
#line 1990 "y.tab.c"
      break;

      case 27: /* oroom_field: FLAGS flags  */
#line 297 "dmcpar.y"
      {
         ROOM_FLAGS(cur) = (yyvsp[0].num);
      }
#line 1998 "y.tab.c"
      break;

      case 28: /* oroom_field: IN reference  */
#line 301 "dmcpar.y"
      {
         UNIT_IN(cur) = (struct unit_data *)(yyvsp[0].str);
      }
#line 2006 "y.tab.c"
      break;

      case 29: /* oroom_field: SPELL number  */
#line 305 "dmcpar.y"
      {
         ROOM_RESISTANCE(cur) = (yyvsp[0].num);
      }
#line 2014 "y.tab.c"
      break;

      case 31: /* exitindex: EXIT index  */
#line 317 "dmcpar.y"
      {
         ROOM_EXIT(cur, cur_ex = (yyvsp[0].num)) = mcreate_exit();
      }
#line 2022 "y.tab.c"
      break;

      case 34: /* exit_field: TO reference  */
#line 325 "dmcpar.y"
      {
         ROOM_EXIT(cur, cur_ex)->to_room = (struct unit_data *)(yyvsp[0].str);
      }
#line 2031 "y.tab.c"
      break;

      case 35: /* exit_field: KEY reference  */
#line 330 "dmcpar.y"
      {
         ROOM_EXIT(cur, cur_ex)->key = (struct file_index_type *)(yyvsp[0].str);
      }
#line 2040 "y.tab.c"
      break;

      case 36: /* exit_field: KEYWORD stringlist  */
#line 335 "dmcpar.y"
      {
         ROOM_EXIT(cur, cur_ex)->open_name.CopyList((const char **)(yyvsp[0].str_list));
      }
#line 2049 "y.tab.c"
      break;

      case 37: /* exit_field: OPEN flags  */
#line 340 "dmcpar.y"
      {
         ROOM_EXIT(cur, cur_ex)->exit_info = (yyvsp[0].num);
      }
#line 2057 "y.tab.c"
      break;

      case 38: /* exit_field: DESCR STRING  */
#line 344 "dmcpar.y"
      {
         extern const char *dirs[];

         if(cur_extra)
         {
            cur_extra->next = new(class extra_descr_data);
            cur_extra       = cur_extra->next;
         }
         else
         {
            UNIT_EXTRA_DESCR(cur) = new(class extra_descr_data);
            cur_extra             = UNIT_EXTRA_DESCR(cur);
         }
         cur_extra->next = 0;

         cur_extra->names.AppendName((char *)dirs[cur_ex]);
         /* strcat($2, "\n\r"); */
         strip_trailing_blanks((yyvsp[0].str));
         cur_extra->descr.Reassign((yyvsp[0].str));
      }
#line 2083 "y.tab.c"
      break;

      case 41: /* $@5: %empty  */
#line 375 "dmcpar.y"
      {
         if(cur)
         {
            cur->next = mcreate_unit(UNIT_ST_OBJ);
            cur       = cur->next;
         }
         else
         {
            zone.z_objects = mcreate_unit(UNIT_ST_OBJ);
            cur            = zone.z_objects;
         }
         cur_extra = 0;
      }
#line 2101 "y.tab.c"
      break;

      case 43: /* object: unitname object_fields END  */
#line 391 "dmcpar.y"
      {
         UNIT_IDENT_ASSIGN(cur, (yyvsp[-2].str));
      }
#line 2109 "y.tab.c"
      break;

      case 49: /* oobject_field: VALUE index flags  */
#line 403 "dmcpar.y"
      {
         OBJ_VALUE(cur, (yyvsp[-1].num)) = (yyvsp[0].num);
      }
#line 2117 "y.tab.c"
      break;

      case 50: /* oobject_field: COST moneylist  */
#line 407 "dmcpar.y"
      {
         OBJ_PRICE(cur) = (yyvsp[0].num);
      }
#line 2125 "y.tab.c"
      break;

      case 51: /* oobject_field: RENT moneylist  */
#line 411 "dmcpar.y"
      {
         OBJ_PRICE_DAY(cur) = (yyvsp[0].num);
      }
#line 2133 "y.tab.c"
      break;

      case 52: /* oobject_field: TYPE number  */
#line 415 "dmcpar.y"
      {
         OBJ_TYPE(cur) = (yyvsp[0].num);
      }
#line 2141 "y.tab.c"
      break;

      case 53: /* oobject_field: FLAGS flags  */
#line 419 "dmcpar.y"
      {
         OBJ_FLAGS(cur) = (yyvsp[0].num);
      }
#line 2149 "y.tab.c"
      break;

      case 54: /* oobject_field: SPELL number  */
#line 423 "dmcpar.y"
      {
         OBJ_RESISTANCE(cur) = (yyvsp[0].num);
      }
#line 2157 "y.tab.c"
      break;

      case 57: /* $@6: %empty  */
#line 436 "dmcpar.y"
      {
         if(cur)
         {
            cur->next = mcreate_unit(UNIT_ST_NPC);
            cur       = cur->next;
         }
         else
         {
            zone.z_mobiles = mcreate_unit(UNIT_ST_NPC);
            cur            = zone.z_mobiles;
         }
         cur_extra = 0;
      }
#line 2175 "y.tab.c"
      break;

      case 59: /* mobile: unitname mobile_fields END  */
#line 452 "dmcpar.y"
      {
         UNIT_IDENT_ASSIGN(cur, (yyvsp[-2].str));
      }
#line 2183 "y.tab.c"
      break;

      case 65: /* omobile_field: MANA number  */
#line 464 "dmcpar.y"
      {
         CHAR_MANA(cur) = (yyvsp[0].num);
      }
#line 2191 "y.tab.c"
      break;

      case 66: /* omobile_field: ACT flags  */
#line 468 "dmcpar.y"
      {
         NPC_FLAGS(cur) = (yyvsp[0].num);
      }
#line 2199 "y.tab.c"
      break;

      case 67: /* omobile_field: MONEY moneylist  */
#line 472 "dmcpar.y"
      {
         char buf[1024], tmp[100];

         if(CHAR_MONEY(cur))
            warning("Redefinition of money...");

         for(myi = 0, *buf = '\0'; myi <= mon_top; myi++)
         {
            sprintf(tmp, " %d %d ~", mon_list[myi][0], mon_list[myi][1]);
            strcat(buf, tmp);
         }

         CHAR_MONEY(cur) = (char *)mmalloc(strlen(buf) + 1);
         strcpy(CHAR_MONEY(cur), buf);
      }
#line 2220 "y.tab.c"
      break;

      case 68: /* omobile_field: EXP number  */
#line 489 "dmcpar.y"
      {
         CHAR_EXP(cur) = (yyvsp[0].num);
      }
#line 2228 "y.tab.c"
      break;

      case 69: /* omobile_field: SEX number  */
#line 493 "dmcpar.y"
      {
         CHAR_SEX(cur) = (yyvsp[0].num);
      }
#line 2236 "y.tab.c"
      break;

      case 70: /* omobile_field: LEVEL number  */
#line 497 "dmcpar.y"
      {
         CHAR_LEVEL(cur) = (yyvsp[0].num);
      }
#line 2244 "y.tab.c"
      break;

      case 71: /* omobile_field: RACE number  */
#line 501 "dmcpar.y"
      {
         CHAR_RACE(cur) = (yyvsp[0].num);
      }
#line 2252 "y.tab.c"
      break;

      case 72: /* omobile_field: ATTACK number  */
#line 505 "dmcpar.y"
      {
         CHAR_ATTACK_TYPE(cur) = (yyvsp[0].num);
      }
#line 2260 "y.tab.c"
      break;

      case 73: /* omobile_field: POSITION number  */
#line 509 "dmcpar.y"
      {
         CHAR_POS(cur) = (yyvsp[0].num);
      }
#line 2268 "y.tab.c"
      break;

      case 74: /* omobile_field: DEFAULT number  */
#line 513 "dmcpar.y"
      {
         NPC_DEFAULT(cur) = (yyvsp[0].num);
      }
#line 2276 "y.tab.c"
      break;

      case 75: /* omobile_field: FLAGS flags  */
#line 517 "dmcpar.y"
      {
         CHAR_FLAGS(cur) = (yyvsp[0].num);
      }
#line 2284 "y.tab.c"
      break;

      case 76: /* omobile_field: ARMOUR number  */
#line 521 "dmcpar.y"
      {
         CHAR_NATURAL_ARMOUR(cur) = (yyvsp[0].num);
      }
#line 2292 "y.tab.c"
      break;

      case 77: /* omobile_field: SPEED number  */
#line 525 "dmcpar.y"
      {
         CHAR_SPEED(cur) = (yyvsp[0].num);
      }
#line 2300 "y.tab.c"
      break;

      case 78: /* omobile_field: DEFENSIVE number  */
#line 529 "dmcpar.y"
      {
         CHAR_DEFENSIVE(cur) = (yyvsp[0].num);
      }
#line 2308 "y.tab.c"
      break;

      case 79: /* omobile_field: OFFENSIVE number  */
#line 533 "dmcpar.y"
      {
         CHAR_OFFENSIVE(cur) = (yyvsp[0].num);
      }
#line 2316 "y.tab.c"
      break;

      case 80: /* omobile_field: ABILITY index number  */
#line 537 "dmcpar.y"
      {
         CHAR_ABILITY(cur, (yyvsp[-1].num)) = (yyvsp[0].num);
      }
#line 2324 "y.tab.c"
      break;

      case 81: /* omobile_field: WEAPON index number  */
#line 541 "dmcpar.y"
      {
         NPC_WPN_SKILL(cur, (yyvsp[-1].num)) = (yyvsp[0].num);
      }
#line 2332 "y.tab.c"
      break;

      case 82: /* omobile_field: SPELL index number  */
#line 545 "dmcpar.y"
      {
         NPC_SPL_SKILL(cur, (yyvsp[-1].num)) = (yyvsp[0].num);
      }
#line 2340 "y.tab.c"
      break;

      case 83: /* unit_field: NAMES stringlist  */
#line 555 "dmcpar.y"
      {
         UNIT_NAMES(cur).CopyList((const char **)(yyvsp[0].str_list));
         /*if (UNIT_NAMES(cur) && UNIT_NAME(cur))
           CAP(UNIT_NAME(cur)); */
      }
#line 2350 "y.tab.c"
      break;

      case 84: /* unit_field: DILPRG  */
#line 561 "dmcpar.y"
      {
         CByteBuffer Buf;
         FILE       *f;
         char        buf[255];

         if((f = fopen((yyvsp[0].str), "rb")) == NULL)
         {
            fprintf(stderr, "%s: ", (yyvsp[0].str));
            fatal("Couldn't open DIL program.");
         }
         else
         {
            if(!UNIT_FUNC(cur))
            {
               UNIT_FUNC(cur) = mcreate_func();
               cur_func       = UNIT_FUNC(cur);
            }
            else
            {
               cur_func->next = mcreate_func();
               cur_func       = cur_func->next;
            }
            cur_func->index      = SFUN_DIL_INTERNAL;
            cur_func->heart_beat = WAIT_SEC * 10;
            cur_func->flags      = SFB_ALL;

            myi = 0;
            int i;
            while(!feof(f))
            {
               i = fgetc(f);
               if(i != -1)
                  Buf.Append8(i);
            }

            /* always read latest version written by DIL */
            cur_func->data = bread_dil(&Buf, NULL, 255, NULL);

            assert(Buf.GetReadPosition() == Buf.GetLength());

            /* fprintf(stderr,"DMC reading program\n");*/
            /*			  dumpdil(cur_func->data);*/

            fclose(f);

            remove((yyvsp[0].str));

            /* register DIL template in zone */
            cur_prg  = (struct dilprg *)(cur_func->data);
            cur_tmpl = cur_prg->sp->tmpl;
            cur_prg->flags |= DILFL_COPY;
            cur_tmpl->flags |= DILFL_COPY;
            /* register DIL template in zone */
            if(search_block(cur_tmpl->prgname, (const char **)tmplnames, TRUE) != -1)
            {
               /* redifinition of template */
               sprintf(buf,
                       "Redefinition of template "
                       "named '%s'",
                       cur_tmpl->prgname);
               fatal(buf);
            }
            else
            {
               if(!zone.z_tmpl)
               {
                  zone.z_tmpl = cur_tmpl;
               }
               else
               {
                  cur_tmpl->next = zone.z_tmpl;
                  zone.z_tmpl    = cur_tmpl;
               }
               tmplnames = add_name(cur_tmpl->prgname, tmplnames);
            }
         }
      }
#line 2428 "y.tab.c"
      break;

      case 85: /* unit_field: DILCOPY reference '(' dilargs ')' ';'  */
#line 635 "dmcpar.y"
      {
         struct dilargstype *argcopy;

         CREATE(argcopy, struct dilargstype, 1);

         *argcopy      = *((yyvsp[-2].dilargs));
         argcopy->name = (yyvsp[-4].str);

         if(!UNIT_FUNC(cur))
         {
            UNIT_FUNC(cur) = mcreate_func();
            cur_func       = UNIT_FUNC(cur);
         }
         else
         {
            cur_func->next = mcreate_func();
            cur_func       = cur_func->next;
         }
         cur_func->index      = SFUN_DILCOPY_INTERNAL;
         cur_func->data       = argcopy;
         cur_func->heart_beat = WAIT_SEC * 10;
         cur_func->flags      = SFB_ALL;
      }
#line 2456 "y.tab.c"
      break;

      case 86: /* unit_field: TITLE STRING  */
#line 659 "dmcpar.y"
      {
         UNIT_TITLE(cur).Reassign((yyvsp[0].str));
      }
#line 2464 "y.tab.c"
      break;

      case 87: /* unit_field: OUT_DESCR STRING  */
#line 663 "dmcpar.y"
      {
         UNIT_OUT_DESCR(cur).Reassign((yyvsp[0].str));
      }
#line 2472 "y.tab.c"
      break;

      case 88: /* unit_field: IN_DESCR STRING  */
#line 667 "dmcpar.y"
      {
         if(IS_ROOM(cur) && !str_is_empty((yyvsp[0].str)))
         {
            memmove((yyvsp[0].str) + 3, (yyvsp[0].str), strlen((yyvsp[0].str)) + 1);

            (yyvsp[0].str)[0] = ' ';
            (yyvsp[0].str)[1] = ' ';
            (yyvsp[0].str)[2] = ' ';
         }

         UNIT_IN_DESCR(cur).Reassign((yyvsp[0].str));
      }
#line 2489 "y.tab.c"
      break;

      case 89: /* unit_field: DESCR STRING  */
#line 680 "dmcpar.y"
      {
         if(IS_ROOM(cur) && !str_is_empty((yyvsp[0].str)))
         {
            memmove((yyvsp[0].str) + 3, (yyvsp[0].str), strlen((yyvsp[0].str)) + 1);

            (yyvsp[0].str)[0] = ' ';
            (yyvsp[0].str)[1] = ' ';
            (yyvsp[0].str)[2] = ' ';

            UNIT_IN_DESCR(cur).Reassign((yyvsp[0].str));
         }
         else
            UNIT_OUT_DESCR(cur).Reassign((yyvsp[0].str));
      }
#line 2508 "y.tab.c"
      break;

      case 90: /* unit_field: ALIGNMENT number  */
#line 695 "dmcpar.y"
      {
         UNIT_ALIGNMENT(cur) = (yyvsp[0].num);
      }
#line 2516 "y.tab.c"
      break;

      case 91: /* unit_field: EXTRA stringlist STRING  */
#line 699 "dmcpar.y"
      {
         if(cur_extra)
         {
            cur_extra->next = new(class extra_descr_data);
            cur_extra       = cur_extra->next;
         }
         else
         {
            UNIT_EXTRA_DESCR(cur) = new(class extra_descr_data);
            cur_extra             = UNIT_EXTRA_DESCR(cur);
         }
         cur_extra->next = 0;

         cur_extra->names.CopyList((const char **)(yyvsp[-1].str_list));
         /* strcat($3, "\n\r"); */
         strip_trailing_blanks((yyvsp[0].str));
         cur_extra->descr.Reassign((yyvsp[0].str));
      }
#line 2540 "y.tab.c"
      break;

      case 92: /* unit_field: KEY reference  */
#line 719 "dmcpar.y"
      {
         UNIT_KEY(cur) = (struct file_index_type *)(yyvsp[0].str);
      }
#line 2548 "y.tab.c"
      break;

      case 93: /* unit_field: OPEN flags  */
#line 723 "dmcpar.y"
      {
         UNIT_OPEN_FLAGS(cur) |= (yyvsp[0].num);
      }
#line 2556 "y.tab.c"
      break;

      case 94: /* unit_field: MANIPULATE flags  */
#line 727 "dmcpar.y"
      {
         UNIT_MANIPULATE(cur) |= (yyvsp[0].num);
      }
#line 2564 "y.tab.c"
      break;

      case 95: /* unit_field: HIT number  */
#line 731 "dmcpar.y"
      {
         UNIT_MAX_HIT(cur) = (yyvsp[0].num);
      }
#line 2572 "y.tab.c"
      break;

      case 96: /* unit_field: UFLAGS flags  */
#line 735 "dmcpar.y"
      {
         UNIT_FLAGS(cur) |= (yyvsp[0].num);
      }
#line 2580 "y.tab.c"
      break;

      case 97: /* unit_field: WEIGHT number  */
#line 739 "dmcpar.y"
      {
         UNIT_BASE_WEIGHT(cur) = (yyvsp[0].num);
      }
#line 2588 "y.tab.c"
      break;

      case 98: /* unit_field: CAPACITY number  */
#line 743 "dmcpar.y"
      {
         UNIT_CAPACITY(cur) = (yyvsp[0].num);
      }
#line 2596 "y.tab.c"
      break;

      case 99: /* unit_field: HEIGHT number  */
#line 747 "dmcpar.y"
      {
         UNIT_SIZE(cur) = (yyvsp[0].num);
      }
#line 2604 "y.tab.c"
      break;

      case 100: /* unit_field: LIGHT number  */
#line 755 "dmcpar.y"
      {
         UNIT_LIGHTS(cur) = (yyvsp[0].num);
      }
#line 2612 "y.tab.c"
      break;

      case 101: /* unit_field: BRIGHT number  */
#line 759 "dmcpar.y"
      {
         UNIT_BRIGHT(cur) = (yyvsp[0].num);
      }
#line 2620 "y.tab.c"
      break;

      case 102: /* unit_field: MINV number  */
#line 763 "dmcpar.y"
      {
         UNIT_MINV(cur) = (yyvsp[0].num);
      }
#line 2628 "y.tab.c"
      break;

      case 103: /* $@7: %empty  */
#line 767 "dmcpar.y"
      {
         if(!UNIT_FUNC(cur))
         {
            UNIT_FUNC(cur) = mcreate_func();
            cur_func       = UNIT_FUNC(cur);
         }
         else
         {
            cur_func->next = mcreate_func();
            cur_func       = cur_func->next;
         }
      }
#line 2645 "y.tab.c"
      break;

      case 104: /* $@8: %empty  */
#line 780 "dmcpar.y"
      {
         switch((yyvsp[0].num))
         {
            case SFUN_PERSIST_INTERNAL:
            case SFUN_DILCOPY_INTERNAL:
            case SFUN_HUNTING:
            case SFUN_INTERN_SHOP:
            case SFUN_EAT_AND_DELETE:
            case SFUN_PAIN:
            case SFUN_NPC_VISIT_ROOM:
            case SFUN_MERCENARY_HUNT:
            case SFUN_PERSIST_INIT:
            case SFUN_TEACHING:
            case SFUN_CHANGE_HOMETOWN:
            case SFUN_DIL_INTERNAL:
            case SFUN_FROZEN:
            case SFUN_RETURN_TO_ORIGIN:
            case SFUN_GUILD_INTERNAL:
            case SFUN_DEMI_STUFF:
            case SFUN_LINK_DEAD:
            case SFUN_DIL_COPY:
               fatal("Illegal special assignment.");
               break;
         }
         cur_func->index = (yyvsp[0].num);
      }
#line 2676 "y.tab.c"
      break;

      case 106: /* $@9: %empty  */
#line 808 "dmcpar.y"
      {
         if(!UNIT_AFFECTED(cur))
         {
            UNIT_AFFECTED(cur) = mcreate_affect();
            cur_aff            = UNIT_AFFECTED(cur);
         }
         else
         {
            cur_aff->next = mcreate_affect();
            cur_aff       = cur_aff->next;
         }
      }
#line 2693 "y.tab.c"
      break;

      case 110: /* optfuncarg: STRINGT STRING  */
#line 826 "dmcpar.y"
      {
         char buf[1024];
         if(cur_func->data)
            fatal("Redefinition of data for special.");

         cur_func->data = (yyvsp[0].str);
      }
#line 2705 "y.tab.c"
      break;

      case 111: /* optfuncarg: stringcomp  */
#line 834 "dmcpar.y"
      {
         if(cur_func->data)
            fatal("Redefinition of data for special.");
         cur_func->data = (yyvsp[0].str);
      }
#line 2715 "y.tab.c"
      break;

      case 112: /* optfuncarg: TIME number  */
#line 840 "dmcpar.y"
      {
         cur_func->heart_beat = (yyvsp[0].num);
      }
#line 2723 "y.tab.c"
      break;

      case 113: /* optfuncarg: BITS flags  */
#line 844 "dmcpar.y"
      {
         cur_func->flags = (yyvsp[0].num);
      }
#line 2731 "y.tab.c"
      break;

      case 116: /* affect_field: ID number  */
#line 852 "dmcpar.y"
      {
         cur_aff->id = (yyvsp[0].num);
      }
#line 2739 "y.tab.c"
      break;

      case 117: /* affect_field: DURATION number  */
#line 856 "dmcpar.y"
      {
         cur_aff->duration = (yyvsp[0].num);
      }
#line 2747 "y.tab.c"
      break;

      case 118: /* affect_field: DATA index number  */
#line 860 "dmcpar.y"
      {
         cur_aff->data[(yyvsp[-1].num)] = (yyvsp[0].num);
      }
#line 2755 "y.tab.c"
      break;

      case 119: /* affect_field: FIRSTF number  */
#line 864 "dmcpar.y"
      {
         cur_aff->firstf_i = (yyvsp[0].num);
      }
#line 2763 "y.tab.c"
      break;

      case 120: /* affect_field: TICKF number  */
#line 868 "dmcpar.y"
      {
         cur_aff->tickf_i = (yyvsp[0].num);
      }
#line 2771 "y.tab.c"
      break;

      case 121: /* affect_field: LASTF number  */
#line 872 "dmcpar.y"
      {
         cur_aff->lastf_i = (yyvsp[0].num);
      }
#line 2779 "y.tab.c"
      break;

      case 122: /* affect_field: APPLYF number  */
#line 876 "dmcpar.y"
      {
         cur_aff->applyf_i = (yyvsp[0].num);
      }
#line 2787 "y.tab.c"
      break;

      case 123: /* affect_field: TIME number  */
#line 880 "dmcpar.y"
      {
         cur_aff->beat = (yyvsp[0].num);
      }
#line 2795 "y.tab.c"
      break;

      case 124: /* dilargs: %empty  */
#line 889 "dmcpar.y"
      {
         dilargs.no      = 0;
         (yyval.dilargs) = &dilargs;
      }
#line 2804 "y.tab.c"
      break;

      case 125: /* dilargs: dilarg  */
#line 894 "dmcpar.y"
      {
         dilargs.no        = 1;
         dilargs.dilarg[0] = *(yyvsp[0].dilarg);
         (yyval.dilargs)   = &dilargs;
      }
#line 2814 "y.tab.c"
      break;

      case 126: /* dilargs: dilargs ',' dilarg  */
#line 900 "dmcpar.y"
      {
         dilargs.no++;
         dilargs.dilarg[dilargs.no - 1] = *(yyvsp[0].dilarg);
         (yyval.dilargs)                = &dilargs;
      }
#line 2824 "y.tab.c"
      break;

      case 127: /* dilarg: hardstringlist  */
#line 908 "dmcpar.y"
      {
         struct dilargtype *dat;

         dat                  = (struct dilargtype *)mmalloc(sizeof(struct dilargtype));
         dat->type            = DILV_SLP;
         dat->data.stringlist = (yyvsp[0].str_list);
         (yyval.dilarg)       = dat;
      }
#line 2838 "y.tab.c"
      break;

      case 128: /* dilarg: stringcomp  */
#line 918 "dmcpar.y"
      {
         struct dilargtype *dat;

         dat              = (struct dilargtype *)mmalloc(sizeof(struct dilargtype));
         dat->type        = DILV_SP;
         dat->data.string = (yyvsp[0].str);
         (yyval.dilarg)   = dat;
      }
#line 2852 "y.tab.c"
      break;

      case 129: /* dilarg: number  */
#line 928 "dmcpar.y"
      {
         struct dilargtype *dat;

         dat            = (struct dilargtype *)mmalloc(sizeof(struct dilargtype));
         dat->type      = DILV_INT;
         dat->data.num  = (yyvsp[0].num);
         (yyval.dilarg) = dat;
      }
#line 2866 "y.tab.c"
      break;

      case 133: /* zone_field: LIFESPAN number  */
#line 946 "dmcpar.y"
      {
         zone.z_zone.lifespan = (yyvsp[0].num);
      }
#line 2874 "y.tab.c"
      break;

      case 134: /* zone_field: WEATHER number  */
#line 950 "dmcpar.y"
      {
         zone.z_zone.weather = (yyvsp[0].num);
      }
#line 2882 "y.tab.c"
      break;

      case 135: /* zone_field: RESET_F number  */
#line 954 "dmcpar.y"
      {
         zone.z_zone.reset_mode = (yyvsp[0].num);
      }
#line 2890 "y.tab.c"
      break;

      case 136: /* zone_field: CREATORS stringlist  */
#line 958 "dmcpar.y"
      {
         zone.z_zone.creators = (yyvsp[0].str_list);
      }
#line 2898 "y.tab.c"
      break;

      case 137: /* zone_field: NOTES STRING  */
#line 962 "dmcpar.y"
      {
         zone.z_zone.notes = (yyvsp[0].str);
      }
#line 2906 "y.tab.c"
      break;

      case 138: /* zone_field: TITLE STRING  */
#line 966 "dmcpar.y"
      {
         zone.z_zone.title = (yyvsp[0].str);
      }
#line 2914 "y.tab.c"
      break;

      case 139: /* zone_field: HELP STRING  */
#line 970 "dmcpar.y"
      {
         zone.z_zone.help = (yyvsp[0].str);
      }
#line 2922 "y.tab.c"
      break;

      case 140: /* zone_field: zonename  */
#line 974 "dmcpar.y"
      {
         zone.z_zone.name = (yyvsp[0].str);
      }
#line 2930 "y.tab.c"
      break;

      case 142: /* $@10: %empty  */
#line 984 "dmcpar.y"
      {
         cur_cmd = 0;
      }
#line 2938 "y.tab.c"
      break;

      case 147: /* reset_command: alloc command  */
#line 994 "dmcpar.y"
      {
         cur_cmd->direction = ZON_DIR_CONT;
      }
#line 2946 "y.tab.c"
      break;

      case 148: /* $@11: %empty  */
#line 998 "dmcpar.y"
      {
         cur_cmd->direction = ZON_DIR_NEST;
      }
#line 2954 "y.tab.c"
      break;

      case 150: /* block: '{' reset_commands '}'  */
#line 1004 "dmcpar.y"
      {
         if(cur_cmd->direction == ZON_DIR_CONT)
            cur_cmd->direction = ZON_DIR_UNNEST;
         else if(cur_cmd->direction == ZON_DIR_NEST)
            cur_cmd->direction = ZON_DIR_CONT;
         else /* UNNEST, gotta put in a nop, here */
         {
            cur_cmd->next      = (struct reset_command *)mmalloc(sizeof(struct reset_command));
            cur_cmd            = cur_cmd->next;
            cur_cmd->cmd_no    = 0; /* nop */
            cur_cmd->direction = ZON_DIR_UNNEST;
         }
      }
#line 2973 "y.tab.c"
      break;

      case 151: /* alloc: %empty  */
#line 1020 "dmcpar.y"
      {
         if(cur_cmd)
         {
            cur_cmd->next = (struct reset_command *)mmalloc(sizeof(struct reset_command));
            cur_cmd       = cur_cmd->next;
         }
         else
         {
            zone.z_table = (struct reset_command *)mmalloc(sizeof(struct reset_command));
            cur_cmd      = zone.z_table;
         }
         /* init ? */
         cur_cmd->next = 0;
         cur_cmd->cmpl = 0;
         /* cur_cmd->num1 = 0;
            cur_cmd->num2 = 0; */
      }
#line 2997 "y.tab.c"
      break;

      case 152: /* command: LOAD reference loadfields  */
#line 1041 "dmcpar.y"
      {
         cur_cmd->cmd_no = 1;
         cur_cmd->ref1   = (yyvsp[-1].str);
         if(!cur_cmd->num1)
            cur_cmd->num1 = 0;
         if(!cur_cmd->num2)
            cur_cmd->num2 = 0;
      }
#line 3010 "y.tab.c"
      break;

      case 153: /* command: FOLLOW reference followfields  */
#line 1050 "dmcpar.y"
      {
         cur_cmd->cmd_no = 6;
         cur_cmd->ref1   = (yyvsp[-1].str);
         if(!cur_cmd->num1)
            cur_cmd->num1 = 0;
         if(!cur_cmd->num2)
            cur_cmd->num2 = 0;
      }
#line 3023 "y.tab.c"
      break;

      case 154: /* command: EQUIP reference equipfields  */
#line 1059 "dmcpar.y"
      {
         cur_cmd->cmd_no = 2;
         cur_cmd->ref1   = (yyvsp[-1].str);
         if(!cur_cmd->num1)
            cur_cmd->num1 = 0;
      }
#line 3034 "y.tab.c"
      break;

      case 155: /* command: DOOR reference number flags  */
#line 1066 "dmcpar.y"
      {
         cur_cmd->cmd_no = 3;
         cur_cmd->ref1   = (yyvsp[-2].str);
         cur_cmd->num1   = (yyvsp[-1].num);
         cur_cmd->num2   = (yyvsp[0].num);
      }
#line 3045 "y.tab.c"
      break;

      case 156: /* command: PURGE reference  */
#line 1073 "dmcpar.y"
      {
         cur_cmd->cmd_no = 4;
         cur_cmd->ref1   = (yyvsp[0].str);
      }
#line 3054 "y.tab.c"
      break;

      case 157: /* command: REMOVE reference IN reference  */
#line 1078 "dmcpar.y"
      {
         cur_cmd->cmd_no = 5;
         cur_cmd->ref1   = (yyvsp[-2].str);
         cur_cmd->ref2   = (yyvsp[0].str);
      }
#line 3064 "y.tab.c"
      break;

      case 158: /* command: RANDOM number  */
#line 1084 "dmcpar.y"
      {
         cur_cmd->cmd_no = 7;
         cur_cmd->ref1   = 0;
         cur_cmd->num1   = (yyvsp[0].num);
         cur_cmd->num2   = 0;
      }
#line 3075 "y.tab.c"
      break;

      case 159: /* command: NOP  */
#line 1091 "dmcpar.y"
      {
         cur_cmd->cmd_no = 0;
      }
#line 3083 "y.tab.c"
      break;

      case 162: /* loadfield: INTO reference  */
#line 1099 "dmcpar.y"
      {
         cur_cmd->ref2 = (yyvsp[0].str);
      }
#line 3091 "y.tab.c"
      break;

      case 166: /* loadfield: COMPLETE  */
#line 1106 "dmcpar.y"
      {
         cur_cmd->cmpl = !0;
      }
#line 3099 "y.tab.c"
      break;

      case 172: /* followfield: COMPLETE  */
#line 1117 "dmcpar.y"
      {
         cur_cmd->cmpl = !0;
      }
#line 3107 "y.tab.c"
      break;

      case 176: /* equipfield: position number  */
#line 1126 "dmcpar.y"
      {
         cur_cmd->num2 = (yyvsp[0].num);
      }
#line 3115 "y.tab.c"
      break;

      case 177: /* equipfield: COMPLETE  */
#line 1130 "dmcpar.y"
      {
         cur_cmd->cmpl = !0;
      }
#line 3123 "y.tab.c"
      break;

      case 178: /* local: LOCAL number  */
#line 1135 "dmcpar.y"
      {
         cur_cmd->num3 = (yyvsp[0].num);
      }
#line 3131 "y.tab.c"
      break;

      case 179: /* zonemax: ZONEMAX number  */
#line 1140 "dmcpar.y"
      {
         cur_cmd->num2 = (yyvsp[0].num);
      }
#line 3139 "y.tab.c"
      break;

      case 180: /* max: MAX number  */
#line 1145 "dmcpar.y"
      {
         cur_cmd->num1 = (yyvsp[0].num);
      }
#line 3147 "y.tab.c"
      break;

      case 183: /* moneylist: number moneytype  */
#line 1160 "dmcpar.y"
      {
         mon_top              = 0;
         mon_list[mon_top][0] = (yyvsp[-1].num);
         mon_list[mon_top][1] = (yyvsp[0].num);

         (yyval.num) = (yyvsp[-1].num) * money_types[(yyvsp[0].num)].relative_value;
      }
#line 3159 "y.tab.c"
      break;

      case 184: /* moneylist: moneylist ',' number moneytype  */
#line 1168 "dmcpar.y"
      {
         ++mon_top;
         mon_list[mon_top][0] = (yyvsp[-1].num);
         mon_list[mon_top][1] = (yyvsp[0].num);

         (yyval.num) = (yyvsp[-3].num);

         (yyval.num) += (yyvsp[-1].num) * money_types[(yyvsp[0].num)].relative_value;
      }
#line 3173 "y.tab.c"
      break;

      case 185: /* moneytype: STRING  */
#line 1179 "dmcpar.y"
      {
         int myi;
         (yyval.num) = -1;

         for(myi = 0; myi <= MAX_MONEY; myi++)
            if(!strcmp((yyvsp[0].str), money_types[myi].abbrev))
            {
               (yyval.num) = myi;
               break;
            }

         if(myi > MAX_MONEY)
            fatal("Not a legal money denominator.");
      }
#line 3192 "y.tab.c"
      break;

      case 186: /* stringlist: hardstringlist  */
#line 1196 "dmcpar.y"
      {
         (yyval.str_list) = (yyvsp[0].str_list);
      }
#line 3200 "y.tab.c"
      break;

      case 187: /* stringlist: STRING  */
#line 1200 "dmcpar.y"
      {
         (yyval.str_list)    = (char **)mmalloc(sizeof(char *) * 2);
         (yyval.str_list)[0] = (yyvsp[0].str);
         (yyval.str_list)[1] = 0;
      }
#line 3210 "y.tab.c"
      break;

      case 188: /* hardstringlist: '{' strings '}'  */
#line 1208 "dmcpar.y"
      {
         // MS2020 $$ = (char **) mmalloc(sizeof(char *) * (str_top + 1));
         // Looks like there's an error in the old code. str_top=0 means there's one string and
         // thus we will need 2 indices.
         // error is compounded by <= in the for loop and the final setting to 0 which will be OOB
         //
         (yyval.str_list) = (char **)mmalloc(sizeof(char *) * (str_top + 2));
         for(myi = 0; myi <= str_top; myi++)
            (yyval.str_list)[myi] = str_list[myi];
         (yyval.str_list)[myi] = 0;
      }
#line 3226 "y.tab.c"
      break;

      case 189: /* hardstringlist: '{' '}'  */
#line 1220 "dmcpar.y"
      {
         (yyval.str_list) = 0;
      }
#line 3234 "y.tab.c"
      break;

      case 190: /* strings: STRING  */
#line 1226 "dmcpar.y"
      {
         str_list[str_top = 0] = (yyvsp[0].str);
      }
#line 3242 "y.tab.c"
      break;

      case 191: /* strings: strings ',' STRING  */
#line 1230 "dmcpar.y"
      {
         str_list[++str_top] = (yyvsp[0].str);
      }
#line 3250 "y.tab.c"
      break;

      case 192: /* stringcomp: STRING  */
#line 1237 "dmcpar.y"
      {
         (yyval.str) = (yyvsp[0].str);
      }
#line 3258 "y.tab.c"
      break;

      case 193: /* stringcomp: stringcomp '+' STRING  */
#line 1241 "dmcpar.y"
      {
         char *res;
         res = (char *)mmalloc(strlen((yyvsp[-2].str)) + strlen((yyvsp[0].str)) + 2 + 3);
         strcpy(res, (yyvsp[-2].str));
         strcat(res, (yyvsp[0].str));
         (yyval.str) = res;
      }
#line 3270 "y.tab.c"
      break;

      case 194: /* stringcomp: stringcomp '+' '(' number ')'  */
#line 1252 "dmcpar.y"
      {
         char *res;
         char  buf[100];
         if(strlen((yyvsp[-4].str)) > 0 && isdigit((yyvsp[-4].str)[strlen((yyvsp[-4].str)) - 1]))
            sprintf(buf, " %d ", (yyvsp[-1].num));
         else
            sprintf(buf, "%d", (yyvsp[-1].num));

         res = (char *)mmalloc(strlen((yyvsp[-4].str)) + strlen(buf) + 2 + 3);
         strcpy(res, (yyvsp[-4].str));
         strcat(res, buf);
         (yyval.str) = res;
      }
#line 3288 "y.tab.c"
      break;

      case 195: /* stringcomp: stringcomp '+' PNUM  */
#line 1267 "dmcpar.y"
      {
         char *res;
         char  buf[100];
         if(strlen((yyvsp[-2].str)) > 0 && isdigit((yyvsp[-2].str)[strlen((yyvsp[-2].str)) - 1]))
            sprintf(buf, " %d", (yyvsp[0].num));
         else
            sprintf(buf, "%d", (yyvsp[0].num));

         res = (char *)mmalloc(strlen((yyvsp[-2].str)) + strlen(buf) + 2 + 3);
         strcpy(res, (yyvsp[-2].str));
         strcat(res, buf);
         (yyval.str) = res;
      }
#line 3306 "y.tab.c"
      break;

      case 196: /* flags: number  */
#line 1284 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3314 "y.tab.c"
      break;

      case 197: /* flags: '{' numbers '}'  */
#line 1288 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-1].num);
      }
#line 3322 "y.tab.c"
      break;

      case 198: /* numbers: flags  */
#line 1293 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3330 "y.tab.c"
      break;

      case 199: /* numbers: numbers ',' flags  */
#line 1297 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-2].num) | (yyvsp[0].num);
      }
#line 3338 "y.tab.c"
      break;

      case 200: /* number: expr  */
#line 1304 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3346 "y.tab.c"
      break;

      case 201: /* expr: term  */
#line 1309 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3354 "y.tab.c"
      break;

      case 202: /* expr: '-' term  */
#line 1313 "dmcpar.y"
      {
         (yyval.num) = -(yyvsp[0].num);
      }
#line 3362 "y.tab.c"
      break;

      case 203: /* expr: '+' term  */
#line 1317 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3370 "y.tab.c"
      break;

      case 204: /* expr: expr '+' term  */
#line 1321 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-2].num) + (yyvsp[0].num);
      }
#line 3378 "y.tab.c"
      break;

      case 205: /* expr: expr '-' term  */
#line 1325 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-2].num) - (yyvsp[0].num);
      }
#line 3386 "y.tab.c"
      break;

      case 206: /* term: factor  */
#line 1330 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3394 "y.tab.c"
      break;

      case 207: /* term: term '*' factor  */
#line 1334 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-2].num) * (yyvsp[0].num);
      }
#line 3402 "y.tab.c"
      break;

      case 208: /* term: term '/' factor  */
#line 1338 "dmcpar.y"
      {
         if((yyvsp[0].num) != 0)
            (yyval.num) = (yyvsp[-2].num) / (yyvsp[0].num);
         else
         {
            warning("Division by zero!");
            (yyval.num) = 0;
         }
      }
#line 3416 "y.tab.c"
      break;

      case 209: /* factor: '(' expr ')'  */
#line 1349 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-1].num);
      }
#line 3424 "y.tab.c"
      break;

      case 210: /* factor: PNUM  */
#line 1353 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[0].num);
      }
#line 3432 "y.tab.c"
      break;

      case 211: /* zonename: czonename  */
#line 1359 "dmcpar.y"
      {
         (yyval.str) = (char *)mmalloc(strlen((yyvsp[0].sym)) + 1);
         strcpy((yyval.str), (yyvsp[0].sym));
      }
#line 3441 "y.tab.c"
      break;

      case 212: /* czonename: SYMBOL  */
#line 1365 "dmcpar.y"
      {
         if(strlen((yyvsp[0].sym)) > FI_MAX_ZONENAME)
         {
            warning("Zone name too long. truncated.");
            (yyvsp[0].sym)[FI_MAX_ZONENAME] = '\0';
         }
         strcpy((yyval.sym), (yyvsp[0].sym));
      }
#line 3454 "y.tab.c"
      break;

      case 213: /* unitname: cunitname  */
#line 1375 "dmcpar.y"
      {
         (yyval.str) = (char *)mmalloc(strlen((yyvsp[0].sym)) + 1);
         strcpy((yyval.str), (yyvsp[0].sym));
      }
#line 3463 "y.tab.c"
      break;

      case 214: /* cunitname: SYMBOL  */
#line 1381 "dmcpar.y"
      {
         if(strlen((yyvsp[0].sym)) > FI_MAX_UNITNAME)
         {
            warning("Unit name too long. truncated.");
            (yyvsp[0].sym)[FI_MAX_UNITNAME] = '\0';
         }
         strcpy((yyval.sym), (yyvsp[0].sym));
      }
#line 3476 "y.tab.c"
      break;

      case 215: /* reference: cunitname  */
#line 1391 "dmcpar.y"
      {
         if(!zone.z_zone.name)
         {
            fatal("local zonename must be defined.");
            (yyval.str) = empty_ref;
         }
         else
         {
            (yyval.str) = (char *)mmalloc(strlen((yyvsp[0].sym)) + strlen(zone.z_zone.name) + 2);
            strcpy((yyval.str), zone.z_zone.name);
            strcpy((yyval.str) + strlen(zone.z_zone.name) + 1, (yyvsp[0].sym));
         }
      }
#line 3495 "y.tab.c"
      break;

      case 216: /* reference: cunitname '@' czonename  */
#line 1406 "dmcpar.y"
      {
         (yyval.str) = (char *)mmalloc(strlen((yyvsp[-2].sym)) + strlen((yyvsp[0].sym)) + 2);
         strcpy((yyval.str), (yyvsp[0].sym));
         strcpy((yyval.str) + strlen((yyvsp[0].sym)) + 1, (yyvsp[-2].sym));
      }
#line 3505 "y.tab.c"
      break;

      case 217: /* reference: czonename '/' cunitname  */
#line 1412 "dmcpar.y"
      {
         (yyval.str) = (char *)mmalloc(strlen((yyvsp[-2].sym)) + strlen((yyvsp[0].sym)) + 2);
         strcpy((yyval.str), (yyvsp[-2].sym));
         strcpy((yyval.str) + strlen((yyvsp[-2].sym)) + 1, (yyvsp[0].sym));
      }
#line 3515 "y.tab.c"
      break;

      case 218: /* index: '[' PNUM ']'  */
#line 1419 "dmcpar.y"
      {
         (yyval.num) = (yyvsp[-1].num);
      }
#line 3523 "y.tab.c"
      break;

#line 3527 "y.tab.c"

      default:
         break;
   }
   /* User semantic actions sometimes alter yychar, and that requires
      that yytoken be updated with the new translation.  We take the
      approach of translating immediately before every use of yytoken.
      One alternative is translating here after every semantic action,
      but that translation would be missed if the semantic action invokes
      YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
      if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
      incorrect destructor might then be invoked immediately.  In the
      case of YYERROR or YYBACKUP, subsequent parser actions might lead
      to an incorrect destructor call or verbose syntax error message
      before the lookahead is translated.  */
   YY_SYMBOL_PRINT("-> $$ =", YY_CAST(yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

   YYPOPSTACK(yylen);
   yylen = 0;

   *++yyvsp = yyval;

   /* Now 'shift' the result of the reduction.  Determine what state
      that goes to, based on the state we popped back to and the rule
      number reduced by.  */
   {
      const int yylhs = yyr1[yyn] - YYNTOKENS;
      const int yyi   = yypgoto[yylhs] + *yyssp;
      yystate         = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp ? yytable[yyi] : yydefgoto[yylhs]);
   }

   goto yynewstate;

/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
   /* Make sure we have latest lookahead translation.  See comments at
      user semantic actions for why this is necessary.  */
   yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE(yychar);
   /* If not already recovering from an error, report this error.  */
   if(!yyerrstatus)
   {
      ++yynerrs;
      yyerror(YY_("syntax error"));
   }

   if(yyerrstatus == 3)
   {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if(yychar <= YYEOF)
      {
         /* Return failure if at end of input.  */
         if(yychar == YYEOF)
            YYABORT;
      }
      else
      {
         yydestruct("Error: discarding", yytoken, &yylval);
         yychar = YYEMPTY;
      }
   }

   /* Else will try to reuse lookahead token after shifting the error
      token.  */
   goto yyerrlab1;

/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
   /* Pacify compilers when the user code never invokes YYERROR and the
      label yyerrorlab therefore never appears in user code.  */
   if(0)
      YYERROR;

   /* Do not reclaim the symbols of the rule whose action triggered
      this YYERROR.  */
   YYPOPSTACK(yylen);
   yylen = 0;
   YY_STACK_PRINT(yyss, yyssp);
   yystate = *yyssp;
   goto yyerrlab1;

/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
   yyerrstatus = 3; /* Each real token shifted decrements this.  */

   /* Pop stack until we find a state that shifts the error token.  */
   for(;;)
   {
      yyn = yypact[yystate];
      if(!yypact_value_is_default(yyn))
      {
         yyn += YYSYMBOL_YYerror;
         if(0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
         {
            yyn = yytable[yyn];
            if(0 < yyn)
               break;
         }
      }

      /* Pop the current state because it cannot handle the error token.  */
      if(yyssp == yyss)
         YYABORT;

      yydestruct("Error: popping", YY_ACCESSING_SYMBOL(yystate), yyvsp);
      YYPOPSTACK(1);
      yystate = *yyssp;
      YY_STACK_PRINT(yyss, yyssp);
   }

   YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   *++yyvsp = yylval;
   YY_IGNORE_MAYBE_UNINITIALIZED_END

   /* Shift the error token.  */
   YY_SYMBOL_PRINT("Shifting", YY_ACCESSING_SYMBOL(yyn), yyvsp, yylsp);

   yystate = yyn;
   goto yynewstate;

/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
   yyresult = 0;
   goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
   yyresult = 1;
   goto yyreturn;

#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
   yyerror(YY_("memory exhausted"));
   yyresult = 2;
   goto yyreturn;
#endif

/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
   if(yychar != YYEMPTY)
   {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE(yychar);
      yydestruct("Cleanup: discarding lookahead", yytoken, &yylval);
   }
   /* Do not reclaim the symbols of the rule whose action triggered
      this YYABORT or YYACCEPT.  */
   YYPOPSTACK(yylen);
   YY_STACK_PRINT(yyss, yyssp);
   while(yyssp != yyss)
   {
      yydestruct("Cleanup: popping", YY_ACCESSING_SYMBOL(+*yyssp), yyvsp);
      YYPOPSTACK(1);
   }
#ifndef yyoverflow
   if(yyss != yyssa)
      YYSTACK_FREE(yyss);
#endif

   return yyresult;
}

#line 1422 "dmcpar.y"

void syntax(const char *str)
{
   extern char *yytext;

   fprintf(stderr, "%s: %d: %s\n   Token: '%s'\n", cur_filename, linenum, str, yytext);
   errcon = 1;
}

void fatal(const char *str)
{
   fprintf(stderr, "%s: %d: %s\n", cur_filename, linenum, str);
   errcon = 1;
}

void real_warning(const char *str)
{
   fprintf(stderr, "WARNING: %s: %d: %s\n", cur_filename, linenum, str);
}

void warning(const char *str)
{
   extern int fatal_warnings;

   if(fatal_warnings)
      fatal(str);
   else
      real_warning(str);
}

/* Read a string from fl, allocate space for it and return it */
char *str_dup_file(FILE *fl)
{
   char tmp[MAX_STRING_LENGTH], *res;

   CREATE(res, char, 1);

   while(fgets(tmp, MAX_STRING_LENGTH, fl))
   {
      RECREATE(res, char, strlen(tmp) + strlen(res) + 1);
      strcat(res, tmp);
   }
   return res;
}

void dumpdiltemplate(struct diltemplate *tmpl)
{
   int i;
   fprintf(stderr, "Name: '%s'\n", tmpl->prgname);
   fprintf(stderr, "Crc: %d\n", tmpl->varcrc);
   fprintf(stderr, "Return type: %d\n", tmpl->rtnt);
   fprintf(stderr, "# Arguments: %d\n", tmpl->argc);
   for(myi = 0; myi < tmpl->argc; myi++)
      fprintf(stderr, "Arg %d type: %d\n", i, tmpl->argt[myi]);
   fprintf(stderr, "Coresize: %d\n", tmpl->coresz);
   fprintf(stderr, "Core: %p\n", tmpl->core);
   fprintf(stderr, "# Vars: %d\n", tmpl->varc);
   for(myi = 0; myi < tmpl->varc; myi++)
      fprintf(stderr, "Var %d type: %d\n", myi, tmpl->vart[myi]);
   fprintf(stderr, "# References: %d\n", tmpl->xrefcount);
   for(myi = 0; myi < tmpl->xrefcount; myi++)
   {
      fprintf(stderr, "Xref %d: '%s'\n", myi, tmpl->xrefs[myi].name);
      fprintf(stderr, "     Return type %d\n", tmpl->xrefs[myi].rtnt);
      fprintf(stderr, "     # Arguments: %d\n", tmpl->xrefs[myi].argc);
   }
}

void dumpdil(struct dilprg *prg)
{
   int i;

   fprintf(stderr, "Flags: %d\n", prg->flags);
   fprintf(stderr, "Varcrc: %d\n", prg->varcrc);
   fprintf(stderr, "Stacksz: %d\n", prg->stacksz);
   fprintf(stderr, "Stackptr: %d\n", (int)(1 + (prg->sp - prg->stack)));
   for(i = 0; i <= (prg->sp - prg->stack); i++)
   {
      fprintf(stderr, "Stackframe %d:\n", i);
      fprintf(stderr, "Return var# %d:\n", prg->stack[i].ret);
      dumpdiltemplate(prg->stack[i].tmpl);
   }
}
