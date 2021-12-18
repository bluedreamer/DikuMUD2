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
#line 37 "dilpar.ypp"

#include <stdio.h>
#include <unistd.h>

#include "structs.h"
#include "utils.h"
#include "dilpar.h"
#include "textutil.h"
#include "db_file.h"
#include "dil.h"

extern char *yytext; 
extern int yylex();
extern int linenum;

 int verbose = 0; // MS2020
 
/*
 *  MUST be changed to
 *  `extern char yytext[]'
 *  if you use lex instead of flex.
 */

int sunlight = 0;
const sbit8 time_light[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

#define yyerror(dum) syntax(dum)

#define DSL_STA 0            /* static expression */
#define DSL_DYN 1            /* dynamic expression */
#define DSL_LFT 2            /* left expression */
#define DSL_FCT 3            /* functon/procedure */
#define DSL_SFT 4            /* symbolic functon/procedure */

char cur_filename[255];

const char *cur_zonename="NOZONE"; /* zone name from argument */
char cur_tmplref[8192];      /* generated template reference (if -t) */

char **var_names;       /* names of variables */
ubit8 in_foreach=0;     /* inside foreach */
 
ubit16 label_no;        /* number of labels */
char **label_names;     /* names of labels */
ubit32 *label_adr;      /* address of label */

ubit16 label_use_no;    /* number of used labels */
ubit32 *label_use_idx;  /* what label is used (label_adr) */
ubit32 *label_use_adr;  /* where a label is used */
ubit32 labelgen;        /* counter for label generation */

ubit16 break_no;		/* size of break stack */
ubit16 cont_no;			/* size of continue stack */
ubit16 *break_idx;		/* break stack (label idx) */
ubit16 *cont_idx;		/* continue stack (label idx) */

int errcon = 0;
int istemplate = 0;		/* is this a template program */

/* work structures */

struct dilref {
   char *zname;		/* func/proc name */
   char *name;          /* zone name */
   ubit8 rtnt;		/* return type */
   ubit8 argc;		/* number of arguments (min 1) */
   ubit8 *argt;		/* argument types */
   char **argv;		/* argument names */
};

struct dilprg prg;              /* current program building */
struct diltemplate tmpl;        /* current template building */
struct dilframe frm;		/* current frame building */
struct dilref ref;		/* current reference */

#define REFMAX 2048 		/* max # of references */
#define VARMAX 255		/* max # of variables */
#define ARGMAX 250		/* max # of arguments */

struct dilref refs[REFMAX];	/* references read  */

/* the registered names in 'name@zone' format */
char **ref_names;		/* for fast search */
char **ref_usednames;           /* used names, registered in tmpl */
int refcount = 0;		/* number of ext. ref. */

ubit8 *wcore;           /* writing pointer (in cur.core )  */
ubit8 *wtmp;            /* writing pointer (in cur.core )  */

char *str_list[300];     /* Data for string lists */
int str_top;            /* Number of strings */
char tmpfname[L_tmpnam] = "";

void add_ref(struct dilref *ref);
void add_var(char *name, ubit16 type);
int add_label(char *str, ubit32 adr);
ubit32 get_label(char *name, ubit32 adr);
void moredilcore(ubit32 size);
void update_labels(void);
void fatal(const char *str);
void warning(const char *str);
void syntax(const char *str);
void dumpdiltemplate(struct diltemplate *tmpl);
void dumpdil(struct dilprg *prg);
int dil_corecrc(ubit8 *core, int len);
int dil_headercrc(char **name, ubit8 *type);

#define INITEXP(v) \
   CREATE(v.code, ubit8, CODESIZE); \
   v.codep=v.code;v.num=0;v.typ=v.rtyp=DILV_NULL;v.dsl=DSL_STA;v.boolean=0;

#define FREEEXP(v) \
   free(v.code); v.code = NULL;

#define checkbool(s,v) do {if (v) fprintf(stderr,"Warning: %s is boolean, not integer\n",s);} while (0)

#line 188 "dilpar.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "dilpar.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_3_ = 3,                         /* '('  */
  YYSYMBOL_4_ = 4,                         /* ')'  */
  YYSYMBOL_5_ = 5,                         /* '{'  */
  YYSYMBOL_6_ = 6,                         /* '}'  */
  YYSYMBOL_7_ = 7,                         /* '['  */
  YYSYMBOL_8_ = 8,                         /* ']'  */
  YYSYMBOL_9_ = 9,                         /* ','  */
  YYSYMBOL_10_ = 10,                       /* ';'  */
  YYSYMBOL_11_ = 11,                       /* ':'  */
  YYSYMBOL_UNKNOWN = 12,                   /* UNKNOWN  */
  YYSYMBOL_STRING = 13,                    /* STRING  */
  YYSYMBOL_SYMBOL = 14,                    /* SYMBOL  */
  YYSYMBOL_PNUM = 15,                      /* PNUM  */
  YYSYMBOL_DILSC_VAR = 16,                 /* DILSC_VAR  */
  YYSYMBOL_DILSC_BEG = 17,                 /* DILSC_BEG  */
  YYSYMBOL_DILSC_COD = 18,                 /* DILSC_COD  */
  YYSYMBOL_DILSC_END = 19,                 /* DILSC_END  */
  YYSYMBOL_DILSC_EXT = 20,                 /* DILSC_EXT  */
  YYSYMBOL_DILSC_REC = 21,                 /* DILSC_REC  */
  YYSYMBOL_DILSC_AWA = 22,                 /* DILSC_AWA  */
  YYSYMBOL_DILSE_ATOI = 23,                /* DILSE_ATOI  */
  YYSYMBOL_DILSE_RND = 24,                 /* DILSE_RND  */
  YYSYMBOL_DILSE_FNDU = 25,                /* DILSE_FNDU  */
  YYSYMBOL_DILSE_FNDRU = 26,               /* DILSE_FNDRU  */
  YYSYMBOL_DILSE_FNDR = 27,                /* DILSE_FNDR  */
  YYSYMBOL_DILSE_LOAD = 28,                /* DILSE_LOAD  */
  YYSYMBOL_DILSE_GETW = 29,                /* DILSE_GETW  */
  YYSYMBOL_DILSE_ITOA = 30,                /* DILSE_ITOA  */
  YYSYMBOL_DILSE_ISS = 31,                 /* DILSE_ISS  */
  YYSYMBOL_DILSE_IN = 32,                  /* DILSE_IN  */
  YYSYMBOL_DILSE_ISA = 33,                 /* DILSE_ISA  */
  YYSYMBOL_DILSE_CMDS = 34,                /* DILSE_CMDS  */
  YYSYMBOL_DILSE_FNDS = 35,                /* DILSE_FNDS  */
  YYSYMBOL_DILSE_GETWS = 36,               /* DILSE_GETWS  */
  YYSYMBOL_DILSE_LEN = 37,                 /* DILSE_LEN  */
  YYSYMBOL_DILSE_PURS = 38,                /* DILSE_PURS  */
  YYSYMBOL_DILSE_TRMO = 39,                /* DILSE_TRMO  */
  YYSYMBOL_DILSE_DLD = 40,                 /* DILSE_DLD  */
  YYSYMBOL_DILSE_DLF = 41,                 /* DILSE_DLF  */
  YYSYMBOL_DILSE_LAND = 42,                /* DILSE_LAND  */
  YYSYMBOL_DILSE_LOR = 43,                 /* DILSE_LOR  */
  YYSYMBOL_DILSE_VISI = 44,                /* DILSE_VISI  */
  YYSYMBOL_DILSE_OPPO = 45,                /* DILSE_OPPO  */
  YYSYMBOL_DILSE_RTI = 46,                 /* DILSE_RTI  */
  YYSYMBOL_DILSE_PCK = 47,                 /* DILSE_PCK  */
  YYSYMBOL_DILSE_AND = 48,                 /* DILSE_AND  */
  YYSYMBOL_DILSE_OR = 49,                  /* DILSE_OR  */
  YYSYMBOL_DILSE_NOT = 50,                 /* DILSE_NOT  */
  YYSYMBOL_DILTO_EQ = 51,                  /* DILTO_EQ  */
  YYSYMBOL_DILTO_NEQ = 52,                 /* DILTO_NEQ  */
  YYSYMBOL_DILTO_PEQ = 53,                 /* DILTO_PEQ  */
  YYSYMBOL_DILTO_SEQ = 54,                 /* DILTO_SEQ  */
  YYSYMBOL_DILTO_LEQ = 55,                 /* DILTO_LEQ  */
  YYSYMBOL_DILTO_GEQ = 56,                 /* DILTO_GEQ  */
  YYSYMBOL_DILTO_GT = 57,                  /* DILTO_GT  */
  YYSYMBOL_DILTO_LE = 58,                  /* DILTO_LE  */
  YYSYMBOL_DILTO_ASS = 59,                 /* DILTO_ASS  */
  YYSYMBOL_DILSE_NULL = 60,                /* DILSE_NULL  */
  YYSYMBOL_DILSE_SELF = 61,                /* DILSE_SELF  */
  YYSYMBOL_DILSE_ACTI = 62,                /* DILSE_ACTI  */
  YYSYMBOL_DILSE_ARGM = 63,                /* DILSE_ARGM  */
  YYSYMBOL_DILSE_HRT = 64,                 /* DILSE_HRT  */
  YYSYMBOL_DILSE_CMST = 65,                /* DILSE_CMST  */
  YYSYMBOL_DILSE_TDA = 66,                 /* DILSE_TDA  */
  YYSYMBOL_DILSE_THO = 67,                 /* DILSE_THO  */
  YYSYMBOL_DILSE_TMD = 68,                 /* DILSE_TMD  */
  YYSYMBOL_DILSE_TYE = 69,                 /* DILSE_TYE  */
  YYSYMBOL_DILSE_SKIP = 70,                /* DILSE_SKIP  */
  YYSYMBOL_DILSE_WEAT = 71,                /* DILSE_WEAT  */
  YYSYMBOL_DILSE_MEDI = 72,                /* DILSE_MEDI  */
  YYSYMBOL_DILSE_TARG = 73,                /* DILSE_TARG  */
  YYSYMBOL_DILSE_POWE = 74,                /* DILSE_POWE  */
  YYSYMBOL_DILSE_CST = 75,                 /* DILSE_CST  */
  YYSYMBOL_DILSE_MEL = 76,                 /* DILSE_MEL  */
  YYSYMBOL_DILSE_EQPM = 77,                /* DILSE_EQPM  */
  YYSYMBOL_DILSE_OPRO = 78,                /* DILSE_OPRO  */
  YYSYMBOL_DILSE_REST = 79,                /* DILSE_REST  */
  YYSYMBOL_DILSE_FIT = 80,                 /* DILSE_FIT  */
  YYSYMBOL_DILSE_CARY = 81,                /* DILSE_CARY  */
  YYSYMBOL_DILSE_PATH = 82,                /* DILSE_PATH  */
  YYSYMBOL_DILSE_MONS = 83,                /* DILSE_MONS  */
  YYSYMBOL_DILSE_SPLX = 84,                /* DILSE_SPLX  */
  YYSYMBOL_DILSE_SPLI = 85,                /* DILSE_SPLI  */
  YYSYMBOL_DILSE_TXF = 86,                 /* DILSE_TXF  */
  YYSYMBOL_DILSE_AST = 87,                 /* DILSE_AST  */
  YYSYMBOL_DILSI_LNK = 88,                 /* DILSI_LNK  */
  YYSYMBOL_DILSI_EXP = 89,                 /* DILSI_EXP  */
  YYSYMBOL_DILSI_SET = 90,                 /* DILSI_SET  */
  YYSYMBOL_DILSI_UST = 91,                 /* DILSI_UST  */
  YYSYMBOL_DILSI_ADE = 92,                 /* DILSI_ADE  */
  YYSYMBOL_DILSI_SUE = 93,                 /* DILSI_SUE  */
  YYSYMBOL_DILSI_DST = 94,                 /* DILSI_DST  */
  YYSYMBOL_DILSI_ADL = 95,                 /* DILSI_ADL  */
  YYSYMBOL_DILSI_SUL = 96,                 /* DILSI_SUL  */
  YYSYMBOL_DILSI_SND = 97,                 /* DILSI_SND  */
  YYSYMBOL_DILSI_SNT = 98,                 /* DILSI_SNT  */
  YYSYMBOL_DILSI_SEC = 99,                 /* DILSI_SEC  */
  YYSYMBOL_DILSI_USE = 100,                /* DILSI_USE  */
  YYSYMBOL_DILSI_ADA = 101,                /* DILSI_ADA  */
  YYSYMBOL_DILSI_SETF = 102,               /* DILSI_SETF  */
  YYSYMBOL_DILSI_CHAS = 103,               /* DILSI_CHAS  */
  YYSYMBOL_DILSI_SUA = 104,                /* DILSI_SUA  */
  YYSYMBOL_DILSI_EQP = 105,                /* DILSI_EQP  */
  YYSYMBOL_DILSI_UEQ = 106,                /* DILSI_UEQ  */
  YYSYMBOL_DILSI_OAC = 107,                /* DILSI_OAC  */
  YYSYMBOL_DILSI_STOR = 108,               /* DILSI_STOR  */
  YYSYMBOL_DILSI_SETE = 109,               /* DILSI_SETE  */
  YYSYMBOL_DILSI_QUIT = 110,               /* DILSI_QUIT  */
  YYSYMBOL_DILSI_LOG = 111,                /* DILSI_LOG  */
  YYSYMBOL_DILSI_SNTA = 112,               /* DILSI_SNTA  */
  YYSYMBOL_DILSI_SNTADIL = 113,            /* DILSI_SNTADIL  */
  YYSYMBOL_DILSI_DLC = 114,                /* DILSI_DLC  */
  YYSYMBOL_DILSE_INTR = 115,               /* DILSE_INTR  */
  YYSYMBOL_DILSI_CLI = 116,                /* DILSI_CLI  */
  YYSYMBOL_DILSI_SWT = 117,                /* DILSI_SWT  */
  YYSYMBOL_DILSI_SBT = 118,                /* DILSI_SBT  */
  YYSYMBOL_DILSE_ATSP = 119,               /* DILSE_ATSP  */
  YYSYMBOL_DILSI_FOLO = 120,               /* DILSI_FOLO  */
  YYSYMBOL_DILSI_LCRI = 121,               /* DILSI_LCRI  */
  YYSYMBOL_DILSI_WHI = 122,                /* DILSI_WHI  */
  YYSYMBOL_DILSI_IF = 123,                 /* DILSI_IF  */
  YYSYMBOL_DILSI_WLK = 124,                /* DILSI_WLK  */
  YYSYMBOL_DILSI_EXE = 125,                /* DILSI_EXE  */
  YYSYMBOL_DILSI_WIT = 126,                /* DILSI_WIT  */
  YYSYMBOL_DILSI_ACT = 127,                /* DILSI_ACT  */
  YYSYMBOL_DILSI_ELS = 128,                /* DILSI_ELS  */
  YYSYMBOL_DILSI_GOT = 129,                /* DILSI_GOT  */
  YYSYMBOL_DILSI_PRI = 130,                /* DILSI_PRI  */
  YYSYMBOL_DILSI_NPR = 131,                /* DILSI_NPR  */
  YYSYMBOL_DILSI_BLK = 132,                /* DILSI_BLK  */
  YYSYMBOL_DILSI_CNT = 133,                /* DILSI_CNT  */
  YYSYMBOL_DILSI_PUP = 134,                /* DILSI_PUP  */
  YYSYMBOL_DILSI_FOR = 135,                /* DILSI_FOR  */
  YYSYMBOL_DILSI_FOE = 136,                /* DILSI_FOE  */
  YYSYMBOL_DILSI_BRK = 137,                /* DILSI_BRK  */
  YYSYMBOL_DILSI_RTS = 138,                /* DILSI_RTS  */
  YYSYMBOL_DILSI_ON = 139,                 /* DILSI_ON  */
  YYSYMBOL_DILSI_AMOD = 140,               /* DILSI_AMOD  */
  YYSYMBOL_DILSF_ZOI = 141,                /* DILSF_ZOI  */
  YYSYMBOL_DILSF_NMI = 142,                /* DILSF_NMI  */
  YYSYMBOL_DILSF_TYP = 143,                /* DILSF_TYP  */
  YYSYMBOL_DILSF_NXT = 144,                /* DILSF_NXT  */
  YYSYMBOL_DILSF_NMS = 145,                /* DILSF_NMS  */
  YYSYMBOL_DILSF_NAM = 146,                /* DILSF_NAM  */
  YYSYMBOL_DILSF_IDX = 147,                /* DILSF_IDX  */
  YYSYMBOL_DILSF_TIT = 148,                /* DILSF_TIT  */
  YYSYMBOL_DILSF_EXT = 149,                /* DILSF_EXT  */
  YYSYMBOL_DILSF_OUT = 150,                /* DILSF_OUT  */
  YYSYMBOL_DILSF_INS = 151,                /* DILSF_INS  */
  YYSYMBOL_DILSF_GNX = 152,                /* DILSF_GNX  */
  YYSYMBOL_DILSF_SPD = 153,                /* DILSF_SPD  */
  YYSYMBOL_DILSF_GPR = 154,                /* DILSF_GPR  */
  YYSYMBOL_DILSF_LGT = 155,                /* DILSF_LGT  */
  YYSYMBOL_DILSF_BGT = 156,                /* DILSF_BGT  */
  YYSYMBOL_DILSF_MIV = 157,                /* DILSF_MIV  */
  YYSYMBOL_DILSF_ILL = 158,                /* DILSF_ILL  */
  YYSYMBOL_DILSF_FL = 159,                 /* DILSF_FL  */
  YYSYMBOL_DILSF_OFL = 160,                /* DILSF_OFL  */
  YYSYMBOL_DILSF_MHP = 161,                /* DILSF_MHP  */
  YYSYMBOL_DILSF_CHP = 162,                /* DILSF_CHP  */
  YYSYMBOL_DILSF_BWT = 163,                /* DILSF_BWT  */
  YYSYMBOL_DILSF_WGT = 164,                /* DILSF_WGT  */
  YYSYMBOL_DILSF_CAP = 165,                /* DILSF_CAP  */
  YYSYMBOL_DILSF_ALG = 166,                /* DILSF_ALG  */
  YYSYMBOL_DILSF_SPL = 167,                /* DILSF_SPL  */
  YYSYMBOL_DILSF_FUN = 168,                /* DILSF_FUN  */
  YYSYMBOL_DILSF_ZON = 169,                /* DILSF_ZON  */
  YYSYMBOL_DILSF_MAS = 170,                /* DILSF_MAS  */
  YYSYMBOL_DILSF_FOL = 171,                /* DILSF_FOL  */
  YYSYMBOL_DILSF_OTY = 172,                /* DILSF_OTY  */
  YYSYMBOL_DILSF_VAL = 173,                /* DILSF_VAL  */
  YYSYMBOL_DILSF_EFL = 174,                /* DILSF_EFL  */
  YYSYMBOL_DILSF_CST = 175,                /* DILSF_CST  */
  YYSYMBOL_DILSF_RNT = 176,                /* DILSF_RNT  */
  YYSYMBOL_DILSF_EQP = 177,                /* DILSF_EQP  */
  YYSYMBOL_DILSF_ONM = 178,                /* DILSF_ONM  */
  YYSYMBOL_DILSF_XNF = 179,                /* DILSF_XNF  */
  YYSYMBOL_DILSF_TOR = 180,                /* DILSF_TOR  */
  YYSYMBOL_DILSF_RFL = 181,                /* DILSF_RFL  */
  YYSYMBOL_DILSF_MOV = 182,                /* DILSF_MOV  */
  YYSYMBOL_DILSF_ACT = 183,                /* DILSF_ACT  */
  YYSYMBOL_DILSF_SEX = 184,                /* DILSF_SEX  */
  YYSYMBOL_DILSF_RCE = 185,                /* DILSF_RCE  */
  YYSYMBOL_DILSF_ABL = 186,                /* DILSF_ABL  */
  YYSYMBOL_DILSF_EXP = 187,                /* DILSF_EXP  */
  YYSYMBOL_DILSF_LVL = 188,                /* DILSF_LVL  */
  YYSYMBOL_DILSF_HGT = 189,                /* DILSF_HGT  */
  YYSYMBOL_DILSF_POS = 190,                /* DILSF_POS  */
  YYSYMBOL_DILSF_ATY = 191,                /* DILSF_ATY  */
  YYSYMBOL_DILSF_MNA = 192,                /* DILSF_MNA  */
  YYSYMBOL_DILSF_END = 193,                /* DILSF_END  */
  YYSYMBOL_DILSF_WPN = 194,                /* DILSF_WPN  */
  YYSYMBOL_DILSF_MAN = 195,                /* DILSF_MAN  */
  YYSYMBOL_DILSF_AFF = 196,                /* DILSF_AFF  */
  YYSYMBOL_DILSF_DRE = 197,                /* DILSF_DRE  */
  YYSYMBOL_DILSF_FGT = 198,                /* DILSF_FGT  */
  YYSYMBOL_DILSF_DEF = 199,                /* DILSF_DEF  */
  YYSYMBOL_DILSF_TIM = 200,                /* DILSF_TIM  */
  YYSYMBOL_DILSF_CRM = 201,                /* DILSF_CRM  */
  YYSYMBOL_DILSF_FLL = 202,                /* DILSF_FLL  */
  YYSYMBOL_DILSF_THR = 203,                /* DILSF_THR  */
  YYSYMBOL_DILSF_DRK = 204,                /* DILSF_DRK  */
  YYSYMBOL_DILSF_SPT = 205,                /* DILSF_SPT  */
  YYSYMBOL_DILSF_APT = 206,                /* DILSF_APT  */
  YYSYMBOL_DILSF_GLD = 207,                /* DILSF_GLD  */
  YYSYMBOL_DILSF_QST = 208,                /* DILSF_QST  */
  YYSYMBOL_DILSF_SKL = 209,                /* DILSF_SKL  */
  YYSYMBOL_DILSF_BIR = 210,                /* DILSF_BIR  */
  YYSYMBOL_DILSF_PTI = 211,                /* DILSF_PTI  */
  YYSYMBOL_DILSF_PCF = 212,                /* DILSF_PCF  */
  YYSYMBOL_DILSF_HOME = 213,               /* DILSF_HOME  */
  YYSYMBOL_DILSF_ODES = 214,               /* DILSF_ODES  */
  YYSYMBOL_DILSF_IDES = 215,               /* DILSF_IDES  */
  YYSYMBOL_DILSF_DES = 216,                /* DILSF_DES  */
  YYSYMBOL_DILSF_LCN = 217,                /* DILSF_LCN  */
  YYSYMBOL_DILSF_ABAL = 218,               /* DILSF_ABAL  */
  YYSYMBOL_DILSF_ATOT = 219,               /* DILSF_ATOT  */
  YYSYMBOL_DILSF_MMA = 220,                /* DILSF_MMA  */
  YYSYMBOL_DILSF_LSA = 221,                /* DILSF_LSA  */
  YYSYMBOL_DILSF_INFO = 222,               /* DILSF_INFO  */
  YYSYMBOL_DILSF_MED = 223,                /* DILSF_MED  */
  YYSYMBOL_DILST_UP = 224,                 /* DILST_UP  */
  YYSYMBOL_DILST_INT = 225,                /* DILST_INT  */
  YYSYMBOL_DILST_SP = 226,                 /* DILST_SP  */
  YYSYMBOL_DILST_SLP = 227,                /* DILST_SLP  */
  YYSYMBOL_DILST_EDP = 228,                /* DILST_EDP  */
  YYSYMBOL_229_ = 229,                     /* '+'  */
  YYSYMBOL_230_ = 230,                     /* '-'  */
  YYSYMBOL_231_ = 231,                     /* '*'  */
  YYSYMBOL_232_ = 232,                     /* '/'  */
  YYSYMBOL_233_ = 233,                     /* '%'  */
  YYSYMBOL_UMINUS = 234,                   /* UMINUS  */
  YYSYMBOL_UPLUS = 235,                    /* UPLUS  */
  YYSYMBOL_236_ = 236,                     /* '.'  */
  YYSYMBOL_YYACCEPT = 237,                 /* $accept  */
  YYSYMBOL_file = 238,                     /* file  */
  YYSYMBOL_program = 239,                  /* program  */
  YYSYMBOL_diloptions = 240,               /* diloptions  */
  YYSYMBOL_dilinit = 241,                  /* dilinit  */
  YYSYMBOL_dildecls = 242,                 /* dildecls  */
  YYSYMBOL_decls = 243,                    /* decls  */
  YYSYMBOL_dilrefs = 244,                  /* dilrefs  */
  YYSYMBOL_refs = 245,                     /* refs  */
  YYSYMBOL_ref = 246,                      /* ref  */
  YYSYMBOL_arginit = 247,                  /* arginit  */
  YYSYMBOL_refarglist = 248,               /* refarglist  */
  YYSYMBOL_somerefargs = 249,              /* somerefargs  */
  YYSYMBOL_refarg = 250,                   /* refarg  */
  YYSYMBOL_fundef = 251,                   /* fundef  */
  YYSYMBOL_type = 252,                     /* type  */
  YYSYMBOL_variable = 253,                 /* variable  */
  YYSYMBOL_idx = 254,                      /* idx  */
  YYSYMBOL_field = 255,                    /* field  */
  YYSYMBOL_scmp = 256,                     /* scmp  */
  YYSYMBOL_pcmp = 257,                     /* pcmp  */
  YYSYMBOL_cmp = 258,                      /* cmp  */
  YYSYMBOL_icmp = 259,                     /* icmp  */
  YYSYMBOL_dilexp = 260,                   /* dilexp  */
  YYSYMBOL_dilsexp = 261,                  /* dilsexp  */
  YYSYMBOL_dilterm = 262,                  /* dilterm  */
  YYSYMBOL_dilfactor = 263,                /* dilfactor  */
  YYSYMBOL_dilfun = 264,                   /* dilfun  */
  YYSYMBOL_stringlist = 265,               /* stringlist  */
  YYSYMBOL_strings = 266,                  /* strings  */
  YYSYMBOL_coreexp = 267,                  /* coreexp  */
  YYSYMBOL_corevar = 268,                  /* corevar  */
  YYSYMBOL_ihold = 269,                    /* ihold  */
  YYSYMBOL_iunhold = 270,                  /* iunhold  */
  YYSYMBOL_ahold = 271,                    /* ahold  */
  YYSYMBOL_label = 272,                    /* label  */
  YYSYMBOL_labelskip = 273,                /* labelskip  */
  YYSYMBOL_labellist = 274,                /* labellist  */
  YYSYMBOL_dilproc = 275,                  /* dilproc  */
  YYSYMBOL_pushbrk = 276,                  /* pushbrk  */
  YYSYMBOL_defbrk = 277,                   /* defbrk  */
  YYSYMBOL_popbrk = 278,                   /* popbrk  */
  YYSYMBOL_pushcnt = 279,                  /* pushcnt  */
  YYSYMBOL_defcnt = 280,                   /* defcnt  */
  YYSYMBOL_popcnt = 281,                   /* popcnt  */
  YYSYMBOL_semicolons = 282,               /* semicolons  */
  YYSYMBOL_optsemicolons = 283,            /* optsemicolons  */
  YYSYMBOL_block = 284,                    /* block  */
  YYSYMBOL_dilinstlist = 285,              /* dilinstlist  */
  YYSYMBOL_dilcomposed = 286,              /* dilcomposed  */
  YYSYMBOL_dilcomplex = 287,               /* dilcomplex  */
  YYSYMBOL_288_1 = 288,                    /* $@1  */
  YYSYMBOL_289_2 = 289,                    /* $@2  */
  YYSYMBOL_arglist = 290,                  /* arglist  */
  YYSYMBOL_someargs = 291,                 /* someargs  */
  YYSYMBOL_dilass = 292,                   /* dilass  */
  YYSYMBOL_proccall = 293,                 /* proccall  */
  YYSYMBOL_corefunc = 294,                 /* corefunc  */
  YYSYMBOL_dilassrgt = 295,                /* dilassrgt  */
  YYSYMBOL_dilinst = 296                   /* dilinst  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 168 "dilpar.ypp"

void add_ubit8(struct exptype *dest, ubit8 d);
void add_ubit32(struct exptype *dest, ubit32 d);
void add_sbit32(struct exptype *dest, sbit32 d);
void add_ubit16(struct exptype *dest, ubit16 d);
void add_string(struct exptype *dest, char *d);
void cat_string(struct exptype *dest, char *d);
void add_stringlist(struct exptype *dest, char **d);
void add_code(struct exptype *dest, struct exptype *src);
void copy_code(struct exptype *dest, struct exptype *src);
void make_code(struct exptype *dest);

#line 531 "dilpar.tab.cpp"


#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2001

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  237
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  302
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  902

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   476


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   233,     2,     2,
       3,     4,   231,   229,     9,   230,   236,   232,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    11,    10,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     7,     2,     8,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     5,     2,     6,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   234,   235
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   257,   257,   336,   357,   362,   368,   373,   444,   445,
     452,   456,   459,   460,   464,   467,   472,   497,   524,   529,
     530,   533,   534,   537,   546,   605,   615,   619,   623,   627,
     631,   641,   688,   744,   751,   758,   765,   772,   779,   786,
     793,   800,   807,   814,   821,   828,   835,   854,   867,   882,
     890,   898,   906,   914,   922,   930,   938,   946,   954,   962,
     970,   978,   986,   994,  1002,  1010,  1018,  1026,  1034,  1042,
    1050,  1058,  1066,  1074,  1082,  1090,  1098,  1106,  1114,  1122,
    1130,  1138,  1146,  1154,  1162,  1170,  1178,  1186,  1194,  1202,
    1211,  1219,  1232,  1240,  1250,  1258,  1266,  1274,  1287,  1297,
    1307,  1317,  1325,  1338,  1346,  1354,  1362,  1370,  1378,  1388,
    1396,  1404,  1412,  1420,  1428,  1436,  1444,  1452,  1460,  1468,
    1483,  1491,  1504,  1512,  1520,  1528,  1536,  1544,  1552,  1560,
    1568,  1576,  1592,  1598,  1604,  1608,  1614,  1618,  1622,  1626,
    1636,  1644,  1704,  1727,  1760,  1785,  1793,  1812,  1824,  1874,
    1901,  1925,  1951,  1959,  1987,  2014,  2037,  2060,  2087,  2128,
    2135,  2142,  2149,  2161,  2174,  2181,  2190,  2211,  2227,  2248,
    2269,  2285,  2301,  2349,  2370,  2393,  2415,  2448,  2476,  2492,
    2508,  2534,  2556,  2578,  2600,  2616,  2673,  2694,  2715,  2741,
    2763,  2790,  2806,  2849,  2892,  2923,  2949,  2975,  3001,  3039,
    3054,  3069,  3084,  3099,  3126,  3137,  3141,  3149,  3175,  3210,
    3217,  3223,  3229,  3239,  3243,  3253,  3259,  3267,  3303,  3314,
    3325,  3338,  3351,  3364,  3378,  3392,  3406,  3420,  3434,  3448,
    3462,  3477,  3492,  3510,  3523,  3538,  3551,  3565,  3599,  3610,
    3621,  3632,  3645,  3659,  3689,  3702,  3714,  3725,  3738,  3751,
    3764,  3777,  3788,  3795,  3802,  3809,  3816,  3827,  3841,  3859,
    3882,  3893,  3906,  3929,  3940,  3952,  3953,  3956,  3957,  3960,
    3964,  3966,  3970,  3972,  3974,  3978,  3991,  4004,  4015,  4020,
    4013,  4043,  4065,  4087,  4088,  4089,  4093,  4102,  4213,  4301,
    4318,  4337,  4341,  4345,  4348,  4350,  4352,  4354,  4371,  4384,
    4397,  4404,  4421
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "'('", "')'", "'{'",
  "'}'", "'['", "']'", "','", "';'", "':'", "UNKNOWN", "STRING", "SYMBOL",
  "PNUM", "DILSC_VAR", "DILSC_BEG", "DILSC_COD", "DILSC_END", "DILSC_EXT",
  "DILSC_REC", "DILSC_AWA", "DILSE_ATOI", "DILSE_RND", "DILSE_FNDU",
  "DILSE_FNDRU", "DILSE_FNDR", "DILSE_LOAD", "DILSE_GETW", "DILSE_ITOA",
  "DILSE_ISS", "DILSE_IN", "DILSE_ISA", "DILSE_CMDS", "DILSE_FNDS",
  "DILSE_GETWS", "DILSE_LEN", "DILSE_PURS", "DILSE_TRMO", "DILSE_DLD",
  "DILSE_DLF", "DILSE_LAND", "DILSE_LOR", "DILSE_VISI", "DILSE_OPPO",
  "DILSE_RTI", "DILSE_PCK", "DILSE_AND", "DILSE_OR", "DILSE_NOT",
  "DILTO_EQ", "DILTO_NEQ", "DILTO_PEQ", "DILTO_SEQ", "DILTO_LEQ",
  "DILTO_GEQ", "DILTO_GT", "DILTO_LE", "DILTO_ASS", "DILSE_NULL",
  "DILSE_SELF", "DILSE_ACTI", "DILSE_ARGM", "DILSE_HRT", "DILSE_CMST",
  "DILSE_TDA", "DILSE_THO", "DILSE_TMD", "DILSE_TYE", "DILSE_SKIP",
  "DILSE_WEAT", "DILSE_MEDI", "DILSE_TARG", "DILSE_POWE", "DILSE_CST",
  "DILSE_MEL", "DILSE_EQPM", "DILSE_OPRO", "DILSE_REST", "DILSE_FIT",
  "DILSE_CARY", "DILSE_PATH", "DILSE_MONS", "DILSE_SPLX", "DILSE_SPLI",
  "DILSE_TXF", "DILSE_AST", "DILSI_LNK", "DILSI_EXP", "DILSI_SET",
  "DILSI_UST", "DILSI_ADE", "DILSI_SUE", "DILSI_DST", "DILSI_ADL",
  "DILSI_SUL", "DILSI_SND", "DILSI_SNT", "DILSI_SEC", "DILSI_USE",
  "DILSI_ADA", "DILSI_SETF", "DILSI_CHAS", "DILSI_SUA", "DILSI_EQP",
  "DILSI_UEQ", "DILSI_OAC", "DILSI_STOR", "DILSI_SETE", "DILSI_QUIT",
  "DILSI_LOG", "DILSI_SNTA", "DILSI_SNTADIL", "DILSI_DLC", "DILSE_INTR",
  "DILSI_CLI", "DILSI_SWT", "DILSI_SBT", "DILSE_ATSP", "DILSI_FOLO",
  "DILSI_LCRI", "DILSI_WHI", "DILSI_IF", "DILSI_WLK", "DILSI_EXE",
  "DILSI_WIT", "DILSI_ACT", "DILSI_ELS", "DILSI_GOT", "DILSI_PRI",
  "DILSI_NPR", "DILSI_BLK", "DILSI_CNT", "DILSI_PUP", "DILSI_FOR",
  "DILSI_FOE", "DILSI_BRK", "DILSI_RTS", "DILSI_ON", "DILSI_AMOD",
  "DILSF_ZOI", "DILSF_NMI", "DILSF_TYP", "DILSF_NXT", "DILSF_NMS",
  "DILSF_NAM", "DILSF_IDX", "DILSF_TIT", "DILSF_EXT", "DILSF_OUT",
  "DILSF_INS", "DILSF_GNX", "DILSF_SPD", "DILSF_GPR", "DILSF_LGT",
  "DILSF_BGT", "DILSF_MIV", "DILSF_ILL", "DILSF_FL", "DILSF_OFL",
  "DILSF_MHP", "DILSF_CHP", "DILSF_BWT", "DILSF_WGT", "DILSF_CAP",
  "DILSF_ALG", "DILSF_SPL", "DILSF_FUN", "DILSF_ZON", "DILSF_MAS",
  "DILSF_FOL", "DILSF_OTY", "DILSF_VAL", "DILSF_EFL", "DILSF_CST",
  "DILSF_RNT", "DILSF_EQP", "DILSF_ONM", "DILSF_XNF", "DILSF_TOR",
  "DILSF_RFL", "DILSF_MOV", "DILSF_ACT", "DILSF_SEX", "DILSF_RCE",
  "DILSF_ABL", "DILSF_EXP", "DILSF_LVL", "DILSF_HGT", "DILSF_POS",
  "DILSF_ATY", "DILSF_MNA", "DILSF_END", "DILSF_WPN", "DILSF_MAN",
  "DILSF_AFF", "DILSF_DRE", "DILSF_FGT", "DILSF_DEF", "DILSF_TIM",
  "DILSF_CRM", "DILSF_FLL", "DILSF_THR", "DILSF_DRK", "DILSF_SPT",
  "DILSF_APT", "DILSF_GLD", "DILSF_QST", "DILSF_SKL", "DILSF_BIR",
  "DILSF_PTI", "DILSF_PCF", "DILSF_HOME", "DILSF_ODES", "DILSF_IDES",
  "DILSF_DES", "DILSF_LCN", "DILSF_ABAL", "DILSF_ATOT", "DILSF_MMA",
  "DILSF_LSA", "DILSF_INFO", "DILSF_MED", "DILST_UP", "DILST_INT",
  "DILST_SP", "DILST_SLP", "DILST_EDP", "'+'", "'-'", "'*'", "'/'", "'%'",
  "UMINUS", "UPLUS", "'.'", "$accept", "file", "program", "diloptions",
  "dilinit", "dildecls", "decls", "dilrefs", "refs", "ref", "arginit",
  "refarglist", "somerefargs", "refarg", "fundef", "type", "variable",
  "idx", "field", "scmp", "pcmp", "cmp", "icmp", "dilexp", "dilsexp",
  "dilterm", "dilfactor", "dilfun", "stringlist", "strings", "coreexp",
  "corevar", "ihold", "iunhold", "ahold", "label", "labelskip",
  "labellist", "dilproc", "pushbrk", "defbrk", "popbrk", "pushcnt",
  "defcnt", "popcnt", "semicolons", "optsemicolons", "block",
  "dilinstlist", "dilcomposed", "dilcomplex", "$@1", "$@2", "arglist",
  "someargs", "dilass", "proccall", "corefunc", "dilassrgt", "dilinst", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,    40,    41,   123,   125,    91,    93,    44,
      59,    58,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
     366,   367,   368,   369,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,   464,   465,
     466,   467,   468,   469,   470,   471,   472,   473,   474,    43,
      45,    42,    47,    37,   475,   476,    46
};
#endif

#define YYPACT_NINF (-622)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-284)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      47,  -622,    68,  -622,    75,  -622,    75,    75,    60,  -622,
    -622,    77,  -622,  -622,  -622,  -622,  -622,  -622,    67,   105,
    -622,    60,   116,   145,   146,  -622,    60,   147,   138,  -622,
     151,   170,  -622,   175,  -622,   182,  -622,   189,   146,   -74,
     188,   146,   -74,  1229,   177,   196,  -622,  -622,  -622,   191,
    1229,   198,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  1927,  -622,   230,  1176,  -622,  -622,  -622,
     229,  -622,   235,   147,   234,   237,   243,   247,   251,   252,
     253,   255,   259,   260,   264,   265,   267,   277,   279,   286,
     297,   307,   308,   313,   314,   316,  -622,   320,   353,  -622,
     354,   356,   357,   358,   359,   361,   362,   364,   365,   374,
    -622,   375,   376,   377,   378,   379,   369,  -622,  -622,  -622,
    -622,   381,   385,  -622,   390,   867,   391,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,   160,   338,   384,  -622,  -622,  -622,   229,  -622,  -622,
    -622,   229,  1229,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,  -622,   867,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,  -622,   867,   867,   867,
     867,   867,  -622,  -622,  -622,   867,   867,  -622,   867,   867,
     386,  -622,  -622,   415,   417,   418,   426,   428,   439,   440,
     445,   446,   449,   456,   457,   461,   462,   468,   469,   470,
     473,   474,   480,   481,  1497,  -622,   482,   483,   484,   487,
     488,   493,   496,   497,   498,   499,   500,   504,   505,   510,
     511,  1497,  1497,   160,    34,    23,    33,  -622,  -622,  -622,
    -622,   867,   957,   142,  -622,  -622,  -622,  -622,  -622,  -622,
     393,   506,   516,   517,   518,   519,   522,   515,   529,   530,
     533,   531,   554,   583,   579,   582,   587,   588,   589,   591,
     596,   598,   594,   600,   601,   602,   603,   604,   611,   607,
     608,   610,   612,  -622,   616,   619,   615,   617,   631,   621,
     636,   624,   318,  -622,    89,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,  -622,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,    33,    33,  -622,  -622,  -622,  -622,  -622,  -622,
     867,   867,  1497,  1497,  -622,  -622,  1497,  1497,   867,   867,
    1497,  1497,  1497,  1497,  1497,  1497,  -622,   637,   867,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,   513,  -622,  -622,  -622,  -622,
    -622,   513,  -622,  -622,  -622,  -622,   513,   513,   513,  -622,
    -622,  -622,  -622,  -622,   513,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,   513,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,   513,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,    40,  -622,  -622,  -622,  -622,  -622,
     629,  -622,   867,   867,   867,   867,   867,   867,   867,  -622,
     867,   867,  -622,   867,   369,  -622,   867,   867,   867,   867,
     867,  -622,   867,  -622,   867,  -622,   867,   867,   867,   867,
    -622,   867,   867,   867,   867,   644,  -622,  -622,   867,   867,
     867,  -622,  -622,  -622,  -622,  -622,   593,   334,   349,   864,
    1375,   399,   411,   423,   437,  1384,  1494,   454,   186,   466,
     478,  1539,  1547,  1576,  1584,  1592,  1600,  1608,  1616,  1628,
    1636,  1644,  1652,  1660,  1668,  1680,  1688,   528,  1696,   578,
     586,  1704,  1712,    30,    30,    33,    33,    33,    33,    30,
      30,  -622,  -622,  -622,  -622,  -622,  -622,   463,   867,   269,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,   642,
     652,   653,   655,   656,   668,   657,   658,   659,   679,   684,
     680,   690,   691,   692,   693,   694,   695,   698,   700,   703,
     704,   710,   711,   712,   708,   867,  1359,   715,   718,   716,
    -622,  -622,  -622,   867,   867,   867,  -622,  -622,  -622,  -622,
     867,   867,  -622,  -622,   867,  -622,  -622,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,  -622,   867,  -622,  -622,   867,   867,    70,
     730,  -622,   268,   867,  -622,  -622,  -622,  -622,   867,  -622,
    -622,  -622,  -622,  -622,   867,  -622,  -622,  -622,  -622,    70,
    -622,  -622,  -622,  -622,    70,  -622,  -622,  -622,   867,   731,
    1229,  -622,  -622,   229,  -622,  -622,   867,  -622,   597,  1720,
    1732,   614,   623,  1740,   635,  1748,   654,   672,   681,   717,
     727,  1756,  1764,   737,   757,   766,  1772,  1784,   776,   951,
    1792,  1800,  1808,  -622,  -622,   734,  -622,  -622,  -622,   736,
     735,   738,   741,   740,   746,   747,   748,  -622,   751,  -622,
     479,  -622,   744,  -622,  -622,   867,   867,  -622,  -622,   867,
    -622,   867,  -622,  -622,  -622,  -622,  -622,   867,   867,  -622,
    -622,  -622,   867,   867,  -622,  -622,   867,   867,   867,    70,
    -622,   268,   867,  -622,   867,  -622,  -622,  -622,  1229,    57,
     626,  -622,   867,  1927,  1816,   961,   983,   991,  1824,  1836,
    1006,  1033,  1844,  1852,  1860,  -622,  -622,   752,   749,  -622,
    1229,   632,   754,  -622,   867,  -622,  -622,  -622,   867,   867,
    -622,  -622,   867,   867,   867,  -622,   867,  -622,  -622,  1229,
     867,   755,  1180,  1868,  1190,  1876,  1888,  1896,   762,  -622,
    -622,   777,  -622,  -622,   867,  -622,   867,   867,   867,   867,
    -622,   867,  1229,  1198,  1904,  1344,  1912,   778,  -622,   760,
    -622,  -622,   867,  -622,   867,   867,  -622,  -622,  -622,  1920,
    1353,   781,  -622,   867,  -622,   867,  -622,  1928,   789,  -622,
     867,   867,  -622,  1367,   790,  -622,  -622,   867,   791,   867,
     797,  -622
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     7,     0,     2,     6,     1,     6,     6,    25,     4,
       5,     0,    26,    27,    28,    29,    30,    24,    13,     0,
      18,     0,     8,     0,    19,    12,    15,    11,     0,    18,
       0,     0,    20,    22,    14,     0,     9,     0,    19,     0,
       0,     0,     0,   209,     0,     0,    23,    16,    21,     0,
     209,     0,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   259,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   294,     0,   209,   274,   295,   296,
       0,     3,     0,    11,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   209,     0,     0,   252,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     262,     0,     0,     0,     0,     0,     0,   254,   255,   253,
     211,     0,     0,   211,   297,   209,     0,    46,    33,    34,
      39,    40,    38,    43,    42,    44,    45,    41,    35,    36,
      37,   208,   209,     0,   217,   269,   271,   265,   273,    17,
      10,   267,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   211,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   263,   209,   209,   209,
     209,   209,   212,   300,   302,   209,   209,   301,   209,   209,
       0,   162,   161,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   209,   164,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   209,   209,   165,   207,   140,   145,   152,   160,   163,
     209,   209,     0,   209,   209,   210,   266,   268,   272,   282,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   205,     0,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   166,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   147,   146,   134,   135,   139,   138,   136,   137,
     209,   209,   209,   209,   133,   132,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,     0,   209,    56,
      57,    58,    59,    60,    61,    55,    65,    66,    67,    68,
      69,   105,    71,    72,    73,    74,    75,    76,    78,    79,
      84,    85,    86,    87,    88,     0,    90,    91,    53,    54,
      92,     0,    94,    95,    96,    97,     0,     0,     0,   101,
     102,   121,   106,   107,     0,   109,   110,   111,   112,   113,
     114,   115,     0,    77,   116,   117,   118,   120,   122,   123,
     124,   125,   126,   127,   128,   129,   130,     0,    49,    50,
      51,    52,    63,    64,    62,    70,   103,   104,    83,    81,
      80,    82,    48,    31,   165,   291,   209,   292,   293,   287,
       0,    32,   209,   209,   209,   209,   209,   209,   209,   238,
     209,   209,   246,   209,     0,   251,   209,   209,   209,   209,
     209,   258,   209,   219,   209,   239,   209,   209,   209,   209,
     218,   209,   209,   209,   209,     0,   211,   256,   209,   209,
     209,   240,   259,   298,   159,   204,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   142,   150,   151,   148,   149,   144,
     143,   158,   155,   156,   153,   154,   157,     0,   209,     0,
      89,    93,    98,    99,   100,   108,   119,   131,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,   209,     0,     0,     0,
     262,   206,   167,   209,   209,   209,   178,   191,   201,   171,
     209,   209,   203,   179,   209,   202,   170,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   184,   209,   199,   200,   209,   209,     0,
       0,    47,   209,   209,   241,   242,   226,   229,   209,   234,
     233,   235,   247,   250,   209,   222,   223,   236,   257,     0,
     228,   248,   249,   227,     0,   225,   224,   221,   209,     0,
     209,   209,   277,   267,   244,   245,   209,   263,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   214,   213,   216,   299,   220,   286,     0,
     285,     0,     0,     0,     0,     0,     0,   211,     0,   211,
     273,   209,     0,   209,   173,   209,   209,   174,   175,   209,
     189,   209,   168,   169,   181,   183,   182,   209,   209,   197,
     196,   195,   209,   209,   187,   186,   209,   209,   209,     0,
     288,   209,   209,   230,   209,   290,   289,   231,   209,   267,
       0,   211,   209,   209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   215,   284,     0,     0,   209,
     209,     0,     0,   211,   209,   177,   180,   190,   209,   209,
     194,   188,   209,   209,   209,   232,   209,   211,   275,   209,
     209,     0,     0,     0,     0,     0,     0,     0,     0,   260,
     276,     0,   278,   176,   209,   198,   209,   209,   209,   209,
     261,   209,   209,     0,     0,     0,     0,     0,   264,     0,
     279,   193,   209,   192,   209,   209,   281,   243,   209,     0,
       0,     0,   211,   209,   172,   209,   260,     0,     0,   261,
     209,   209,   264,     0,     0,   280,   185,   209,     0,   209,
       0,   237
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -622,  -622,  -622,   152,  -622,  -622,   689,  -622,   780,   758,
     -27,   772,   763,  -622,  -622,   104,  -102,   750,  -622,  -622,
    -622,  -622,  -622,   197,  -329,  -118,  -170,  -622,  -622,  -622,
     -94,  -287,   -43,  -622,   -75,  -148,  -621,    22,  -622,   274,
     -66,   -64,   190,   109,   -63,  -105,   123,   201,   -46,  -189,
    -622,  -622,  -622,    48,  -622,  -622,   545,   550,  -622,   208
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,     3,     8,     4,    28,    36,    22,    25,    26,
      24,    31,    32,    33,    18,    19,   283,   492,   493,   398,
     399,   390,   391,   284,   285,   286,   287,   288,   289,   344,
     748,   182,   183,   501,   234,   744,   745,   746,   104,   150,
     860,   868,   226,   333,   876,   297,   298,    44,   105,   106,
     107,   862,   878,   749,   750,   108,   184,   109,   499,   110
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     103,   181,    38,   299,   114,   188,   496,   103,   233,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,  -208,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     186,   583,   584,   103,     1,   400,   392,   187,     5,   589,
     590,   290,   393,   392,    11,   401,   394,   395,   754,   393,
      20,   402,   296,   755,   232,   384,   385,    21,   237,   386,
     387,   388,   389,   213,   366,   545,     6,     7,   546,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,    23,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,    27,   334,   335,   336,   337,   338,   320,   294,
     743,   339,   340,    46,   341,   239,    49,   240,    29,   103,
      12,    13,    14,    15,    16,   241,    37,   242,     9,    10,
      30,    35,    39,   382,   383,   243,   244,   245,   246,   247,
     248,   249,   250,   251,    40,   252,   253,   254,   255,   256,
     257,   258,   259,   260,    41,  -269,   261,   262,   167,   263,
     653,   494,   264,    42,    43,   654,   111,   407,    47,   495,
     112,   113,   265,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   115,   177,   178,   179,   180,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     591,   592,   593,   594,   595,   596,   185,   384,   385,   187,
     191,   386,   387,   388,   389,   189,   193,   406,   192,    72,
     194,   500,   396,   397,   195,   196,   197,    80,   198,   396,
     397,   279,   199,   200,   403,   404,   405,   201,   202,   280,
     203,   239,  -283,   240,   585,   586,   292,   681,   587,   588,
     204,   241,   205,   242,    12,    13,    14,    15,    16,   206,
     535,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     207,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     208,   209,   261,   262,   167,   263,   210,   211,   264,   212,
     384,   385,   544,   214,   386,   387,   388,   389,   265,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   642,   177,
     178,   179,   180,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   215,   216,   643,   217,
     218,   219,   220,   597,   221,   222,   619,   223,   224,   384,
     385,   281,   282,   386,   387,   388,   389,   225,   227,   228,
     229,   230,   231,   232,   235,   384,   385,   279,   236,   386,
     387,   388,   389,   238,   291,   280,   292,   293,   295,   343,
     384,   385,   502,   646,   386,   387,   388,   389,   609,   610,
     611,   612,   613,   614,   615,   647,   616,   617,   345,   618,
     346,   347,   620,   621,   622,   623,   624,   648,   625,   348,
     626,   349,   627,   628,   629,   630,   342,   631,   632,   633,
     634,   649,   350,   351,   637,   638,   639,   712,   352,   353,
     384,   385,   354,   294,   386,   387,   388,   389,   652,   355,
     356,   636,   384,   385,   357,   358,   386,   387,   388,   389,
     655,   359,   360,   361,   384,   385,   362,   363,   386,   387,
     388,   389,   656,   364,   365,   367,   368,   369,   384,   385,
     370,   371,   386,   387,   388,   389,   372,   281,   282,   373,
     374,   375,   376,   377,   680,   384,   385,   378,   379,   386,
     387,   388,   389,   380,   381,   503,   823,   384,   385,   509,
     408,   386,   387,   388,   389,   504,   505,   506,   507,   384,
     385,   508,   673,   386,   387,   388,   389,   512,   510,   511,
     513,   709,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   514,   568,   569,   570,   571,   572,   573,
     574,   575,   576,   577,   578,   579,   580,   581,   582,   384,
     385,   682,   675,   386,   387,   388,   389,   515,   516,   751,
     676,   517,   679,   103,   752,   521,   518,   519,   520,   522,
     753,   764,   523,   524,   525,   599,   641,  -268,   760,   819,
     526,   527,   528,   529,   756,   530,   531,   532,   767,   533,
     536,   534,   762,   537,   538,   541,   539,   768,   543,   384,
     385,   838,   608,   386,   387,   388,   389,   384,   385,   770,
     540,   386,   387,   388,   389,   542,   598,   635,   384,   385,
     850,   683,   386,   387,   388,   389,   684,   685,   772,   686,
     687,   689,   690,   691,   758,   384,   385,   103,   759,   386,
     387,   388,   389,   870,   384,   385,   773,   688,   386,   387,
     388,   389,   798,   692,   800,   774,   384,   385,   693,   694,
     386,   387,   388,   389,   695,   696,   697,   698,   817,   700,
     818,   181,   701,   699,   702,   384,   385,   703,   822,   386,
     387,   388,   389,   704,   705,   706,   707,   708,   801,   714,
     803,   775,   715,   384,   385,   716,   821,   386,   387,   388,
     389,   776,   384,   385,   747,   757,   386,   387,   388,   389,
     790,   779,   848,   789,   791,   793,   851,   792,   841,   794,
     795,   796,   797,   802,   820,   103,   835,   799,   836,   852,
     839,   780,   849,   840,   877,   867,    17,   869,   384,   385,
     781,   859,   386,   387,   388,   389,   837,   103,   384,   385,
     784,   881,   386,   387,   388,   389,   861,   875,   384,   385,
     885,   888,   386,   387,   388,   389,   103,   894,   891,   897,
     899,   901,   190,   898,    48,   900,    34,   886,   384,   385,
      45,   815,   386,   387,   388,   389,   640,   384,   385,   103,
     889,   386,   387,   388,   389,   892,   763,   384,   385,   895,
     717,   386,   387,   388,   389,   882,   761,   711,   497,   816,
     718,   719,   720,   498,   713,     0,     0,   721,   722,     0,
       0,   723,     0,     0,   724,   725,   726,   727,   728,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   739,
     239,   740,   240,   644,   741,   742,     0,     0,     0,     0,
     241,     0,   242,     0,     0,     0,     0,     0,     0,     0,
     243,   244,   245,   246,   247,   248,   249,   250,   251,     0,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
       0,   261,   262,   167,   263,   384,   385,   264,     0,   386,
     387,   388,   389,     0,     0,     0,     0,   265,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   785,     0,     0,     0,     0,
       0,     0,   804,   805,   408,   825,   806,     0,   807,     0,
       0,     0,     0,     0,   808,   809,     0,     0,     0,   810,
     811,     0,     0,   812,   813,   814,   279,   826,     0,     0,
       0,     0,     0,     0,   280,   827,     0,     0,     0,     0,
       0,     0,   384,   385,     0,     0,   386,   387,   388,   389,
     830,     0,   384,   385,     0,     0,   386,   387,   388,   389,
       0,   842,     0,     0,     0,   843,   844,     0,     0,   845,
     846,   847,     0,     0,   384,   385,     0,   831,   386,   387,
     388,   389,   384,   385,     0,     0,   386,   387,   388,   389,
       0,   863,     0,   864,   865,   866,     0,   384,   385,     0,
       0,   386,   387,   388,   389,     0,     0,     0,     0,   879,
       0,   880,     0,     0,     0,     0,     0,     0,     0,     0,
     887,     0,     0,     0,   384,   385,     0,   893,   386,   387,
     388,   389,     0,     0,     0,     0,   281,   282,   409,   410,
     411,   412,   413,   414,   415,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   426,   427,   428,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   466,   467,   468,   469,   470,
     471,   472,   473,   474,   475,   476,   477,   478,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   490,
     491,    50,  -270,     0,   853,   600,     0,    51,     0,     0,
       0,   601,     0,     0,   855,     0,   602,   603,   604,     0,
       0,     0,   871,     0,   605,     0,     0,     0,     0,     0,
       0,     0,   606,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   607,     0,     0,
       0,   384,   385,     0,    50,   386,   387,   388,   389,     0,
      51,   384,   385,     0,     0,   386,   387,   388,   389,   384,
     385,    52,     0,   386,   387,   388,   389,     0,     0,     0,
       0,     0,     0,     0,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,     0,    84,    85,    86,    87,
      88,    89,    90,    91,    52,    92,    93,    94,    95,    96,
      97,     0,    98,    99,   100,   101,   102,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,   873,    84,
      85,    86,    87,    88,    89,    90,    91,   884,    92,    93,
      94,    95,    96,    97,   710,    98,    99,   100,   101,   102,
      51,   896,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   645,     0,     0,     0,     0,     0,
       0,     0,     0,   650,     0,   384,   385,     0,     0,   386,
     387,   388,   389,     0,   384,   385,     0,     0,   386,   387,
     388,   389,     0,     0,     0,     0,     0,     0,   384,   385,
       0,     0,   386,   387,   388,   389,   384,   385,     0,     0,
     386,   387,   388,   389,    52,   384,   385,     0,     0,   386,
     387,   388,   389,     0,     0,     0,     0,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,     0,    84,
      85,    86,    87,    88,    89,    90,    91,     0,    92,    93,
      94,    95,    96,    97,     0,    98,    99,   100,   101,   102,
     239,     0,   240,   651,     0,     0,     0,     0,     0,     0,
     241,     0,   242,     0,     0,     0,     0,     0,     0,     0,
     243,   244,   245,   246,   247,   248,   249,   250,   251,     0,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
       0,   261,   262,   167,   263,   384,   385,   264,   657,   386,
     387,   388,   389,     0,     0,     0,   658,   265,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   659,     0,     0,     0,     0,
     384,   385,     0,   660,   386,   387,   388,   389,   384,   385,
       0,   661,   386,   387,   388,   389,     0,     0,     0,   662,
       0,     0,     0,     0,     0,     0,   279,   663,     0,     0,
       0,     0,     0,     0,   280,   664,     0,   384,   385,     0,
       0,   386,   387,   388,   389,   384,   385,   665,     0,   386,
     387,   388,   389,   384,   385,   666,     0,   386,   387,   388,
     389,   384,   385,   667,     0,   386,   387,   388,   389,   384,
     385,   668,     0,   386,   387,   388,   389,   384,   385,   669,
       0,   386,   387,   388,   389,     0,     0,   670,     0,   384,
     385,     0,     0,   386,   387,   388,   389,   384,   385,   671,
       0,   386,   387,   388,   389,   384,   385,   672,     0,   386,
     387,   388,   389,   384,   385,   674,     0,   386,   387,   388,
     389,   384,   385,   677,     0,   386,   387,   388,   389,   384,
     385,   678,     0,   386,   387,   388,   389,     0,     0,   765,
       0,   384,   385,     0,     0,   386,   387,   388,   389,   384,
     385,   766,     0,   386,   387,   388,   389,   384,   385,   769,
       0,   386,   387,   388,   389,   384,   385,   771,     0,   386,
     387,   388,   389,   384,   385,   777,     0,   386,   387,   388,
     389,   384,   385,   778,     0,   386,   387,   388,   389,     0,
       0,   782,     0,   384,   385,     0,     0,   386,   387,   388,
     389,   384,   385,   783,     0,   386,   387,   388,   389,   384,
     385,   786,     0,   386,   387,   388,   389,   384,   385,   787,
       0,   386,   387,   388,   389,   384,   385,   788,     0,   386,
     387,   388,   389,   384,   385,   824,     0,   386,   387,   388,
     389,     0,     0,   828,     0,   384,   385,     0,     0,   386,
     387,   388,   389,   384,   385,   829,     0,   386,   387,   388,
     389,   384,   385,   832,     0,   386,   387,   388,   389,   384,
     385,   833,     0,   386,   387,   388,   389,   384,   385,   834,
       0,   386,   387,   388,   389,   384,   385,   854,     0,   386,
     387,   388,   389,     0,     0,   856,     0,   384,   385,     0,
       0,   386,   387,   388,   389,   384,   385,   857,     0,   386,
     387,   388,   389,   384,   385,   858,     0,   386,   387,   388,
     389,   384,   385,   872,     0,   386,   387,   388,   389,   384,
     385,   874,     0,   386,   387,   388,   389,   384,   385,   883,
       0,   386,   387,   388,   389,     0,     0,   890,     0,   384,
     385,     0,     0,   386,   387,   388,   389,   384,   385,     0,
       0,   386,   387,   388,   389,   384,   385,     0,     0,   386,
     387,   388,   389,   384,   385,     0,     0,   386,   387,   388,
     389,   384,   385,   167,     0,   386,   387,   388,   389,   384,
     385,     0,     0,   386,   387,   388,   389,     0,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180
};

static const yytype_int16 yycheck[] =
{
      43,   103,    29,   192,    50,   110,   293,    50,   156,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,     3,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     106,   390,   391,   106,    17,    32,    43,    10,     0,   398,
     399,   165,    49,    43,    14,    42,    53,    54,   699,    49,
       3,    48,   187,   704,    14,    51,    52,    20,   163,    55,
      56,    57,    58,   136,   264,     6,    21,    22,     9,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,    14,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,    16,   227,   228,   229,   230,   231,   213,   182,
      70,   235,   236,    39,   238,     3,    42,     5,     3,   192,
     224,   225,   226,   227,   228,    13,    18,    15,     6,     7,
      14,    14,    11,   281,   282,    23,    24,    25,    26,    27,
      28,    29,    30,    31,     4,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     9,   128,    44,    45,    46,    47,
       4,   293,    50,    11,     5,     9,    19,   291,    10,   293,
       4,    10,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    14,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
     400,   401,   402,   403,   404,   405,     6,    51,    52,    10,
       6,    55,    56,    57,    58,    10,     3,   290,    11,   107,
       3,   294,   229,   230,     3,     3,     3,   115,     3,   229,
     230,   119,     3,     3,   231,   232,   233,     3,     3,   127,
       3,     3,     4,     5,   392,   393,   236,     8,   396,   397,
       3,    13,     3,    15,   224,   225,   226,   227,   228,     3,
     333,    23,    24,    25,    26,    27,    28,    29,    30,    31,
       3,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       3,     3,    44,    45,    46,    47,     3,     3,    50,     3,
      51,    52,     4,     3,    55,    56,    57,    58,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     4,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,     3,     3,     9,     3,
       3,     3,     3,   406,     3,     3,   514,     3,     3,    51,
      52,   229,   230,    55,    56,    57,    58,     3,     3,     3,
       3,     3,     3,    14,     3,    51,    52,   119,     3,    55,
      56,    57,    58,     3,     3,   127,   236,    59,    14,    13,
      51,    52,     9,     4,    55,    56,    57,    58,   502,   503,
     504,   505,   506,   507,   508,     4,   510,   511,     3,   513,
       3,     3,   516,   517,   518,   519,   520,     4,   522,     3,
     524,     3,   526,   527,   528,   529,   239,   531,   532,   533,
     534,     4,     3,     3,   538,   539,   540,   636,     3,     3,
      51,    52,     3,   496,    55,    56,    57,    58,     4,     3,
       3,   536,    51,    52,     3,     3,    55,    56,    57,    58,
       4,     3,     3,     3,    51,    52,     3,     3,    55,    56,
      57,    58,     4,     3,     3,     3,     3,     3,    51,    52,
       3,     3,    55,    56,    57,    58,     3,   229,   230,     3,
       3,     3,     3,     3,   598,    51,    52,     3,     3,    55,
      56,    57,    58,     3,     3,     9,   803,    51,    52,     4,
       7,    55,    56,    57,    58,     9,     9,     9,     9,    51,
      52,     9,     4,    55,    56,    57,    58,     4,     9,     9,
       9,   635,   345,   346,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,   357,   358,   359,   360,   361,   362,
     363,   364,   365,     9,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,    51,
      52,   608,     4,    55,    56,    57,    58,     4,     9,   683,
       4,     9,   129,   636,   688,     4,     9,     9,     9,     3,
     694,     4,     4,     9,     4,   408,    13,   128,   713,   798,
       9,     9,     9,     9,   708,     4,     9,     9,     4,     9,
       4,     9,   716,     4,     9,     4,     9,     4,     4,    51,
      52,   820,     3,    55,    56,    57,    58,    51,    52,     4,
       9,    55,    56,    57,    58,     9,     9,     3,    51,    52,
     839,     9,    55,    56,    57,    58,     4,     4,     4,     4,
       4,     4,     4,     4,   710,    51,    52,   710,   711,    55,
      56,    57,    58,   862,    51,    52,     4,     9,    55,    56,
      57,    58,   757,     4,   759,     4,    51,    52,     4,     9,
      55,    56,    57,    58,     4,     4,     4,     4,   792,     4,
     794,   803,     4,     9,     4,    51,    52,     4,   802,    55,
      56,    57,    58,     9,     4,     4,     4,     9,   761,     4,
     763,     4,     4,    51,    52,     9,   801,    55,    56,    57,
      58,     4,    51,    52,     4,     4,    55,    56,    57,    58,
       4,     4,   836,     9,     9,     4,   840,     9,   823,     9,
       4,     4,     4,     9,   128,   798,     4,     6,     9,     4,
     128,     4,   837,     9,     4,   859,     8,   861,    51,    52,
       4,     9,    55,    56,    57,    58,   819,   820,    51,    52,
       4,   875,    55,    56,    57,    58,     9,     9,    51,    52,
       9,   885,    55,    56,    57,    58,   839,   891,     9,     9,
       9,     4,   113,   897,    41,   899,    26,   882,    51,    52,
      38,   789,    55,    56,    57,    58,   542,    51,    52,   862,
     886,    55,    56,    57,    58,   889,   717,    51,    52,   892,
     640,    55,    56,    57,    58,   878,   713,   636,   293,   791,
     643,   644,   645,   293,   636,    -1,    -1,   650,   651,    -1,
      -1,   654,    -1,    -1,   657,   658,   659,   660,   661,   662,
     663,   664,   665,   666,   667,   668,   669,   670,   671,   672,
       3,   674,     5,     9,   677,   678,    -1,    -1,    -1,    -1,
      13,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    45,    46,    47,    51,    52,    50,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,     4,    -1,    -1,    -1,    -1,
      -1,    -1,   765,   766,     7,     4,   769,    -1,   771,    -1,
      -1,    -1,    -1,    -1,   777,   778,    -1,    -1,    -1,   782,
     783,    -1,    -1,   786,   787,   788,   119,     4,    -1,    -1,
      -1,    -1,    -1,    -1,   127,     4,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    52,    -1,    -1,    55,    56,    57,    58,
       4,    -1,    51,    52,    -1,    -1,    55,    56,    57,    58,
      -1,   824,    -1,    -1,    -1,   828,   829,    -1,    -1,   832,
     833,   834,    -1,    -1,    51,    52,    -1,     4,    55,    56,
      57,    58,    51,    52,    -1,    -1,    55,    56,    57,    58,
      -1,   854,    -1,   856,   857,   858,    -1,    51,    52,    -1,
      -1,    55,    56,    57,    58,    -1,    -1,    -1,    -1,   872,
      -1,   874,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     883,    -1,    -1,    -1,    51,    52,    -1,   890,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,   229,   230,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,     5,     6,    -1,     4,   435,    -1,    11,    -1,    -1,
      -1,   441,    -1,    -1,     4,    -1,   446,   447,   448,    -1,
      -1,    -1,     4,    -1,   454,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   462,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   477,    -1,    -1,
      -1,    51,    52,    -1,     5,    55,    56,    57,    58,    -1,
      11,    51,    52,    -1,    -1,    55,    56,    57,    58,    51,
      52,    75,    -1,    55,    56,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,    75,   129,   130,   131,   132,   133,
     134,    -1,   136,   137,   138,   139,   140,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     4,   120,
     121,   122,   123,   124,   125,   126,   127,     4,   129,   130,
     131,   132,   133,   134,     5,   136,   137,   138,   139,   140,
      11,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     9,    -1,    51,    52,    -1,    -1,    55,
      56,    57,    58,    -1,    51,    52,    -1,    -1,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    51,    52,
      -1,    -1,    55,    56,    57,    58,    51,    52,    -1,    -1,
      55,    56,    57,    58,    75,    51,    52,    -1,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,   120,
     121,   122,   123,   124,   125,   126,   127,    -1,   129,   130,
     131,   132,   133,   134,    -1,   136,   137,   138,   139,   140,
       3,    -1,     5,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      13,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    45,    46,    47,    51,    52,    50,     9,    55,
      56,    57,    58,    -1,    -1,    -1,     9,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,     9,    -1,    -1,    -1,    -1,
      51,    52,    -1,     9,    55,    56,    57,    58,    51,    52,
      -1,     9,    55,    56,    57,    58,    -1,    -1,    -1,     9,
      -1,    -1,    -1,    -1,    -1,    -1,   119,     9,    -1,    -1,
      -1,    -1,    -1,    -1,   127,     9,    -1,    51,    52,    -1,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    -1,    -1,     9,    -1,    51,
      52,    -1,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    -1,    -1,     9,
      -1,    51,    52,    -1,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    -1,
      -1,     9,    -1,    51,    52,    -1,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    -1,    -1,     9,    -1,    51,    52,    -1,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    -1,    -1,     9,    -1,    51,    52,    -1,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    -1,    -1,     9,    -1,    51,
      52,    -1,    -1,    55,    56,    57,    58,    51,    52,    -1,
      -1,    55,    56,    57,    58,    51,    52,    -1,    -1,    55,
      56,    57,    58,    51,    52,    -1,    -1,    55,    56,    57,
      58,    51,    52,    46,    -1,    55,    56,    57,    58,    51,
      52,    -1,    -1,    55,    56,    57,    58,    -1,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,    17,   238,   239,   241,     0,    21,    22,   240,   240,
     240,    14,   224,   225,   226,   227,   228,   246,   251,   252,
       3,    20,   244,    14,   247,   245,   246,    16,   242,     3,
      14,   248,   249,   250,   245,    14,   243,    18,   247,    11,
       4,     9,    11,     5,   284,   248,   252,    10,   249,   252,
       5,    11,    75,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   120,   121,   122,   123,   124,   125,
     126,   127,   129,   130,   131,   132,   133,   134,   136,   137,
     138,   139,   140,   269,   275,   285,   286,   287,   292,   294,
     296,    19,     4,    10,   285,    14,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     276,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,    46,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    71,    72,    73,
      74,   253,   268,   269,   293,     6,   285,    10,   282,    10,
     243,     6,    11,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,   269,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,   279,     3,     3,     3,
       3,     3,    14,   272,   271,     3,     3,   271,     3,     3,
       5,    13,    15,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    44,    45,    47,    50,    60,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,   119,
     127,   229,   230,   253,   260,   261,   262,   263,   264,   265,
     267,     3,   236,    59,   269,    14,   282,   282,   283,   286,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     271,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   280,   267,   267,   267,   267,   267,   267,
     267,   267,   260,    13,   266,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,   263,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,   262,   262,    51,    52,    55,    56,    57,    58,
     258,   259,    43,    49,    53,    54,   229,   230,   256,   257,
      32,    42,    48,   231,   232,   233,   269,   267,     7,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   254,   255,   253,   267,   268,   293,   294,   295,
     269,   270,     9,     9,     9,     9,     9,     9,     9,     4,
       9,     9,     4,     9,     9,     4,     9,     9,     9,     9,
       9,     4,     3,     4,     9,     4,     9,     9,     9,     9,
       4,     9,     9,     9,     9,   269,     4,     4,     9,     9,
       9,     4,     9,     4,     4,     6,     9,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   261,   261,   262,   262,   262,   262,   261,
     261,   263,   263,   263,   263,   263,   263,   269,     9,   260,
     254,   254,   254,   254,   254,   254,   254,   254,     3,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   272,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,     3,   271,   267,   267,   267,
     276,    13,     4,     9,     9,     9,     4,     4,     4,     4,
       9,     9,     4,     4,     9,     4,     4,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     4,     9,     4,     4,     9,     9,   129,
     267,     8,   247,     9,     4,     4,     4,     4,     9,     4,
       4,     4,     4,     4,     9,     4,     4,     4,     4,     9,
       4,     4,     4,     4,     9,     4,     4,     4,     9,   267,
       5,   284,   286,   296,     4,     4,     9,   279,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,    70,   272,   273,   274,     4,   267,   290,
     291,   267,   267,   267,   273,   273,   267,     4,   285,   269,
     282,   283,   267,   280,     4,     9,     9,     4,     4,     9,
       4,     9,     4,     4,     4,     4,     4,     9,     9,     4,
       4,     4,     9,     9,     4,     4,     9,     9,     9,     9,
       4,     9,     9,     4,     9,     4,     4,     4,   271,     6,
     271,   269,     9,   269,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   274,   290,   267,   267,   286,
     128,   271,   267,   268,     9,     4,     4,     4,     9,     9,
       4,     4,     9,     9,     9,     4,     9,   269,   286,   128,
       9,   271,   260,   260,   260,   260,   260,   260,   267,   271,
     286,   267,     4,     4,     9,     4,     9,     9,     9,     9,
     277,     9,   288,   260,   260,   260,   260,   267,   278,   267,
     286,     4,     9,     4,     9,     9,   281,     4,   289,   260,
     260,   267,   269,     9,     4,     9,   271,   260,   267,   277,
       9,     9,   278,   260,   267,   281,     4,     9,   267,     9,
     267,     4
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   237,   238,   239,   240,   240,   240,   241,   242,   242,
     243,   243,   244,   244,   245,   245,   246,   246,   247,   248,
     248,   249,   249,   250,   251,   251,   252,   252,   252,   252,
     252,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   254,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   256,   257,   258,   258,   259,   259,   259,   259,
     260,   260,   260,   260,   260,   261,   261,   261,   261,   261,
     261,   261,   262,   262,   262,   262,   262,   262,   262,   263,
     263,   263,   263,   263,   263,   263,   263,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   265,   266,   266,   267,   268,   269,
     270,   271,   272,   273,   273,   274,   274,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   276,
     277,   278,   279,   280,   281,   282,   282,   283,   283,   284,
     285,   285,   286,   286,   286,   287,   287,   287,   288,   289,
     287,   287,   287,   290,   290,   290,   291,   292,   293,   294,
     294,   295,   295,   295,   296,   296,   296,   296,   296,   296,
     296,   296,   296
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     9,     2,     2,     0,     0,     0,     2,
       5,     0,     2,     0,     2,     1,     6,     7,     0,     0,
       1,     3,     1,     3,     1,     0,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     2,     1,     1,     1,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     1,     2,     2,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     2,     4,     6,     6,
       4,     4,    14,     6,     6,     6,    10,     8,     4,     4,
       8,     6,     6,     6,     4,    18,     6,     6,     8,     6,
       8,     4,    12,    12,     8,     6,     6,     6,    10,     4,
       4,     4,     4,     4,     3,     1,     3,     1,     1,     0,
       0,     0,     1,     1,     1,     3,     1,     2,     5,     5,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       9,     9,    11,     7,     7,     7,     7,    25,     5,     5,
       5,     7,     7,    15,     7,     7,     5,     7,     7,     7,
       7,     5,     2,     2,     2,     2,     5,     7,     5,     0,
       0,     0,     0,     0,     0,     1,     2,     0,     1,     3,
       1,     2,     4,     2,     1,    11,    12,     7,     0,     0,
      20,    15,     4,     0,     3,     1,     1,     4,     7,     9,
       9,     1,     1,     1,     1,     1,     1,     2,     5,     7,
       3,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
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

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

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
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
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
  if (yyn == 0)
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
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* file: program  */
#line 258 "dilpar.ypp"
         {
            ubit8 *b;
            FILE *f;

#ifdef MARCEL
            sprintf(tmpfname, "/tmp/dil.XX");
            if (tmpfname != mktemp(tmpfname)) {
               perror("mktemp");
               exit(1);
            }
#else
            tmpnam(tmpfname);
#endif

            if ((f = fopen(tmpfname, "wb")) == NULL)
               fatal("Couldn't write final code.");

            /* truncate core to used size ! */
            tmpl.coresz = wcore - tmpl.core + 1;

#ifndef DEMO_VERSION
	    CByteBuffer *pBuf = &g_FileBuffer;
	    pBuf->Clear();

	    {
	       char zBuf[500], nBuf[500];

	       split_fi_ref(prg.sp->tmpl->prgname, zBuf, nBuf);

	       if (!istemplate)
	       {
		  if (verbose)
		    fprintf(stderr," prg : %-20s ", nBuf);

		  bwrite_dil(pBuf, &prg);
		  /* dumpdil(&prg); */
	       }
	       else
	       {
		  if (verbose)
		    fprintf(stderr," tmpl: %-20s ", nBuf);

		  bwrite_diltemplate(pBuf, &tmpl);
		  /* dumpdiltemplate(&tmpl); */
	       }
	    }

            pBuf->FileWrite(f);
#endif
            fclose(f);

#ifndef DEMO_VERSION

	    if (istemplate) {
	       FILE *f;
	       char fname[255];

               sprintf(fname,"%s.dh",cur_zonename);
               if ((f = fopen(fname, "a")) == NULL) {
                 fatal("Couldn't append to .dh file");
               }
               fprintf(f,"%s\n",cur_tmplref);
	       fclose(f);               
            }
#endif       

            free_namelist(var_names);
            free_namelist(label_names);
            if (label_no)
               free(label_adr);
	    if (verbose)
	      fprintf(stderr," (%5d bytes)\n", pBuf->GetLength());
            printf("Data in: %s\n", tmpfname);
	    if (errcon)
	      remove(tmpfname);
         }
#line 2422 "dilpar.tab.cpp"
    break;

  case 3: /* program: DILSC_BEG dilinit diloptions fundef dilrefs dildecls DILSC_COD block DILSC_END  */
#line 338 "dilpar.ypp"
         {
            /* start at the top again */
            moredilcore(5);
            bwrite_ubit8(&wcore, DILI_GOTO);
            bwrite_ubit32(&wcore, 0);
            /* truncate surplus core space */
            tmpl.coresz = wcore - tmpl.core + 1;
            update_labels();
            tmpl.corecrc = dil_corecrc(tmpl.core, tmpl.coresz);
	    prg.corecrc = tmpl.corecrc;
	    /* create clean interrupts */
	    if (frm.intrcount)
	      CREATE(frm.intr,struct dilintr,frm.intrcount);
	    else
	      frm.intr = NULL;
         }
#line 2443 "dilpar.tab.cpp"
    break;

  case 4: /* diloptions: DILSC_REC diloptions  */
#line 358 "dilpar.ypp"
          {
             SET_BIT(tmpl.flags, DILFL_RECALL);
             SET_BIT(prg.flags, DILFL_RECALL);
          }
#line 2452 "dilpar.tab.cpp"
    break;

  case 5: /* diloptions: DILSC_AWA diloptions  */
#line 363 "dilpar.ypp"
          {
             SET_BIT(tmpl.flags, DILFL_AWARE);
             SET_BIT(prg.flags, DILFL_AWARE);
          }
#line 2461 "dilpar.tab.cpp"
    break;

  case 6: /* diloptions: %empty  */
#line 368 "dilpar.ypp"
          {
          }
#line 2468 "dilpar.tab.cpp"
    break;

  case 7: /* dilinit: %empty  */
#line 373 "dilpar.ypp"
         {
	    if (verbose)
	      fprintf(stderr, "DIL (line %5d)",linenum);

            /* Set up template  */
            CREATE(tmpl.argt, ubit8, ARGMAX);
            CREATE(tmpl.core, ubit8, CODESIZE);
            CREATE(tmpl.vart, ubit8, VARMAX);
            tmpl.prgname = "NONAME";
            tmpl.varc = 0;
            tmpl.coresz = CODESIZE;
            tmpl.flags = 0;
            tmpl.argc = 0;
            tmpl.xrefcount = 0;
            CREATE(tmpl.xrefs, struct dilxref, REFMAX);
            tmpl.extprg = NULL;
            tmpl.varcrc = 0;
            tmpl.corecrc = 0;
	    tmpl.intrcount = 0;

            wcore = tmpl.core;

            /* setup tmp. reference */
            CREATE(ref.argt, ubit8, ARGMAX);
            CREATE(ref.argv, char *, ARGMAX);
            ref.name= NULL;
            ref.rtnt= DILV_NULL;
            ref.argc= 0;
            
            /* setup program (not used with -t) */
            prg.flags = 0;
            prg.varcrc = 0;
            prg.corecrc = 0;
            prg.stacksz = 1;
            prg.stack = &frm;
            prg.sp = prg.stack;

            /* set up frame (not used with -t) */  
            frm.tmpl = &tmpl;
            frm.ret = 0;
            CREATE(frm.vars, struct dilvar, VARMAX);
            /* not used: */
            frm.pc = tmpl.core;
            frm.securecount = 0;
            frm.secure=NULL;
	    frm.intrcount = 0;

            /* compiler variables */
            var_names = create_namelist();
            ref_names = create_namelist();
	    refcount=0;
            ref_usednames = create_namelist();
            /* labels */
            labelgen = 0;
            label_names = create_namelist();
            label_adr = NULL;
            label_no = 0;
            /* label uses */
            label_use_no = 0;
            label_use_adr = NULL;
            label_use_idx = NULL;
            /* break and continue */
            break_no = 0;
            cont_no = 0;
            break_idx = NULL;
            cont_idx = NULL;
	    *cur_tmplref='\0';

         }
#line 2542 "dilpar.tab.cpp"
    break;

  case 9: /* dildecls: DILSC_VAR decls  */
#line 446 "dilpar.ypp"
         {
            tmpl.varcrc = dil_headercrc(var_names, tmpl.vart);
	    prg.varcrc = tmpl.varcrc;
         }
#line 2551 "dilpar.tab.cpp"
    break;

  case 10: /* decls: SYMBOL ':' type ';' decls  */
#line 453 "dilpar.ypp"
         {
            add_var((yyvsp[-4].sym),(yyvsp[-2].num));
         }
#line 2559 "dilpar.tab.cpp"
    break;

  case 14: /* refs: ref refs  */
#line 465 "dilpar.ypp"
         {
         }
#line 2566 "dilpar.tab.cpp"
    break;

  case 15: /* refs: ref  */
#line 468 "dilpar.ypp"
         {
         }
#line 2573 "dilpar.tab.cpp"
    break;

  case 16: /* ref: SYMBOL '(' arginit refarglist ')' ';'  */
#line 473 "dilpar.ypp"
         {
            char zbuf[255];
            char nbuf[255];
            
            /* define procedure setup */
            split_fi_ref((yyvsp[-5].sym),zbuf,nbuf);

	    if ((strlen(zbuf) ? strlen(zbuf) : -1) + strlen(nbuf) + 1 !=
		strlen((yyvsp[-5].sym)))
	    {
	       sprintf(zbuf, "Name of dilprogram '%s' was too long.",
		       nbuf);
	       fatal(zbuf);
	    }

            if (!*zbuf)
              strcpy(zbuf,cur_zonename);

            ref.name = str_dup(nbuf);
            ref.zname = str_dup(zbuf);
            ref.rtnt = DILV_NULL;

	    add_ref(&ref);
         }
#line 2602 "dilpar.tab.cpp"
    break;

  case 17: /* ref: type SYMBOL '(' arginit refarglist ')' ';'  */
#line 498 "dilpar.ypp"
         {
            char zbuf[255];
            char nbuf[255];

            /* define function setup */
            split_fi_ref((yyvsp[-5].sym),zbuf,nbuf);

	    if ((strlen(zbuf) ? strlen(zbuf) : -1) + strlen(nbuf) + 1 !=
		strlen((yyvsp[-5].sym)))
	    {
	       sprintf(zbuf, "Name of dilprogram '%s' was too long.",
		       nbuf);
	       fatal(zbuf);
	    }

            if (!*zbuf)
              strcpy(zbuf,cur_zonename);

            ref.name = str_dup(nbuf);
            ref.zname = str_dup(zbuf);
            ref.rtnt = (yyvsp[-6].num);
	    add_ref(&ref);
         }
#line 2630 "dilpar.tab.cpp"
    break;

  case 18: /* arginit: %empty  */
#line 524 "dilpar.ypp"
         {
	    /* init arglist */
            ref.argc=0;
         }
#line 2639 "dilpar.tab.cpp"
    break;

  case 23: /* refarg: SYMBOL ':' type  */
#line 538 "dilpar.ypp"
         {
            /* collect argument */
            ref.argt[ref.argc] = (yyvsp[0].num);
            ref.argv[ref.argc] = str_dup((yyvsp[-2].sym));
            if (++ref.argc > ARGMAX)
               fatal("Too many arguments");
         }
#line 2651 "dilpar.tab.cpp"
    break;

  case 24: /* fundef: ref  */
#line 547 "dilpar.ypp"
         {
            static const char *typname[] = {
               "void",
               "unitptr",
               "string",
               "stringlist",
               "extraptr",
               "integer"};
            char buf[255],nbuf[255],zbuf[255];
            int i;

            /* define this procedure */
	    split_fi_ref(ref.name,zbuf,nbuf);
	    if (!*zbuf)
	      strcpy(zbuf,cur_zonename);
	    sprintf(buf,"%s@%s",nbuf,zbuf);
            tmpl.prgname = str_dup(buf);
            tmpl.rtnt = ref.rtnt;
            tmpl.argc = ref.argc;

            if (tmpl.argc && !istemplate)
	      fatal("Arguments not allowed on inline DIL programs, make "
		    "it into a template instead.");

	    if (tmpl.argc) {
	       CREATE(tmpl.argt, ubit8, ref.argc);
	       memcpy(tmpl.argt,ref.argt,ref.argc);
	    } else 
	      tmpl.argt=NULL;

            /* create template reference line for .dh file */
            if (ref.rtnt == DILV_NULL)
              sprintf(cur_tmplref,"%s (", tmpl.prgname);
            else
              sprintf(cur_tmplref,"%s %s (", typname[tmpl.rtnt], tmpl.prgname);
              
            for (i=0;i<ref.argc;i++)
	    {
	       /* add arguments as variables */
	       add_var(ref.argv[i], ref.argt[i]);

	       /* generate template reference line */
	       strcat(cur_tmplref, ref.argv[i]);
	       strcat(cur_tmplref, " : ");
	       strcat(cur_tmplref, typname[ref.argt[i]]);
	       if (i<ref.argc-1)
		 strcat(cur_tmplref, ", ");
	    }
	    strcat(cur_tmplref, ");");


	    /* clear used names ! 
	    ref_usednames[0]=NULL;
	    ref_names[0]=NULL;
	    refcount=0;
	    */
         }
#line 2713 "dilpar.tab.cpp"
    break;

  case 25: /* fundef: %empty  */
#line 605 "dilpar.ypp"
         {
	    fatal("All DIL programs must have a name");
            *cur_tmplref='\0';
         }
#line 2722 "dilpar.tab.cpp"
    break;

  case 26: /* type: DILST_UP  */
#line 616 "dilpar.ypp"
         {
            (yyval.num) = DILV_UP;
         }
#line 2730 "dilpar.tab.cpp"
    break;

  case 27: /* type: DILST_INT  */
#line 620 "dilpar.ypp"
         {
            (yyval.num) = DILV_INT;
         }
#line 2738 "dilpar.tab.cpp"
    break;

  case 28: /* type: DILST_SP  */
#line 624 "dilpar.ypp"
         {
            (yyval.num) = DILV_SP;
         }
#line 2746 "dilpar.tab.cpp"
    break;

  case 29: /* type: DILST_SLP  */
#line 628 "dilpar.ypp"
         {
            (yyval.num) = DILV_SLP;
         }
#line 2754 "dilpar.tab.cpp"
    break;

  case 30: /* type: DILST_EDP  */
#line 632 "dilpar.ypp"
         {
            (yyval.num) = DILV_EDP;
         }
#line 2762 "dilpar.tab.cpp"
    break;

  case 31: /* variable: variable '.' field  */
#line 642 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    if ((yyvsp[-2].exp).dsl==DSL_FCT)
	       fatal("Attempting to derive fields of proc/func");

            switch ((yyvsp[0].exp).rtyp)
	    {
               /* these structures have fields */
               case DILV_UP:
               case DILV_EDP:
               case DILV_SLP:
               case DILV_SP:
                  if ((yyvsp[-2].exp).typ != (yyvsp[0].exp).rtyp)
                     fatal("Illegal type for field");
               break;

               case DILV_UEDP: /* shared fields */
                  if ((yyvsp[-2].exp).typ != DILV_UP && (yyvsp[-2].exp).typ != DILV_EDP)
                     fatal("Illegal type for field");
               break;
               default:
                  fatal("Illegal field");
               break;
            }

            /* create code */
            add_ubit8(&((yyval.exp)),DILE_FLD);
            add_ubit8(&((yyval.exp)),(yyvsp[0].exp).num);
            add_code(&((yyval.exp)),&((yyvsp[-2].exp)));

            if ((yyvsp[0].exp).codep != (yyvsp[0].exp).code) /* index optinal */
               add_code(&((yyval.exp)),&((yyvsp[0].exp))); 

            if ((yyvsp[0].exp).typ == DILV_UEDP)
               (yyval.exp).typ = (yyvsp[-2].exp).typ; /* inherit type */
            else
               (yyval.exp).typ = (yyvsp[0].exp).typ; /* follow type */

            (yyval.exp).dsl = (yyvsp[0].exp).dsl;   /* static/dynamic/lvalue */

	    if ((yyvsp[-2].exp).typ == DILV_SLP && (yyvsp[-2].exp).dsl == DSL_LFT)
	      (yyval.exp).dsl = DSL_LFT;

            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 2813 "dilpar.tab.cpp"
    break;

  case 32: /* variable: ihold SYMBOL iunhold  */
#line 689 "dilpar.ypp"
         {
            int i,refnum;
	    char nbuf[255],zbuf[255],buf[255];

            INITEXP((yyval.exp));

            if ((i = search_block((yyvsp[-1].sym), (const char **) var_names, TRUE)) == -1)
	    {
	       /* not a variable */
	       /* check external function */

	       split_fi_ref((yyvsp[-1].sym),zbuf,nbuf);
	       if (!*zbuf)
		 strcpy(zbuf,cur_zonename);
	       sprintf(buf,"%s@%s",nbuf,zbuf);

	       /* mark reference as used */
	       if ((refnum = (search_block(buf, (const char **) ref_usednames, TRUE))) == -1)
	       {
		  /* not used before, add to tmpl */
		  if ((i = (search_block(buf, (const char **) ref_names, TRUE))) == -1)
		  {
		     char tmpbuf[256];
		     sprintf(tmpbuf, "No such variable or reference: %s", buf);
		     fatal(tmpbuf);
		  }
	       
		  /* they share the arrays !! */
		  sprintf(buf,"%s@%s",refs[i].name,refs[i].zname);
		  tmpl.xrefs[tmpl.xrefcount].name=str_dup(buf);
		  tmpl.xrefs[tmpl.xrefcount].rtnt=refs[i].rtnt;
		  tmpl.xrefs[tmpl.xrefcount].argc=refs[i].argc;
		  tmpl.xrefs[tmpl.xrefcount].argt=refs[i].argt;

		  refnum=tmpl.xrefcount++; /* this is the reference location */
		  
		  /* it would be wierd if this uccurs, but better safe... */
		  if (tmpl.xrefcount > REFMAX)
		    fatal("Too many references");
	       }
	       (yyval.exp).dsl = DSL_FCT;
	       (yyval.exp).rtyp = refnum;
	       (yyval.exp).typ = tmpl.xrefs[refnum].rtnt;
               /*fprintf(stderr, "Adding ubit16 reference %d %p.\n",
		       refnum, &($$));*/
               add_ubit16(&((yyval.exp)), refnum);
            }
	    else /* A variable */
	    {
               (yyval.exp).typ = tmpl.vart[i];
               (yyval.exp).dsl = DSL_LFT;
               add_ubit8(&((yyval.exp)),DILE_VAR);
               add_ubit16(&((yyval.exp)),i);
            }
         }
#line 2873 "dilpar.tab.cpp"
    break;

  case 33: /* variable: DILSE_SELF  */
#line 745 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_SELF);
         }
