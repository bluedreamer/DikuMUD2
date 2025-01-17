#include "comm.h"
#include "CServerConfiguration.h"
#include "db.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "main.h"
#include "structs.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "utility.h"
#include "utils.h"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define ELIZA_TALKFILE "talk.eli"
#define ELIZA_LOGFILE  "log.eli"

#define MAX_HISTORY 4

// TODO Extract this
struct oracle_data
{
   int       *nextrep;
   unit_data *patient;
   unit_data *doctor;
   int        oldkeywd[MAX_HISTORY];     /* queue of indices of most recent keywds */
   char       own_name[FI_MAX_UNITNAME]; /* Lowercase name of Doc.                */
   char       laststr[MAX_INPUT_LENGTH]; /* Don't reply to same string twice      */
   char       lastrep[MAX_INPUT_LENGTH]; /* Don't make same reply twice!          */
};

// TODO Extract this
struct subject_type
{
   char **replies; /* NULL terminated         */
} *eliza_subjects     = nullptr;
int eliza_maxsubjects = 0;

struct template_type
{
   char **exp; /* What shall we react on? */
   int    subjno;
} *eliza_template      = nullptr;
int eliza_maxtemplates = 0;

struct keyword_type
{
   char **keyword;  /* What shall we react on? */
   char   priority; /* how important is keywd: 0 worst, 9 best */
   int    subjno;
} *eliza_keyword              = nullptr;
int         eliza_maxkeywords = 0;

int         eliza_booted      = static_cast<int>(FALSE);

static char words[400];

extern char libdir[];

/* ============================================================= */

void        preprocess_string(char *str, struct oracle_data *od)
{
   str_lower(str);
   str_rem(str, '\'');
   str_blank_punct(str);
   str_chraround(str, ' ');

   str_substitute(" cannot ", " can not ", str);
   str_substitute(" cant ", " can not ", str);
   str_substitute(" dont ", " do not ", str);
   str_substitute(" ive ", " i have ", str);
   str_substitute(" youve ", " you have ", str);
   str_substitute(" youre ", " you are ", str);
   str_substitute(" im ", " i am ", str);
   str_substitute(" id ", " i would ", str);
   str_substitute(" but ", " , ", str);
   str_substitute(od->own_name, " ", str);

   str_remspc(str);
}

/* ================================================================== */

/*
  Check to see if the input line (input) matches template
  patterns rexp and lexp. Both expressions are delimited by
  zero character.
  Case 1. Lexp is identical with the input.
  Case 2. Template is of form Lexp <anything>.
  Case 3. Template is of form Lexp <anything> Rexp.
*/

auto match_templ(char *input, struct template_type *tem) -> char *
{
   char *respons;
   char *rp;
   char *lp;
   char *ip;
   char *tcp;
   char *cp;
   int   i;
   int   j;

   respons    = words;
   ip         = input;
   respons[0] = 0;

   if((tem->exp[0] != nullptr) && (tem->exp[1] == nullptr)) /* Case with no % signs */
   {
      cp = str_str(ip, tem->exp[0]);
      if(cp == nullptr)
      {
         return nullptr;
      }

      /* Only OK if there is a punctation left of expression */
      for(tcp = cp - 1; tcp >= input; tcp--)
      {
         if((*tcp == 0) || (ispunct(*tcp) != 0))
         {
            break;
         }
         if(isalnum(*tcp) != 0)
         {
            return nullptr;
         }
      }

      /* Only OK if there is a punctation right of expression */
      for(tcp = cp + strlen(tem->exp[0]); *tcp != 0; tcp++)
      {
         if(ispunct(*tcp) != 0)
         {
            break;
         }
         if(isalnum(*tcp) != 0)
         {
            return nullptr;
         }
      }

      return respons;
   }

   for(i = 0; tem->exp[i] != nullptr; i++)
   {
      if(*tem->exp[i] == 0)
      {
         if(i > 0) /* Case: "text %" */
         {
            lp = str_str(ip, tem->exp[i - 1]);
            if(lp == nullptr)
            {
               return nullptr;
            }
            for(tcp = lp - 1; tcp >= ip; tcp--)
            {
               if((*tcp == 0) || (ispunct(*tcp) != 0))
               {
                  break;
               }

               return nullptr;
            }

            lp += strlen(tem->exp[i - 1]);
            ip = lp;
         }
         else
         {
            lp = ip;
         }

         if(tem->exp[i + 1] != nullptr) /* Case: "% text" */
         {
            rp = str_str(ip, tem->exp[i + 1]);
            if(rp == nullptr)
            {
               return nullptr;
            }

            /* String must end with the appropriate "text" */
            for(cp = rp + strlen(tem->exp[i + 1]); *cp != 0; cp++)
            {
               if(isalpha(*cp) != 0)
               {
                  return nullptr;
               }
               if(isspace(*cp) == 0)
               {
                  break;
               }
            }

            for(cp = rp; cp > ip; cp--)
            {
               if(ispunct(*cp) != 0)
               {
                  return nullptr;
               }
            }

            ip = rp + strlen(tem->exp[i + 1]);
         }
         else /* Case: "text %" with nothing more (find end of string) */
         {
            for(rp = ip; *rp != 0; rp++)
            {
               if(ispunct(*rp) != 0)
               {
                  rp--;
                  break;
               }
            }
         }

         for(j = 0, cp = lp; cp < rp; j++, cp++)
         {
            respons[j] = *cp;
         }
         respons[j] = 0;
      }
   }
   if(*respons != 0)
   {
      return respons;
   }
   return nullptr;
}

