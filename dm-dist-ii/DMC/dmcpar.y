/* *********************************************************************** *
 * File   : dmcpar.y                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : quinn@diku.dk                                                  *
 *                                                                         *
 * Purpose: Database compiler. See dmc.doc for specs.                      *
 *          Source for yacc/bison.                                         *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

/* Fri Feb 25 00:25:58 MET 1994, gnort
 *   Subtle(!) change:
 *     char yytext[];  changed to
 *     char *yytext;   due to sudden change from lex to flex....
 */

/* 16-Aug-94, gnort: Introduced object-money */

/*
 * Definitions and type associations.
 */


%{
#include <stdio.h>
#include <ctype.h>

#include "dmc.h"
#include "dil.h"
#include "utils.h"
#include "textutil.h"
#include "money.h"
#include "db_file.h"

extern char *yytext; /*  MUST be changed to
		      *  `extern char yytext[]'
		      *  if you use lex instead of flex.
		      */
int yylex(void);


#define ZON_DIR_CONT 0
#define ZON_DIR_NEST 1
#define ZON_DIR_UNNEST 2

#define yyerror(dum) syntax(dum)

extern char cur_filename[];
extern int linenum;
extern struct zone_info zone;
extern int nooutput;
struct unit_data *cur;
struct extra_descr_data *cur_extra;
struct reset_command *cur_cmd;
struct unit_affected_type *cur_aff;
struct unit_fptr *cur_func;
struct diltemplate *cur_tmpl;
struct dilprg *cur_prg;

int myi, cur_ex, errcon = 0;

/* Temporary data for stringlists */
int str_top = 0; //MS2020
char *str_list[50];
char empty_ref[] = {'\0', '\0'};
int istemplate;
char **tmplnames=NULL;

struct dilargstype dilargs;
int dilarg_top;

/* Temporary data for moneylists */
int mon_top, mon_list[50][2];

struct unit_fptr *mcreate_func(void);
void dumpdiltemplate(struct diltemplate *tmpl);
void dumpdil(struct dilprg *prg);

 void syntax(const char *str);
void fatal(const char *str);
void warning(const char *str);

%}


%union
{
  char *str;
  char sym[SYMSIZE + 1];
  char **str_list;
  struct dilargtype *dilarg;
  struct dilargstype *dilargs;
  int num;
}

/* general */
%token FLAGS END UNKNOWN
%token OBJECTS ZONE RESET ROOMS MOBILES ENDMARK DIL
%token <str> DILPRG				/* for the tmp-filename */
%token '{' '}' '[' ']' ',' ';'
%token <str> STRING
%token <sym> SYMBOL
%token <num> PNUM
%type <num> number numbers flags index expr term factor moneylist moneytype
%type <str_list> stringlist hardstringlist strings
%type <str> reference unitname zonename stringcomp
%type <sym> cunitname czonename
%type <dilarg> dilarg
%type <dilargs> dilargs

/* unit fields */
%token NAMES TITLE DESCR OUT_DESCR IN_DESCR EXTRA KEY OPEN MANIPULATE UFLAGS
%token WEIGHT CAPACITY SPECIAL LIGHT BRIGHT MINV RANDOM DILCOPY
%token ID DATA DURATION FIRSTF TICKF LASTF APPLYF TIME BITS STRINGT

/* room fields */
%token EXIT MOVEMENT IN SPELL LINK
%token TO KEYWORD

/* object fields */
%token VALUE COST RENT TYPE
%token AFFECT

/* char fields */
%token MANA HIT MONEY EXP ALIGNMENT SEX LEVEL HEIGHT RACE
%token POSITION ABILITY WEAPON
%token ATTACK ARMOUR DEFENSIVE OFFENSIVE SPEED

/* npc fields */
%token DEFAULT ACT

/* zone fields */
%token RESET_F LIFESPAN CREATORS NOTES HELP WEATHER

/* reset fields */
%token LOAD FOLLOW MAX ZONEMAX LOCAL INTO NOP EQUIP DOOR PURGE REMOVE COMPLETE

%left '+' '-' '*' '/' SPLUS
%right UMINUS UPLUS

 /*
% expect 2   I can't seem to get rid of the two shift/reduce in `expr'
	   *  And they're not really bothering anyone, so let's ignore them!
	   *  /gnort 08-Aug-94
	   */
%%

/*
 * specifications ************************************
 */

file		: {cur = NULL;} sections ENDMARK
		;
sections	: zone_section dil_section room_section 
                  uglykludge reset_section
		;

uglykludge      : object_section uglykludge
                | mobile_section uglykludge
                | /* naught */
                ;

