#include "comm.h"
#include "config.h"
#include "cQueueElem.h"
#include "CServerConfiguration.h"
#include "db_file.h"
#include "handler.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "trie.h"
#include "trie_type.h"
#include "unit_fptr.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_ALIAS_LENGTH  10   /* Max length of alias */
#define MAX_ALIAS_COUNT   2000 /* Max room for string saved to file */
#define ALIAS_UPPER_BOUND 50   /* Max number of nodes in alias call-tree */
#define ALIAS_NAME                                                                                                                         \
   21 /* Length of extra string allocated for the                                                                                          \
       * name of dictionary owner + 2                                                                                                      \
       * Unforgiveably hardcoded in a str_to_alias                                                                                         \
       * call of sscanf()...                                                                                                               \
       */
struct alias_head
{
   uint16_t   char_count;
   trie_type *trie;
   char       owner[20];
};

struct alias_t
{
   char *key;
   char *value;
};

/* Procedure supplied to free_trie and del_trie */
void free_alias(void *data)
{
   auto *al = (struct alias_t *)data;

   free(al->key);
   free(al->value);
   free(al);
}

/* Word is here defined as an unbroken sequence of nonspaces */
static auto get_next_word(const char *argument, char *first_arg) -> char *
{
   /* Find first non blank */
   argument = skip_spaces(argument);

   /* Copy next word and make it lower case */
   for(; (*argument != 0) && (isspace(*argument) == 0); argument++)
   {
      *first_arg++ = tolower(*argument);
   }

   *first_arg = '\0';

   return (char *)argument;
}

/* Setup owner-ship of dictionary. */
static void set_owner(unit_data *obj, struct alias_head *ah, unit_data *ch)
{
   char buf[256];

   strcpy(ah->owner, UNIT_NAME(ch));

   sprintf(buf, "On the ground lies %s's %s.", ah->owner, UNIT_NAME(obj));
   UNIT_OUT_DESCR(obj).Reassign(buf);

   sprintf(buf, "%s's %s", ah->owner, UNIT_NAME(obj));
   UNIT_TITLE(obj).Reassign(buf);
}

/* Allocate `exd' if needed, and erase old description */
static auto fix_extra_descr(unit_data *obj, class extra_descr_data *exd) -> extra_descr_data *
{
   static const char *aliaslist[] = {"$alias", nullptr};

   if(exd == nullptr)
   {
      exd = UNIT_EXTRA_DESCR(obj) = UNIT_EXTRA_DESCR(obj)->add(aliaslist, nullptr);
   }

   return exd;
}

/* Parse the alias, according to the &-rule sketched out in help-page */
static auto parse_alias(char *src, char *arg) -> char *
{
   static char buf[2 * MAX_INPUT_LENGTH + 2];
   char       *argv[10];
   char       *cnew = buf;
   int         argc;
   int         i = 0;

   for(argc = 1; argc < 10; argc++) /* store word-pointers in argv */
   {
      while((*arg != 0) && (isspace(*arg) != 0))
      {
         arg++; /* skip spaces */
      }
      argv[argc] = arg; /* record first non-space pos. */
      while((*arg != 0) && (isspace(*arg) == 0))
      {
         arg++; /* skip non-spaces */
      }
   }

   for(argc = 1; (*cnew = *src) != 0;) /* check src for `&' substitution */
   {
      if(*src++ == '&')
      {
         if((isdigit(*src) != 0) && (i = *src - '0') > 0) /* Digit picks argument */
         {
            get_next_word(argv[i], cnew);
            src++;
         }
         else if(strchr(src, '&') != nullptr)
         { /* Current `&' isn't the last */
            get_next_word(argv[argc++], cnew);
         }
         else
         { /* It is the last, so it takes rest of argument */
            strcpy(cnew, argv[argc]);
         }
         TAIL(cnew);
      }
      else
      {
         cnew++;
      }

      if(cnew - buf >= (int32_t)sizeof(buf) / 2 - 1)
      {
         return nullptr; /* we wrote too much, so abort */
      }
   }

   return buf;
}