/*
try to match the current line to a template.  in turn, try all
the templates to see if the first two letters match.  if so,
read in the template and call match to check the entire template.
*/
auto trytempl(char *line) -> int
{
   int i;

   for(i = 0; i < eliza_maxtemplates; i++)
   {
      /* compare input line (line[]) to template (t[]) */
      if(match_templ(line, &eliza_template[i]) != nullptr)
      {
         return i;
      }
   }

   return -1;
}

void shift(int base, int delta)
{
   int i;
   int k;

   if(delta == 0)
   {
      return;
   }

   if(delta > 0)
   {
      k = base;
      while(words[k] != 0)
      {
         k++;
      }
      for(i = k; i >= base; i--)
      {
         words[i + delta] = words[i];
      }
   }
   else
   { /* delta <0 */
      for(i = 0; i == 0 || words[base + i - 1] != 0; i++)
      {
         words[base + i] = words[base + i - delta];
      }
   }
}

/*
  scan the array "words" for the string OLD.  if it occurs,
  replace it by NEW.  also set the parity bits on in the
  replacement text to  mark them as already modified
  */
void subst(const char *old, const char *pnew)
{
   int i;
   int nlen;
   int olen;
   int flag;
   int base;
   int delim;

   olen = strlen(old);
   nlen = strlen(pnew);

   for(base = 0; words[base] != 0; base++)
   {
      flag = 1;
      for(i = 0; i < olen; i++)
      {
         if(old[i] != words[base + i])
         {
            flag = 0;
            break;
         }
      }

      delim = words[base + olen];
      if(flag == 1 && (base == 0 || words[base - 1] == ' ') && (delim == ' ' || delim == 0))
      {
         shift(base, nlen - olen);
         for(i = 0; i < nlen; i++)
         {
            words[base + i] = pnew[i] + 128;
         }
      }
   }
}

void fix()
{
   /*   subst("ive", "you've"); Preproc   essed */
   /*   subst("youve", "I've"); Preprocessed */
   /*   subst("youre", "I am"); Preprocessed */
   /*   subst("im", "you're");  Preprocessed */
   /*   subst("we are", "we are"); UH? */
   subst("my dear", "love");
   subst("you were", "I was");
   subst("i was", "you were");
   subst("that you", "that i");
   subst("my", "your");
   subst("you", "me"); /* Or perhaps, "I"??? */
   subst("your", "my");
   subst("me", "you");
   subst("mine", "yours");
   subst("am", "are");
   subst("yours", "mine");
   subst("yourself", "myself");
   subst("myself", "yourself");
   subst("are", "am");
   subst("i", "you");
}

void grammar(char *str)
{
   int i;

   for(i = 0; str[i] != 0; i++)
   {
      str[i] = str[i] & 127;
   }

   str_substitute(" i are ", " I am ", str);
   str_substitute(" you am ", " you are ", str);
   str_substitute(" me am ", " I am ", str);
   /*   str_substitute(" me do  ", " I do ", str); */
}

/* Remember which keywords have previously been used... */
/* Register 'n' as the index                            */
void eliza_store_memory(struct oracle_data *od, int n, int pri)
{
   assert(n < eliza_maxsubjects);

   if((pri > '5') && (number('5', pri) > '5'))
   {
      memmove(&od->oldkeywd[1], &od->oldkeywd[0], sizeof(int) * (MAX_HISTORY - 1));
      od->oldkeywd[0] = n;
   }
}