/*
 * DIL specifications *****************************
 */

dil_section	: DIL
                {
		   char fname[256];
		   sprintf(fname,"%s.dh", zone.z_zone.name);
		   remove(fname);
		   istemplate = 1;
		}
                dils
                {
		   istemplate = 0;
		}
                | /* Naught */
		;

dils		: /* naught */
		| dils dil
		;

dil		: DILPRG
		{
		  /* read and register template */
		  char buf[255];
		  FILE *f;
		  CByteBuffer Buf;

		  if ((f = fopen($1, "rb")) == NULL)
		  {
		    fprintf(stderr, "%s: ", $1);
		    fatal("Couldn't open DIL template.");
		  }
		  else
		  {
		     /* read the template from temporary file */
		     int i;

		     while (!feof(f)) {
		       i = fgetc(f);
		       if (i != -1)
			 Buf.Append8(i);
		     }

		     cur_tmpl = bread_diltemplate(&Buf);

		     assert(Buf.GetReadPosition() == Buf.GetLength());

		     /*fprintf(stderr,"DMC reading template (core %d==%d)\n",
			     Buf.GetLength(), cur_tmpl->coresz);*/
		     /*		  dumpdiltemplate(cur_tmpl);*/

		     fclose(f);
		     remove($1);
		  
		     /* register DIL template in zone */
		     if (search_block(cur_tmpl->prgname, 
				      (const char **) tmplnames, TRUE) != -1) 
		     {
			/* redifinition of template */
			sprintf(buf,"Redefinition of template named '%s'",
				cur_tmpl->prgname);
			fatal(buf);			
		     } else {
			if (!zone.z_tmpl) {
			   zone.z_tmpl = cur_tmpl;
			} else {
			   cur_tmpl->next = zone.z_tmpl;
			   zone.z_tmpl = cur_tmpl;
			}
			tmplnames = add_name(cur_tmpl->prgname, tmplnames);
		     }
		  }
	       }
               ;

/*
 * ROOM specifications *****************************
 */

room_section	: ROOMS { cur = 0; } rooms
                | /* Naught */
		;

rooms		: /* naught */
		| rooms
			{
			  if (cur)
			  {
			    cur->next = mcreate_unit(UNIT_ST_ROOM);
			    cur = cur->next;
			  }
			  else
			  {
			    zone.z_rooms = mcreate_unit(UNIT_ST_ROOM);
			    cur = zone.z_rooms;
			  }
			  cur_extra = 0;
			}
		  room
		;
room		: unitname room_fields END
			{
			  UNIT_IDENT_ASSIGN(cur,$1);
			}
		;
room_fields	: /* naught */
		| room_fields room_field
		;
room_field	: unit_field
		| oroom_field
		| error
		;
oroom_field	: MOVEMENT PNUM
			{
			  ROOM_LANDSCAPE(cur) = $2;
			}
		| FLAGS flags
			{
			  ROOM_FLAGS(cur) = $2;
			}
		| IN reference
			{
			  UNIT_IN(cur) = (struct unit_data *) $2;
			}
		| SPELL number
			{
			  ROOM_RESISTANCE(cur) = $2;
			}
		| exitindex exit_fields ';'

/*
               | LINK reference exitindex exit_fields ';'

link midgaard/temple_square west to ... open ... ;
*/
		;
exitindex	: EXIT index
			{
			  ROOM_EXIT(cur, cur_ex = $2) = mcreate_exit();
			}
		;
exit_fields	: /* naught */
		| exit_fields exit_field
		;
exit_field	: TO reference
			{
			  ROOM_EXIT(cur,cur_ex)->to_room =
			    (struct unit_data *) $2;
			}
		| KEY reference
			{
			  ROOM_EXIT(cur,cur_ex)->key=std::shared_ptr<file_index_type>(reinterpret_cast<file_index_type*>($2));
			}
		| KEYWORD stringlist
			{
			   ROOM_EXIT(cur, cur_ex)->open_name.
			     CopyList((const char **) $2);
			}
		| OPEN flags
			{
			  ROOM_EXIT(cur, cur_ex)->exit_info = $2;
			}
		| DESCR STRING
                        {
			   extern const char *dirs[];

			   if (cur_extra)
			   {
			      cur_extra->next = new (class extra_descr_data);
			      cur_extra = cur_extra->next;
			   }
			   else
			   {
			      UNIT_EXTRA_DESCR(cur) =
				new (class extra_descr_data);
			      cur_extra = UNIT_EXTRA_DESCR(cur);
			   }
			   cur_extra->next = 0;
			   
			   cur_extra->names.AppendName((char *) dirs[cur_ex]);
			   /* strcat($2, "\n\r"); */
			   strip_trailing_blanks($2);
			   cur_extra->descr.Reassign($2);
			}
		;