#line 2884 "dilpar.tab.cpp"
    break;

  case 34: /* variable: DILSE_ACTI  */
#line 752 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_ACTI);
         }
#line 2895 "dilpar.tab.cpp"
    break;

  case 35: /* variable: DILSE_MEDI  */
#line 759 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_MEDI);
         }
#line 2906 "dilpar.tab.cpp"
    break;

  case 36: /* variable: DILSE_TARG  */
#line 766 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_TARG);
         }
#line 2917 "dilpar.tab.cpp"
    break;

  case 37: /* variable: DILSE_POWE  */
#line 773 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_POWE);
         }
#line 2928 "dilpar.tab.cpp"
    break;

  case 38: /* variable: DILSE_CMST  */
#line 780 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_SP;
            add_ubit8(&((yyval.exp)),DILE_CMST);
         }
#line 2939 "dilpar.tab.cpp"
    break;

  case 39: /* variable: DILSE_ARGM  */
#line 787 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_SP;
            add_ubit8(&((yyval.exp)),DILE_ARGM);
         }
#line 2950 "dilpar.tab.cpp"
    break;

  case 40: /* variable: DILSE_HRT  */
#line 794 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_HRT);
         }
#line 2961 "dilpar.tab.cpp"
    break;

  case 41: /* variable: DILSE_WEAT  */