/* Remember which keywords have previously been used... */
/* Register 'n' as the index                            */
auto eliza_retrieve_memory(struct oracle_data *od) -> int
{
   int mem;

   mem = od->oldkeywd[0];

   memmove(&od->oldkeywd[0], &od->oldkeywd[1], sizeof(int) * (MAX_HISTORY - 1));

   od->oldkeywd[MAX_HISTORY - 1] = eliza_maxsubjects - 1;

   return mem;
}

/*
  getscript is set up to return the correct response.
  words contains the extracted words.  make reply
  */
auto response(oracle_data *od, int subjno) -> char *
{
   static char resp[400];
   char        c1;
   char       *cp;
   int         i;
   int         k;
   int         thisrep;

   thisrep = od->nextrep[subjno]++;
   if(eliza_subjects[subjno].replies[od->nextrep[subjno]] == nullptr)
   {
      od->nextrep[subjno] = 0;
   }

   cp = eliza_subjects[subjno].replies[thisrep];

   if(strcmp(od->lastrep, cp) == 0)
   {
      od->lastrep[0] = 0;
      return response(od, eliza_retrieve_memory(od));
   }
   strcpy(od->lastrep, cp);

   fix(); /* replace i by you etc. */

   for(k = 0; *cp != 0; cp++)
   {
      switch(*cp)
      {
         case '%':
            for(i = 0; words[i] != 0; i++)
            {
               c1 = words[i] & 0177;
               if(c1 != 0)
               {
                  resp[k++] = c1;
               }
            }
            break;

         case '&':
            for(i = 0; UNIT_NAME(od->patient)[i] != 0; i++)
            {
               resp[k++] = UNIT_NAME(od->patient)[i];
            }
            break;

         default:
            resp[k++] = *cp;
            break;
      }
   }

   resp[k] = 0;

   /* fix gross errors in grammar like "you am", "i are" */
   grammar(resp);

   return resp;
}

/* the line does not match a template.  are there any keywords in it */
auto trykeywd(char *line, int *score) -> int
{
   int i;
   int j;
   int index;
   int mi;

   i      = 0;
   index  = -1;
   *score = -1;

   for(j = 0; j < eliza_maxkeywords; j++)
   {
      for(mi = 0; eliza_keyword[j].keyword[mi] != nullptr; mi++)
      {
         if(str_str(line, eliza_keyword[j].keyword[mi]) != nullptr)
         {
            if(eliza_keyword[j].priority >= *score)
            {
               if((eliza_keyword[j].priority > *score) || (number(0, 1) != 0))
               {
                  *score = eliza_keyword[j].priority;
                  index  = j;
               }
            }
         }
      }
   }

   return index;
}

auto eliza_process(struct oracle_data *od, char *s) -> char *
{
   int i;
   int pri;

   if(strlen(s) <= 1)
   {
      return nullptr;
   }

   preprocess_string(s, od);

   if(strcmp(s, od->laststr) == 0)
   {
      return nullptr;
   }
   strcpy(od->laststr, s);

   words[0] = 0;

   i        = trytempl(s);
   if(i >= 0)
   {
      return response(od, eliza_template[i].subjno);
   }

   i = trykeywd(s, &pri);
   if(i >= 0)
   {
      eliza_store_memory(od, eliza_keyword[i].subjno, pri);
      return response(od, eliza_keyword[i].subjno);
   }

   /* nothing matched.  use an old keyword */
   return response(od, eliza_retrieve_memory(od));
}

/* ====================================================================== */
/* ====================================================================== */

void delayed_action(void *p1, void *p2)
{
   auto *npc = (unit_data *)p1;
   char *str = (char *)p2;

   command_interpreter(npc, str);
   free(str);
}

void set_delayed_action(unit_data *npc, char *str)
{
   int   when;
   char *cp;

   when = WAIT_SEC * 1;

   while((cp = strchr(str, '@')) != nullptr)
   {
      *cp = 0;
      event_enq(when, delayed_action, npc, str_dup(str));
      str = cp + 1;
      when += WAIT_SEC * 1;
   }

   when += WAIT_SEC * 3;

   event_enq(when, delayed_action, npc, str_dup(str));
}

#define MAX_ELIBUF 50

