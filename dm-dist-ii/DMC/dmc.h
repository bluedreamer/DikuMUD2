#pragma once

#include "../utils.h"

#define VERSION "v1.30"

#define MAX_STRING 4096
#define MAX_SYMBOL 21

#define SYMSIZE 40

#define INPUT_SUFFIX   "zon"
#define OUTPUT_RSUFFIX "reset"
#define OUTPUT_WSUFFIX "data"

#ifndef L_tmpnam
   #define L_tmpnam 1024 /* Oh thank you Marcel! */
#endif

#ifdef MARCEL
int   mkstemp(char *);
char *mktemp(char *);
#endif

#ifdef CPP
   #undef CPP
#endif

/* I hate that you can't get `gcc -E' to write to stdout... /gnort */

#if defined(MARCEL)
   #define CPP "/usr/local/src/DikuII/cccp/cccp"
#elif defined(AMIGA)
   #define CPP "gnu:lib/gcc-lib/mc68020-cbm-amigados/2.6.3/cpp"
#else
   #define CPP "pp "
#endif

#ifndef CPPI
   #define CPPI "."
#endif

#ifndef ZCAT
   #define ZCAT "zcat" /* filter to use with .Z-suffixes */
#endif

#ifndef CAT
   #define CAT "cat"
#endif

struct zone_header
{
   char          *title;
   char          *name;
   unsigned short lifespan;
   unsigned char  reset_mode;
   char         **creators;
   const char    *notes;
   const char    *help;
   int            weather;
};

struct zone_info
{
   struct unit_data     *z_rooms;
   struct unit_data     *z_mobiles;
   struct unit_data     *z_objects;
   struct reset_command *z_table;
   struct zone_header    z_zone;
   diltemplate          *z_tmpl;
};

struct reset_command
{
   char                  cmd_no;
   char                 *ref1;
   char                 *ref2;
   int16_t               num1;
   int16_t               num2;
   int16_t               num3;
   char                  direction;
   char                  cmpl;
   struct reset_command *next;
};

unit_data           *mcreate_unit(int type);
room_direction_data *mcreate_exit(void);
unit_affected_type  *mcreate_affect();
void                *mmalloc(int size);
void                 mem_reset(void);
void                 mem_init(void);

void                 process_unit(struct unit_data *u);

#define MCREATE(result, type, number) ((result) = (type *)mmalloc(sizeof(type) * number))

#define UNIT_IDENT(unit) ((char *)(unit)->gnext) /* can you say 'filth'? */
                                                 /* You betcha... */
#define UNIT_IDENT_ASSIGN(unit, val) ((unit)->gnext = (struct unit_data *)(val))