#line 801 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_WEAT);
	 }
#line 2972 "dilpar.tab.cpp"
    break;

  case 42: /* variable: DILSE_THO  */
#line 808 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_THO);
         }
#line 2983 "dilpar.tab.cpp"
    break;

  case 43: /* variable: DILSE_TDA  */
#line 815 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_TDA);
         }
#line 2994 "dilpar.tab.cpp"
    break;

  case 44: /* variable: DILSE_TMD  */
#line 822 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_TMD);
         }
#line 3005 "dilpar.tab.cpp"
    break;

  case 45: /* variable: DILSE_TYE  */
#line 829 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_TYE);
         }
#line 3016 "dilpar.tab.cpp"
    break;

  case 46: /* variable: DILSE_RTI  */
#line 836 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_RTI);
         }
#line 3027 "dilpar.tab.cpp"
    break;

  case 47: /* idx: '[' dilexp ']'  */
#line 855 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    checkbool("index in array",(yyvsp[-1].exp).boolean);
            if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Integer expected for index");
            else 
               make_code(&((yyvsp[-1].exp)));     /* make it dynamic */
            copy_code(&((yyval.exp)),&((yyvsp[-1].exp))); /* copy the code   */
            FREEEXP((yyvsp[-1].exp));
         }
#line 3042 "dilpar.tab.cpp"
    break;

  case 48: /* field: idx  */