/*  Parse the alias-substitution in s, using the arguments from arg, and
 *  looking up nested alias-calls in the alias-trie t.
 *  check_count makes sure that the user doesn't make nasty (!) computationally
 *  heavy (!!!) aliases that will bog the mud enormously.
 */
static auto push_alias(char *s, char *arg, trie_type *t, unit_data *ch, bool first) -> int
{
   char            cmd[MAX_INPUT_LENGTH + 1];
   char            parsed[2 * MAX_INPUT_LENGTH + 2];
   char           *par;
   char           *newarg;
   char           *c;
   struct alias_t *al;

   static uint8_t  check_count = 0;

   if(first)
   {
      check_count = 0;
   }
   else if(check_count++ > ALIAS_UPPER_BOUND)
   {
      send_to_char("Alias was too deeply nested. Aborted.\n\r", ch);
      return -1;
   }

   do
   {
      /* Cut off at semi-colons, but remember position for next iteration */
      if((c = strchr(s, ';')) != nullptr)
      {
         *c = '\0';
      }

      /* Parse the alias substitution */
      if((par = parse_alias(s, arg)) == nullptr)
      {
         send_to_char("Overflow while parsing alias. Aborted.\n\r", ch);
         return -1;
      }

      /* Try to parse result of parsing (!) */
      strcpy(parsed, par);
      newarg = (char *)get_next_word(parsed, cmd);

      if((al = (struct alias_t *)search_trie(cmd, t)) != nullptr)
      {
         if(push_alias(al->value, newarg, t, ch, FALSE) < 0)
         {
            return -1;
         }
      }
      else
      { /* not a new alias, so it's probably a command */
         CHAR_DESCRIPTOR(ch)->qInput.Append(new cQueueElem(par));
      }

      if(c != nullptr)
      { /* Restore the semi-colon */
         *c = ';';
      }

      s = c + 1;
   } while(c != nullptr);

   return 0;
}

/* Merely prints a formatted alias-definition out to the char */
static void alias_to_char(struct alias_t *al, unit_data *ch)
{
   char buf[2 * MAX_INPUT_LENGTH + 2];

   sprintf(buf, " %-*s%s", MAX_ALIAS_LENGTH + 5, al->key, al->value);
   act("$2t", A_ALWAYS, ch, buf, nullptr, TO_CHAR);
}

/*  Prints all defined aliases in `t' alphabetically to char by
 *  recursively walking the trie
 */
static auto print_alias(trie_type *t, unit_data *ch) -> int
{
   trie_type      *t2;
   struct alias_t *al;
   int             i;
   int             count = 0;

   for(i = 0; (t != nullptr) && i < t->size; i++)
   {
      t2 = t->nexts[i].t;

      if((al = (struct alias_t *)t2->data) != nullptr)
      {
         alias_to_char(al, ch);
         count++;
      }
      count += print_alias(t2, ch);
   }
   return count;
}

/*  Add the new alias `key' to `ah'.
 *  single says if we plan to add only one alias or several at `boottime'.
 *  If false, it is assumed the user sorts the trie himself
 */
static auto add_alias(struct alias_head *ah, char *key, char *val, bool single) -> int
{
   struct alias_t *al = nullptr;
   struct alias_t *tmp_al;

   /* There is already an alias for key - Delete it */
   if(single && (ah->trie != nullptr) && ((al = (struct alias_t *)search_trie(key, ah->trie)) != nullptr))
   {
      free(al->value);
      al->value = str_dup(val);
      return 0;
   }

   CREATE(tmp_al, struct alias_t, 1);
   tmp_al->key   = str_dup(key);
   tmp_al->value = str_dup(val);
   ah->char_count += (strlen(key) + strlen(val) + 2);

   ah->trie = add_trienode(key, ah->trie);
   if(single)
   {
      qsort_triedata(ah->trie);
   }
   set_triedata(key, ah->trie, tmp_al, TRUE);

   return 1;
}