/*
 * OBJECT specifications ******************************
 */

object_section	: OBJECTS objects
		;

objects		: /* naught */
		| objects
			{
			  if (cur)
			  {
			    cur->next = mcreate_unit(UNIT_ST_OBJ);
			    cur = cur->next;
			  }
			  else
			  {
			    zone.z_objects = mcreate_unit(UNIT_ST_OBJ);
			    cur = zone.z_objects;
			  }
			  cur_extra = 0;
			}
		  object
		;
object		: unitname object_fields END
			{
			  UNIT_IDENT_ASSIGN(cur, $1);
			}
		;
object_fields	: /* naught */
		| object_fields object_field
		;
object_field	: unit_field
		| oobject_field
		| error
		;
oobject_field	: VALUE index flags
			{
			  OBJ_VALUE(cur, $2) = $3;
			}
		| COST moneylist
			{
			  OBJ_PRICE(cur) = $2;
			}
		| RENT moneylist
			{
			  OBJ_PRICE_DAY(cur) = $2;
			}
		| TYPE number
			{
			  OBJ_TYPE(cur) = $2;
			}
		| FLAGS flags
			{
			  OBJ_FLAGS(cur) = $2;
			}
		| SPELL number
			{
			  OBJ_RESISTANCE(cur) = $2;
			}
		;
/*
 * MOBILE specifications ******************************
 */

mobile_section	: MOBILES mobiles
		;

mobiles		: /* naught */
		| mobiles
			{
			  if (cur)
			  {
			    cur->next = mcreate_unit(UNIT_ST_NPC);
			    cur = cur->next;
			  }
			  else
			  {
			    zone.z_mobiles = mcreate_unit(UNIT_ST_NPC);
			    cur = zone.z_mobiles;
			  }
			  cur_extra = 0;
			}
		  mobile
		;
mobile		: unitname mobile_fields END
			{
			  UNIT_IDENT_ASSIGN(cur, $1);
			}
		;
mobile_fields	: /* naught */
		| mobile_fields mobile_field
		;
mobile_field	: unit_field
		| omobile_field
		| error
		;
omobile_field	: MANA number
			{
			  CHAR_MANA(cur) = $2;
			}
		| ACT flags
			{
			  NPC_FLAGS(cur) = $2;
			}
		| MONEY moneylist
			{
			   char buf[1024], tmp[100];

			   if (CHAR_MONEY(cur))
			     warning("Redefinition of money...");

			   for (myi = 0, *buf = '\0'; myi <= mon_top; myi++)
			   {
			      sprintf(tmp, " %d %d ~",
				      mon_list[myi][0], mon_list[myi][1]);
			      strcat(buf, tmp);
			   }

			   CHAR_MONEY(cur) = (char *) mmalloc(strlen(buf) + 1);
			   strcpy(CHAR_MONEY(cur), buf);
			}
		| EXP number
			{
			  CHAR_EXP(cur) = $2;
			}
		| SEX number
			{
			  CHAR_SEX(cur) = $2;
			}
		| LEVEL number
			{
			  CHAR_LEVEL(cur) = $2;
			}
		| RACE number
			{
			  CHAR_RACE(cur) = $2;
			}
		| ATTACK number
			{
			  CHAR_ATTACK_TYPE(cur) = $2;
			}
		| POSITION number
			{
			  CHAR_POS(cur) = $2;
			}
		| DEFAULT number
			{
			  NPC_DEFAULT(cur) = $2;
			}
		| FLAGS flags
			{
			  CHAR_FLAGS(cur) = $2;
			}
		| ARMOUR number
			{
		 	  CHAR_NATURAL_ARMOUR(cur) = $2;
			}
		| SPEED number
			{
		 	  CHAR_SPEED(cur) = $2;
			}
		| DEFENSIVE number
			{
			  CHAR_DEFENSIVE(cur) = $2;
		        }
		| OFFENSIVE number
			{
			  CHAR_OFFENSIVE(cur) = $2;
			}
		| ABILITY index number
			{
			  CHAR_ABILITY(cur,$2) = $3;
			}
		| WEAPON index number
			{
			  NPC_WPN_SKILL(cur,$2) = $3;
			}
		| SPELL index number
			{
			  NPC_SPL_SKILL(cur,$2) = $3;
			}
		;
/*
 * UNIT specifications ******************************
 */