#line 868 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_SLP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_IDX;
            FREEEXP((yyvsp[0].exp));
         }
#line 3056 "dilpar.tab.cpp"
    break;

  case 49: /* field: DILSF_BIR  */
#line 883 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_BIR;
         }
#line 3068 "dilpar.tab.cpp"
    break;

  case 50: /* field: DILSF_PTI  */
#line 891 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_PTI;
         }
#line 3080 "dilpar.tab.cpp"
    break;

  case 51: /* field: DILSF_PCF  */
#line 899 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_PCF;
         }
#line 3092 "dilpar.tab.cpp"
    break;

  case 52: /* field: DILSF_HOME  */
#line 907 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_HOME;
         }
#line 3104 "dilpar.tab.cpp"
    break;

  case 53: /* field: DILSF_MAS  */
#line 915 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_MAS;
         }
#line 3116 "dilpar.tab.cpp"
    break;

  case 54: /* field: DILSF_FOL  */
#line 923 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_FOL;
         }
#line 3128 "dilpar.tab.cpp"
    break;

  case 55: /* field: DILSF_IDX  */
#line 931 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_SID;
         }
#line 3140 "dilpar.tab.cpp"
    break;

  case 56: /* field: DILSF_ZOI  */
#line 939 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ZOI;
         }
#line 3152 "dilpar.tab.cpp"
    break;

  case 57: /* field: DILSF_NMI  */
#line 947 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_NMI;
         }
#line 3164 "dilpar.tab.cpp"
    break;

  case 58: /* field: DILSF_TYP  */
#line 955 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_TYP;
         }
#line 3176 "dilpar.tab.cpp"
    break;

  case 59: /* field: DILSF_NXT  */
#line 963 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UEDP;
            (yyval.exp).typ = DILV_UEDP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_NXT;
         }
#line 3188 "dilpar.tab.cpp"
    break;

  case 60: /* field: DILSF_NMS  */
#line 971 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UEDP;
            (yyval.exp).typ = DILV_SLP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_NMS;
         }
#line 3200 "dilpar.tab.cpp"
    break;

  case 61: /* field: DILSF_NAM  */
#line 979 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UEDP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_NAM;
         }
#line 3212 "dilpar.tab.cpp"
    break;

  case 62: /* field: DILSF_DES  */
#line 987 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_EDP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_DES;
         }
#line 3224 "dilpar.tab.cpp"
    break;

  case 63: /* field: DILSF_ODES  */
#line 995 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ODES;
         }
#line 3236 "dilpar.tab.cpp"
    break;

  case 64: /* field: DILSF_IDES  */
#line 1003 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_IDES;
         }
#line 3248 "dilpar.tab.cpp"
    break;

  case 65: /* field: DILSF_TIT  */
#line 1011 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_TIT;
         }
#line 3260 "dilpar.tab.cpp"
    break;

  case 66: /* field: DILSF_EXT  */
#line 1019 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_EDP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_EXT;
         }
#line 3272 "dilpar.tab.cpp"
    break;

  case 67: /* field: DILSF_OUT  */
#line 1027 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_OUT;
         }