/* Delete an alias, using the free_alias procedure */
static auto del_alias(struct alias_head *ah, char *key) -> bool
{
   struct alias_t *al = nullptr;

   if((ah->trie != nullptr) && ((al = (struct alias_t *)search_trie(key, ah->trie)) != nullptr))
   {
      ah->char_count -= (strlen(al->key) + strlen(al->value) + 2);

      del_trie(key, &ah->trie, free_alias);

      return TRUE;
   }

   return FALSE;
}

/*  Parse the alias to see if `val' calls something that will eventually call
 *  `key', or if it contains state-changing commands.
 *  check_count makes sure that the user doesn't make nasty (!) computationally
 *  heavy (!!!) aliases that will bog the mud enormously.
 */
static auto circle_alias(char *key, char *val, trie_type *t, bool first) -> uint8_t
{
   char           *tmp;
   char           *sc;
   char            comm[MAX_INPUT_LENGTH + 2];
   struct alias_t *tmp_al      = nullptr;
   uint8_t         res         = 0;
   static uint8_t  check_count = 0;

   if(first)
   {
      check_count = 0;
   }
   else if(check_count++ > ALIAS_UPPER_BOUND)
   {
      return 3;
   }

   tmp = val;

   do
   {
      if((sc = strchr(tmp, ';')) != nullptr)
      {
         *sc = '\0';
      }

      get_next_word(tmp, comm);

      if(sc != nullptr)
      {
         *sc = ';';
      }

      if(str_ccmp(comm, key) == 0)
      {
         return 1;
      }
      if(first && (is_abbrev(comm, "alias") || is_abbrev(comm, "unalias")))
      {
         return 2;
      }
      if(first && *comm == '!')
         return 4;
      else if(first && is_abbrev(comm, "shout"))
         return 5;

      if((t != nullptr) && ((tmp_al = (struct alias_t *)search_trie(comm, t)) != nullptr) &&
         ((res = circle_alias(key, tmp_al->value, t, FALSE)) != 0U))
      {
         return res;
      }

      tmp = sc + 1;
   } while(sc != nullptr);

   return 0;
}

/*  Is `key' a legal new alias or not?
 *  This should all be linear, except for the recursion check,
 *  which has a fixed upper bound.
 */
static auto alias_is_ok(struct alias_head *ah, char *key, char *val, unit_data *ch) -> bool
{
   char           *tmp;
   struct alias_t *al = nullptr;
   int             count;

   extern int      bModeBBS;

   if(strlen(key) > MAX_ALIAS_LENGTH)
   {
      count = MAX_ALIAS_LENGTH;

      act("Aliasname too long. Max $2d chars.", A_ALWAYS, ch, &count, nullptr, TO_CHAR);
      return FALSE;
   }

   count = strlen(key) + strlen(val) + ah->char_count + 3;
   if((ah->trie != nullptr) && ((al = (struct alias_t *)search_trie(key, ah->trie)) != nullptr))
   {
      count -= strlen(al->value);
   }

   if(count > MAX_ALIAS_COUNT)
   {
      send_to_char("You will have to delete another alias first.\n\r", ch);
      return FALSE;
   }

   if((static_cast<unsigned int>(is_abbrev(key, "alias")) != 0U) || (static_cast<unsigned int>(is_abbrev(key, "unalias")) != 0U))
   {
      send_to_char("Alas, but that is a reserved name.\n\r", ch);
      return FALSE;
   }

   for(count = 0, tmp = val; *tmp != 0; tmp++)
   {
      switch(*tmp)
      {
         case '~':
            send_to_char("Sorry, `~' is used as a control character.\n\r", ch);
            return FALSE;

         case ';':
            if(*skip_spaces(tmp + 1) == ';')
            {
               send_to_char("Semicolon error in alias.\n\r", ch);
               return FALSE;
            }
            break;

         case '&':
            if((isdigit(tmp[1]) == 0) && ++count > 9)
            {
               send_to_char("Sorry, `&' can't be used more than 9 times.\n\r", ch);
               return FALSE;
            }
      }
   }

   switch(circle_alias(key, val, ah->trie, TRUE))
   {
      case 1:
         send_to_char("Aliases are not allowed to call in circle.\n\r", ch);
         return FALSE;
      case 2:
         send_to_char("Aliases can't be defined or erased within an alias.\n\r", ch);
         return FALSE;
      case 3:
         send_to_char("You're not allowed to nest aliases that deep.\n\r", ch);
         return FALSE;
      case 4:
         send_to_char("You can't use '!' from within an alias.\n\r", ch);
         return FALSE;
      case 5:
         if(g_cServerConfig.m_bAliasShout == 0)
         {
            send_to_char("Using `shout' in an alias is not very nice...\n\r", ch);
            return FALSE;
         }
   }

   return TRUE;
}