void eliza_log(unit_data *who, const char *str, int comms)
{
   static int   idx = -1;
   static char *buf[MAX_ELIBUF];

   FILE        *f;

   if(idx == -1)
   {
      // MS2020: memset(buf, 0, sizeof(buf) * 4);
      memset(buf, 0, sizeof(buf));
      idx = 0;
   }

   if(str == nullptr)
   {
      idx = 0;
      for(auto &i : buf)
      {
         if(i != nullptr)
         {
            free(i);
         }
         i = nullptr;
      }
      return;
   }

   if((comms < 5) && (idx < MAX_ELIBUF))
   {
      char tmp[1024];

      sprintf(tmp, "%-12s::  %s\n", UNIT_NAME(who), str);

      buf[idx++] = str_dup(tmp);
      return;
   }

   if((f = fopen_cache(str_cc(libdir, ELIZA_LOGFILE), "a+")) == nullptr)
   {
      abort();
   }

   if(idx <= MAX_ELIBUF)
   {
      for(int i = 0; i < idx; i++)
      {
         fprintf(f, "%s", buf[i]);
      }
      idx = MAX_ELIBUF + 1;
   }

   fprintf(f, "%-12s::  %s\n", UNIT_NAME(who), str);
   fflush(f);
}

#define ELIZA_DEBUG TRUE

/* BUG: HAS MEMORY LEAK ERROR: WHEN KILLED, EVENTS ARE DEENQ'ed AND
        THE STRDUP'ED STRINGS ARE NOT FREED */
auto oracle(struct spec_arg *sarg) -> int
{
   static int          comms = 0;
   char                buf[2 * MAX_INPUT_LENGTH];
   char               *response;
   struct oracle_data *od;
   int                 i;

   void                eliza_boot();

   od = (struct oracle_data *)sarg->fptr->data;

   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if(od != nullptr)
      {
         free(od->nextrep);
         free(od);
      }
      sarg->fptr->data = nullptr;
      event_deenq_relaxed(delayed_action, sarg->owner, nullptr);
      return SFR_BLOCK;
   }
   if(sarg->cmd->no == CMD_WHO)
   {
      do_who(sarg->activator, (char *)sarg->arg, sarg->cmd);
      sprintf(buf, g_cServerConfig.m_sColor.pWhoElem, UNIT_NAME(sarg->owner), " ", "the human", " (", "Udgaard Fighter", ")\n\r");
      send_to_char(buf, sarg->activator);
      return SFR_BLOCK;
   }

   if(sarg->fptr->data == nullptr)
   {
      if(eliza_booted == static_cast<int>(FALSE))
      {
         eliza_boot();
         if(eliza_maxsubjects < 10)
         {
            destroy_fptr(sarg->owner, sarg->fptr);
            return SFR_SHARE;
         }
         eliza_booted = static_cast<int>(TRUE);
      }

      CREATE(sarg->fptr->data, struct oracle_data, 1);
      od             = (struct oracle_data *)sarg->fptr->data;
      od->laststr[0] = 0;
      od->lastrep[0] = 0;
      strcpy(od->own_name, UNIT_NAME(sarg->owner));
      str_lower(od->own_name);
      od->patient = nullptr;
      od->doctor  = sarg->owner;

      CREATE(od->nextrep, int, eliza_maxsubjects);
      for(i = 0; i < eliza_maxsubjects; i++)
      {
         od->nextrep[i] = 0;
      }
      for(i = 0; i < MAX_HISTORY; i++)
      {
         od->oldkeywd[i] = eliza_maxsubjects - 1;
      }
   }

   if(!CHAR_IS_READY(sarg->owner))
   {
      return SFR_SHARE;
   }

   if((od->patient != nullptr) && (scan4_ref(sarg->owner, od->patient) == nullptr))
   {
      REMOVE_BIT(sarg->fptr->flags, SFB_PRIORITY);
      od->patient = nullptr;
   }

   if(od->patient != sarg->activator)
   {
      if((od->patient == nullptr) && CHAR_CAN_SEE(sarg->owner, sarg->activator))
      {
         switch(number(0, 3))
         {
            case 0:
               sprintf(buf, "smile %s", UNIT_NAME(sarg->activator));
               break;

            case 1:
               sprintf(buf, "hug %s", UNIT_NAME(sarg->activator));
               break;

            case 2:
               sprintf(buf, "ruffle %s", UNIT_NAME(sarg->activator));
               break;

            case 3:
               sprintf(buf, "bow %s...", UNIT_NAME(sarg->activator));
               break;
         }
         od->patient = sarg->activator;
         set_delayed_action(sarg->owner, buf);
         comms = 0;
         eliza_log(sarg->owner, nullptr, comms);
         eliza_log(sarg->owner, "========== oOo ============", comms);
         SET_BIT(sarg->fptr->flags, SFB_PRIORITY);
      }
      return SFR_SHARE;
   }

   if(IS_PC(sarg->activator) || ELIZA_DEBUG)
   {
      sprintf(buf, "%s %s", sarg->cmd->cmd_str, sarg->arg);
      eliza_log(sarg->activator, buf, comms);
   }

   if((sarg->cmd->no == CMD_TELL) || (sarg->cmd->no == CMD_ASK) || (sarg->cmd->no == CMD_WHISPER))
   {
      unit_data *u;
      char      *c = (char *)sarg->arg;

      u            = find_unit(sarg->activator, &c, nullptr, FIND_UNIT_SURRO);
      if(u != sarg->owner)
      {
         return SFR_SHARE;
      }
   }
   else if((sarg->cmd->no != CMD_SAY) && (sarg->cmd->no != CMD_SHOUT))
   {
      return SFR_SHARE;
   }

   comms++;

   strcpy(buf, sarg->arg);
   response = eliza_process(od, buf);
   if(response == nullptr)
   {
      return SFR_SHARE;
   }

   if(IS_PC(sarg->activator) || ELIZA_DEBUG)
   {
      eliza_log(sarg->owner, response, comms);
   }

   set_delayed_action(sarg->owner, response);

   return SFR_SHARE;
}