#line 3284 "dilpar.tab.cpp"
    break;

  case 68: /* field: DILSF_INS  */
#line 1035 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_INS;
         }
#line 3296 "dilpar.tab.cpp"
    break;

  case 69: /* field: DILSF_GNX  */
#line 1043 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_GNX;
         }
#line 3308 "dilpar.tab.cpp"
    break;

  case 70: /* field: DILSF_LCN  */
#line 1051 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_LCN;
         }
#line 3320 "dilpar.tab.cpp"
    break;

  case 71: /* field: DILSF_GPR  */
#line 1059 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_GPR;
         }
#line 3332 "dilpar.tab.cpp"
    break;

  case 72: /* field: DILSF_LGT  */
#line 1067 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_LGT;
         }
#line 3344 "dilpar.tab.cpp"
    break;

  case 73: /* field: DILSF_BGT  */
#line 1075 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_BGT;
         }
#line 3356 "dilpar.tab.cpp"
    break;

  case 74: /* field: DILSF_MIV  */
#line 1083 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MIV;
         }
#line 3368 "dilpar.tab.cpp"
    break;

  case 75: /* field: DILSF_ILL  */
#line 1091 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ILL;
         }
#line 3380 "dilpar.tab.cpp"
    break;

  case 76: /* field: DILSF_FL  */
#line 1099 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_FL;
         }
#line 3392 "dilpar.tab.cpp"
    break;

  case 77: /* field: DILSF_MAN  */
#line 1107 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MAN;
         }
#line 3404 "dilpar.tab.cpp"
    break;

  case 78: /* field: DILSF_OFL  */
#line 1115 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_OFL;
         }
#line 3416 "dilpar.tab.cpp"
    break;

  case 79: /* field: DILSF_MHP  */
#line 1123 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MHP;
         }
#line 3428 "dilpar.tab.cpp"
    break;

  case 80: /* field: DILSF_INFO  */
#line 1131 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_EDP;
            (yyval.exp).dsl = DSL_DYN;
	    (yyval.exp).num = DILF_INFO;
         }
#line 3440 "dilpar.tab.cpp"
    break;

  case 81: /* field: DILSF_LSA  */
#line 1139 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_LSA;
         }
#line 3452 "dilpar.tab.cpp"
    break;

  case 82: /* field: DILSF_MED  */
#line 1147 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_MED;
         }
#line 3464 "dilpar.tab.cpp"
    break;

  case 83: /* field: DILSF_MMA  */
#line 1155 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_MMA;
         }
#line 3476 "dilpar.tab.cpp"
    break;

  case 84: /* field: DILSF_CHP  */
#line 1163 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CHP;
         }
#line 3488 "dilpar.tab.cpp"
    break;

  case 85: /* field: DILSF_BWT  */
#line 1171 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_BWT;
         }
#line 3500 "dilpar.tab.cpp"
    break;

  case 86: /* field: DILSF_WGT  */
#line 1179 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_WGT;
         }
#line 3512 "dilpar.tab.cpp"
    break;

  case 87: /* field: DILSF_CAP  */
#line 1187 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CAP;
         }
#line 3524 "dilpar.tab.cpp"
    break;

  case 88: /* field: DILSF_ALG  */
#line 1195 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ALG;
         }
#line 3536 "dilpar.tab.cpp"
    break;

  case 89: /* field: DILSF_SPL idx  */
#line 1203 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_SPL;
         }
#line 3549 "dilpar.tab.cpp"
    break;

  case 90: /* field: DILSF_FUN  */
#line 1212 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_FUN;
         }
#line 3561 "dilpar.tab.cpp"
    break;

  case 91: /* field: DILSF_ZON  */
#line 1220 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ZON;
         }
#line 3573 "dilpar.tab.cpp"
    break;

  case 92: /* field: DILSF_OTY  */
#line 1233 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_OTY;
         }
#line 3585 "dilpar.tab.cpp"
    break;

  case 93: /* field: DILSF_VAL idx  */
#line 1241 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_VAL;
            FREEEXP((yyvsp[0].exp));
         }
#line 3599 "dilpar.tab.cpp"
    break;

  case 94: /* field: DILSF_EFL  */
#line 1251 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_EFL;
         }
#line 3611 "dilpar.tab.cpp"
    break;

  case 95: /* field: DILSF_CST  */
#line 1259 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CST;
         }
#line 3623 "dilpar.tab.cpp"
    break;

  case 96: /* field: DILSF_RNT  */
#line 1267 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_RNT;
         }
#line 3635 "dilpar.tab.cpp"
    break;

  case 97: /* field: DILSF_EQP  */
#line 1275 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_EQP;
         }
#line 3647 "dilpar.tab.cpp"
    break;

  case 98: /* field: DILSF_ONM idx  */
#line 1288 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SLP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ONM;
            FREEEXP((yyvsp[0].exp));
         }
#line 3661 "dilpar.tab.cpp"
    break;

  case 99: /* field: DILSF_XNF idx  */
#line 1298 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_XNF;
            FREEEXP((yyvsp[0].exp));
         }
#line 3675 "dilpar.tab.cpp"
    break;

  case 100: /* field: DILSF_TOR idx  */
#line 1308 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_TOR;
            FREEEXP((yyvsp[0].exp));
         }
#line 3689 "dilpar.tab.cpp"
    break;

  case 101: /* field: DILSF_RFL  */
#line 1318 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_RFL;
         }
#line 3701 "dilpar.tab.cpp"
    break;

  case 102: /* field: DILSF_MOV  */
#line 1326 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MOV;
         }
#line 3713 "dilpar.tab.cpp"
    break;

  case 103: /* field: DILSF_ABAL  */
#line 1339 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ABAL;
         }
#line 3725 "dilpar.tab.cpp"
    break;

  case 104: /* field: DILSF_ATOT  */
#line 1347 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ATOT;
         }
#line 3737 "dilpar.tab.cpp"
    break;

  case 105: /* field: DILSF_SPD  */
#line 1355 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_SPD;
         }
#line 3749 "dilpar.tab.cpp"
    break;

  case 106: /* field: DILSF_SEX  */
#line 1363 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_SEX;
         }
#line 3761 "dilpar.tab.cpp"
    break;

  case 107: /* field: DILSF_RCE  */
#line 1371 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_RCE;
         }
#line 3773 "dilpar.tab.cpp"
    break;

  case 108: /* field: DILSF_ABL idx  */
#line 1379 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ABL;
            FREEEXP((yyvsp[0].exp));
         }
#line 3787 "dilpar.tab.cpp"
    break;

  case 109: /* field: DILSF_EXP  */
#line 1389 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_EXP;
         }
#line 3799 "dilpar.tab.cpp"
    break;

  case 110: /* field: DILSF_LVL  */
#line 1397 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_LVL;
         }
#line 3811 "dilpar.tab.cpp"
    break;

  case 111: /* field: DILSF_HGT  */
#line 1405 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_HGT;
         }
#line 3823 "dilpar.tab.cpp"
    break;

  case 112: /* field: DILSF_POS  */
#line 1413 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_POS;
         }
#line 3835 "dilpar.tab.cpp"
    break;

  case 113: /* field: DILSF_ATY  */
#line 1421 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ATY;
         }
#line 3847 "dilpar.tab.cpp"
    break;

  case 114: /* field: DILSF_MNA  */
#line 1429 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MNA;
         }
#line 3859 "dilpar.tab.cpp"
    break;

  case 115: /* field: DILSF_END  */
#line 1437 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_END;
         }
#line 3871 "dilpar.tab.cpp"
    break;

  case 116: /* field: DILSF_AFF  */
#line 1445 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_AFF;
         }
#line 3883 "dilpar.tab.cpp"
    break;

  case 117: /* field: DILSF_DRE  */
#line 1453 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_DRE;
         }
#line 3895 "dilpar.tab.cpp"
    break;

  case 118: /* field: DILSF_FGT  */
#line 1461 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_FGT;
         }
#line 3907 "dilpar.tab.cpp"
    break;

  case 119: /* field: DILSF_WPN idx  */
#line 1469 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_WPN;
            FREEEXP((yyvsp[0].exp));
         }
#line 3921 "dilpar.tab.cpp"
    break;

  case 120: /* field: DILSF_DEF  */
#line 1484 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_DEF;
         }
#line 3933 "dilpar.tab.cpp"
    break;

  case 121: /* field: DILSF_ACT  */
#line 1492 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ACT;
         }
#line 3945 "dilpar.tab.cpp"
    break;

  case 122: /* field: DILSF_TIM  */
#line 1505 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_TIM;
         }
#line 3957 "dilpar.tab.cpp"
    break;

  case 123: /* field: DILSF_CRM  */
#line 1513 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CRM;
         }
#line 3969 "dilpar.tab.cpp"
    break;

  case 124: /* field: DILSF_FLL  */
#line 1521 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_FLL;
         }
#line 3981 "dilpar.tab.cpp"
    break;

  case 125: /* field: DILSF_THR  */
#line 1529 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_THR;
         }
#line 3993 "dilpar.tab.cpp"
    break;

  case 126: /* field: DILSF_DRK  */
#line 1537 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_DRK;
         }
#line 4005 "dilpar.tab.cpp"
    break;

  case 127: /* field: DILSF_SPT  */
#line 1545 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_SPT;
         }
#line 4017 "dilpar.tab.cpp"
    break;

  case 128: /* field: DILSF_APT  */
#line 1553 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_APT;
         }
#line 4029 "dilpar.tab.cpp"
    break;

  case 129: /* field: DILSF_GLD  */
#line 1561 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_GLD;
         }
#line 4041 "dilpar.tab.cpp"
    break;

  case 130: /* field: DILSF_QST  */
#line 1569 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_EDP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_QST;
         }
#line 4053 "dilpar.tab.cpp"
    break;

  case 131: /* field: DILSF_SKL idx  */
#line 1577 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_SKL;
            FREEEXP((yyvsp[0].exp));
         }
#line 4067 "dilpar.tab.cpp"
    break;

  case 132: /* scmp: DILTO_SEQ  */
#line 1593 "dilpar.ypp"
         {
            (yyval.num) = DILE_SE;
         }
#line 4075 "dilpar.tab.cpp"
    break;

  case 133: /* pcmp: DILTO_PEQ  */
#line 1599 "dilpar.ypp"
         {
            (yyval.num) = DILE_PE;
         }
#line 4083 "dilpar.tab.cpp"
    break;

  case 134: /* cmp: DILTO_EQ  */
#line 1605 "dilpar.ypp"
         {
            (yyval.num) = DILE_EQ;
         }
#line 4091 "dilpar.tab.cpp"
    break;

  case 135: /* cmp: DILTO_NEQ  */
#line 1609 "dilpar.ypp"
         {
            (yyval.num) = DILE_NE;
         }
#line 4099 "dilpar.tab.cpp"
    break;

  case 136: /* icmp: DILTO_GT  */
#line 1615 "dilpar.ypp"
         {
            (yyval.num) = DILE_GT;
         }
#line 4107 "dilpar.tab.cpp"
    break;

  case 137: /* icmp: DILTO_LE  */
#line 1619 "dilpar.ypp"
         {
            (yyval.num) = DILE_LT;
         }
#line 4115 "dilpar.tab.cpp"
    break;

  case 138: /* icmp: DILTO_GEQ  */
#line 1623 "dilpar.ypp"
         {
            (yyval.num) = DILE_GE;
         }
#line 4123 "dilpar.tab.cpp"
    break;

  case 139: /* icmp: DILTO_LEQ  */
#line 1627 "dilpar.ypp"
         {
            (yyval.num) = DILE_LE;
         }
#line 4131 "dilpar.tab.cpp"
    break;

  case 140: /* dilexp: dilsexp  */
#line 1637 "dilpar.ypp"
         {
/*          fprintf(stderr,"dilsexp->dilexp\n");*/
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4143 "dilpar.tab.cpp"
    break;

  case 141: /* dilexp: dilexp cmp dilsexp  */
#line 1645 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            switch ((yyvsp[-2].exp).typ) {
               /*
                * Replaces integer function identifies with
                * appropiate pointer/string function identifier
                * where appropriate.
                */

               case DILV_INT:
                  if ((yyvsp[0].exp).typ != DILV_INT)
                     fatal("Arg 2 of integer comparison not integer");
                  break;
               case DILV_NULL:
               case DILV_UP:
               case DILV_EDP:
               case DILV_SLP:
                  if (((yyvsp[-2].exp).typ != (yyvsp[0].exp).typ) &&
                      ((yyvsp[-2].exp).typ != DILV_NULL) && ((yyvsp[0].exp).typ != DILV_NULL))
                     fatal("Argument of pointer compare not of same type.");
                  if ((yyvsp[-1].num) == DILE_NE)
                     (yyvsp[-1].num) = DILE_PNEQ; /* snask */
                  else
                     (yyvsp[-1].num) = DILE_PE; /* snask */
                  break;
               case DILV_SP:
                  if ((yyvsp[0].exp).typ == DILV_NULL) {
                     if ((yyvsp[-1].num) == DILE_NE)
                        (yyvsp[-1].num) = DILE_PNEQ; /* snask */
                     else
                        (yyvsp[-1].num) = DILE_PE; /* snask */
                     break;
                  }
                  if ((yyvsp[0].exp).typ != DILV_SP)
                     fatal("Arg 2 of string comp. not a string");
                  else {
                     if ((yyvsp[-1].num) == DILE_NE)
                        (yyvsp[-1].num) = DILE_SNEQ; /* snask */
                     else
                        (yyvsp[-1].num) = DILE_SE; /* snask */
                  }
                  break;
               default:
                  fatal("Arg 1 of compare invalid type");
               break;
            }
            /* Make nodes dynamic */
            /* Integer compares are not _yet_ static */
            make_code(&((yyvsp[-2].exp)));
            make_code(&((yyvsp[0].exp)));
            add_ubit8(&((yyval.exp)),(yyvsp[-1].num));
            add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
            add_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4207 "dilpar.tab.cpp"
    break;

  case 142: /* dilexp: dilexp icmp dilsexp  */
#line 1705 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of integer comparison not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of integer comparison not integer");
            else
            {
               /* Make nodes dynamic */
               /* Integer compares are not _yet_ static */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)),(yyvsp[-1].num));
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4234 "dilpar.tab.cpp"
    break;

  case 143: /* dilexp: dilsexp pcmp dilsexp  */
#line 1728 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            switch ((yyvsp[-2].exp).typ)
            {
             case DILV_NULL:
             case DILV_UP:
             case DILV_SP:
             case DILV_EDP:
             case DILV_SLP:
               if (((yyvsp[-2].exp).typ != (yyvsp[0].exp).typ) &&
               ((yyvsp[-2].exp).typ != DILV_NULL) && ((yyvsp[0].exp).typ != DILV_NULL))
                  fatal("Argument of pointer compare not of same type.");
               else
               {
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)),(yyvsp[-1].num)); /* compare funct */
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
                  (yyval.exp).typ = DILV_INT;
               }
               break;

             default:
               fatal("Arg 1 of pointer comp. not a pointer");
               break;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4271 "dilpar.tab.cpp"
    break;

  case 144: /* dilexp: dilsexp scmp dilsexp  */
#line 1761 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-2].exp).typ != DILV_SP)
               fatal("Arg 1 of string comp. not a string");
            else if ((yyvsp[-2].exp).typ != DILV_SP)
               fatal("Arg 2 of string comp. not a string");
            else
            {
               /* Strings are not _yet_ static */
               /* String compare not _yet_ static */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)),(yyvsp[-1].num)); /* compare funct */
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4298 "dilpar.tab.cpp"
    break;

  case 145: /* dilsexp: dilterm  */
#line 1786 "dilpar.ypp"
         {
/*          fprintf(stderr,"dilterm->dilsexp\n");*/
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4310 "dilpar.tab.cpp"
    break;

  case 146: /* dilsexp: '-' dilterm  */
#line 1794 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 1 of 'unary -' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               (yyval.exp).dsl = (yyvsp[0].exp).dsl;
               if (!(yyvsp[0].exp).dsl)
                  (yyval.exp).num =   - (yyvsp[0].exp).num; /* static */
               else {
                  add_ubit8(&((yyval.exp)), DILE_UMIN); 
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               }
            }
            FREEEXP((yyvsp[0].exp));
         }
#line 4333 "dilpar.tab.cpp"
    break;

  case 147: /* dilsexp: '+' dilterm  */
#line 1813 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 1 of 'unary +' not integer");
            else {
               /* Type is ok */
               copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            }
            FREEEXP((yyvsp[0].exp));
         }
#line 4349 "dilpar.tab.cpp"
    break;

  case 148: /* dilsexp: dilsexp '+' dilterm  */
#line 1825 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
            {
               if ((yyvsp[-2].exp).typ != DILV_SP)
                  fatal("Arg 1 of '+' not integer or string");
               else if ((yyvsp[0].exp).typ != DILV_SP)
                  fatal("Arg 2 of '+' not string");
               else {
                  /* Type is SP + SP */
                  (yyval.exp).typ = DILV_SP;
                  if (!((yyvsp[-2].exp).dsl+(yyvsp[0].exp).dsl)) {
                     /* both static */
                     copy_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                     cat_string(&((yyval.exp)),(char *) &((yyvsp[0].exp).code[1]));
                     (yyval.exp).dsl = DSL_STA;
                  } else {
                     /* one is dynamic */
                     make_code(&((yyvsp[-2].exp)));
                     make_code(&((yyvsp[0].exp)));
                     add_ubit8(&((yyval.exp)),DILE_PLUS);
                     add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                     add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                     (yyval.exp).dsl = DSL_DYN;
                  }
               }
            }
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '+' not integer");
            else {
               /* Type is INT + INT */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).typ)) {
                  (yyval.exp).num = (yyvsp[-2].exp).num + (yyvsp[0].exp).num;
                  (yyval.exp).dsl = DSL_STA;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_PLUS);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4403 "dilpar.tab.cpp"
    break;

  case 149: /* dilsexp: dilsexp '-' dilterm  */
#line 1875 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '-' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '-' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  (yyval.exp).num = (yyvsp[-2].exp).num - (yyvsp[0].exp).num;
                  (yyval.exp).dsl = DSL_STA;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_MIN);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4434 "dilpar.tab.cpp"
    break;

  case 150: /* dilsexp: dilsexp DILSE_LOR dilterm  */
#line 1902 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               /* static integers */
               (yyval.exp).num = (yyvsp[-2].exp).num || (yyvsp[0].exp).num;
               (yyval.exp).dsl = DSL_STA;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_LOR);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4462 "dilpar.tab.cpp"
    break;

  case 151: /* dilsexp: dilsexp DILSE_OR dilterm  */
#line 1926 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               /* static integers */
               (yyval.exp).num = (yyvsp[-2].exp).num | (yyvsp[0].exp).num;
               (yyval.exp).dsl = DSL_STA;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_OR);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4490 "dilpar.tab.cpp"
    break;

  case 152: /* dilterm: dilfactor  */
#line 1952 "dilpar.ypp"
         {
/*          fprintf(stderr,"dilfactor->dilterm\n");*/
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4502 "dilpar.tab.cpp"
    break;

  case 153: /* dilterm: dilterm '*' dilfactor  */
#line 1960 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '*' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '*' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  /* Both values are static */
                  (yyval.exp).dsl = DSL_STA;
                  (yyval.exp).num = (yyvsp[-2].exp).num * (yyvsp[0].exp).num;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_MUL);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4534 "dilpar.tab.cpp"
    break;

  case 154: /* dilterm: dilterm '/' dilfactor  */
#line 1988 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '/' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '/' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  (yyval.exp).dsl = DSL_STA;
                  (yyval.exp).num = (yyvsp[-2].exp).num / (yyvsp[0].exp).num;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_DIV);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4565 "dilpar.tab.cpp"
    break;

  case 155: /* dilterm: dilterm DILSE_LAND dilfactor  */
#line 2015 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               (yyval.exp).dsl = DSL_STA;
               (yyval.exp).num = (yyvsp[-2].exp).num && (yyvsp[0].exp).num;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_LAND);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4592 "dilpar.tab.cpp"
    break;

  case 156: /* dilterm: dilterm DILSE_AND dilfactor  */
#line 2038 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               (yyval.exp).dsl = DSL_STA;
               (yyval.exp).num = (yyvsp[-2].exp).num & (yyvsp[0].exp).num;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_AND);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4619 "dilpar.tab.cpp"
    break;

  case 157: /* dilterm: dilterm '%' dilfactor  */
#line 2061 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '%' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '%' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  (yyval.exp).dsl = DSL_STA;
                  (yyval.exp).num = (yyvsp[-2].exp).num % (yyvsp[0].exp).num;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_MOD);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4650 "dilpar.tab.cpp"
    break;

  case 158: /* dilterm: dilterm DILSE_IN dilfactor  */
#line 2088 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_SP)
               fatal("Arg 1 of 'in' not string");
            else if (((yyvsp[0].exp).typ != DILV_EDP) &&
                     ((yyvsp[0].exp).typ != DILV_SLP) &&
                      ((yyvsp[0].exp).typ != DILV_SP))
               fatal("Arg 2 of 'in' not string, string "
                     "list or extra description");
            else {
               /* Type is string in stringlist */
               /* Strings are not _yet_ static here */
               (yyval.exp).dsl = DSL_DYN;
               switch ((yyvsp[0].exp).typ) {
                  
                case DILV_SLP:
                  (yyval.exp).typ = DILV_INT;
                  break;

                case DILV_EDP:
                  (yyval.exp).typ = DILV_EDP;
                  break;

                case DILV_SP:
                  (yyval.exp).typ = DILV_INT;
                  break;
               }
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_IN);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4693 "dilpar.tab.cpp"
    break;

  case 159: /* dilfactor: '(' dilexp ')'  */
#line 2129 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-1].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[-1].exp)));
	    FREEEXP((yyvsp[-1].exp)); 
         }
#line 4704 "dilpar.tab.cpp"
    break;

  case 160: /* dilfactor: dilfun  */
#line 2136 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
	    FREEEXP((yyvsp[0].exp));
         }
#line 4715 "dilpar.tab.cpp"
    break;

  case 161: /* dilfactor: PNUM  */
#line 2143 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_STA;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).num = (yyvsp[0].num);
         }
#line 4726 "dilpar.tab.cpp"
    break;

  case 162: /* dilfactor: STRING  */
#line 2150 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
/*            if (*$1) { */
               /* Strings are now static */
               (yyval.exp).typ = DILV_SP;
               (yyval.exp).dsl = DSL_STA;
               add_ubit8(&((yyval.exp)),DILE_FS);
               add_string(&((yyval.exp)),(yyvsp[0].str));
/* WHY NOT?           } else
               fatal("empty strings not allowed for now"); */
         }
#line 4742 "dilpar.tab.cpp"
    break;

  case 163: /* dilfactor: stringlist  */
#line 2162 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if (!(yyvsp[0].str_list)[0])
               fatal("empty stringlists not allowed for now");
            else {
               /* write stringlist _NOT_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SLP;
               add_ubit8(&((yyval.exp)),DILE_FSL);
               add_stringlist(&((yyval.exp)),(yyvsp[0].str_list));
            }
         }
#line 4759 "dilpar.tab.cpp"
    break;

  case 164: /* dilfactor: DILSE_NULL  */
#line 2175 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            (yyval.exp).typ = DILV_NULL;
            (yyval.exp).dsl = DSL_STA;
            (yyval.exp).num = 0;
         }
#line 4770 "dilpar.tab.cpp"
    break;

  case 165: /* dilfactor: variable  */
#line 2182 "dilpar.ypp"
         {
	    if ((yyvsp[0].exp).dsl==DSL_FCT)
	       fatal("Illegal use of proc/func");

            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4783 "dilpar.tab.cpp"
    break;

  case 166: /* dilfactor: DILSE_NOT dilfactor  */
#line 2191 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = (yyvsp[0].exp).dsl;
            if (!(yyvsp[0].exp).dsl && (yyvsp[0].exp).typ == DILV_INT)
               (yyval.exp).num = !(yyvsp[0].exp).num;
            else {
               add_ubit8(&((yyval.exp)),DILE_NOT);
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
            }
            FREEEXP((yyvsp[0].exp));
         }
#line 4802 "dilpar.tab.cpp"
    break;

  case 167: /* dilfun: DILSE_ATOI '(' dilexp ')'  */
#line 2212 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'atoi' not string");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ATOI);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 4822 "dilpar.tab.cpp"
    break;

  case 168: /* dilfun: DILSE_DLD '(' dilexp ',' dilexp ')'  */
#line 2228 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_SP)
               fatal("Arg 1 of 'dildestroy' not string");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'dildestouy' not unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_DLD);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 4847 "dilpar.tab.cpp"
    break;

  case 169: /* dilfun: DILSE_DLF '(' dilexp ',' dilexp ')'  */