/*  This walks the trie recursively to note down the aliases in the buffer
 *  pointed to by bufp
 */
static void rec_alias_to_str(trie_type *t, char **bufp)
{
   int             i;
   struct alias_t *al;

   if(t != nullptr)
   {
      if((al = (struct alias_t *)t->data) != nullptr)
      {
         sprintf(*bufp, "%s %s~", al->key, al->value);
         TAIL(*bufp);
      }

      for(i = 0; i < t->size; i++)
      {
         rec_alias_to_str(t->nexts[i].t, bufp);
      }
   }
}

/* Convert a trie to a string.  Needed for saving the aliases */
static auto alias_to_str(struct alias_head *ah) -> char *
{
   static char buf[MAX_ALIAS_COUNT + ALIAS_NAME + 2 + 500];
   char       *bufp = buf;

   sprintf(bufp, "%s~ ", ah->owner); /* Note down owner of dictionary */
   TAIL(bufp);

   rec_alias_to_str(ah->trie, &bufp);

   *bufp = '\0';

   return buf;
}

/* Take the above constructed string and turn it into an alias-set. */
static auto str_to_alias(const char *str) -> struct alias_head *
{
   struct alias_head *ah;
   char               cmd[MAX_ALIAS_LENGTH + 1];
   char              *tmp;
   char              *tilde;

   CREATE(ah, struct alias_head, 1);
   ah->char_count = 0;
   ah->trie       = nullptr;
   strcpy(ah->owner, "none");

   if(str != nullptr)
   {
      /*  This takes the first word of the alias-string.
       *  If it's tilde-terminated, it's the name of the owner.
       *  Otherwise it's an old-style dictionary, which will be throw into
       *  space, where it will die like a miserable pig.
       */

      if(sscanf(str, "%19s", ah->owner) == 1 && ah->owner[strlen(ah->owner) - 1] == '~')
      {
         str += strlen(ah->owner);
         str                              = skip_spaces(str);

         ah->owner[strlen(ah->owner) - 1] = '\0'; /* Cut of tilde */
      }
      else
      {
         sprintf(ah->owner, "old_hat");
      }

      while(*str != 0)
      {
         tilde  = (char *)strchr(str, '~'); /* Find first tilde in string */
         *tilde = '\0';
         tmp    = get_next_word(str, cmd); /* Get alias-name */

         /* Add alias, but don't sort the trie yet */
         add_alias(ah, cmd, skip_spaces(tmp), FALSE);
         *tilde = '~';
         str    = tilde + 1;
      }

      if(ah->trie != nullptr)
      {
         qsort_triedata(ah->trie);
      }
   }

   return ah;
}

/* ********** The Alias Commands ********** */

static void cmd_alias(unit_data *ch, char *arg, struct alias_head *alias_h)
{
   char            comm[MAX_INPUT_LENGTH + 1];
   struct alias_t *al = nullptr;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      /* No argument lists the defined aliases */
      if(print_alias(alias_h->trie, ch) == 0)
      {
         send_to_char("No aliases defined.\n\r", ch);
         if(alias_h->char_count > 0)
         {
            slog(LOG_BRIEF, 0, "%s: No aliases, but %d in char_count.", __FILE__, alias_h->char_count);
         }
      }

      return;
   }

   /* Cut off first argument */
   arg = get_next_word(arg, comm);

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      /* No further arguments lists this alias, if defined */
      if(alias_h->trie == nullptr || (al = (struct alias_t *)search_trie(comm, alias_h->trie)) == nullptr)
      {
         act("No alias defined for `$2t'.", A_ALWAYS, ch, comm, nullptr, TO_CHAR);
      }
      else
      {
         alias_to_char(al, ch);
      }

      return;
   }

   /* There's a new alias to re/define */
   arg = skip_spaces(arg);

   if(alias_is_ok(alias_h, comm, arg, ch))
   {
      int status = add_alias(alias_h, comm, arg, TRUE);

      act("Alias for `$2t' $3t.", A_ALWAYS, ch, comm, status != 0 ? "added" : "changed", TO_CHAR);
   }
}

