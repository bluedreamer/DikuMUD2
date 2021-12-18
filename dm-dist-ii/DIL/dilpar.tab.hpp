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

#ifndef YY_YY_DILPAR_TAB_HPP_INCLUDED
# define YY_YY_DILPAR_TAB_HPP_INCLUDED
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
    UNKNOWN = 258,                 /* UNKNOWN  */
    STRING = 259,                  /* STRING  */
    SYMBOL = 260,                  /* SYMBOL  */
    PNUM = 261,                    /* PNUM  */
    DILSC_VAR = 262,               /* DILSC_VAR  */
    DILSC_BEG = 263,               /* DILSC_BEG  */
    DILSC_COD = 264,               /* DILSC_COD  */
    DILSC_END = 265,               /* DILSC_END  */
    DILSC_EXT = 266,               /* DILSC_EXT  */
    DILSC_REC = 267,               /* DILSC_REC  */
    DILSC_AWA = 268,               /* DILSC_AWA  */
    DILSE_ATOI = 269,              /* DILSE_ATOI  */
    DILSE_RND = 270,               /* DILSE_RND  */
    DILSE_FNDU = 271,              /* DILSE_FNDU  */
    DILSE_FNDRU = 272,             /* DILSE_FNDRU  */
    DILSE_FNDR = 273,              /* DILSE_FNDR  */
    DILSE_LOAD = 274,              /* DILSE_LOAD  */
    DILSE_GETW = 275,              /* DILSE_GETW  */
    DILSE_ITOA = 276,              /* DILSE_ITOA  */
    DILSE_ISS = 277,               /* DILSE_ISS  */
    DILSE_IN = 278,                /* DILSE_IN  */
    DILSE_ISA = 279,               /* DILSE_ISA  */
    DILSE_CMDS = 280,              /* DILSE_CMDS  */
    DILSE_FNDS = 281,              /* DILSE_FNDS  */
    DILSE_GETWS = 282,             /* DILSE_GETWS  */
    DILSE_LEN = 283,               /* DILSE_LEN  */
    DILSE_PURS = 284,              /* DILSE_PURS  */
    DILSE_TRMO = 285,              /* DILSE_TRMO  */
    DILSE_DLD = 286,               /* DILSE_DLD  */
    DILSE_DLF = 287,               /* DILSE_DLF  */
    DILSE_LAND = 288,              /* DILSE_LAND  */
    DILSE_LOR = 289,               /* DILSE_LOR  */
    DILSE_VISI = 290,              /* DILSE_VISI  */
    DILSE_OPPO = 291,              /* DILSE_OPPO  */
    DILSE_RTI = 292,               /* DILSE_RTI  */
    DILSE_PCK = 293,               /* DILSE_PCK  */
    DILSE_AND = 294,               /* DILSE_AND  */
    DILSE_OR = 295,                /* DILSE_OR  */
    DILSE_NOT = 296,               /* DILSE_NOT  */
    DILTO_EQ = 297,                /* DILTO_EQ  */
    DILTO_NEQ = 298,               /* DILTO_NEQ  */
    DILTO_PEQ = 299,               /* DILTO_PEQ  */
    DILTO_SEQ = 300,               /* DILTO_SEQ  */
    DILTO_LEQ = 301,               /* DILTO_LEQ  */
    DILTO_GEQ = 302,               /* DILTO_GEQ  */
    DILTO_GT = 303,                /* DILTO_GT  */
    DILTO_LE = 304,                /* DILTO_LE  */
    DILTO_ASS = 305,               /* DILTO_ASS  */
    DILSE_NULL = 306,              /* DILSE_NULL  */
    DILSE_SELF = 307,              /* DILSE_SELF  */
    DILSE_ACTI = 308,              /* DILSE_ACTI  */
    DILSE_ARGM = 309,              /* DILSE_ARGM  */
    DILSE_HRT = 310,               /* DILSE_HRT  */
    DILSE_CMST = 311,              /* DILSE_CMST  */
    DILSE_TDA = 312,               /* DILSE_TDA  */
    DILSE_THO = 313,               /* DILSE_THO  */
    DILSE_TMD = 314,               /* DILSE_TMD  */
    DILSE_TYE = 315,               /* DILSE_TYE  */
    DILSE_SKIP = 316,              /* DILSE_SKIP  */
    DILSE_WEAT = 317,              /* DILSE_WEAT  */
    DILSE_MEDI = 318,              /* DILSE_MEDI  */
    DILSE_TARG = 319,              /* DILSE_TARG  */
    DILSE_POWE = 320,              /* DILSE_POWE  */
    DILSE_CST = 321,               /* DILSE_CST  */
    DILSE_MEL = 322,               /* DILSE_MEL  */
    DILSE_EQPM = 323,              /* DILSE_EQPM  */
    DILSE_OPRO = 324,              /* DILSE_OPRO  */
    DILSE_REST = 325,              /* DILSE_REST  */
    DILSE_FIT = 326,               /* DILSE_FIT  */
    DILSE_CARY = 327,              /* DILSE_CARY  */
    DILSE_PATH = 328,              /* DILSE_PATH  */
    DILSE_MONS = 329,              /* DILSE_MONS  */
    DILSE_SPLX = 330,              /* DILSE_SPLX  */
    DILSE_SPLI = 331,              /* DILSE_SPLI  */
    DILSE_TXF = 332,               /* DILSE_TXF  */
    DILSE_AST = 333,               /* DILSE_AST  */
    DILSI_LNK = 334,               /* DILSI_LNK  */
    DILSI_EXP = 335,               /* DILSI_EXP  */
    DILSI_SET = 336,               /* DILSI_SET  */
    DILSI_UST = 337,               /* DILSI_UST  */
    DILSI_ADE = 338,               /* DILSI_ADE  */
    DILSI_SUE = 339,               /* DILSI_SUE  */
    DILSI_DST = 340,               /* DILSI_DST  */
    DILSI_ADL = 341,               /* DILSI_ADL  */
    DILSI_SUL = 342,               /* DILSI_SUL  */
    DILSI_SND = 343,               /* DILSI_SND  */
    DILSI_SNT = 344,               /* DILSI_SNT  */
    DILSI_SEC = 345,               /* DILSI_SEC  */
    DILSI_USE = 346,               /* DILSI_USE  */
    DILSI_ADA = 347,               /* DILSI_ADA  */
    DILSI_SETF = 348,              /* DILSI_SETF  */
    DILSI_CHAS = 349,              /* DILSI_CHAS  */
    DILSI_SUA = 350,               /* DILSI_SUA  */
    DILSI_EQP = 351,               /* DILSI_EQP  */
    DILSI_UEQ = 352,               /* DILSI_UEQ  */
    DILSI_OAC = 353,               /* DILSI_OAC  */
    DILSI_STOR = 354,              /* DILSI_STOR  */
    DILSI_SETE = 355,              /* DILSI_SETE  */
    DILSI_QUIT = 356,              /* DILSI_QUIT  */
    DILSI_LOG = 357,               /* DILSI_LOG  */
    DILSI_SNTA = 358,              /* DILSI_SNTA  */
    DILSI_SNTADIL = 359,           /* DILSI_SNTADIL  */
    DILSI_DLC = 360,               /* DILSI_DLC  */
    DILSE_INTR = 361,              /* DILSE_INTR  */
    DILSI_CLI = 362,               /* DILSI_CLI  */
    DILSI_SWT = 363,               /* DILSI_SWT  */
    DILSI_SBT = 364,               /* DILSI_SBT  */
    DILSE_ATSP = 365,              /* DILSE_ATSP  */
    DILSI_FOLO = 366,              /* DILSI_FOLO  */
    DILSI_LCRI = 367,              /* DILSI_LCRI  */
    DILSI_WHI = 368,               /* DILSI_WHI  */
    DILSI_IF = 369,                /* DILSI_IF  */
    DILSI_WLK = 370,               /* DILSI_WLK  */
    DILSI_EXE = 371,               /* DILSI_EXE  */
    DILSI_WIT = 372,               /* DILSI_WIT  */
    DILSI_ACT = 373,               /* DILSI_ACT  */
    DILSI_ELS = 374,               /* DILSI_ELS  */
    DILSI_GOT = 375,               /* DILSI_GOT  */
    DILSI_PRI = 376,               /* DILSI_PRI  */
    DILSI_NPR = 377,               /* DILSI_NPR  */
    DILSI_BLK = 378,               /* DILSI_BLK  */
    DILSI_CNT = 379,               /* DILSI_CNT  */
    DILSI_PUP = 380,               /* DILSI_PUP  */
    DILSI_FOR = 381,               /* DILSI_FOR  */
    DILSI_FOE = 382,               /* DILSI_FOE  */
    DILSI_BRK = 383,               /* DILSI_BRK  */
    DILSI_RTS = 384,               /* DILSI_RTS  */
    DILSI_ON = 385,                /* DILSI_ON  */
    DILSI_AMOD = 386,              /* DILSI_AMOD  */
    DILSF_ZOI = 387,               /* DILSF_ZOI  */
    DILSF_NMI = 388,               /* DILSF_NMI  */
    DILSF_TYP = 389,               /* DILSF_TYP  */
    DILSF_NXT = 390,               /* DILSF_NXT  */
    DILSF_NMS = 391,               /* DILSF_NMS  */
    DILSF_NAM = 392,               /* DILSF_NAM  */
    DILSF_IDX = 393,               /* DILSF_IDX  */
    DILSF_TIT = 394,               /* DILSF_TIT  */
    DILSF_EXT = 395,               /* DILSF_EXT  */
    DILSF_OUT = 396,               /* DILSF_OUT  */
    DILSF_INS = 397,               /* DILSF_INS  */
    DILSF_GNX = 398,               /* DILSF_GNX  */
    DILSF_SPD = 399,               /* DILSF_SPD  */
    DILSF_GPR = 400,               /* DILSF_GPR  */
    DILSF_LGT = 401,               /* DILSF_LGT  */
    DILSF_BGT = 402,               /* DILSF_BGT  */
    DILSF_MIV = 403,               /* DILSF_MIV  */
    DILSF_ILL = 404,               /* DILSF_ILL  */
    DILSF_FL = 405,                /* DILSF_FL  */
    DILSF_OFL = 406,               /* DILSF_OFL  */
    DILSF_MHP = 407,               /* DILSF_MHP  */
    DILSF_CHP = 408,               /* DILSF_CHP  */
    DILSF_BWT = 409,               /* DILSF_BWT  */
    DILSF_WGT = 410,               /* DILSF_WGT  */
    DILSF_CAP = 411,               /* DILSF_CAP  */
    DILSF_ALG = 412,               /* DILSF_ALG  */
    DILSF_SPL = 413,               /* DILSF_SPL  */
    DILSF_FUN = 414,               /* DILSF_FUN  */
    DILSF_ZON = 415,               /* DILSF_ZON  */
    DILSF_MAS = 416,               /* DILSF_MAS  */
    DILSF_FOL = 417,               /* DILSF_FOL  */
    DILSF_OTY = 418,               /* DILSF_OTY  */
    DILSF_VAL = 419,               /* DILSF_VAL  */
    DILSF_EFL = 420,               /* DILSF_EFL  */
    DILSF_CST = 421,               /* DILSF_CST  */
    DILSF_RNT = 422,               /* DILSF_RNT  */
    DILSF_EQP = 423,               /* DILSF_EQP  */
    DILSF_ONM = 424,               /* DILSF_ONM  */
    DILSF_XNF = 425,               /* DILSF_XNF  */
    DILSF_TOR = 426,               /* DILSF_TOR  */
    DILSF_RFL = 427,               /* DILSF_RFL  */
    DILSF_MOV = 428,               /* DILSF_MOV  */
    DILSF_ACT = 429,               /* DILSF_ACT  */
    DILSF_SEX = 430,               /* DILSF_SEX  */
    DILSF_RCE = 431,               /* DILSF_RCE  */
    DILSF_ABL = 432,               /* DILSF_ABL  */
    DILSF_EXP = 433,               /* DILSF_EXP  */
    DILSF_LVL = 434,               /* DILSF_LVL  */
    DILSF_HGT = 435,               /* DILSF_HGT  */
    DILSF_POS = 436,               /* DILSF_POS  */
    DILSF_ATY = 437,               /* DILSF_ATY  */
    DILSF_MNA = 438,               /* DILSF_MNA  */
    DILSF_END = 439,               /* DILSF_END  */
    DILSF_WPN = 440,               /* DILSF_WPN  */
    DILSF_MAN = 441,               /* DILSF_MAN  */
    DILSF_AFF = 442,               /* DILSF_AFF  */
    DILSF_DRE = 443,               /* DILSF_DRE  */
    DILSF_FGT = 444,               /* DILSF_FGT  */
    DILSF_DEF = 445,               /* DILSF_DEF  */
    DILSF_TIM = 446,               /* DILSF_TIM  */
    DILSF_CRM = 447,               /* DILSF_CRM  */
    DILSF_FLL = 448,               /* DILSF_FLL  */
    DILSF_THR = 449,               /* DILSF_THR  */
    DILSF_DRK = 450,               /* DILSF_DRK  */
    DILSF_SPT = 451,               /* DILSF_SPT  */
    DILSF_APT = 452,               /* DILSF_APT  */
    DILSF_GLD = 453,               /* DILSF_GLD  */
    DILSF_QST = 454,               /* DILSF_QST  */
    DILSF_SKL = 455,               /* DILSF_SKL  */
    DILSF_BIR = 456,               /* DILSF_BIR  */
    DILSF_PTI = 457,               /* DILSF_PTI  */
    DILSF_PCF = 458,               /* DILSF_PCF  */
    DILSF_HOME = 459,              /* DILSF_HOME  */
    DILSF_ODES = 460,              /* DILSF_ODES  */
    DILSF_IDES = 461,              /* DILSF_IDES  */
    DILSF_DES = 462,               /* DILSF_DES  */
    DILSF_LCN = 463,               /* DILSF_LCN  */
    DILSF_ABAL = 464,              /* DILSF_ABAL  */
    DILSF_ATOT = 465,              /* DILSF_ATOT  */
    DILSF_MMA = 466,               /* DILSF_MMA  */
    DILSF_LSA = 467,               /* DILSF_LSA  */
    DILSF_INFO = 468,              /* DILSF_INFO  */
    DILSF_MED = 469,               /* DILSF_MED  */
    DILST_UP = 470,                /* DILST_UP  */
    DILST_INT = 471,               /* DILST_INT  */
    DILST_SP = 472,                /* DILST_SP  */
    DILST_SLP = 473,               /* DILST_SLP  */
    DILST_EDP = 474,               /* DILST_EDP  */
    UMINUS = 475,                  /* UMINUS  */
    UPLUS = 476                    /* UPLUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 154 "dilpar.ypp"

   sbit32 num;
   char *str;
   char sym[SYMSIZE + 1];
   char **str_list;
   struct exptype exp;
   struct {
      ubit32 fst,lst; /* first, last addr in core */
      ubit8 dsl,typ;      /* if expression: leftvalue, type */
      ubit8 boolean;
   } ins;
   struct dilxref xref;

#line 299 "dilpar.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_DILPAR_TAB_HPP_INCLUDED  */