#line 2249 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_SP)
               fatal("Arg 1 of 'dildestroy' not string");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'dildestouy' not unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_DLF);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 4872 "dilpar.tab.cpp"
    break;

  case 170: /* dilfun: DILSE_LEN '(' dilexp ')'  */
#line 2270 "dilpar.ypp"
         {
	    INITEXP((yyval.exp));
            if (((yyvsp[-1].exp).typ != DILV_SP) && ((yyvsp[-1].exp).typ != DILV_SLP))
               fatal("Arg 1 of 'length' not string or stringlist");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_LEN);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 4892 "dilpar.tab.cpp"
    break;

  case 171: /* dilfun: DILSE_ITOA '(' dilexp ')'  */
#line 2286 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 1 of 'itoa' not integer");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ITOA);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 4912 "dilpar.tab.cpp"
    break;

  case 172: /* dilfun: DILSI_ACT '(' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ')'  */
#line 2302 "dilpar.ypp"
         {
	    /* Fri Jan 24 12:44:22 PST 1997   HHS:
	       This function writes act() as a function that
	       returns a string in an expression
	       */
	    INITEXP((yyval.exp))
	    checkbool("argument 2 of act",(yyvsp[-9].exp).boolean);
	    checkbool("argument 6 of act",(yyvsp[-1].exp).boolean);
            if ((yyvsp[-11].exp).typ != DILV_SP)
               fatal("Arg 1 of 'act' not a string");
            else if ((yyvsp[-9].exp).typ != DILV_INT)
               fatal("Arg 2 of 'act' not an integer");
            else if ((yyvsp[-7].exp).typ != DILV_NULL &&
                (yyvsp[-7].exp).typ != DILV_SP &&
                (yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 3 of 'act' not a unit or string");
            else if ((yyvsp[-5].exp).typ != DILV_NULL &&
                (yyvsp[-5].exp).typ != DILV_SP &&
                (yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 4 of 'act' not a unit or string");
            else if ((yyvsp[-3].exp).typ != DILV_NULL &&
                (yyvsp[-3].exp).typ != DILV_SP &&
                (yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 5 of 'act' not a unit or string");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 6 of 'act' not an integer");
            else {
	       /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;

	       make_code(&((yyvsp[-11].exp)));
	       make_code(&((yyvsp[-9].exp)));
	       make_code(&((yyvsp[-7].exp)));
	       make_code(&((yyvsp[-5].exp)));
	       make_code(&((yyvsp[-3].exp)));
	       make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)),DILE_ACT);
	       add_code(&((yyval.exp)),&((yyvsp[-11].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
         }
#line 4964 "dilpar.tab.cpp"
    break;

  case 173: /* dilfun: DILSE_RND '(' dilexp ',' dilexp ')'  */
#line 2350 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 1 of 'rnd' not integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'rnd' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_RND);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 4989 "dilpar.tab.cpp"
    break;

  case 174: /* dilfun: DILSE_ISS '(' dilexp ',' dilexp ')'  */
#line 2371 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 1 of 'isset' not integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'isset' not integer");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ISS);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5016 "dilpar.tab.cpp"
    break;

  case 175: /* dilfun: DILSE_ISA '(' dilexp ',' dilexp ')'  */
#line 2394 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'isaff' not a unit");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'isaff' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ISA);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5042 "dilpar.tab.cpp"
    break;

  case 176: /* dilfun: DILSE_FNDU '(' dilexp ',' dilexp ',' dilexp ',' dilexp ')'  */
#line 2416 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    checkbool("argument 3 of findunit",(yyvsp[-3].exp).boolean);
            if ((yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 1 of 'findunit' not a unit");
            else if ((yyvsp[-5].exp).typ != DILV_SP)
               fatal("Arg 2 of 'findunit' not a string");
            else if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 3 of 'findunit' not integer");
            else if ((yyvsp[-1].exp).typ != DILV_UP &&
                (yyvsp[-1].exp).typ != DILV_NULL)
               fatal("Arg 4 of 'findunit' not unit");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDU);
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5079 "dilpar.tab.cpp"
    break;

  case 177: /* dilfun: DILSE_FNDRU '(' dilexp ',' dilexp ',' dilexp ')'  */
#line 2449 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 1 of 'findrndunit' not an unitptr.");
            else if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 2 of 'findrndunit' not an integer.");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 3 of 'findrndunit' not an integer");
            else
	    {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDRU);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5111 "dilpar.tab.cpp"
    break;

  case 178: /* dilfun: DILSE_FNDR '(' dilexp ')'  */
#line 2477 "dilpar.ypp"
         {   
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'findroom' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDR);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
	    FREEEXP((yyvsp[-1].exp));
         }
#line 5131 "dilpar.tab.cpp"
    break;

  case 179: /* dilfun: DILSE_FNDS '(' dilexp ')'  */
#line 2493 "dilpar.ypp"
         {   
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'findsymbolic' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5151 "dilpar.tab.cpp"
    break;

  case 180: /* dilfun: DILSE_FNDS '(' dilexp ',' dilexp ',' dilexp ')'  */
#line 2509 "dilpar.ypp"
         {   
            INITEXP((yyval.exp));
            if ((yyvsp[-5].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'findsymbolic' not a unitptr");
	    else if ((yyvsp[-3].exp).typ != DILV_SP)
	      fatal("Arg 2 of 'findsymbolic' not a string");
	    else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 3 of 'findsymbolic' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDS2);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5181 "dilpar.tab.cpp"
    break;

  case 181: /* dilfun: DILSE_VISI '(' dilexp ',' dilexp ')'  */
#line 2535 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'visible' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'visible' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_VISI);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5207 "dilpar.tab.cpp"
    break;

  case 182: /* dilfun: DILSE_PCK '(' dilexp ',' dilexp ')'  */
#line 2557 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'paycheck' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'paycheck' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_PCK);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5233 "dilpar.tab.cpp"
    break;

  case 183: /* dilfun: DILSE_OPPO '(' dilexp ',' dilexp ')'  */
#line 2579 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'opponent' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'opponent' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_OPPO);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5259 "dilpar.tab.cpp"
    break;

  case 184: /* dilfun: DILSE_SPLX '(' dilexp ')'  */
#line 2601 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'spellindex' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_SPLX);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5279 "dilpar.tab.cpp"
    break;

  case 185: /* dilfun: DILSE_SPLI '(' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ')'  */
#line 2618 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;

            if ((yyvsp[-15].exp).typ != DILV_INT)
	      fatal("Arg 1 of 'spellinfo' not an integer");
            else if ((yyvsp[-13].exp).typ != DILV_INT || (yyvsp[-13].exp).dsl != DSL_LFT)
	      fatal("Arg 2 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-11].exp).typ != DILV_INT || (yyvsp[-11].exp).dsl != DSL_LFT)
	      fatal("Arg 3 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-9].exp).typ != DILV_INT || (yyvsp[-9].exp).dsl != DSL_LFT)
	      fatal("Arg 4 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-7].exp).typ != DILV_INT || (yyvsp[-7].exp).dsl != DSL_LFT)
	      fatal("Arg 5 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-5].exp).typ != DILV_INT || (yyvsp[-5].exp).dsl != DSL_LFT)
	      fatal("Arg 6 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-3].exp).typ != DILV_INT || (yyvsp[-3].exp).dsl != DSL_LFT)
	      fatal("Arg 7 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-1].exp).typ != DILV_INT || (yyvsp[-1].exp).dsl != DSL_LFT)
	      fatal("Arg 8 of 'spellinfo' not an integer variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;

               make_code(&((yyvsp[-15].exp)));
               make_code(&((yyvsp[-13].exp)));
               make_code(&((yyvsp[-11].exp)));
               make_code(&((yyvsp[-9].exp)));
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_SPLI);

               add_code(&((yyval.exp)),&((yyvsp[-15].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-13].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-11].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-15].exp));
            FREEEXP((yyvsp[-13].exp));
            FREEEXP((yyvsp[-11].exp));
            FREEEXP((yyvsp[-9].exp));
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5339 "dilpar.tab.cpp"
    break;

  case 186: /* dilfun: DILSE_MONS '(' dilexp ',' dilexp ')'  */
#line 2674 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 1 of 'money_string' not an integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'money_string' not a boolean");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_MONS);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5364 "dilpar.tab.cpp"
    break;

  case 187: /* dilfun: DILSE_PATH '(' dilexp ',' dilexp ')'  */
#line 2695 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'pathto' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'pathto' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_PATH);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5389 "dilpar.tab.cpp"
    break;

  case 188: /* dilfun: DILSE_CARY '(' dilexp ',' dilexp ',' dilexp ')'  */
#line 2716 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 1 of 'can_carry' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 2 of 'can_carry' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
 	       fatal("Arg 3 of 'can_carry' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_CARY);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
	 }
#line 5419 "dilpar.tab.cpp"
    break;

  case 189: /* dilfun: DILSE_PURS '(' dilexp ',' dilexp ')'  */
#line 2742 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'purse' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 2 of 'purse' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_PURS);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5445 "dilpar.tab.cpp"
    break;

  case 190: /* dilfun: DILSE_TRMO '(' dilexp ',' dilexp ',' dilexp ')'  */
#line 2764 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if (((yyvsp[-5].exp).typ != DILV_UP) && ((yyvsp[-5].exp).typ != DILV_NULL))
               fatal("Arg 1 of 'transfermoney' not a unitptr");
            else if (((yyvsp[-3].exp).typ != DILV_UP) && ((yyvsp[-3].exp).typ != DILV_NULL))
               fatal("Arg 2 of 'transfermoney' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 3 of 'transfermoney' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_TRMO);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5476 "dilpar.tab.cpp"
    break;

  case 191: /* dilfun: DILSE_LOAD '(' dilexp ')'  */
#line 2791 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'load' not name string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_LOAD);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5496 "dilpar.tab.cpp"
    break;

  case 192: /* dilfun: DILSE_ATSP '(' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ')'  */
#line 2808 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    checkbool("argument 1 of attack_spell", (yyvsp[-9].exp).boolean);

            if ((yyvsp[-9].exp).typ != DILV_INT)
               fatal("Arg 1 of 'attack_spell' not a number");
            else if ((yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 2 of 'attack_spell' not a unitptr");
            else if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 3 of 'attack_spell' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 4 of 'attack_spell' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 5 of 'attack_spell' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-9].exp)));
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_ATSP);

               add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-9].exp));
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5542 "dilpar.tab.cpp"
    break;

  case 193: /* dilfun: DILSE_CST '(' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ')'  */
#line 2851 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    checkbool("argument 1 of cast",(yyvsp[-9].exp).boolean);

            if ((yyvsp[-9].exp).typ != DILV_INT)
               fatal("Arg 1 of 'cast_spell' not a number");
            else if ((yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 2 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 3 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 4 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 5 of 'cast_spell' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-9].exp)));
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_CAST2);

               add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-9].exp));
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5588 "dilpar.tab.cpp"
    break;

  case 194: /* dilfun: DILSE_FIT '(' dilexp ',' dilexp ',' dilexp ')'  */
#line 2893 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-5].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'fits' not an unitptr.");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
	      fatal("Arg 2 of 'fits' not an unitptr.");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 3 of 'fits' not an integer.");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;

               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_FIT);

               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5623 "dilpar.tab.cpp"
    break;

  case 195: /* dilfun: DILSE_REST '(' dilexp ',' dilexp ')'  */
#line 2924 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_SP)
	      fatal("Arg 1 of 'restore' not a string");
            else if ((yyvsp[-1].exp).typ != DILV_SP)
	      fatal("Arg 2 of 'restore' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;

               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_REST);

               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5653 "dilpar.tab.cpp"
    break;

  case 196: /* dilfun: DILSE_OPRO '(' dilexp ',' dilexp ')'  */
#line 2950 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_INT)
	      fatal("Arg 1 of 'openroll' not an integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 2 of 'openroll' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_OPRO);

               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5683 "dilpar.tab.cpp"
    break;

  case 197: /* dilfun: DILSE_EQPM '(' dilexp ',' dilexp ')'  */
#line 2976 "dilpar.ypp"
         {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'equipment' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 2 of 'equipment' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;

               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_EQPM);

               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5713 "dilpar.tab.cpp"
    break;

  case 198: /* dilfun: DILSE_MEL '(' dilexp ',' dilexp ',' dilexp ',' dilexp ')'  */
#line 3002 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    checkbool("argument 1 of MeleeAttack",(yyvsp[-7].exp).boolean);

            if ((yyvsp[-7].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'MeleeAttack' not a unitptr");
            else if ((yyvsp[-5].exp).typ != DILV_UP)
	      fatal("Arg 2 of 'MeleeAttack' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_INT)
	      fatal("Arg 3 of 'MeleeAttack' not an integer");	      
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 4 of 'MeleeAttack' not an integer");	      
            else
	    {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_MEL);

               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5755 "dilpar.tab.cpp"
    break;

  case 199: /* dilfun: DILSE_TXF '(' dilexp ')'  */
#line 3040 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'textformat' not string variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               add_ubit8(&((yyval.exp)),DILE_TXF);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5774 "dilpar.tab.cpp"
    break;

  case 200: /* dilfun: DILSE_AST '(' dilexp ')'  */
#line 3055 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 1 of 'asctime' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               add_ubit8(&((yyval.exp)),DILE_AST);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5793 "dilpar.tab.cpp"
    break;

  case 201: /* dilfun: DILSE_GETW '(' dilexp ')'  */
#line 3070 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'getword' not string variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               add_ubit8(&((yyval.exp)),DILE_GETW);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5812 "dilpar.tab.cpp"
    break;

  case 202: /* dilfun: DILSE_GETWS '(' dilexp ')'  */
#line 3085 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'getwords' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SLP;
               add_ubit8(&((yyval.exp)),DILE_GETWS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5831 "dilpar.tab.cpp"
    break;

  case 203: /* dilfun: DILSE_CMDS '(' dilexp ')'  */
#line 3100 "dilpar.ypp"
         {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-1].exp).typ == DILV_SP)
            {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_CMDS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            } else if ((yyvsp[-1].exp).typ == DILV_INT) {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_CMDS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            } else
                fatal("Arg 1 of 'command' not a string or an integer");
            FREEEXP((yyvsp[-1].exp));
         }
#line 5860 "dilpar.tab.cpp"
    break;

  case 204: /* stringlist: '{' strings '}'  */
#line 3127 "dilpar.ypp"
         {
            int i;

            CREATE((yyval.str_list), char *, (str_top + 2));
            for (i = 0; i <= str_top; i++)
               (yyval.str_list)[i] = str_list[i];
            (yyval.str_list)[i] = 0;
         }
#line 5873 "dilpar.tab.cpp"
    break;

  case 205: /* strings: STRING  */
#line 3138 "dilpar.ypp"
         {
            str_list[str_top = 0] = (yyvsp[0].str);
         }
#line 5881 "dilpar.tab.cpp"
    break;

  case 206: /* strings: strings ',' STRING  */
#line 3142 "dilpar.ypp"
         {
            str_list[++str_top] = (yyvsp[0].str);
	    if (str_top > sizeof(str_list) - 3)
	      fatal("Static stringlist has too many elements.");
         }
#line 5891 "dilpar.tab.cpp"
    break;

  case 207: /* coreexp: dilexp  */
#line 3154 "dilpar.ypp"
         {
            int i;

/*          fprintf(stderr,"COREEXP\n");*/
	    if (((yyvsp[0].exp).typ==DILV_PROC)||((yyvsp[0].exp).typ==DILV_FUNC))
	      fatal("Illegal use of proc/func");

            make_code(&((yyvsp[0].exp)));
	    (yyval.ins).boolean=(yyvsp[0].exp).boolean;
            /* write dynamic expression in core */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore((yyvsp[0].exp).codep - (yyvsp[0].exp).code);
            for(i=0;i<(yyvsp[0].exp).codep-(yyvsp[0].exp).code;i++,wcore++)
               *wcore=(yyvsp[0].exp).code[i];
            (yyval.ins).lst = wcore - tmpl.core;
            (yyval.ins).dsl = (yyvsp[0].exp).dsl;
            (yyval.ins).typ = (yyvsp[0].exp).typ;
            FREEEXP((yyvsp[0].exp));
         }
#line 5915 "dilpar.tab.cpp"
    break;

  case 208: /* corevar: variable  */
#line 3180 "dilpar.ypp"
         {
            int i;
/*          fprintf(stderr,"COREVAR\n");*/
	    if (((yyvsp[0].exp).dsl != DSL_LFT)&&
		((yyvsp[0].exp).dsl != DSL_FCT)&&
		((yyvsp[0].exp).dsl != DSL_SFT))
	      fatal("Illegal lvalue");
            make_code(&((yyvsp[0].exp))); /* does nothing!? */
	    (yyval.ins).boolean=(yyvsp[0].exp).boolean;
            /* write dynamic expression in core */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore((yyvsp[0].exp).codep-(yyvsp[0].exp).code);
            for(i=0;i<(yyvsp[0].exp).codep-(yyvsp[0].exp).code;i++,wcore++)
               *wcore=(yyvsp[0].exp).code[i];
            (yyval.ins).lst = wcore - tmpl.core;
	    /* fprintf(stderr, "Refnum supposedly : %d, %d == %d, %d\n",
		    $1.code[0], $1.code[1], *(wcore-2), *(wcore-1));
	    fprintf(stderr, "Corevar : fst %p, lst %p.\n",
		    $$.fst, $$.lst);*/
            (yyval.ins).dsl = (yyvsp[0].exp).dsl;
            (yyval.ins).typ = (yyvsp[0].exp).typ;
            FREEEXP((yyvsp[0].exp));
         }
#line 5943 "dilpar.tab.cpp"
    break;

  case 209: /* ihold: %empty  */
#line 3210 "dilpar.ypp"
         {
            (yyval.num) = wcore - tmpl.core;
            wcore++;      /* ubit8 */
         }
#line 5952 "dilpar.tab.cpp"
    break;

  case 210: /* iunhold: %empty  */
#line 3217 "dilpar.ypp"
         {
            wcore--;      /* ubit8 */
            (yyval.num) = wcore - tmpl.core;
	 }
#line 5961 "dilpar.tab.cpp"
    break;

  case 211: /* ahold: %empty  */
#line 3223 "dilpar.ypp"
         {
            (yyval.num) = wcore - tmpl.core;
            wcore+=4;   /* ubit32 */
         }
#line 5970 "dilpar.tab.cpp"
    break;

  case 212: /* label: SYMBOL  */
#line 3230 "dilpar.ypp"
         {
            /* lable reference */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore(4);
            bwrite_ubit32(&wcore,get_label((yyvsp[0].sym),wcore-tmpl.core)); /* here */
            (yyval.ins).lst = wcore - tmpl.core;
         }
#line 5982 "dilpar.tab.cpp"
    break;

  case 213: /* labelskip: label  */
#line 3240 "dilpar.ypp"
         {
            (yyval.ins) = (yyvsp[0].ins);
         }
#line 5990 "dilpar.tab.cpp"
    break;

  case 214: /* labelskip: DILSE_SKIP  */
#line 3244 "dilpar.ypp"
         {
            /* 'skip' reference */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore(4);
            bwrite_ubit32(&wcore,SKIP);  /* address null value */
            (yyval.ins).lst = wcore - tmpl.core;
         }
#line 6002 "dilpar.tab.cpp"
    break;

  case 215: /* labellist: labelskip ',' labellist  */
#line 3254 "dilpar.ypp"
         {
           (yyval.ins).typ=(yyvsp[0].ins).typ+1;
	   (yyval.ins).fst=(yyvsp[-2].ins).fst;
	   (yyval.ins).lst=(yyvsp[0].ins).lst;
         }
#line 6012 "dilpar.tab.cpp"
    break;

  case 216: /* labellist: labelskip  */
#line 3260 "dilpar.ypp"
         {
           (yyval.ins).typ=1;
	   (yyval.ins).fst=(yyvsp[0].ins).fst;
	   (yyval.ins).lst=(yyvsp[0].ins).lst;
         }
#line 6022 "dilpar.tab.cpp"
    break;

  case 217: /* dilproc: ihold proccall  */
#line 3268 "dilpar.ypp"
         {
            char nbuf[255],zbuf[255],buf[255];
            int i;

            /* ignore returning variable ... */
	    if ((yyvsp[0].ins).typ == DILV_SP) {

	       /*
		* Write remote symbolic procedure call
		*
		* <rspcall> <var exp> <nargs> <arg> <arg> ...
		*/

	       wtmp = &tmpl.core[(yyvsp[-1].num)];
	       bwrite_ubit8(&wtmp,DILI_SPC);
	       (yyval.ins).fst = (yyvsp[-1].num);
	       (yyval.ins).lst = wcore - tmpl.core;
	    } else {
	       /*
		* Write remote normal procedure call
		* 
		* <rpcall> <funcnumber> <arg> <arg> ...
		*/

	       if (((yyvsp[0].ins).dsl != DSL_FCT) ||
		   ((yyvsp[0].ins).typ != DILV_NULL))
		 fatal ("refernce not a procedure");

	       wtmp = &tmpl.core[(yyvsp[-1].num)];
	       bwrite_ubit8(&wtmp,DILI_RPC);

	       (yyval.ins).fst = (yyvsp[-1].num);
	       (yyval.ins).lst = wcore - tmpl.core;
	    }
         }
#line 6062 "dilpar.tab.cpp"
    break;

  case 218: /* dilproc: DILSI_CLI ihold '(' coreexp ')'  */
#line 3304 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 1 of 'clear' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_CLI);
            }
         }
#line 6077 "dilpar.tab.cpp"
    break;

  case 219: /* dilproc: DILSI_STOR ihold '(' coreexp ')'  */
#line 3315 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'store' not an unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_STOR);
            }
         }
#line 6092 "dilpar.tab.cpp"
    break;

  case 220: /* dilproc: DILSI_AMOD ihold '(' coreexp ',' coreexp ')'  */
#line 3326 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'acc_modify' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'acc_modify' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_AMOD);
            }
         }
#line 6109 "dilpar.tab.cpp"
    break;

  case 221: /* dilproc: DILSI_FOLO ihold '(' coreexp ',' coreexp ')'  */
#line 3339 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'follow' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'follow' not an unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_FOLO);
            }
         }
#line 6126 "dilpar.tab.cpp"
    break;

  case 222: /* dilproc: DILSI_SETF ihold '(' coreexp ',' coreexp ')'  */
#line 3352 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'set_fighting' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'set_fighting' not an unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SETF);
            }
         }
#line 6143 "dilpar.tab.cpp"
    break;

  case 223: /* dilproc: DILSI_CHAS ihold '(' coreexp ',' coreexp ')'  */
#line 3365 "dilpar.ypp"
         {
	    checkbool("argument 2 of change_speed",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'change_speed' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'change_speed' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_CHAS);
            }
         }
#line 6161 "dilpar.tab.cpp"
    break;

  case 224: /* dilproc: DILSI_SBT ihold '(' coreexp ',' coreexp ')'  */
#line 3379 "dilpar.ypp"
         {
	    checkbool("argument 2 of setbright",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'setbright' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'setbright' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SBT);
            }
         }
#line 6179 "dilpar.tab.cpp"
    break;

  case 225: /* dilproc: DILSI_SWT ihold '(' coreexp ',' coreexp ')'  */
#line 3393 "dilpar.ypp"
         {
	    checkbool("argument 2 of setweight",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'setweight' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'setweight' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SWT);
            }
         }
#line 6197 "dilpar.tab.cpp"
    break;

  case 226: /* dilproc: DILSI_SET ihold '(' coreexp ',' coreexp ')'  */
#line 3407 "dilpar.ypp"
         {
	    checkbool("argument 2 of set",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT || (yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'set' not an integer variable");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'set' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SET);
            }
         }
#line 6215 "dilpar.tab.cpp"
    break;

  case 227: /* dilproc: DILSI_DLC ihold '(' coreexp ',' coreexp ')'  */
#line 3421 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'dilcopy' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'dilcopy' not a unitptr");
            else
	    {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_DLC);
            }
         }
#line 6233 "dilpar.tab.cpp"
    break;

  case 228: /* dilproc: DILSI_SETE ihold '(' coreexp ',' coreexp ')'  */
#line 3435 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendtext' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'sendtext' not a unitptr");
            else
	    {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SETE);
            }
         }
#line 6251 "dilpar.tab.cpp"
    break;

  case 229: /* dilproc: DILSI_UST ihold '(' coreexp ',' coreexp ')'  */