static void cmd_unalias(unit_data *ch, char *arg, struct alias_head *alias_h)
{
   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      act("Unalias what??", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
   }
   else
   {
      char comm[MAX_INPUT_LENGTH + 1];

      /* unalias cycles through all its arguments and undefines them */
      do
      {
         arg = get_next_word(arg, comm);

         act(del_alias(alias_h, comm) ? "Alias `$2t' deleted." : "No alias defined for `$2t'.", A_ALWAYS, ch, comm, nullptr, TO_CHAR);
      } while(static_cast<unsigned int>(str_is_empty(arg)) == 0U);
   }
}

static void cmd_claim(unit_data *ch, char *arg, unit_data *obj, struct alias_head *alias_h)
{
   char buf[MAX_INPUT_LENGTH + 1];

   one_argument(arg, buf);

   if((static_cast<unsigned int>(str_is_empty(buf)) != 0U) || (UNIT_NAMES(obj).IsName(buf) == nullptr))
   {
      act("You can only claim $2n.", A_ALWAYS, ch, obj, nullptr, TO_CHAR);
   }
   else
   {
      act("You claim $2n as your property.", A_ALWAYS, ch, obj, nullptr, TO_CHAR);
      set_owner(obj, alias_h, ch);
   }
}

/* ********** The Dictionary Specproc ********** */