/* ====================================================================== */
auto eliza_find_template(int subjno) -> struct template_type *
{
   int i;

   for(i = 0; i < eliza_maxtemplates; i++)
   {
      if(eliza_template[i].subjno == subjno)
      {
         return &eliza_template[i];
      }
   }

   eliza_maxtemplates++;
   if(eliza_maxtemplates == 1)
   {
      CREATE(eliza_template, struct template_type, 1);
   }
   else
   {
      RECREATE(eliza_template, struct template_type, eliza_maxtemplates);
   }

   eliza_template[eliza_maxtemplates - 1].subjno = subjno;
   eliza_template[eliza_maxtemplates - 1].exp    = create_namelist();

   return &eliza_template[eliza_maxtemplates - 1];
}

auto eliza_find_keyword(int subjno) -> struct keyword_type *
{
   int i;

   for(i = 0; i < eliza_maxkeywords; i++)
   {
      if(eliza_keyword[i].subjno == subjno)
      {
         return &eliza_keyword[i];
      }
   }

   eliza_maxkeywords++;
   if(eliza_maxkeywords == 1)
   {
      CREATE(eliza_keyword, struct keyword_type, 1);
   }
   else
   {
      RECREATE(eliza_keyword, struct keyword_type, eliza_maxkeywords);
   }

   eliza_keyword[eliza_maxkeywords - 1].keyword  = create_namelist();
   eliza_keyword[eliza_maxkeywords - 1].priority = '5';
   eliza_keyword[eliza_maxkeywords - 1].subjno   = subjno;

   return &eliza_keyword[eliza_maxkeywords - 1];
}

void eliza_get_template(char *buf, int subjno)
{
   struct template_type *tem;
   char                 *b;
   char                  tmp[400];
   int                   i;

   tem = eliza_find_template(subjno);
   b   = buf;

   for(; static_cast<unsigned int>(str_is_empty(b)) == 0U;)
   {
      for(i = 0; b[i] != 0; i++)
      {
         tmp[i] = b[i];
         if(b[i] == '%')
         {
            break;
         }
      }
      tmp[i] = 0;

      if(static_cast<unsigned int>(str_is_empty(tmp)) == 0U)
      {
         tem->exp = add_name(tmp, tem->exp);
      }

      if(b[i] == '%')
      {
         tem->exp = add_name("", tem->exp);
         b += i + 1;
      }
      else
      {
         b += i;
      }
   }
}

void eliza_get_keyword(char *buf, int subjno, int priority)
{
   struct keyword_type *kwd;

   kwd           = eliza_find_keyword(subjno);

   kwd->priority = priority;
   kwd->keyword  = add_name(buf, kwd->keyword);
}