#line 3449 "dilpar.ypp"
         {
	    checkbool("argument 2 of unset",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT || (yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'unset' not an integer variable");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'unset' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_USET);
            }
         }
#line 6269 "dilpar.tab.cpp"
    break;

  case 230: /* dilproc: DILSI_ADE ihold '(' coreexp ',' coreexp ',' coreexp ')'  */
#line 3463 "dilpar.ypp"
         {
	    if (((yyvsp[-5].ins).typ != DILV_EDP) || ((yyvsp[-5].ins).dsl != DSL_DYN))
               fatal("Arg 1 of 'addextra' not a unit extra description");
            else if ((yyvsp[-3].ins).typ != DILV_SLP)
               fatal("Arg 2 of 'addextra' not a stringlist");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 3 of 'addextra' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-7].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-7].num)];
               bwrite_ubit8(&wtmp,DILI_ADE);
            }
         }
#line 6288 "dilpar.tab.cpp"
    break;

  case 231: /* dilproc: DILSI_LCRI ihold '(' coreexp ',' coreexp ',' coreexp ')'  */
#line 3478 "dilpar.ypp"
         {
            if ((yyvsp[-5].ins).typ != DILV_UP)
               fatal("Arg 1 of 'logcrime' not an unitptr");
            else if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 2 of 'logcrime' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 3 of 'logcrime' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-7].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-7].num)];
               bwrite_ubit8(&wtmp,DILI_LCRI);
            }
         }
#line 6307 "dilpar.tab.cpp"
    break;

  case 232: /* dilproc: DILSE_CST ihold '(' coreexp ',' coreexp ',' coreexp ',' coreexp ')'  */
#line 3493 "dilpar.ypp"
         {
	    checkbool("argument 1 of cast",(yyvsp[-7].ins).boolean);
            if ((yyvsp[-7].ins).typ != DILV_INT)
               fatal("Arg 1 of 'cast_spell' not a number");
            else if ((yyvsp[-5].ins).typ != DILV_UP)
               fatal("Arg 2 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 3 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 4 of 'cast_spell' not a unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-9].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-9].num)];
               bwrite_ubit8(&wtmp,DILI_CAST);
            }
         }
#line 6329 "dilpar.tab.cpp"
    break;

  case 233: /* dilproc: DILSI_ADL ihold '(' coreexp ',' coreexp ')'  */
#line 3511 "dilpar.ypp"
         {
            if (((yyvsp[-3].ins).typ != DILV_SLP) || ((yyvsp[-3].ins).dsl != DSL_LFT))
               fatal("Arg 1 of 'addlist' not a stringlist variable");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'addlist' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_ADL);
            }
         }
#line 6346 "dilpar.tab.cpp"
    break;

  case 234: /* dilproc: DILSI_SUE ihold '(' coreexp ',' coreexp ')'  */
#line 3524 "dilpar.ypp"
         {
	   /*fprintf(stderr, "SUE DSL %d TYP %d\n", $4.dsl, $4.typ);*/

	    if (((yyvsp[-3].ins).typ != DILV_EDP) || ((yyvsp[-3].ins).dsl != DSL_DYN))
               fatal("Arg 1 of 'subextra' not a unit extra description");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'subextra' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SUE);
            }
         }
#line 6365 "dilpar.tab.cpp"
    break;

  case 235: /* dilproc: DILSI_SUL ihold '(' coreexp ',' coreexp ')'  */
#line 3539 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SLP || (yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'sublist' not a stringlist variable");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'sublist' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SUL);
            }
         }
#line 6382 "dilpar.tab.cpp"
    break;

  case 236: /* dilproc: DILSI_SUA ihold '(' coreexp ',' coreexp ')'  */
#line 3552 "dilpar.ypp"
         {
	    checkbool("argument 2 of subaff",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'subaff' not a unit");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'subaff' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SUA);
            }
         }
#line 6400 "dilpar.tab.cpp"
    break;

  case 237: /* dilproc: DILSI_ADA ihold '(' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ')'  */
#line 3569 "dilpar.ypp"
         {
            if ((yyvsp[-21].ins).typ != DILV_UP)
               fatal("Arg 1 of 'addaff' not a unit");
            else if ((yyvsp[-19].ins).typ != DILV_INT)
               fatal("Arg 2 of 'addaff' not an integer");
            else if ((yyvsp[-17].ins).typ != DILV_INT)
               fatal("Arg 3 of 'addaff' not an integer");
            else if ((yyvsp[-15].ins).typ != DILV_INT)
               fatal("Arg 4 of 'addaff' not an integer");
            else if ((yyvsp[-13].ins).typ != DILV_INT)
               fatal("Arg 5 of 'addaff' not an integer");
            else if ((yyvsp[-11].ins).typ != DILV_INT)
               fatal("Arg 6 of 'addaff' not an integer");
            else if ((yyvsp[-9].ins).typ != DILV_INT)
               fatal("Arg 7 of 'addaff' not an integer");
            else if ((yyvsp[-7].ins).typ != DILV_INT)
               fatal("Arg 8 of 'addaff' not an integer");
            else if ((yyvsp[-5].ins).typ != DILV_INT)
               fatal("Arg 9 of 'addaff' not an integer");
            else if ((yyvsp[-3].ins).typ != DILV_INT)
               fatal("Arg 10 of 'addaff' not an integer");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 11 of 'addaff' not an integer");
            else   {
               (yyval.ins).fst = (yyvsp[-23].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-23].num)];
               bwrite_ubit8(&wtmp,DILI_ADA);
            }
         }
#line 6435 "dilpar.tab.cpp"
    break;

  case 238: /* dilproc: DILSI_DST ihold '(' coreexp ')'  */
#line 3600 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'destruct' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_DST);
            }
         }
#line 6450 "dilpar.tab.cpp"
    break;

  case 239: /* dilproc: DILSI_LOG ihold '(' coreexp ')'  */
#line 3611 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 1 of 'log' not a string.");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_LOG);
            }
         }
#line 6465 "dilpar.tab.cpp"
    break;

  case 240: /* dilproc: DILSI_PUP ihold '(' coreexp ')'  */
#line 3622 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'position_update' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_PUP);
            }
         }
#line 6480 "dilpar.tab.cpp"
    break;

  case 241: /* dilproc: DILSI_LNK ihold '(' coreexp ',' coreexp ')'  */
#line 3633 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'link' not a unit");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'link' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_LNK);
            }
         }
#line 6497 "dilpar.tab.cpp"
    break;

  case 242: /* dilproc: DILSI_EXP ihold '(' coreexp ',' coreexp ')'  */
#line 3646 "dilpar.ypp"
         {
	    checkbool("argument 1 of experience",(yyvsp[-3].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT)
               fatal("Arg 1 of 'experience' not an integer");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'experience' not a unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_EXP);
            }
         }
#line 6515 "dilpar.tab.cpp"
    break;

  case 243: /* dilproc: DILSI_ACT ihold '(' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ')'  */
#line 3660 "dilpar.ypp"
         {
	    checkbool("argument 2 of act",(yyvsp[-9].ins).boolean);
	    checkbool("argument 6 of act",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-11].ins).typ != DILV_SP)
               fatal("Arg 1 of 'act' not a string");
            else if ((yyvsp[-9].ins).typ != DILV_INT)
               fatal("Arg 2 of 'act' not an integer");
            else if ((yyvsp[-7].ins).typ != DILV_NULL &&
                (yyvsp[-7].ins).typ != DILV_SP &&
                (yyvsp[-7].ins).typ != DILV_UP)
               fatal("Arg 3 of 'act' not a unit or string");
            else if ((yyvsp[-5].ins).typ != DILV_NULL &&
                (yyvsp[-5].ins).typ != DILV_SP &&
                (yyvsp[-5].ins).typ != DILV_UP)
               fatal("Arg 4 of 'act' not a unit or string");
            else if ((yyvsp[-3].ins).typ != DILV_NULL &&
                (yyvsp[-3].ins).typ != DILV_SP &&
                (yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 5 of 'act' not a unit or string");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 6 of 'act' not an integer");
            else {
               /* make code */
               (yyval.ins).fst = (yyvsp[-13].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-13].num)];
               bwrite_ubit8(&wtmp,DILI_ACT);
            }
         }
#line 6549 "dilpar.tab.cpp"
    break;

  case 244: /* dilproc: DILSI_EXE ihold '(' coreexp ',' coreexp ')'  */
#line 3690 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'exec' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'exec' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_EXEC);
            }
         }
#line 6566 "dilpar.tab.cpp"
    break;

  case 245: /* dilproc: DILSI_WIT ihold '(' coreexp ',' coreexp ')'  */
#line 3703 "dilpar.ypp"
         {
	    checkbool("argument 1 of wait",(yyvsp[-3].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT)
               fatal("Arg 1 of 'wait' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_WIT);
            }
         }
#line 6582 "dilpar.tab.cpp"
    break;

  case 246: /* dilproc: DILSI_SND ihold '(' coreexp ')'  */
#line 3715 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 1 of 'send' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_SND);
            }
         }
#line 6597 "dilpar.tab.cpp"
    break;

  case 247: /* dilproc: DILSI_SNT ihold '(' coreexp ',' coreexp ')'  */
#line 3726 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendto' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'sendto' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SNT);
            }
         }
#line 6614 "dilpar.tab.cpp"
    break;

  case 248: /* dilproc: DILSI_SNTA ihold '(' coreexp ',' coreexp ')'  */
#line 3739 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendtoall' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'sendtoall' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SNTA);
            }
         }
#line 6631 "dilpar.tab.cpp"
    break;

  case 249: /* dilproc: DILSI_SNTADIL ihold '(' coreexp ',' coreexp ')'  */
#line 3752 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendtoalldil' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'sendtoalldil' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SNTADIL);
            }
         }
#line 6648 "dilpar.tab.cpp"
    break;

  case 250: /* dilproc: DILSI_SEC ihold '(' coreexp ',' label ')'  */
#line 3765 "dilpar.ypp"
         {
            if ((yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'secure' not a user-variable.");
            else if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'secure' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SEC);
            }
         }
#line 6665 "dilpar.tab.cpp"
    break;

  case 251: /* dilproc: DILSI_USE ihold '(' coreexp ')'  */
#line 3778 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'unsecure' not a unit");
            else { 
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_USE);
            }
         }
#line 6680 "dilpar.tab.cpp"
    break;

  case 252: /* dilproc: DILSI_QUIT ihold  */
#line 3789 "dilpar.ypp"
         {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_QUIT);
         }
#line 6691 "dilpar.tab.cpp"
    break;

  case 253: /* dilproc: DILSI_BLK ihold  */
#line 3796 "dilpar.ypp"
         {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_BLK);
         }
#line 6702 "dilpar.tab.cpp"
    break;

  case 254: /* dilproc: DILSI_PRI ihold  */
#line 3803 "dilpar.ypp"
         {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_PRI);
         }
#line 6713 "dilpar.tab.cpp"
    break;

  case 255: /* dilproc: DILSI_NPR ihold  */
#line 3810 "dilpar.ypp"
         {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_NPR);
         }
#line 6724 "dilpar.tab.cpp"
    break;

  case 256: /* dilproc: DILSI_WLK ihold '(' coreexp ')'  */
#line 3817 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'walkto' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_WALK);
            }
         }
#line 6739 "dilpar.tab.cpp"
    break;

  case 257: /* dilproc: DILSI_EQP ihold '(' coreexp ',' coreexp ')'  */
#line 3828 "dilpar.ypp"
         {
	    checkbool("argument 1 of addequip",(yyvsp[-3].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'addequip' not a unit");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'addequip' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_EQP);
            }
         }
#line 6757 "dilpar.tab.cpp"
    break;

  case 258: /* dilproc: DILSI_UEQ ihold '(' coreexp ')'  */
#line 3842 "dilpar.ypp"
         {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'unequip' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_UEQ);
            }
         }
#line 6772 "dilpar.tab.cpp"
    break;

  case 259: /* pushbrk: %empty  */
#line 3859 "dilpar.ypp"
         {
            /*
             * Defines name and pushes
             * break label on break stack
             * Inserted in structure from point
             * where break is allowed.
             */
             char buf[255];
			 	
             /* create label */
             sprintf(buf,"__break%d",labelgen++);
			 	
             if (break_no) {
                /* reallocate break stack */
                RECREATE(break_idx,ubit16,break_no+1);
             } else {
                /* allocate new break stack */
                CREATE(break_idx,ubit16,break_no+1);
             }
             break_idx[break_no++]=add_label(buf,SKIP);
         }
#line 6798 "dilpar.tab.cpp"
    break;

  case 260: /* defbrk: %empty  */
#line 3882 "dilpar.ypp"
         {
             /*
              * Defines address of break label on top of stack.
              */
             if (!break_no)
                fatal("INTERNAL ERROR (break stack def)");
             else
                label_adr[break_idx[break_no-1]] = wcore-tmpl.core;
         }
#line 6812 "dilpar.tab.cpp"
    break;

  case 261: /* popbrk: %empty  */
#line 3893 "dilpar.ypp"
         {
             /*
              * Pops the current break label from the break stack.
              * Inserted in structure from point where
              * break is no longer alowed
              */
             if (!break_no)
                fatal("INTERNAL ERROR (break stack def)");
             else
                break_no--;
         }
#line 6828 "dilpar.tab.cpp"
    break;

  case 262: /* pushcnt: %empty  */
#line 3906 "dilpar.ypp"
         {
             /*
              * Defines name and pushes
              * cont label on cont stack
              * Inserted in structure from point
              * where cont is allowed.
              */
             char buf[255];
			 	
             /* create label */
             sprintf(buf,"__cont%d",labelgen++);
			 	
             if (cont_no) {
                /* reallocate cont stack */
                RECREATE(cont_idx,ubit16,cont_no+1);
             } else {
                /* allocate new cont stack */
                CREATE(cont_idx,ubit16,cont_no+1);
             }
             cont_idx[cont_no++]=add_label(buf,SKIP);		 		
         }
#line 6854 "dilpar.tab.cpp"
    break;

  case 263: /* defcnt: %empty  */
#line 3929 "dilpar.ypp"
         {
            /*
             * Defines address of cont label on top of stack.
             */
            if (!cont_no)
               fatal("INTERNAL ERROR (cont stack def)");
            else
               label_adr[cont_idx[cont_no-1]] = wcore-tmpl.core;
         }
#line 6868 "dilpar.tab.cpp"
    break;

  case 264: /* popcnt: %empty  */
#line 3940 "dilpar.ypp"
         {
            /*
             * Pops the current cont label from the cont stack.
             * Inserted in structure from point where
             * cont is no longer alowed
             */
            if (!cont_no)
               fatal("INTERNAL ERROR (cont stack def)");
            else 
               cont_no--;
         }
#line 6884 "dilpar.tab.cpp"
    break;

  case 269: /* block: '{' dilinstlist '}'  */
#line 3961 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[-1].ins); }
#line 6890 "dilpar.tab.cpp"
    break;

  case 270: /* dilinstlist: dilcomposed  */
#line 3965 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[0].ins); }
#line 6896 "dilpar.tab.cpp"
    break;

  case 271: /* dilinstlist: dilcomposed dilinstlist  */
#line 3967 "dilpar.ypp"
         { (yyval.ins).fst = (yyvsp[-1].ins).fst; (yyval.ins).lst = (yyvsp[0].ins).lst; }
#line 6902 "dilpar.tab.cpp"
    break;

  case 272: /* dilcomposed: '{' dilinstlist '}' optsemicolons  */
#line 3971 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[-2].ins); }
#line 6908 "dilpar.tab.cpp"
    break;

  case 273: /* dilcomposed: dilinst semicolons  */
#line 3973 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[-1].ins); }
#line 6914 "dilpar.tab.cpp"
    break;

  case 274: /* dilcomposed: dilcomplex  */
#line 3975 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[0].ins); }
#line 6920 "dilpar.tab.cpp"
    break;

  case 275: /* dilcomplex: DILSI_IF ihold '(' coreexp ')' ahold block ihold ahold DILSI_ELS dilcomposed  */