static auto local_dictionary(struct spec_arg *sarg) -> int
{
   char              *pcomm = nullptr;
   char              *cmd_array[256];
   uint8_t            i;
   struct alias_t    *al = nullptr;
   struct alias_head *alias_h;
   extra_descr_data  *exd;

   /* specproc initialization */
   if((alias_h = (struct alias_head *)sarg->fptr->data) == nullptr)
   {
      exd              = UNIT_EXTRA_DESCR(sarg->owner)->find_raw("$alias");
      sarg->fptr->data = str_to_alias(exd != nullptr ? exd->descr.String() : nullptr);
      alias_h          = (struct alias_head *)sarg->fptr->data;
   }

   /* clean up if destroyed */
   if(sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if(alias_h->trie != nullptr)
      {
         free_trie(alias_h->trie, free_alias);
      }
      free(alias_h);
      sarg->fptr->data = nullptr;
      return SFR_BLOCK;
   }

   /* create a snapshot of alias-set if saved */
   if(sarg->cmd->no == CMD_AUTO_SAVE)
   {
      char *tmp;

      exd = UNIT_EXTRA_DESCR(sarg->owner)->find_raw("$alias");

      if(exd == nullptr)
      {
         exd = fix_extra_descr(sarg->owner, exd);
      }

      tmp = alias_to_str(alias_h);
      assert(strlen(tmp) < MAX_ALIAS_COUNT + ALIAS_NAME + 2 + 500);
      exd->descr.Reassign(tmp);

      return SFR_SHARE;
   }

   /* Not an applicaple command */
   if((sarg->activator == nullptr) || !IS_CHAR(sarg->activator) || (CHAR_DESCRIPTOR(sarg->activator) == nullptr) ||
      sarg->activator != UNIT_IN(sarg->owner))
   {
      return SFR_SHARE;
   }

   /* Check for the dictionary-commands */

   /* The alias command */
   if(static_cast<unsigned int>(is_command(sarg->cmd, "alias")) != 0U)
   {
      /* This check is needed too, so people can't wreak havoc with another
       * person's dictionary...
       */
      if(str_ccmp(UNIT_NAME(sarg->activator), alias_h->owner) != 0)
      {
         act("You can't use the alias command before you type `claim $3N'.", A_ALWAYS, sarg->activator, nullptr, sarg->owner, TO_CHAR);
      }
      else
      {
         cmd_alias(sarg->activator, (char *)sarg->arg, alias_h);
      }

      return SFR_BLOCK;
   }
   /* The unalias command */
   if(is_command(sarg->cmd, "unalias"))
   {
      cmd_unalias(sarg->activator, (char *)sarg->arg, alias_h);
      return SFR_BLOCK;
   }
   /* The claim command is used to `accept' the current dictionary */
   if(is_command(sarg->cmd, "claim"))
   {
      cmd_claim(sarg->activator, (char *)sarg->arg, sarg->owner, alias_h);
      return SFR_BLOCK;
   }

   /*  Have to get the typed command from descriptor.last_cmd,
    *  as the user might have typed `in', which the interpreter expands
    *  to `inventory' in the cmd->cmd_str.
    */
   if(alias_h->trie == nullptr ||
      (al = (struct alias_t *)search_trie(CHAR_DESCRIPTOR(sarg->activator)->last_cmd, alias_h->trie)) == nullptr)
   {
      return SFR_SHARE;
   }

   /* We now have an alias to parse... */

   /* Check if this dictionary belongs to user */
   if(str_ccmp(UNIT_NAME(sarg->activator), alias_h->owner) != 0)
   {
      act(
         "You can't use the alias `$2t' before you type `claim $3N'.", A_ALWAYS, sarg->activator, sarg->cmd->cmd_str, sarg->owner, TO_CHAR);
      return SFR_BLOCK;
   }

   sarg->arg = skip_spaces(sarg->arg);

   /*  In the unlikely (?) event that one or more commands sneak in on the
    *  command queue before we get to to process, we have to empty it...
    */
   for(i = 0; CHAR_DESCRIPTOR(sarg->activator)->qInput.IsEmpty() == 0;)
   {
      if(i < 256) /* 256 should certainly be enough, but check anyway */
      {
         struct cQueueElem *qe = CHAR_DESCRIPTOR(sarg->activator)->qInput.GetHead();
         cmd_array[i++]        = (char *)qe->Data();
         qe->SetNull();
         delete qe;
      }
   }

   /* Now do the alias trickery */
   if(push_alias(al->value, (char *)sarg->arg, alias_h->trie, sarg->activator, TRUE) == 0)
   {
      /* Execute first of `derived' commands to avoid silly `command lag'
       * This is very ugly
       */
      if(CHAR_DESCRIPTOR(sarg->activator)->qInput.IsEmpty() == 0)
      {
         struct cQueueElem *qe = CHAR_DESCRIPTOR(sarg->activator)->qInput.GetHead();
         pcomm                 = (char *)qe->Data();
         qe->SetNull();
         delete qe;
      }
   }

   /* Put popped commands back on queue and clean up */
   while((i--) != 0U)
   {
      CHAR_DESCRIPTOR(sarg->activator)->qInput.Append(new cQueueElem(cmd_array[i]));
   }

   /* Ok, now we can safely call command interpreter. */
   if(pcomm != nullptr)
   {
      if(*(skip_spaces(pcomm)) == '!')
      {
         send_to_char("Whoops, you just called an alias with '!' in it!\n\r", sarg->activator);
      }
      else
      {
         command_interpreter(sarg->activator, pcomm);
      }
      free(pcomm);
   }

   return SFR_BLOCK;
}

auto dictionary(struct spec_arg *sarg) -> int
{
   /* Recursive calls can occur via 'at':

      alias mail at hansen mail paper papi

      (since mail is in the "at" command!).
      So Gnort's routine wont be able to catch it. I'll make a double
      check here for no more than 100 nests. */

   static int recursive_calls = 0;
   int        n               = SFR_BLOCK;

   recursive_calls++;

   if(recursive_calls > 100)
   {
      send_to_char("Alias is recursive, it has been terminated!\n\r", sarg->activator);
   }
   else
   {
      n = local_dictionary(sarg);
   }

   recursive_calls--;

   return n;
}