unit_field	: NAMES stringlist
			{
			   UNIT_NAMES(cur).CopyList((const char **) $2);
			   /*if (UNIT_NAMES(cur) && UNIT_NAME(cur))
			     CAP(UNIT_NAME(cur)); */
			}
		| DILPRG
			{
			  CByteBuffer Buf;
			  FILE *f;
			  char buf[255];

			  if ((f = fopen($1, "rb")) == NULL)
			  {
			    fprintf(stderr, "%s: ", $1);
			    fatal("Couldn't open DIL program.");
			  }
			  else
			  {
			     if (!UNIT_FUNC(cur))
			     {
				UNIT_FUNC(cur) = mcreate_func();
				cur_func = UNIT_FUNC(cur);
			     }
			     else
			     {
				cur_func->next = mcreate_func();
				cur_func = cur_func->next;
			     }
			     cur_func->index = SFUN_DIL_INTERNAL;
			     cur_func->heart_beat = WAIT_SEC * 10;
			     cur_func->flags = SFB_ALL;

			     myi = 0;
			     int i;
			     while (!feof(f))
			     {
				i = fgetc(f);
				if (i != -1)
				  Buf.Append8(i);
			     }

			     /* always read latest version written by DIL */
			     cur_func->data = bread_dil(&Buf, NULL, 255, NULL);

			     assert(Buf.GetReadPosition() == Buf.GetLength());

			     /* fprintf(stderr,"DMC reading program\n");*/
			     /*			  dumpdil(cur_func->data);*/

			     fclose(f);
			     
			     remove($1);

			     /* register DIL template in zone */
			     cur_prg = (struct dilprg *)(cur_func->data);
			     cur_tmpl= cur_prg->sp->tmpl;
			     cur_prg->flags |= DILFL_COPY;
			     cur_tmpl->flags |= DILFL_COPY;
			     /* register DIL template in zone */
			     if (search_block(cur_tmpl->prgname, 
					      (const char **) tmplnames, TRUE) != -1) 
			     {
				/* redifinition of template */
				sprintf(buf,"Redefinition of template "
					"named '%s'",
					cur_tmpl->prgname);
				fatal(buf);			
			     } else {
				if (!zone.z_tmpl) {
				   zone.z_tmpl = cur_tmpl;
				} else {
				   cur_tmpl->next = zone.z_tmpl;
				   zone.z_tmpl = cur_tmpl;
				}
				tmplnames = add_name(cur_tmpl->prgname,
						     tmplnames);
			     }
			  }
		       }
                | DILCOPY reference '(' dilargs ')' ';'
                       {
			  struct dilargstype *argcopy;

			  CREATE(argcopy, struct dilargstype, 1);

			  *argcopy = *($4);
			  argcopy->name = $2;

			  if (!UNIT_FUNC(cur))
			  {
			    UNIT_FUNC(cur) = mcreate_func();
			    cur_func = UNIT_FUNC(cur);
			  }
			  else
			  {
			    cur_func->next = mcreate_func();
			    cur_func = cur_func->next;
			  }
			  cur_func->index = SFUN_DILCOPY_INTERNAL;
                          cur_func->data = argcopy;
			  cur_func->heart_beat = WAIT_SEC * 10;
			  cur_func->flags = SFB_ALL;
		       }
		| TITLE STRING
			{
			   UNIT_TITLE(cur).Reassign($2);
			}
                | OUT_DESCR STRING
                        {
			   UNIT_OUT_DESCR(cur).Reassign($2);
			}
                | IN_DESCR STRING
                        {
			   if (IS_ROOM(cur) && !str_is_empty($2))
			   {
			      memmove($2+3, $2, strlen($2)+1);
			      
			      $2[0] = ' ';
			      $2[1] = ' ';
			      $2[2] = ' ';
			   }

			   UNIT_IN_DESCR(cur).Reassign($2);
			}
		| DESCR STRING
			{
			   if (IS_ROOM(cur) && !str_is_empty($2))
			   {
			      memmove($2+3, $2, strlen($2)+1);

			      $2[0] = ' ';
			      $2[1] = ' ';
			      $2[2] = ' ';

			      UNIT_IN_DESCR(cur).Reassign($2);
			   }
			   else
			     UNIT_OUT_DESCR(cur).Reassign($2);
			}
		| ALIGNMENT number
			{
			  UNIT_ALIGNMENT(cur) = $2;
			}
		| EXTRA stringlist STRING
			{
			  if (cur_extra)
			  {
			     cur_extra->next = new (class extra_descr_data);
			     cur_extra = cur_extra->next;
			  }
			  else
			  {
			    UNIT_EXTRA_DESCR(cur) =
			      new (class extra_descr_data);
			    cur_extra = UNIT_EXTRA_DESCR(cur);
			  }
			  cur_extra->next = 0;

			  cur_extra->names.CopyList((const char **) $2);
			  /* strcat($3, "\n\r"); */
			  strip_trailing_blanks($3);
			  cur_extra->descr.Reassign($3);
			}
		| KEY reference
			{
			  UNIT_KEY(cur)=std::shared_ptr<file_index_type>(reinterpret_cast<file_index_type*>($2));
			}
		| OPEN flags
			{
			  UNIT_OPEN_FLAGS(cur) |= $2;
			}
		| MANIPULATE flags
			{
			  UNIT_MANIPULATE(cur) |= $2;
			}
		| HIT number
			{
			  UNIT_MAX_HIT(cur) = $2;
			}
		| UFLAGS flags
			{
			  UNIT_FLAGS(cur) |= $2;
			}
		| WEIGHT number
			{
			  UNIT_BASE_WEIGHT(cur) = $2;
			}
		| CAPACITY number
			{
			  UNIT_CAPACITY(cur) = $2;
			}
		| HEIGHT number
			{
			  UNIT_SIZE(cur) = $2;
			}