#line 3979 "dilpar.ypp"
         {
            wtmp = &tmpl.core[(yyvsp[-9].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-5].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).fst); /* address of else */
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* skip else */
            wtmp = &tmpl.core[(yyvsp[-2].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).lst); /* end of else */
            (yyval.ins).fst = (yyvsp[-9].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 6937 "dilpar.tab.cpp"
    break;

  case 276: /* dilcomplex: DILSI_IF ihold '(' coreexp ')' ahold dilinst optsemicolons ihold ahold DILSI_ELS dilcomposed  */
#line 3992 "dilpar.ypp"
         {
            wtmp = &tmpl.core[(yyvsp[-10].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-6].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).fst); /* address of else */
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* skip else */
            wtmp = &tmpl.core[(yyvsp[-2].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).lst); /* end of else */
            (yyval.ins).fst = (yyvsp[-10].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 6954 "dilpar.tab.cpp"
    break;

  case 277: /* dilcomplex: DILSI_IF ihold '(' coreexp ')' ahold dilcomposed  */
#line 4005 "dilpar.ypp"
         {
            wtmp = &tmpl.core[(yyvsp[-5].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-1].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).lst); /* address of else */
            (yyval.ins).fst = (yyvsp[-5].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 6967 "dilpar.tab.cpp"
    break;

  case 278: /* $@1: %empty  */
#line 4015 "dilpar.ypp"
           { in_foreach++;
            if (in_foreach>1)
	      fatal("'foreach' may not be nested!");	      
	   }
#line 6976 "dilpar.tab.cpp"
    break;

  case 279: /* $@2: %empty  */
#line 4020 "dilpar.ypp"
           { in_foreach--; }
#line 6982 "dilpar.tab.cpp"
    break;

  case 280: /* dilcomplex: DILSI_FOE ihold '(' coreexp ',' pushbrk pushcnt defcnt ihold corevar ahold ')' $@1 dilcomposed $@2 ihold ahold defbrk popbrk popcnt  */
#line 4022 "dilpar.ypp"
         {
            /*
             * allows both break and continue inside the composed
             * statement. continue starts at the next element
             * break starts with the instruction after loop.
             */

            if ((yyvsp[-16].ins).typ != DILV_INT)
	      fatal("Arg 1 of 'foreach' not an integer");
            wtmp = &tmpl.core[(yyvsp[-18].num)];
            bwrite_ubit8(&wtmp,DILI_FOE); /* foreach - clear / build */
            wtmp = &tmpl.core[(yyvsp[-11].num)];
            bwrite_ubit8(&wtmp,DILI_FON); /* foreach - next */
            wtmp = &tmpl.core[(yyvsp[-9].num)];
            bwrite_ubit32(&wtmp,wcore-tmpl.core);
            wtmp = &tmpl.core[(yyvsp[-4].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* loop */
            bwrite_ubit32(&wtmp,(yyvsp[-11].num));
            (yyval.ins).fst=(yyvsp[-18].num);
            (yyval.ins).lst=wcore-tmpl.core;
         }
#line 7008 "dilpar.tab.cpp"
    break;

  case 281: /* dilcomplex: DILSI_WHI pushbrk pushcnt defcnt ihold '(' coreexp ')' ahold dilcomposed ihold ahold defbrk popbrk popcnt  */
#line 4046 "dilpar.ypp"
         {
            /*
             * allows both break and continue inside the composed
             * statement. continue starts at the test statement.
             * break starts with the instruction after loop.
             */
         	 
            /* made with 'if' and 'goto' */
            wtmp = &tmpl.core[(yyvsp[-10].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-6].num)];
            bwrite_ubit32(&wtmp,wcore - tmpl.core); /* address of break */
            wtmp = &tmpl.core[(yyvsp[-4].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* test again */
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit32(&wtmp,(yyvsp[-10].num)); /* address of start */
            (yyval.ins).fst = (yyvsp[-10].num);
            (yyval.ins).lst = wcore-tmpl.core;
         }
#line 7032 "dilpar.tab.cpp"
    break;

  case 282: /* dilcomplex: ':' SYMBOL ':' dilcomposed  */
#line 4066 "dilpar.ypp"
         {
	    int i;

            /* register label */
            if ((i = search_block((yyvsp[-2].sym), (const char **) label_names, TRUE)) != -1)
	    {
	       if (label_adr[i] != SKIP)
		 fatal("Redefinition of label");
	       else
	       {
		  /* update label */
		  label_adr[i]= (yyvsp[0].ins).fst;
		  /* fprintf(stderr,"LABEL %s had adress %d\n",
			  label_names[i], $4.fst); */
	       }
            } else
	      add_label((yyvsp[-2].sym), (yyvsp[0].ins).fst);
            (yyval.ins) = (yyvsp[0].ins);
         }
#line 7056 "dilpar.tab.cpp"
    break;

  case 286: /* someargs: coreexp  */
#line 4094 "dilpar.ypp"
         {
            /* collect argument types */
            ref.argt[ref.argc] = (yyvsp[0].ins).typ;
            if (++ref.argc > ARGMAX)
               fatal("Too many arguments");
         }
#line 7067 "dilpar.tab.cpp"
    break;

  case 287: /* dilass: ihold corevar DILTO_ASS dilassrgt  */
#line 4103 "dilpar.ypp"
         {
	    if ((yyvsp[0].ins).dsl==DSL_FCT)
	    {
	       /* this is an assignment from a normal function */
	       int i, varnum, refnum;
	       char buf[255],nbuf[255],zbuf[255];

	       /*
		* For function calls:
		* <rfcall> <var> <var#> <funcnumber> <nargs> <arg> <arg> ...
		*/
             
	       /* fprintf(stderr,"Function call\n"); */

	       /* check if corevar is true variable */
	       if (tmpl.core[(yyvsp[-2].ins).fst]!=DILE_VAR)
		 fatal("Variable must be true local variable");
              
	       /* read variable number from core */
	       wtmp = &tmpl.core[1+((yyvsp[-2].ins).fst)];
	       varnum=bread_ubit16(&wtmp);

	       wtmp = &tmpl.core[(yyvsp[0].ins).fst];
	       refnum=bread_ubit16(&wtmp);

	       /* check func/var types */
	       switch (tmpl.vart[varnum])
	       {
		 case DILV_SP:
		 case DILV_EDP:
		 case DILV_SLP:
		 case DILV_UP:
		  //if ((tmpl.vart[varnum] != tmpl.xrefs[refnum].rtnt) &&
		  //	(tmpl.vart[varnum] != DILV_NULL))
		  if (tmpl.vart[varnum] != tmpl.xrefs[refnum].rtnt)
		    fatal("Assigning incompatible types (A)");
		  break;
		 case DILV_INT:
                  if (tmpl.vart[varnum] != tmpl.xrefs[refnum].rtnt)
		    fatal("Assigning incompatible types (B)");
		  break;
		 default:
                  fatal("Type error in function assign");
		  break;
	       }
	       wtmp = &tmpl.core[(yyvsp[-3].num)];
	       bwrite_ubit8(&wtmp,DILI_RFC);
	       (yyval.ins).fst = (yyvsp[-3].num);
	       (yyval.ins).lst = (yyvsp[0].ins).lst;
	       
	    } else if ((yyvsp[0].ins).dsl==DSL_SFT) {

	       /* this is an assignment from a symbolic function */
	       int i, varnum;
	       char buf[255],nbuf[255],zbuf[255];

	       /*
		* For symbolic function calls:
		* <rsfcall> <var> <var#> <funcnumber> <nargs> <arg> <arg> ...
		*/

	       /* fprintf(stderr,"Symbolic function call\n"); */

	       /* check if corevar is true variable */
	       if (tmpl.core[(yyvsp[-2].ins).fst]!=DILE_VAR)
		 fatal("Variable must be true local variable");
              
	       /* check func/var types impossible! */

	       wtmp = &tmpl.core[(yyvsp[-3].num)];
	       bwrite_ubit8(&wtmp,DILI_SFC);
	       (yyval.ins).fst = (yyvsp[-3].num);
	       (yyval.ins).lst = (yyvsp[0].ins).lst;	       
	       
	    } else {
	       /* this is an assignment to an expression */	 

	       /* fprintf(stderr,"Assignment\n"); */
	       switch ((yyvsp[-2].ins).typ) {
		 case DILV_SP:
		 case DILV_EDP:
		 case DILV_SLP:
		 case DILV_UP:
                  if (((yyvsp[-2].ins).typ != (yyvsp[0].ins).typ) && ((yyvsp[0].ins).typ != DILV_NULL))
		    fatal("Assigning incompatible types (C)");
                  else {
                     wtmp = &tmpl.core[(yyvsp[-3].num)];
                     bwrite_ubit8(&wtmp,DILI_ASS);
                     (yyval.ins).fst = (yyvsp[-3].num);
                     (yyval.ins).lst = (yyvsp[0].ins).lst;
                  }
		  break;
		 case DILV_INT:
                  if ((yyvsp[-2].ins).typ != (yyvsp[0].ins).typ)
		    fatal("Assigning incompatible types (D)");
                  else {
                     wtmp = &tmpl.core[(yyvsp[-3].num)];
                     bwrite_ubit8(&wtmp,DILI_ASS);
                     (yyval.ins).fst = (yyvsp[-3].num);
                     (yyval.ins).lst = (yyvsp[0].ins).lst;
                  }
		  break;
		 default:
                  fatal("Type error in assign");
		  break;
	       }
	    }
	 }
#line 7180 "dilpar.tab.cpp"
    break;

  case 288: /* proccall: corevar ihold ihold '(' arginit arglist ')'  */
#line 4214 "dilpar.ypp"
         {
	    int i, refnum;
	    char buf[256];

	    if ((yyvsp[-6].ins).dsl != DSL_FCT)
	    {
	       if ((yyvsp[-6].ins).typ == DILV_SP)
	       {
		  /* this is a symbolic proc/func call */
		  (yyval.ins).dsl=DSL_SFT;

		  (yyval.ins).fst = (yyvsp[-6].ins).fst;
		  (yyval.ins).typ = (yyvsp[-6].ins).typ;
		  wtmp=&tmpl.core[(yyvsp[-5].num)];
		  bwrite_ubit16(&wtmp,ref.argc);
		  (yyval.ins).lst = wcore-tmpl.core;
	       }
	       else
		 fatal("Report to Papi.");
	    }
	    else
	    {
	       /* this is a normal proc/func call */
	       (yyval.ins).dsl=DSL_FCT;

	       if ((yyvsp[-6].ins).dsl!=DSL_FCT)
		 fatal("reference not a procedure or function");

	       /* fprintf(stderr, "Func : fst %p, lst %p.\n",
		       $1.fst, $1.lst); */

	       /* read refnum from core */
	       wtmp = &tmpl.core[(yyvsp[-6].ins).fst];

               refnum = bread_ubit16(&wtmp);
	       /*fprintf(stderr, "refnum read %d, %p.\n",
		       refnum, &wcore[$1.fst]);*/


	       if (tmpl.xrefs[refnum].argc != ref.argc)
	       {
		  char buf[256];
		  sprintf(buf, "Illegal number of arguments %d, %d expected."
			  " Refnum %d.",
			  ref.argc, tmpl.xrefs[refnum].argc, refnum);
		  fatal(buf);
	       }

	       /* check argument types/count */
	       for (i=0;i<tmpl.xrefs[refnum].argc;i++)
	       {
		  /* argument types collected in ref */
		  switch (tmpl.xrefs[refnum].argt[i])
		  {
		    case DILV_SP:
		    case DILV_EDP:
		    case DILV_SLP:
		    case DILV_UP:
                     if ((tmpl.xrefs[refnum].argt[i] != ref.argt[i]) &&
                         (ref.argt[i] != DILV_NULL)) {
                        sprintf(buf, "Incompatible argument "
                                "types in argument %d (A)", i+1);
                        fatal(buf);
                     }
		     break;
		    case DILV_INT:
                     if (tmpl.xrefs[refnum].argt[i] != ref.argt[i]) {
                        sprintf(buf, "Incompatible argument "
                                "types in argument %d (B)", i+1);
                        fatal(buf);
		     }
		     break;
		    default:
                     fatal("Type error in argument");
		     break;
		  }
	       }

	       (yyval.ins).fst = (yyvsp[-6].ins).fst;
	       (yyval.ins).typ = (yyvsp[-6].ins).typ;
	       wtmp=&tmpl.core[(yyvsp[-5].num)];
	       bwrite_ubit16(&wtmp,ref.argc);
	       (yyval.ins).lst = wcore-tmpl.core;
	    }
	 }
#line 7270 "dilpar.tab.cpp"
    break;

  case 289: /* corefunc: DILSE_INTR ihold '(' coreexp ',' coreexp ',' labelskip ')'  */
#line 4302 "dilpar.ypp"
         {
	    checkbool("argument 1 in 'interrupt'",(yyvsp[-5].ins).boolean);
            if ((yyvsp[-5].ins).typ != DILV_INT)
               fatal("Arg 1 of 'interrupt' not number");
            else {
	       (yyval.ins).fst=(yyvsp[-7].num);
	       (yyval.ins).lst=(yyvsp[-1].ins).lst;
               /* Type is ok */
	       wtmp=&tmpl.core[(yyvsp[-7].num)];
               bwrite_ubit8(&wtmp,DILE_INTR);
	       tmpl.intrcount++;
	       frm.intrcount++;
            }
	    (yyval.ins).dsl=DSL_DYN;
	    (yyval.ins).typ=DILV_INT;
	 }
#line 7291 "dilpar.tab.cpp"
    break;

  case 290: /* corefunc: DILSI_OAC ihold ihold ahold '(' coreexp ',' labelskip ')'  */
#line 4319 "dilpar.ypp"
         {
	    ubit32 flags;

            (yyval.ins).fst = (yyvsp[-7].num);
            (yyval.ins).lst = (yyvsp[-1].ins).lst;
            /* write an interrupt instead! */
            wtmp = &tmpl.core[(yyvsp[-7].num)];
            bwrite_ubit8(&wtmp,DILE_INTR);
	    bwrite_ubit8(&wtmp,DILE_INT);
	    flags = SFB_ACTIVATE;
            bwrite_ubit32(&wtmp,flags);
	    tmpl.intrcount++;
	    frm.intrcount++;
	    (yyval.ins).dsl=DSL_DYN;
	    (yyval.ins).typ=DILV_INT;
         }
#line 7312 "dilpar.tab.cpp"
    break;

  case 291: /* dilassrgt: coreexp  */
#line 4338 "dilpar.ypp"
         {
	    (yyval.ins)=(yyvsp[0].ins);
	 }
#line 7320 "dilpar.tab.cpp"
    break;

  case 292: /* dilassrgt: proccall  */
#line 4342 "dilpar.ypp"
         {
	    (yyval.ins)=(yyvsp[0].ins);
         }
#line 7328 "dilpar.tab.cpp"
    break;

  case 294: /* dilinst: dilproc  */
#line 4349 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[0].ins); }
#line 7334 "dilpar.tab.cpp"
    break;

  case 295: /* dilinst: dilass  */
#line 4351 "dilpar.ypp"
         {  (yyval.ins) = (yyvsp[0].ins); }
#line 7340 "dilpar.tab.cpp"
    break;

  case 296: /* dilinst: corefunc  */
#line 4353 "dilpar.ypp"
         { (yyval.ins) = (yyvsp[0].ins); }
#line 7346 "dilpar.tab.cpp"
    break;

  case 297: /* dilinst: DILSI_RTS ihold  */
#line 4355 "dilpar.ypp"
         {
            if (!istemplate) {
              fatal("return only allowed in templates");
            }
            if (tmpl.rtnt != DILV_NULL) {
              fatal("no return expression expected");
	    }            	

            /* WAS: bwrite_ubit8(&wcore,DILI_RTS); */ /* the instruction */

            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_RTS); /* the instruction */

	    (yyval.ins).fst = (yyvsp[0].num);
	    (yyval.ins).lst = (yyvsp[0].num)+1;
         }
#line 7367 "dilpar.tab.cpp"
    break;

  case 298: /* dilinst: DILSI_RTS ihold '(' coreexp ')'  */
#line 4372 "dilpar.ypp"
         {
            if (!istemplate) {
              fatal("return only allowed in templates");
            }
            if (tmpl.rtnt != (yyvsp[-1].ins).typ) {
              fatal("return expression not of correct type");
	    }            	
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit8(&wtmp,DILI_RTF); /* the instruction */
	    (yyval.ins).fst = (yyvsp[-3].num); 
	    (yyval.ins).lst = (yyvsp[-1].ins).lst;
         }
#line 7384 "dilpar.tab.cpp"
    break;

  case 299: /* dilinst: DILSI_ON ihold coreexp ihold ihold DILSI_GOT labellist  */
#line 4385 "dilpar.ypp"
         {
	    if ((yyvsp[-4].ins).typ != DILV_INT)
	      fatal("Expression after 'on' does not return integer");
	    else {
	       (yyval.ins).fst=(yyvsp[-5].num);
	       (yyval.ins).lst=(yyvsp[0].ins).lst;
	       wtmp=&tmpl.core[(yyvsp[-5].num)];
	       bwrite_ubit8(&wtmp, DILI_ON);
	       wtmp=&tmpl.core[(yyvsp[-3].num)];
	       bwrite_ubit16(&wtmp, (yyvsp[0].ins).typ);
	    }
         }
#line 7401 "dilpar.tab.cpp"
    break;

  case 300: /* dilinst: DILSI_GOT ihold label  */
#line 4398 "dilpar.ypp"
         {
            wtmp = &tmpl.core[(yyvsp[-1].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO);
            (yyval.ins).fst = (yyvsp[-1].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 7412 "dilpar.tab.cpp"
    break;

  case 301: /* dilinst: DILSI_BRK ihold ahold  */
#line 4405 "dilpar.ypp"
         {
            /* break statement */
            if (!break_no) {
               /* not in loop */
               fatal("Using break outside loop");
            } else {
               wtmp = &tmpl.core[(yyvsp[-1].num)];
               bwrite_ubit8(&wtmp,DILI_GOTO);
               wtmp = &tmpl.core[(yyvsp[0].num)];
               /* register use or find break label */
               bwrite_ubit32(&wtmp,
                  get_label(label_names[break_idx[break_no-1]],(yyvsp[0].num))); 
            }
            (yyval.ins).fst = (yyvsp[-1].num);
            (yyval.ins).lst = (yyvsp[0].num)+4;
         }
#line 7433 "dilpar.tab.cpp"
    break;

  case 302: /* dilinst: DILSI_CNT ihold ahold  */
#line 4422 "dilpar.ypp"
         {
            /* continue statement */
            if (!cont_no) {
               /* not in loop */
               fatal("Using continue outside loop");
            } else {
               wtmp = &tmpl.core[(yyvsp[-1].num)];
               bwrite_ubit8(&wtmp,DILI_GOTO);
               wtmp = &tmpl.core[(yyvsp[0].num)];
               /* register use or find continue label */
               bwrite_ubit32(&wtmp,
                  get_label(label_names[cont_idx[break_no-1]],(yyvsp[0].num))); 
            }
            (yyval.ins).fst = (yyvsp[-1].num);
            (yyval.ins).lst = (yyvsp[0].num)+4;
         }
#line 7454 "dilpar.tab.cpp"
    break;


#line 7458 "dilpar.tab.cpp"

      default: break;
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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
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
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 4439 "dilpar.ypp"


void add_var(char *name, ubit16 type)
{            
   str_lower(name);

   if (search_block(name, (const char **) var_names, TRUE) != -1)
     fatal("Duplicate variable identifier");

   var_names = add_name(name, var_names);
   tmpl.vart[tmpl.varc]=type;
   frm.vars[tmpl.varc].val.integer=0;
   frm.vars[tmpl.varc].type=type;
   
   if (++tmpl.varc > VARMAX)
     fatal("Too many variables");
}

int add_label(char *name, ubit32 adr)
{
   str_lower(name);

   /* add new label here */
   label_names = add_name(name, label_names);

   if (label_no == 0)
      CREATE(label_adr, ubit32, 1);
   else
      RECREATE(label_adr, ubit32, label_no+1);

   label_adr[label_no] = adr; /* index adress */

   /*fprintf(stderr, "ADD LABEL %s (%d)\n", name, adr);*/

   return (label_no++);
}

ubit32 get_label(char *name, ubit32 adr)
{
   /* get number of referenced label */
   int i;

   if ((i = search_block(name, (const char **) label_names, TRUE)) == -1) {
      /* does not exist, register label */
     /*fprintf(stderr,"GET LABEL UNKNOWN: %s\n", name);*/
      i = add_label(name, SKIP);
   }

   if (label_adr[i] == SKIP) {
      /* register use of label before definition */
      if (label_use_no == 0) {
         CREATE(label_use_adr, ubit32, 1);
         CREATE(label_use_idx, ubit32, 1);
      } else {
         RECREATE(label_use_adr, ubit32, label_use_no+1);
         RECREATE(label_use_idx, ubit32, label_use_no+1);
      }
      /*fprintf(stderr,"GET LABEL REGISTERED LOCATION: %s %d\n", name, adr);*/
      label_use_adr[label_use_no] = adr; /* update here */
      label_use_idx[label_use_no] = i;
      label_use_no++;
      return SKIP; /* address is updated later */
   } else {
      /* just return address */
     /*fprintf(stderr,
       "GET LABEL WROTE LOCATION: %s %d\n", name, label_adr[i]);*/
      return label_adr[i];
   }
}

void update_labels(void)
{
   int i;
   char buf[255];
   ubit8 *wtmp;
   
   for(i = 0; i < label_use_no; i++)
   {
      /* Make sure ALL functions with labels have
       * the NOLAB number replaced with the label
       * address.
       */

       if (label_use_adr[i]!=SKIP)
       {
	  /*fprintf(stderr,"UPDATE LABEL %s: location %d to adress %d\n",
		  label_names[label_use_idx[i]],
		  label_use_adr[i], label_adr[label_use_idx[i]]);*/
          wtmp=&tmpl.core[label_use_adr[i]];

          if (label_adr[label_use_idx[i]] == SKIP)
	  {
             sprintf(buf,"Undefined label: %s",label_names[label_use_idx[i]]);
             fatal(buf);
          } 
          bwrite_ubit32(&wtmp, label_adr[label_use_idx[i]]);
	  if (label_adr[label_use_idx[i]] > (wcore - tmpl.core))
	  {
	     sprintf(buf, "Internal compiler error when resolving label %s.",
		   label_names[label_use_idx[i]]);
	     fatal(buf);
	  }
      }
   }
}

void syntax(const char *str)
{
   fprintf(stderr, "\n%d: %s\n    Token: '%s'\n", linenum, str, yytext);
   errcon = 1;
}

void fatal(const char *str)
{
   fprintf(stderr, "\n%d: %s\n    Token: '%s'\n", linenum, str, yytext);
   if (*tmpfname)
     remove(tmpfname);
   exit(1);
}


void warning(const char *str)
{
   fprintf(stderr, "\n%d: %s\n    Token: '%s'\n", linenum, str, yytext);
}


/* Read a string of arbitrary length, allocate space for and return it */
char *str_dup_file(FILE *fl)
{
   char tmp[MAX_STRING_LENGTH], *res;

   CREATE(res, char, 1);

   while (fgets(tmp, MAX_STRING_LENGTH, fl))
   {
      RECREATE(res, char, strlen(tmp) + strlen(res) + 1);
      strcat(res, tmp);
   }
   return res;
}

int strlstlen(char **strlst)
{
   char **slp=strlst;
   int len = 0;
   while (*slp) 
      len+=strlen(*(slp++))+1;
   len++;
   return len;
}

/* code manipulation */

/* increase core size of tmpl.core if need be, and updates wcore */
void moredilcore(ubit32 size)
{
   ubit32 p1,p2,pos;
   p1 = tmpl.coresz;
   p2 = (wcore - tmpl.core) + size;
   
   if ( p1 < p2 ) {
      pos = wcore - tmpl.core;
      RECREATE(tmpl.core, ubit8, tmpl.coresz+CODESIZE);
      tmpl.coresz+=CODESIZE;
      wcore = &tmpl.core[pos];
    }
}



/* expression manipulation */

void add_ubit8(struct exptype *dest, ubit8 d)
{
/*   fprintf(stderr, "UBIT8\n");*/
   if (dest->codep - dest->code + sizeof(ubit8) >= CODESIZE) 
      fatal("U8: Expression too large");
   bwrite_ubit8(&(dest->codep), d);
}

void add_ubit32(struct exptype *dest, ubit32 d)
{
/*   fprintf(stderr, "UBIT32\n");*/
   if (dest->codep - dest->code + sizeof(ubit32) >= CODESIZE) 
      fatal("U32: Expression too large");
   bwrite_ubit32(&(dest->codep), d);
}

void add_sbit32(struct exptype *dest, sbit32 d)
{
/*   fprintf(stderr, "SBIT32\n");*/
   if (dest->codep - dest->code + sizeof(sbit32) >= CODESIZE) 
      fatal("S32: Expression too large");
   bwrite_ubit32(&(dest->codep), (ubit32) d);
}

void add_ubit16(struct exptype *dest, ubit16 d)
{
/*   fprintf(stderr, "UBIT16\n");*/
   if (dest->codep - dest->code + sizeof(ubit16) >= CODESIZE) 
      fatal("U16: Expression too large");
   bwrite_ubit16(&(dest->codep), d);
}

void add_string(struct exptype *dest, char *d)
{
/*   fprintf(stderr, "STRING %d , %d\n",
         dest->codep - dest->code,
         strlen(d));
*/
   if (dest->codep - dest->code + strlen(d) + 1 >= CODESIZE) 
      fatal("AS: Expression too large");
   bwrite_string(&(dest->codep), d);
}

void cat_string(struct exptype *dest, char *d)
{
/*   fprintf(stderr, "STRING %d , %d\n",
         dest->codep - dest->code,
         strlen(d));
*/
   dest->codep--;  /* Overwrite trailing \0 char */
   if (dest->codep - dest->code + strlen(d) + 1 >= CODESIZE) 
      fatal("CS: Expression too large");
   bwrite_string(&(dest->codep), d);
}

void add_stringlist(struct exptype *dest, char **d)
{
/*   fprintf(stderr, "STRINGLIST\n");*/
   if (dest->codep - dest->code + strlstlen(d) + 1 >= CODESIZE) 
      fatal("ASL: Expression too large.");
   bwrite_nameblock(&(dest->codep), d);
}

void add_code(struct exptype *dest, struct exptype *src)
{
   sbit32 len = src->codep - src->code;

/*   fprintf(stderr, "ADD CODE\n");*/

   /* adds code in src to dest */
   if ((dest->codep - dest->code + len) >= CODESIZE)
      fatal("ACO: expression too large");
   memcpy(dest->codep, src->code, len);
   dest->codep += len;
}

void copy_code(struct exptype *dest, struct exptype *src)
{
   sbit32 len = src->codep - src->code;

/*   fprintf(stderr, "COPY CODE %d\n", len);*/

   /* copy code in src to dest */
   dest->codep = dest->code;
   memcpy(dest->code, src->code,len);
   dest->codep = &(dest->code[len]);
   dest->typ = src->typ;
   dest->dsl = src->dsl;
   dest->num = src->num;
}



ubit16 UpdateCRC( ubit8 c,  ubit16 crc)
{
/*

  The UpdateCRC is as follows in 8088 assembler:

  AL = Char to update with (8  bit)
  BX = CRC value to update (16 bit) (BH is MSB, BL is LSB of CRC).

  At the start of a packet, CRC should be set to 0.
  Always at the end of a packet before transmitting the CRC, do:

     CRC = UpdateCRC(0, UpdateCRC(0, CRC))

  Then transmit the CRC in MSB, LSB order.

  When receiving: The two CRC value characters should be calculated
  as part of the package. Then CRC will be zero if no error occurred.

         MOV CX, 8      ; The generator is X^16+X^12+X^5+1
LOOP1:                  ; as recommended by CCITT. An alternate
         ROL AL, 1      ; generator often used in synchronous
         RCL BX, 1      ; communication is X^16+X^15+X^2+1
         JNB SKIP       ; This may be used by substituting XOR 8005H
         XOR BX, 8005H  ; for XOR 1021H in the adjacent code.
SKIP:
         LOOP LOOP1

*/

    int i;
    int rcl_cf;
    
   for (i=0; i < 8; i++)
   {
      c = (c << 1) | ((c & 128) >> 7);
      rcl_cf = crc & 0x8000;
      crc = (crc << 1) | (c & 1);
      if (rcl_cf)     
	crc ^= 0x8005;
   }

   return crc;
}
      


int dil_headercrc(char **name, ubit8 *type)
{
   int i, j;
   ubit16 crc = 0;

   for (i=0; name[i]; i++)
   {
      crc = UpdateCRC(type[i], crc);

      for (j=0; name[i][j]; j++)
	crc = UpdateCRC(name[i][j], crc);
   }

   return crc;
}


int dil_corecrc(ubit8 *core, int len)
{
   int i, j;
   ubit16 crc = 0;

   for (i=0; i < len; i++)
     crc = UpdateCRC(core[i], crc);

   return crc;
}


void make_code(struct exptype *dest)
{
/*   fprintf(stderr, "MAKE CODE\n");*/

   if (dest->dsl == DSL_STA) {
      switch (dest->typ) {
         case DILV_INT: /* static integer */
             dest->codep = dest->code;
             dest->dsl = DSL_DYN;
             bwrite_ubit8(&(dest->codep), DILE_INT);
             bwrite_ubit32(&(dest->codep),(ubit32) dest->num);
         break;
          case DILV_NULL: /* null pointer */
             dest->codep = dest->code;
             dest->dsl = DSL_DYN;
             bwrite_ubit8(&(dest->codep),DILE_NULL);
         break;
         default: /* static other */
             dest->dsl = DSL_DYN; /* then its allready there */
         break;
      }
   }
}


void add_ref(struct dilref *ref)
{
   int i;
   char nbuf[255];


   /* register reference */
   refs[refcount] = *ref;


   if (ref->argc)
   {
      CREATE(refs[refcount].argv, char *, ref->argc);
      memcpy(refs[refcount].argv, ref->argv, ref->argc*sizeof(char *));
      CREATE(refs[refcount].argt, ubit8, ref->argc);
      memcpy(refs[refcount].argt, ref->argt, ref->argc*sizeof(ubit8));
   }
   else
   {
      refs[refcount].argv=NULL;
      refs[refcount].argt=NULL;
   }

   sprintf(nbuf,"%s@%s", ref->name, ref->zname);

   if (is_name(nbuf, (const char **) ref_names))
     fatal("Redefinition of external reference.");

   ref_names = add_name(nbuf, ref_names);
   
   if (++refcount > REFMAX)
     fatal("Too many references");
}


void dumpdiltemplate(struct diltemplate *tmpl) {

  int i;
  fprintf(stderr,"Name: '%s'\n",tmpl->prgname);
  fprintf(stderr,"Crc: %d\n",tmpl->varcrc);
  fprintf(stderr,"Return type: %d\n",tmpl->rtnt);
  fprintf(stderr,"# Arguments: %d\n",tmpl->argc);
  for (i=0;i<tmpl->argc;i++)
    fprintf(stderr,"Arg %d type: %d\n",i,tmpl->argt[i]);
  fprintf(stderr,"Coresize: %d\n",tmpl->coresz);
  fprintf(stderr,"Core: %p\n",tmpl->core);
  fprintf(stderr,"# Vars: %d\n",tmpl->varc);
  for (i=0;i<tmpl->varc;i++) 
    fprintf(stderr,"Var %d type: %d\n",i,tmpl->vart[i]);
  fprintf(stderr,"# References: %d\n",tmpl->xrefcount);
  for (i=0;i<tmpl->xrefcount;i++) {
     fprintf(stderr,"Xref %d: '%s'\n",i,tmpl->xrefs[i].name);
     fprintf(stderr,"     Return type %d\n",tmpl->xrefs[i].rtnt);
     fprintf(stderr,"     # Arguments: %d\n",tmpl->xrefs[i].argc);
  }
}

void dumpdil(struct dilprg *prg) {
  int i;

  fprintf(stderr,"Flags: %d\n",prg->flags);
  fprintf(stderr,"Varcrc: %d\n",prg->varcrc);
  fprintf(stderr,"Stacksz: %d\n",prg->stacksz);
  fprintf(stderr,"Stackptr: %d\n",(int) (1+(prg->sp - prg->stack)));
  for (i=0;i<=(prg->sp - prg->stack);i++) {
    fprintf(stderr,"Stackframe %d:\n",i);
    fprintf(stderr,"Return var# %d:\n",prg->stack[i].ret);
    dumpdiltemplate(prg->stack[i].tmpl);
  }
}

int main(int argc, char **argv)
{
   int result, argn;

   extern int linenum;

   int yyparse(void);

#ifndef DOS
   alarm(60*15);  /* If not done in five minutes, abort! */
#endif

   if ((argc < 2) || (argc > 6))
   {
      fprintf(stderr, "Usage: %s [lineno] [-v] [-t] [-z zonename] (%d)\n",
	      argv[0],argc);
      exit(1);
   }

   for (argn=1;argn<argc;argn++) {
      if (!strcmp(argv[argn],"-t"))
         istemplate = TRUE;
      else if (!strcmp(argv[argn],"-v"))
	fprintf(stderr,
		"DIL " DIL_VERSION " Copyright (C) 1995 - 1996 by "
		"Valhalla [" __DATE__ "]\n");
      else if (!strcmp(argv[argn],"-z")) {
         argn++;
         cur_zonename=argv[argn];
      }
      else
	linenum = atoi(argv[argn]);
   }

   result = yyparse();

   return(0);
}