void eliza_get_reacts(FILE *f, int subjno)
{
   char buf[240];
   int  i;
   int  cnt;
   int  priority;
   char c;

   priority = '5';

   for(cnt = 1;; cnt++)
   {
      c = fgetc(f);
      if(c == '*')
      {
         ungetc(c, f);
         return;
      }

      char *ms2020         = fgets(buf, 240, f);
      buf[strlen(buf) - 1] = 0; /* Destroy the newline from fgets */

      for(i = 0; buf[i] != 0; i++)
      {
         if(isdigit(buf[i]) != 0)
         {
            priority = buf[i];
            buf[i]   = 0;
         }
      }

      while(buf[--i] == ' ')
      {
         buf[i] = 0;
      }

      str_chraround(buf, ' ');
      str_remspc(buf);

      switch(c)
      {
         case '+':
            eliza_get_keyword(buf, subjno, priority);
            break;

         case '#':
            eliza_get_template(buf, subjno);
            break;

         default:
            slog(LOG_ALL, 0, "Illegal eliza-code: %s", buf);
            assert(FALSE);
      }
   }
}

void eliza_get_subjects(FILE *f)
{
   char buf[240];

   eliza_maxsubjects++;
   if(eliza_maxsubjects == 1)
   {
      CREATE(eliza_subjects, struct subject_type, 1);
   }
   else
   {
      RECREATE(eliza_subjects, struct subject_type, eliza_maxsubjects);
   }

   eliza_subjects[eliza_maxsubjects - 1].replies = create_namelist();

   for(;;)
   {
      buf[0] = fgetc(f);
      if(buf[0] != '*')
      {
         ungetc(buf[0], f);
         break;
      }

      char *ms2020                                  = fgets(buf, 240, f);
      buf[strlen(buf) - 1]                          = 0; /* Destroy the newline from fgets */

      eliza_subjects[eliza_maxsubjects - 1].replies = add_name(buf, eliza_subjects[eliza_maxsubjects - 1].replies);
   }
}

void eliza_gen_test_template(char *buf, struct template_type *tem)
{
   int j;

   for(j = 0; tem->exp[j] != nullptr; j++)
   {
      if(*tem->exp[j] == 0)
      {
         strcpy(buf, " michael ");
      }
      else
      {
         strcpy(buf, tem->exp[j]);
      }
      TAIL(buf);
   }
}

void eliza_integrity()
{
   int  i;
   int  j;
   int  k;
   int  l;
   char buf[240];

   /* Test if any templates are overshadowed by previous templates */
   for(i = 1; i < eliza_maxtemplates; i++)
   {
      eliza_gen_test_template(buf, &eliza_template[i]);
      j = trytempl(buf);

      if((j != -1) && (j < i))
      {
         slog(LOG_ALL, 0, "Eliza Template: '%s' conflicts with '%s/%s'", buf, STR(eliza_template[j].exp[0]), STR(eliza_template[j].exp[1]));
      }
   }

   /* Test if any keywords are overshadowed by templates */
   for(i = 1; i < eliza_maxkeywords; i++)
   {
      for(j = 0; eliza_keyword[i].keyword[j] != nullptr; j++)
      {
         strcpy(buf, eliza_keyword[i].keyword[j]);
         k = trytempl(buf);

         if(k != -1)
         {
            slog(LOG_ALL,
                 0,
                 "Eliza Keyword '%s' may be shadowed by '%s/%s'",
                 buf,
                 STR(eliza_template[k].exp[0]),
                 STR(eliza_template[k].exp[1]));
         }
      }
   }

   /* Test if any keywords are overshadowed by keywords */
   for(i = 1; i < eliza_maxkeywords; i++)
   {
      for(j = 0; eliza_keyword[i].keyword[j] != nullptr; j++)
      {
         strcpy(buf, eliza_keyword[i].keyword[j]);
         k = trykeywd(buf, &l);

         if(k != i)
         {
            slog(LOG_ALL, 0, "Eliza Keyword '%s' may be shadowed by keywd '%s'", buf, STR(eliza_keyword[k].keyword[j]));
         }
      }
   }
}

void eliza_boot()
{
   FILE *f;
   char  c;
   int   i;

   slog(LOG_ALL, 0, "Booting Eliza");

   if((f = fopen(str_cc(libdir, ELIZA_TALKFILE), "r")) == nullptr)
   {
      slog(LOG_ALL, 0, "ELIZA BOOT FILE NOT FOUND.");
      return;
   }

   for(i = 0; feof(f) == 0; i++)
   {
      for(; feof(f) == 0;)
      {
         c = fgetc(f);

         if(isspace(c) == 0)
         {
            ungetc(c, f);
            break;
         }
      }

      if(feof(f) != 0)
      {
         break;
      }

      eliza_get_reacts(f, i);
      eliza_get_subjects(f);
   }

   fclose(f);

   /* eliza_integrity(); I know it's ok now... Use once when mod. talk.eli */

   slog(LOG_ALL, 0, "Booting Eliza Done.");
}