/*		| TOUGHNESS number
			{
			  UNIT_TGH(cur) = $2;
			} */
		| LIGHT number
			{
			  UNIT_LIGHTS(cur) = $2;
			}
		| BRIGHT number
			{
			  UNIT_BRIGHT(cur) = $2;
			}
		| MINV number
			{
			  UNIT_MINV(cur) = $2;
			}
		| SPECIAL
			{
			  if (!UNIT_FUNC(cur))
			  {
			    UNIT_FUNC(cur) = mcreate_func();
			    cur_func = UNIT_FUNC(cur);
			  }
			  else
			  {
			    cur_func->next = mcreate_func();
			    cur_func = cur_func->next;
			  }
			}
		  PNUM
			{
			  switch ($3)
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
			  cur_func->index = $3;
			}
		  optfuncargs
		| AFFECT
			{
			  if (!UNIT_AFFECTED(cur))
			  {
			    UNIT_AFFECTED(cur) = mcreate_affect();
			    cur_aff = UNIT_AFFECTED(cur);
			  }
			  else
			  {
			    cur_aff->next = mcreate_affect();
			    cur_aff = cur_aff->next;
			  }
			}
		  affect_fields ';'
		;
optfuncargs	: /* naught */
		| optfuncargs optfuncarg
		;
optfuncarg	: STRINGT STRING
			{
			   char buf[1024];
			   if (cur_func->data)
			     fatal("Redefinition of data for special.");
			   
			   cur_func->data = $2;
			}
		| stringcomp
			{
			   if (cur_func->data)
			     fatal("Redefinition of data for special.");
			   cur_func->data = $1;
			}
		| TIME number
			{
			  cur_func->heart_beat = $2;
			}
		| BITS flags
			{
			  cur_func->flags = $2;
			}
		;
affect_fields	: affect_field
		| affect_fields affect_field
		;
affect_field	: ID number
			{
			  cur_aff->id = $2;
			}
		| DURATION number
			{
			  cur_aff->duration = $2;
			}
		| DATA index number
			{
			  cur_aff->data[$2] = $3;
			}
		| FIRSTF number
			{
			  cur_aff->firstf_i = $2;
			}
		| TICKF number
			{
			  cur_aff->tickf_i = $2;
			}
		| LASTF number
			{
			  cur_aff->lastf_i = $2;
			}
		| APPLYF number
			{
			  cur_aff->applyf_i = $2;
			}
		| TIME number
			{
			  cur_aff->beat = $2;
			}
		;
/*
 * ZONE definitions ******************************
 */

dilargs         : /*empty */
                {
                   dilargs.no = 0;
                   $$ = &dilargs;
                }
                | dilarg
                {
                   dilargs.no = 1;
		   dilargs.dilarg[0] = *$1;
                   $$ = &dilargs;
                }
                | dilargs ',' dilarg
                {
                   dilargs.no++;
		   dilargs.dilarg[dilargs.no-1] = *$3;
                   $$ = &dilargs;
                }
                ;

dilarg          : hardstringlist
                {
		   struct dilargtype *dat;

		   dat = (struct dilargtype *)
		     mmalloc(sizeof(struct dilargtype));
		   dat->type = DILV_SLP;
		   dat->data.stringlist = $1;
		   $$ = dat;
                }
                | stringcomp
                {
		   struct dilargtype *dat;

		   dat = (struct dilargtype *)
		     mmalloc(sizeof(struct dilargtype));
		   dat->type = DILV_SP;
		   dat->data.string = $1;
		   $$ = dat;
                }
                | number
                {
		   struct dilargtype *dat;

		   dat = (struct dilargtype *)
		     mmalloc(sizeof(struct dilargtype));
		   dat->type = DILV_INT;
		   dat->data.num = $1;
		   $$ = dat;
                }
                ;

