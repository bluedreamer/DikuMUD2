/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    FLAGS = 258,                   /* FLAGS  */
    END = 259,                     /* END  */
    UNKNOWN = 260,                 /* UNKNOWN  */
    OBJECTS = 261,                 /* OBJECTS  */
    ZONE = 262,                    /* ZONE  */
    RESET = 263,                   /* RESET  */
    ROOMS = 264,                   /* ROOMS  */
    MOBILES = 265,                 /* MOBILES  */
    ENDMARK = 266,                 /* ENDMARK  */
    DIL = 267,                     /* DIL  */
    DILPRG = 268,                  /* DILPRG  */
    STRING = 269,                  /* STRING  */
    SYMBOL = 270,                  /* SYMBOL  */
    PNUM = 271,                    /* PNUM  */
    NAMES = 272,                   /* NAMES  */
    TITLE = 273,                   /* TITLE  */
    DESCR = 274,                   /* DESCR  */
    OUT_DESCR = 275,               /* OUT_DESCR  */
    IN_DESCR = 276,                /* IN_DESCR  */
    EXTRA = 277,                   /* EXTRA  */
    KEY = 278,                     /* KEY  */
    OPEN = 279,                    /* OPEN  */
    MANIPULATE = 280,              /* MANIPULATE  */
    UFLAGS = 281,                  /* UFLAGS  */
    WEIGHT = 282,                  /* WEIGHT  */
    CAPACITY = 283,                /* CAPACITY  */
    SPECIAL = 284,                 /* SPECIAL  */
    LIGHT = 285,                   /* LIGHT  */
    BRIGHT = 286,                  /* BRIGHT  */
    MINV = 287,                    /* MINV  */
    RANDOM = 288,                  /* RANDOM  */
    DILCOPY = 289,                 /* DILCOPY  */
    ID = 290,                      /* ID  */
    DATA = 291,                    /* DATA  */
    DURATION = 292,                /* DURATION  */
    FIRSTF = 293,                  /* FIRSTF  */
    TICKF = 294,                   /* TICKF  */
    LASTF = 295,                   /* LASTF  */
    APPLYF = 296,                  /* APPLYF  */
    TIME = 297,                    /* TIME  */
    BITS = 298,                    /* BITS  */
    STRINGT = 299,                 /* STRINGT  */
    EXIT = 300,                    /* EXIT  */
    MOVEMENT = 301,                /* MOVEMENT  */
    IN = 302,                      /* IN  */
    SPELL = 303,                   /* SPELL  */
    LINK = 304,                    /* LINK  */
    TO = 305,                      /* TO  */
    KEYWORD = 306,                 /* KEYWORD  */
    VALUE = 307,                   /* VALUE  */
    COST = 308,                    /* COST  */
    RENT = 309,                    /* RENT  */
    TYPE = 310,                    /* TYPE  */
    AFFECT = 311,                  /* AFFECT  */
    MANA = 312,                    /* MANA  */
    HIT = 313,                     /* HIT  */
    MONEY = 314,                   /* MONEY  */
    EXP = 315,                     /* EXP  */
    ALIGNMENT = 316,               /* ALIGNMENT  */
    SEX = 317,                     /* SEX  */
    LEVEL = 318,                   /* LEVEL  */
    HEIGHT = 319,                  /* HEIGHT  */
    RACE = 320,                    /* RACE  */
    POSITION = 321,                /* POSITION  */
    ABILITY = 322,                 /* ABILITY  */
    WEAPON = 323,                  /* WEAPON  */
    ATTACK = 324,                  /* ATTACK  */
    ARMOUR = 325,                  /* ARMOUR  */
    DEFENSIVE = 326,               /* DEFENSIVE  */
    OFFENSIVE = 327,               /* OFFENSIVE  */
    SPEED = 328,                   /* SPEED  */
    DEFAULT = 329,                 /* DEFAULT  */
    ACT = 330,                     /* ACT  */
    RESET_F = 331,                 /* RESET_F  */
    LIFESPAN = 332,                /* LIFESPAN  */
    CREATORS = 333,                /* CREATORS  */
    NOTES = 334,                   /* NOTES  */
    HELP = 335,                    /* HELP  */
    WEATHER = 336,                 /* WEATHER  */
    LOAD = 337,                    /* LOAD  */
    FOLLOW = 338,                  /* FOLLOW  */
    MAX = 339,                     /* MAX  */
    ZONEMAX = 340,                 /* ZONEMAX  */
    LOCAL = 341,                   /* LOCAL  */
    INTO = 342,                    /* INTO  */
    NOP = 343,                     /* NOP  */
    EQUIP = 344,                   /* EQUIP  */
    DOOR = 345,                    /* DOOR  */
    PURGE = 346,                   /* PURGE  */
    REMOVE = 347,                  /* REMOVE  */
    COMPLETE = 348,                /* COMPLETE  */
    SPLUS = 349,                   /* SPLUS  */
    UMINUS = 350,                  /* UMINUS  */
    UPLUS = 351                    /* UPLUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define FLAGS 258
#define END 259
#define UNKNOWN 260
#define OBJECTS 261
#define ZONE 262
#define RESET 263
#define ROOMS 264
#define MOBILES 265
#define ENDMARK 266
#define DIL 267
#define DILPRG 268
#define STRING 269
#define SYMBOL 270
#define PNUM 271
#define NAMES 272
#define TITLE 273
#define DESCR 274
#define OUT_DESCR 275
#define IN_DESCR 276
#define EXTRA 277
#define KEY 278
#define OPEN 279
#define MANIPULATE 280
#define UFLAGS 281
#define WEIGHT 282
#define CAPACITY 283
#define SPECIAL 284
#define LIGHT 285
#define BRIGHT 286
#define MINV 287
#define RANDOM 288
#define DILCOPY 289
#define ID 290
#define DATA 291
#define DURATION 292
#define FIRSTF 293
#define TICKF 294
#define LASTF 295
#define APPLYF 296
#define TIME 297
#define BITS 298
#define STRINGT 299
#define EXIT 300
#define MOVEMENT 301
#define IN 302
#define SPELL 303
#define LINK 304
#define TO 305
#define KEYWORD 306
#define VALUE 307
#define COST 308
#define RENT 309
#define TYPE 310
#define AFFECT 311
#define MANA 312
#define HIT 313
#define MONEY 314
#define EXP 315
#define ALIGNMENT 316
#define SEX 317
#define LEVEL 318
#define HEIGHT 319
#define RACE 320
#define POSITION 321
#define ABILITY 322
#define WEAPON 323
#define ATTACK 324
#define ARMOUR 325
#define DEFENSIVE 326
#define OFFENSIVE 327
#define SPEED 328
#define DEFAULT 329
#define ACT 330
#define RESET_F 331
#define LIFESPAN 332
#define CREATORS 333
#define NOTES 334
#define HELP 335
#define WEATHER 336
#define LOAD 337
#define FOLLOW 338
#define MAX 339
#define ZONEMAX 340
#define LOCAL 341
#define INTO 342
#define NOP 343
#define EQUIP 344
#define DOOR 345
#define PURGE 346
#define REMOVE 347
#define COMPLETE 348
#define SPLUS 349
#define UMINUS 350
#define UPLUS 351

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 102 "dmcpar.y"

  char *str;
  char sym[SYMSIZE + 1];
  char **str_list;
  struct dilargtype *dilarg;
  struct dilargstype *dilargs;
  int num;

#line 268 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