zone_section	: ZONE zone_fields
		;

zone_fields	: /*naught */
		| zone_fields zone_field
		;
zone_field	: LIFESPAN number
			{
			   zone.z_zone.lifespan = $2;
			}
                | WEATHER number
                        {
			   zone.z_zone.weather = $2;
			}
		| RESET_F number
			{
			  zone.z_zone.reset_mode = $2;
			}
		| CREATORS stringlist
			{
			  zone.z_zone.creators = $2;
			}
		| NOTES STRING
			{
			  zone.z_zone.notes = $2;
			}
		| TITLE STRING
			{
			  zone.z_zone.title = $2;
			}
		| HELP STRING
			{
			  zone.z_zone.help = $2;
			}
		| zonename
			{
			  zone.z_zone.name = $1;
			}
		| error
		;
/*
 * RESET definitions ******************************
 */

reset_section 	: RESET
			{
			  cur_cmd = 0;
			}
		  reset_commands
                | /* Naught */
		;
reset_commands	: reset_command
		| reset_commands reset_command
		;
reset_command	: alloc command
			{
			  cur_cmd->direction = ZON_DIR_CONT;
			}
		| alloc command
			{
			  cur_cmd->direction = ZON_DIR_NEST;
			}
		  block
		;
block		: '{' reset_commands '}'
			{
			  if (cur_cmd->direction == ZON_DIR_CONT)
			    cur_cmd->direction = ZON_DIR_UNNEST;
			  else if (cur_cmd->direction == ZON_DIR_NEST)
			    cur_cmd->direction = ZON_DIR_CONT;
			  else /* UNNEST, gotta put in a nop, here */
			  {
			    cur_cmd->next = (struct reset_command *)
			      mmalloc(sizeof(struct reset_command));
			    cur_cmd = cur_cmd->next;
			    cur_cmd->cmd_no = 0; /* nop */
			    cur_cmd->direction = ZON_DIR_UNNEST;
			  }
			}
		;
alloc		: /* naught */
			{
			  if (cur_cmd)
			  {
			    cur_cmd->next = (struct reset_command *)
			      mmalloc(sizeof(struct reset_command));
			    cur_cmd = cur_cmd->next;
			  }
			  else
			  {
			    zone.z_table = (struct reset_command *)
			      mmalloc(sizeof(struct reset_command));
			    cur_cmd = zone.z_table;
			  }
			  /* init ? */
			  cur_cmd->next = 0;
			  cur_cmd->cmpl = 0;
			  /* cur_cmd->num1 = 0;
			     cur_cmd->num2 = 0; */
			}
		;
command		: LOAD reference loadfields
			{
			  cur_cmd->cmd_no = 1;
			  cur_cmd->ref1 = $2;
			  if (!cur_cmd->num1)
			    cur_cmd->num1 = 0;
			  if (!cur_cmd->num2)
			    cur_cmd->num2 = 0;
			}
		| FOLLOW reference followfields
			{
			  cur_cmd->cmd_no = 6;
			  cur_cmd->ref1 = $2;
			  if (!cur_cmd->num1)
			    cur_cmd->num1 = 0;
			  if (!cur_cmd->num2)
			    cur_cmd->num2 = 0;
			}
		| EQUIP reference equipfields
			{
			  cur_cmd->cmd_no = 2;
			  cur_cmd->ref1 = $2;
			  if (!cur_cmd->num1)
			    cur_cmd->num1 = 0;
			}
		| DOOR reference number flags
			{
			  cur_cmd->cmd_no = 3;
			  cur_cmd->ref1 = $2;
			  cur_cmd->num1 = $3;
			  cur_cmd->num2 = $4;
			}
		| PURGE reference
			{
			  cur_cmd->cmd_no = 4;
			  cur_cmd->ref1 = $2;
			}
		| REMOVE reference IN reference
			{
			  cur_cmd->cmd_no = 5;
			  cur_cmd->ref1 = $2;
			  cur_cmd->ref2 = $4;
			}
                | RANDOM number
                        {
			   cur_cmd->cmd_no = 7;
			   cur_cmd->ref1 = 0;
			   cur_cmd->num1 = $2;
			   cur_cmd->num2 = 0;
			}
		| NOP
			{
			  cur_cmd->cmd_no = 0;
			}
		;
loadfields	: /* naught */
		| loadfields loadfield
		;
loadfield	: INTO reference
			{
			  cur_cmd->ref2 = $2;
			}
		| max
		| zonemax
		| local
		| COMPLETE
			{
			  cur_cmd->cmpl = !0;
			}
		;
followfields	: /* naught */
		| followfields followfield
		;
followfield	: max
		| zonemax
		| local
		| COMPLETE
			{
			  cur_cmd->cmpl = !0;
			}
		;
equipfields	: /* naught */
		| equipfields equipfield
		;
equipfield	: max
		| position number
			{
			  cur_cmd->num2 = $2;
			}
		| COMPLETE
			{
			  cur_cmd->cmpl = !0;
			}
		;
local		: LOCAL number
			{
			  cur_cmd->num3 = $2;
			}
		;
zonemax		: ZONEMAX number
			{
			  cur_cmd->num2 = $2;
			}
		;
max		: MAX number
			{
			  cur_cmd->num1 = $2;
			}
		;
position	: /*naught */
		| POSITION
		;
/*
 * UTILITY definitions ******************************
 */

/*  The return value of moneylist is the actual amount.
 *  What is set in the mon_list is the value needed for the string. 
 */
moneylist	: number moneytype
			{
			   mon_top = 0;
			   mon_list[mon_top][0] = $1;
			   mon_list[mon_top][1] = $2;

			   $$ = $1 * money_types[$2].relative_value;
			}
		| moneylist ',' number moneytype
			{
			   ++mon_top;
			   mon_list[mon_top][0] = $3;
			   mon_list[mon_top][1] = $4;

			   $$ = $1;

			   $$ += $3 * money_types[$4].relative_value;
			}
		;
moneytype	: STRING
			{
			   int myi;
			   $$ = -1;

			   for (myi = 0; myi <= MAX_MONEY; myi++)
			     if (!strcmp($1, money_types[myi].abbrev))
			     {
				$$ = myi;
				break;
			     }

			   if (myi > MAX_MONEY)
			     fatal("Not a legal money denominator.");
			}
		;

stringlist	: hardstringlist
			{
			  $$ = $1;
			}
		| STRING
			{
			   $$ = (char **) mmalloc(sizeof(char *) * 2);
			   $$[0] = $1;
			   $$[1] = 0;
			}
		;

hardstringlist	: '{' strings '}'
			{
			  // MS2020 $$ = (char **) mmalloc(sizeof(char *) * (str_top + 1));
			  // Looks like there's an error in the old code. str_top=0 means there's one string and
			  // thus we will need 2 indices.
			  // error is compounded by <= in the for loop and the final setting to 0 which will be OOB
			  //
			  $$ = (char **) mmalloc(sizeof(char *) * (str_top + 2));
			  for (myi = 0; myi <= str_top; myi++)
			    $$[myi] = str_list[myi];
			  $$[myi] = 0;
			}
		| '{' '}'
			{
			  $$ = 0;
			}
		;

strings		: STRING
			{
			  str_list[str_top = 0] = $1;
			}
		| strings ',' STRING
			{
			  str_list[++str_top] = $3;
			}
		;


stringcomp      : STRING
                	{
			   $$ = $1;
			}
		| stringcomp '+' STRING
			{
			   char *res;
			   res = (char *) mmalloc(strlen($1) + strlen($3) + 2 + 3);
			   strcpy(res, $1);
			   strcat(res, $3);
			   $$ = res;
			}
/* MS: Shit... I have made a feeble attempt to make this work, but it
       does not without the () around number since number can contain +. */

		| stringcomp '+' '(' number ')'
			{
			   char *res;
			   char buf[100];
			   if (strlen($1) > 0 && isdigit($1[strlen($1)-1]))
			     sprintf(buf, " %d ", $4);
			   else
			     sprintf(buf, "%d", $4);

			   res = (char *) mmalloc(strlen($1) + strlen(buf)+2+3);
			   strcpy(res, $1);
			   strcat(res, buf);
			   $$ = res;
			}

		| stringcomp '+' PNUM
			{
			   char *res;
			   char buf[100];
			   if (strlen($1) > 0 && isdigit($1[strlen($1)-1]))
			     sprintf(buf, " %d", $3);
			   else
			     sprintf(buf, "%d", $3);

			   res = (char *) mmalloc(strlen($1) + strlen(buf)+2+3);
			   strcpy(res, $1);
			   strcat(res, buf);
			   $$ = res;
			}

                ;

flags		: number
			{
			  $$ = $1;
			}
		| '{' numbers '}'
			{
			  $$ = $2;
			}
		;
numbers		: flags
			{
			  $$ = $1;
			}
		| numbers ',' flags
			{
			  $$ = $1 | $3;
			}
		;


number          : expr
			{
			  $$ = $1;
			}
		;
expr            : term
			{
			  $$ = $1;
			}
		| '-' term %prec UMINUS
			{
			  $$ = -$2;
			}
		| '+' term %prec UPLUS
			{
			  $$ = $2;
			}
		| expr '+' term
			{
			  $$ = $1 + $3;
			}
		| expr '-' term
			{
			  $$ = $1 - $3;
			}
		;
term            : factor
			{
			  $$ = $1;
			}
		| term '*' factor
			{
			  $$ = $1 * $3;
			}
		| term '/' factor
			{
			  if ($3 != 0)
			    $$ = $1 / $3;
			  else
			  {
			    warning("Division by zero!");
			    $$ = 0;
			  }
			}
		;
factor          : '(' expr ')'
			{
			  $$ = $2;
			}
		| PNUM
			{
			  $$ = $1;
			}
		;

zonename	: czonename
			{
			   $$ = (char *) mmalloc(strlen($1) + 1);
			   strcpy($$, $1);
			}
		;
czonename	: SYMBOL
			{
			  if (strlen($1) > FI_MAX_ZONENAME)
			  {
			    warning("Zone name too long. truncated.");
			    $1[FI_MAX_ZONENAME] = '\0';
			  }
			  strcpy($$, $1);
			}
		;
unitname	: cunitname
			{
			   $$ = (char *) mmalloc(strlen($1) + 1);
			   strcpy($$, $1);
			}
		;
cunitname	: SYMBOL
			{
			  if (strlen($1) > FI_MAX_UNITNAME)
			  {
			    warning("Unit name too long. truncated.");
			    $1[FI_MAX_UNITNAME] = '\0';
			  }
			  strcpy($$, $1);
			}
		;
reference	: cunitname
			{
			  if (!zone.z_zone.name)
			  {
			    fatal("local zonename must be defined.");
			    $$ = empty_ref;
			  }
			  else
			  {
			     $$ = (char *) mmalloc(strlen($1) +
					     strlen(zone.z_zone.name) + 2);
			     strcpy($$, zone.z_zone.name);
			     strcpy($$ + strlen(zone.z_zone.name) + 1, $1);
			  }
			}
		| cunitname '@' czonename
			{
			   $$ = (char *) mmalloc(strlen($1) + strlen($3) + 2);
			   strcpy($$, $3);
			   strcpy($$ + strlen($3) + 1, $1);
			}
		| czonename '/' cunitname
			{
			   $$ = (char *) mmalloc(strlen($1) + strlen($3) + 2);
			  strcpy($$, $1);
			  strcpy($$ + strlen($1) + 1, $3);
			}
		;
index		: '[' PNUM ']'
			{
			  $$ = $2;
			}
%%

void syntax(const char *str)
{
   extern char *yytext;

   fprintf(stderr, "%s: %d: %s\n   Token: '%s'\n", cur_filename,
	   linenum, str, yytext);
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

   if (fatal_warnings)
     fatal(str);
   else
     real_warning(str);
}


/* Read a string from fl, allocate space for it and return it */
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

void dumpdiltemplate(struct diltemplate *tmpl) {

  int i;
  fprintf(stderr,"Name: '%s'\n",tmpl->prgname);
  fprintf(stderr,"Crc: %d\n",tmpl->varcrc);
  fprintf(stderr,"Return type: %d\n",tmpl->rtnt);
  fprintf(stderr,"# Arguments: %d\n",tmpl->argc);
  for (myi=0;myi<tmpl->argc;myi++)
    fprintf(stderr,"Arg %d type: %d\n",i,tmpl->argt[myi]);
  fprintf(stderr,"Coresize: %d\n",tmpl->coresz);
  fprintf(stderr,"Core: %p\n",tmpl->core);
  fprintf(stderr,"# Vars: %d\n",tmpl->varc);
  for (myi=0;myi<tmpl->varc;myi++) 
    fprintf(stderr,"Var %d type: %d\n",myi,tmpl->vart[myi]);
  fprintf(stderr,"# References: %d\n",tmpl->xrefcount);
  for (myi=0;myi<tmpl->xrefcount;myi++) {
     fprintf(stderr,"Xref %d: '%s'\n",myi,tmpl->xrefs[myi].name);
     fprintf(stderr,"     Return type %d\n",tmpl->xrefs[myi].rtnt);
     fprintf(stderr,"     # Arguments: %d\n",tmpl->xrefs[myi].argc);
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
