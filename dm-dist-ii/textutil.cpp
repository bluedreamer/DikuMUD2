#include "textutil.h"

#include "common.h"
#include "structs.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*  From char * input stream 'str' copy characters into 'buf' until
 *  end of string or newline. Returns position of 'str' after copied
 *  characters.
 */
auto str_line(const char *str, char *buf) -> char *
{
   if(str == nullptr || buf == nullptr)
   {
      return nullptr;
   }

   for(; *str == ' ' || *str == '\n' || *str == '\r'; str++)
   {
      ;
   }

   if(*str == '\0')
   {
      return nullptr;
   }

   for(; (*buf = *str) != 0; buf++, str++)
   {
      if(*str == '\n' || *str == '\r')
      {
         break;
      }
   }

   *buf = '\0'; /* Erase last separator */

   return (char *)str;
}

/* Make a string lowercase, and also return length of string */
auto str_lower(char *s) -> int
{
   int l;

   for(l = 0; *s != 0; s++, l++)
   {
      if(isupper(*s) != 0)
      {
         *s = tolower(*s);
      }
   }

   return l;
}

/* Make a string lowercase in another buffer. Also return length of string */

auto str_lower(const char *s, char *d, int nBufSize) -> int
{
   int l;

   for(l = 0; (*s != 0) && (l < nBufSize); s++, d++, l++)
   {
      if(isupper(*s) != 0)
      {
         *d = tolower(*s);
      }
      else
      {
         *d = *s;
      }
   }

   if(l < nBufSize)
   {
      *d = 0;
   }
   else
   {
      slog(LOG_ALL,
           0,
           "ERROR: str_lower destination buffer too "
           "small! Advise Papi.");
      *(d - 1) = 0;
   }

   return l;
}

/* Return a string consisting of `n' spaces */
auto spc(int n) -> char *
{
   static char buf[256];

   buf[n] = '\0';

   if(n > 256)
   {
      n = 256;
   }

   for(; n != 0;)
   {
      buf[--n] = ' ';
   }

   return buf;
}

/*  Return a pointer to the string containing the ascii reresentation
 *  of the integer 'n'
 *  I've made it the easy way :)
 */
#ifdef DOS
char *itoa_dos(int n)
#else
auto itoa(int n) -> char *
#endif
{
   static char buf[32]; /* 32 digits can even cope with 64 bit ints */

   sprintf(buf, "%d", n);
   return buf;
}

/*  Return a pointer to the string containing the ascii reresentation
 *  of the integer 'n'
 *  I've made it the easy way :)
 */
auto ltoa(long n) -> char *
{
   static char buf[32]; /* 32 digits can even cope with 64 bit ints */

   sprintf(buf, "%ld", n);
   return buf;
}

/*  STR Convention: "str_" [n] [c] <meaning>
 *  n means - take number as argument, to limit operation
 *  c means - ignore possible difference in case sensitivity
 *  n always precedes c
 *  Example: str_nccmp - compare n letters non-casesensitive
 */

/*  Compare two strings without case-sensitivity
 *  Return result as defined in strcmp
 */
auto str_ccmp(const char *s, const char *d) -> int
{
   if(s == d)
   {
      return 0;
   }

   if(s == nullptr)
   {
      return -1;
   }
   if(d == nullptr)
   {
      return 1;
   }

   for(; tolower(*s) == tolower(*d); s++, d++)
   {
      if(*s == '\0')
      {
         return 0;
      }
   }

   return (tolower(*s) - tolower(*d));
}

/*  Compare max n chars without case-sensitivity
 *  Return result as defined in strcmp
 */
auto str_nccmp(const char *s, const char *d, int n) -> int
{
   if(s == d)
   {
      return 0;
   }

   if(s == nullptr)
   {
      return -1;
   }
   if(d == nullptr)
   {
      return 1;
   }

   for(n--; tolower(*s) == tolower(*d); s++, d++, n--)
   {
      if(*s == '\0' || n <= 0)
      {
         return 0;
      }
   }

   return (tolower(*s) - tolower(*d));
}

/* Allocate space for a copy of source */
auto str_dup(const char *source) -> char *
{
   char *dest;

   if(source != nullptr)
   {
      CREATE(dest, char, strlen(source) + 1);
      strcpy(dest, source);

      return dest;
   }

   return nullptr;
}

/*  As defined by 2nd Ed. of K&R ANSI C
 *  Return pointer to first occurence of ct in cs - or NULL
 *  Used to determine ei. "from" and "in"
 */
auto str_str(const char *cs, const char *ct) -> char *
{
   char *si;
   char *ti;

   do
   {
      si = (char *)cs;
      ti = (char *)ct;

      while(*si++ == *ti++)
      {
         if(*ti == '\0')
         {
            return (char *)cs;
         }
      }

   } while(*cs++ != 0);

   return nullptr;
}

/*  As defined by 2nd Ed. of K&R ANSI C, but non case sensitive
 *  Return pointer to first occurence of ct in cs - or NULL
 *  Used to determine ei. "from" and "in"
 */
auto str_cstr(const char *cs, const char *ct) -> char *
{
   char *si;
   char *ti;

   do
   {
      for(si = (char *)cs, ti = (char *)ct; tolower(*si) == tolower(*ti); si++)
      {
         if(*++ti == '\0')
         {
            return (char *)cs;
         }
      }
   } while(*cs++ != 0);

   return nullptr;
}

/* return string without leading spaces */
auto skip_blanks(const char *string) -> char *
{
   if(string == nullptr)
   {
      return nullptr;
   }

   for(; (*string != 0) && (isspace(*string) != 0); string++)
   {
      ;
   }

   return (char *)string;
}

/* return string without leading spaces */
auto skip_spaces(const char *string) -> char *
{
   if(string == nullptr)
   {
      return nullptr;
   }

   for(; (*string != 0) && isaspace(*string); string++)
   {
      ;
   }

   return (char *)string;
}

void strip_trailing_blanks(char *str)
{
   if(*str == 0)
   { /* empty string: return at once      */
      return;
   }

   for(; *str != 0; ++str)
   { /* wind to end of string             */
      ;
   }

   if(isspace(*--str) == 0)
   {          /* Not a spaceterminated string      */
      return; /* This is mainly for `inter-code' strings */
   }

   while(isspace(*--str) != 0)
   { /* rewind to last nonspace character */
      ;
   }

   *++str = '\0'; /* step ahead and end string         */
}

void strip_trailing_spaces(char *str)
{
   if(*str == 0)
   { /* empty string: return at once      */
      return;
   }

   for(; *str != 0; ++str)
   { /* wind to end of string             */
      ;
   }

   if(!isaspace(*--str))
   {          /* Not a spaceterminated string      */
      return; /* This is mainly for `inter-code' strings */
   }

   while(isaspace(*--str))
   { /* rewind to last nonspace character */
      ;
   }

   *++str = '\0'; /* step ahead and end string         */
}

/* Returns true is arg is empty */
auto str_is_empty(const char *arg) -> bool
{
   if(arg == nullptr)
   {
      return true;
   }

   return static_cast<bool>(*(skip_blanks(arg)) == '\0');
}

/* Check if the string contains nothing but numbers */
auto str_is_number(const char *str) -> bool
{
   if(*str == 0)
   {
      return FALSE;
   }

   for(; *str != 0; str++)
   {
      if(isdigit(*str) == 0)
      {
         return FALSE;
      }
   }

   return TRUE;
}

/* Check if the next word in str is a number */
auto next_word_is_number(const char *str) -> bool
{
   char tmp[MAX_STRING_LENGTH];
   str_next_word(str, tmp);
   return str_is_number(tmp);
}

/* Block must end with null pointer, -1 if not found otherwise index */
/* Warning, argument 1 is made into lowercase!                       */
/* Warning, list of names must be in lowercase to match up! */
auto search_block(const char *oarg, const char **list, bool exact) -> int
{
   char arg[4096];
   int  i;
   int  l;

   /* Make into lower case, and get length of string */
   l = str_lower(oarg, arg, sizeof(arg));

   if(static_cast<unsigned int>(exact) != 0U)
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
         if(strncmp(arg, list[i], l) == 0)
         {
            return i;
         }
      }
   }

   return -1;
}

/* Block must end with null pointer                                  */
/* Warning, argument 1 is made into lowercase!                       */
auto search_block_length(const char *oarg, int length, const char **list, bool exact) -> int
{
   char arg[4096];
   int  i;

   /* Make into lower case, and get length of string */
   str_lower(oarg, arg, sizeof(arg));

   if(static_cast<unsigned int>(exact) != 0U)
   {
      for(i = 0; list[i] != nullptr; i++)
      {
         if(strncmp(arg, list[i], length) == 0)
         {
            if(list[i][length] <= ' ')
            {
               return i;
            }
         }
      }
   }
   else
   {
      if(length == 0)
      {
         length = 1; /* Avoid "" to match the first available string */
      }
      for(i = 0; list[i] != nullptr; i++)
      {
         if(strncmp(arg, list[i], length) == 0)
         {
            return i;
         }
      }
   }

   return -1;
}

auto fill_word(const char *argument) -> int
{
   return static_cast<int>(search_block(argument, fillwords, TRUE) >= 0);
}

/* Exactly as str_next-word, except it wont change the case */
auto str_next_word_copy(const char *argument, char *first_arg) -> char *
{
   /* Find first non blank */
   argument = skip_spaces(argument);

   /* Copy next word and make it lower case */
   for(; *argument > ' '; argument++)
   {
      *first_arg++ = *argument;
   }

   *first_arg = '\0';

   return skip_spaces(argument);
}

/* Copy next word from argument into first_arg and make lowercase*/
auto str_next_word(const char *argument, char *first_arg) -> char *
{
   /* Find first non blank */
   argument = skip_spaces(argument);

   /* Copy next word and make it lower case */
   for(; *argument > ' '; argument++)
   {
      *first_arg++ = tolower(*argument);
   }

   *first_arg = '\0';

   return skip_spaces(argument);
}

/*  Find the first sub-argument of a string,
 *  return pointer to first char in primary argument, following the sub-arg
 */
auto one_argument(const char *argument, char *first_arg) -> char *
{
   assert(argument && first_arg);

   do
   {
      argument = str_next_word(argument, first_arg);
   } while(fill_word(first_arg) != 0);

   return (char *)argument;
}

/* determine if 'arg1' is an abbreviation of 'arg2'. */
auto is_abbrev(const char *arg1, const char *arg2) -> bool
{
   if(*arg1 == 0)
   {
      return FALSE;
   }

   for(; *arg1 != 0; arg1++, arg2++)
   {
      if(tolower(*arg1) != tolower(*arg2))
      {
         return FALSE;
      }
   }

   return TRUE;
}

/* determine if 'arg1' is an abbreviation of 'arg2'.          */
/* The empty string is never an abbrevation of another string */
auto is_multi_abbrev(const char *arg1, const char *arg2) -> bool
{
   if(*arg1 == 0)
   {
      return FALSE;
   }

   for(; (*arg1 != 0) && (isspace(*arg1) == 0); arg1++, arg2++)
   {
      if(tolower(*arg1) != tolower(*arg2))
      {
         return FALSE;
      }
   }

   return TRUE;
}

/* Block must end with null pointer, -1 if not found otherwise index */
/* Warning, argument 1 is converted into lowercase!                  */
auto search_block_abbrevs(const char *oarg, const char **list, const char **end) -> int
{
   char        arg[4096];
   char        buf1[MAX_INPUT_LENGTH];
   char        buf2[MAX_INPUT_LENGTH];
   char       *s;
   char       *ps;
   char       *bestpos;
   const char *d;
   const char *pd;
   int         i;
   int         match;
   int         bestidx;

   i       = str_lower(skip_spaces(oarg), arg, sizeof(arg));

   bestidx = -1;
   bestpos = nullptr;

   for(i = 0; list[i] != nullptr; i++)
   {
      for(match = 0, s = arg, d = list[i];;)
      {
         ps = s;
         pd = d;
         s  = str_next_word(s, buf1);
         d  = str_next_word(d, buf2);
         if(static_cast<unsigned int>(is_multi_abbrev(buf1, buf2)) != 0U)
         {
            s = skip_spaces(s);
            d = skip_spaces(d);
            match++;
         }
         else
         {
            break;
         }
      }
      if((match != 0) && ps > bestpos)
      {
         bestpos = ps;
         bestidx = i;
      }
   }

   if(bestidx != -1)
   {
      *end = oarg + (bestpos - arg);
      return bestidx;
   }

   *end = oarg;

   return -1;
}

/* returns a pointer to a (static) string wich contains s1 and s2 */
// TODO horrible function - ugh
//auto str_cc(const char *s1, const char *s2) -> char *
//{
//   static char buf[1024];
//
//   strncpy(buf, s1, sizeof(buf) - 1);
//   strncat(buf, s2, sizeof(buf) - 1 - strlen(buf));
//
//   return buf;
//}

/* =================================================================== */
/* Insert the string "str" at the first position in the "itxt" string. */

void str_insert(char *str, char *itxt)
{
   int nlen;
   int slen;

   nlen = strlen(itxt);
   slen = strlen(str);

   memmove(str + nlen, str, slen + 1);
   memcpy(str, itxt, nlen);
}

/*
   Scan the string 'str' for the string OLD and replace each
   occurence with new.
   */
void str_substitute(const char *old, const char *newstr, char *str)
{
   char *b;
   int   olen;
   int   nlen;
   int   slen;

   olen = strlen(old);
   nlen = strlen(newstr);
   slen = strlen(str);

   while((b = str_str(str, old)) != nullptr)
   {
      if(nlen <= olen)
      {
         memcpy(b, newstr, nlen);
         memmove(b + nlen, b + olen, slen - (b - str + olen) + 1);
         slen -= olen - nlen;
      }
      else
      {
         memmove(b + nlen, b + olen, slen - (b - str));
         memcpy(b, newstr, nlen);
         slen += nlen - olen;
      }
   }
}

/* Remove all occurences of c in s */
void str_rem(char *s, char c)
{
   int diff;

   if(c == '\0')
   {
      return;
   }

   for(diff = 0; *(s + diff) != 0;)
   {
      if(*s == c)
      {
         diff++;
      }
      else
      {
         s++;
      }

      *s = *(s + diff);
   }
}

/* Replace all occurences of non alfa, digit with space in s */
void str_rem_punct(char *s)
{
   int diff;

   for(diff = 0; *s != 0; s++)
   {
      if(ispunct(*s) != 0)
      {
         *s = ' ';
      }
   }
}

/* Replace all occurences of non alfa, digit with space in s */
void str_blank_punct(char *s)
{
   static char c[3] = {' ', ' ', 0};
   int         diff;

   for(diff = 0; *s != 0; s++)
   {
      if(ispunct(*s) != 0)
      {
         c[0] = *s;
         *s   = ' ';
         str_insert(s + 1, c);
         s += 2;
      }
   }
}

/* Remove all multiple space occurences in s */
void str_remspc(char *s)
{
   char *cp;

   while((*s != 0) && (*s != ' ' || *(s + 1) != ' '))
   {
      s++;
   }

   if(*s == 0)
   {
      return;
   }

   cp = s;

   while(*cp != 0)
   {
      while(*cp == ' ' && *(cp + 1) == ' ')
      {
         cp++;
      }

      *s++ = *cp++;
   }
   *s = 0;
}

void str_chraround(char *str, char c)
{
   int l;

   l = strlen(str);
   memmove(str + 1, str, l);
   str[0]     = c;
   str[l + 1] = c;
   str[l + 2] = 0;
}

/* Names has the format :                                 */
/*     names[0] pointer to a string                       */
/*     names[N] pointer to the last N'th string           */
/*     names[N+1] NIL pointer                             */
/*                                                        */
/*   Assumes nothing that arg is without leading spaces,  */
/*   no double spaces and contains text                   */

auto is_name_raw(const char *arg, const char *names[]) -> char *
{
   int i;
   int j;

   for(i = 0; names[i] != nullptr; i++)
   {
      for(j = 0; names[i][j] != 0; j++)
      {
         if(tolower(arg[j]) != tolower(names[i][j]))
         {
            break;
         }
      }

      if(names[i][j] == 0)
      {
         if((arg[j] == 0) || isaspace(arg[j]))
         {
            return ((char *)arg) + j;
         }
      }
   }

   return nullptr;
}

/*
 ******** Utility routines for the name lists. *******
 */

/*  Names has the format:
 *     names[0] pointer to a string
 *     names[N] pointer to the last N'th string
 *     names[N+1] NIL pointer
 *  Assumes nothing nice about arg - only about names[]
 */
/* We need to copy to BUF in order to prevent crash when */
/* str_remspc might want to change "constant" strings    */

auto is_name(const char *arg, const char *names[]) -> char *
{
   char buf[MAX_INPUT_LENGTH];

   for(; isaspace(*arg); arg++)
   {
      ;
   }

   if(*arg == 0)
   {
      return nullptr;
   }

   strcpy(buf, arg);
   str_remspc(buf);

   return is_name_raw(buf, names);
}

/* Create an empty namelist */
auto create_namelist() -> char **
{
   char **list;

   CREATE(list, char *, 1);
   list[0] = nullptr;

   return list;
}

/* Add a new name to the end of an existing namelist */
auto add_name(const char *name, char **namelist) -> char **
{
   int pos = 0;

   assert(name && namelist);

   while(namelist[pos] != nullptr)
   {
      pos++;
   }

   RECREATE(namelist, char *, pos + 2);

   namelist[pos]     = str_dup(name);
   namelist[pos + 1] = nullptr;

   return namelist;
}

/* Delete a name from namelist, move last name to field */
auto del_name(const char *name, char **namelist) -> char **
{
   int pos = 0;
   int end = 0;

   assert(name && namelist);

   if((namelist[0] == nullptr) || (namelist[1] == nullptr))
   {
      return namelist;
   }

   while(namelist[end] != nullptr)
   {
      end++;
   }

   for(pos = 0; pos < end; pos++)
   {
      if(strcmp(name, namelist[pos]) == 0)
      {
         free(namelist[pos]);
         while(++pos <= end)
         {
            namelist[pos - 1] = namelist[pos];
         }

         assert(end > 0);
         RECREATE(namelist, char *, end);
         return namelist;
      }
   }

   return namelist;
}

/* free space allocated to a namelist */
void free_namelist(char **list)
{
   char **original;

   assert(list);

   original = list;

   while(*list != nullptr)
   {
      free(*(list));
      /* MS: Well, ugly but we have to do while free macro is in use! */
      list++;
   }

   free(original);
}

/* don't look too closely at this obscenity - it won't stay long */
auto namestring_to_namelist(const char *str) -> char **
{
   char   buf[100];
   char **rslt;
   char  *save;

   assert(str && *str);

   save = const_cast<char *>(str);

   rslt = create_namelist();
   while((str = one_argument(str, buf)), *buf != 0)
   {
      rslt = add_name(buf, rslt);
   }

   free(save); /* yes, it is pathetic */

   return rslt;
}

auto copy_namelist(char **source) -> char **
{
   char **rslt;

   assert(source);

   rslt = create_namelist();

   while(*source != nullptr)
   {
      rslt = add_name(*source, rslt);
      source++;
   }

   return rslt;
}

/*  Compare if 'next_word' is the next word in buf - if true return pointer
 *  Put no space before or after 'next_word'
 *  Return NIL if not identical, otherwise return pointer to location
 *  just after the match
 */
auto str_ccmp_next_word(const char *buf, const char *next_word) -> char *
{
   buf = skip_spaces(buf);

   for(; *next_word != 0; next_word++, buf++)
   {
      if(*next_word != *buf)
      {
         return nullptr;
      }
   }

   if((*buf == 0) || isaspace(*buf))
   { /* Buf must end here or be word separated */
      return (char *)buf;
   }

   return nullptr;
}

/*  Must receive a string of the format 'name@zone\0' or
 *  'zone/name\0'. It sabotages the special symbol '@'
 *  or '/' if any. If no special symbol name will
 *  contain the string
 */
void split_fi_ref(const char *str, char *zone, char *name)
{
   const char *c;
   const char *t;
   int         l;

   str = skip_spaces(str);

   if((c = strchr(str, '@')) != nullptr)
   {
      l = std::min(c - str, static_cast<long>(FI_MAX_UNITNAME));
      strncpy(name, str, l);
      name[l] = '\0';

      l       = std::min(strlen(c + 1), static_cast<size_t>(FI_MAX_ZONENAME));
      if((t = strchr(c + 1, ' ')) != nullptr)
      {
         l = std::min(static_cast<long>(l), t - (c + 1));
      }
      strncpy(zone, c + 1, l);
      zone[l] = 0;
   }
   else if((c = strchr(str, '/')) != nullptr)
   {
      l = std::min(c - str, static_cast<long>(FI_MAX_ZONENAME));
      strncpy(zone, str, l);
      zone[l] = '\0';

      l       = std::min(strlen(c + 1), static_cast<size_t>(FI_MAX_UNITNAME));
      if((t = strchr(c + 1, ' ')) != nullptr)
      {
         l = std::min(static_cast<long>(l), t - (c + 1));
      }
      strncpy(name, c + 1, l);
      name[l] = 0;
   }
   else
   {
      if((c = strchr(str, ' ')) != nullptr)
      {
         l = std::min(c - str, static_cast<long>(FI_MAX_UNITNAME));
         strncpy(name, str, l);
         name[l] = '\0';
      }
      else
      {
         l = std::min(strlen(str), static_cast<size_t>(FI_MAX_UNITNAME));
         strncpy(name, str, l);
         name[l] = 0;
      }

      *zone = '\0';
   }
}

auto catnames(char *s, const char **names) -> char *
{
   const char **nam;
   bool         ok = FALSE;

   if(names != nullptr)
   {
      strcpy(s, "{");
      TAIL(s);
      for(nam = names; *nam != nullptr; nam++)
      {
         ok = TRUE;
         sprintf(s, "\"%s\",", *nam);
         TAIL(s);
      }
      if(static_cast<unsigned int>(ok) != 0U)
      {
         s--; /* remove the comma */
      }
      strcpy(s, "}");
   }
   else
   {
      sprintf(s, "NULL");
   }

   TAIL(s);

   return s;
}

/* Takes a string and return a pointer to the string on the next line */
/* or the end of string                                               */
auto text_skip_line(char *buf) -> char *
{
   char *c;

   if(*buf == 0)
   {
      return nullptr;
   }

   if(((c = strchr(buf, '\n')) == nullptr) || ((c = strchr(buf, '\r')) == nullptr))
   {
      TAIL(buf);
      return buf;
   }

   while(*c == '\n' || *c == '\r')
   {
      ++c;
   }

   return c;
}

auto text_read_line(char *buf, char *res) -> char *
{
   while((*buf != 0) && *buf != '\n' && *buf != '\r')
   {
      *res++ = *buf++;
   }

   *res = '\0';

   while(*buf == '\n' || *buf == '\r')
   {
      buf++;
   }

   return buf;
}

/* Format the string "str" following the format rules. No longer than */
/* 'destlen' result. Returns resulting length.                        */

auto str_escape_format(const char *src, char *dest, int destlen, int formatting) -> int
{
   const char *org_dest = dest;

   /* Process string */

   if(formatting != 0)
   {
      src = skip_blanks(src);
   }

   while(*src != 0)
   {
      if(org_dest - dest > destlen - 3)
      {
         break;
      }

      /* Make double space after '.' */
      if((formatting != 0) && *src == '.' && (isspace(*(src + 1)) != 0))
      {
         *dest++ = *src++;
         *dest++ = ' ';
         continue;
      }

      if(*src == '&')
      {
         src++;
         switch(tolower(*src))
         {
            case 'l':
               /* if (!formatting)
                  slog(LOG_ALL, 0, "Redundant format code '&l'."); */
               formatting = static_cast<int>(FALSE);
               break;

            case 'h':
               strcpy(dest, CONTROL_HOME);
               TAIL(dest);
               break;

            case 'f':
               /* if (formatting)
                  slog(LOG_ALL, 0, "Redundant format code '&f'."); */
               formatting = static_cast<int>(TRUE);
               break;

            case '&':
               *dest++ = '&';
               break;

            case 'c':
            {
               int bright = static_cast<int>(FALSE);

               if(*++src == '+')
               {
                  bright = static_cast<int>(TRUE);
                  src++;
               }

               switch(tolower(*src))
               {
                  case 'n':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_BLACK);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_BLACK);
                     }
                     break;

                  case 'r':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_RED);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_RED);
                     }
                     break;

                  case 'g':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_GREEN);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_GREEN);
                     }
                     break;

                  case 'y':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_YELLOW);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_YELLOW);
                     }
                     break;

                  case 'b':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_BLUE);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_BLUE);
                     }
                     break;

                  case 'm':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_MAGENTA);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_MAGENTA);
                     }
                     break;

                  case 'c':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_CYAN);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_CYAN);
                     }
                     break;

                  case 'w':
                     if(bright != 0)
                     {
                        strcpy(dest, CONTROL_FGB_WHITE);
                     }
                     else
                     {
                        strcpy(dest, CONTROL_FG_WHITE);
                     }
                     break;

                  default:
                     slog(LOG_OFF, 0, "Illegal color value '&f%c'", *src);
                     break;
               }
               TAIL(dest);
               break;
            }

            case 'b':
            {
               src++;
               switch(tolower(*src))
               {
                  case 'n':
                     strcpy(dest, CONTROL_BG_BLACK);
                     break;
                  case 'r':
                     strcpy(dest, CONTROL_BG_RED);
                     break;
                  case 'g':
                     strcpy(dest, CONTROL_BG_GREEN);
                     break;
                  case 'y':
                     strcpy(dest, CONTROL_BG_YELLOW);
                     break;
                  case 'b':
                     strcpy(dest, CONTROL_BG_BLUE);
                     break;
                  case 'm':
                     strcpy(dest, CONTROL_BG_MAGENTA);
                     break;
                  case 'c':
                     strcpy(dest, CONTROL_BG_CYAN);
                     break;
                  case 'w':
                     strcpy(dest, CONTROL_BG_WHITE);
                     break;
                  default:
                     slog(LOG_OFF, 0, "Illegal color value '&b%c'", *src);
                     break;
               }
               TAIL(dest);
               break;
            }

            case 's':
            {
               char        Buf[10];
               const char *c;
               int         rep = 0;
               int         i;

               src++;
               c = src;
               while((isdigit(*c) != 0) && rep < (int)sizeof(Buf) - 1)
               {
                  Buf[rep++] = *c++;
               }
               Buf[rep] = 0;

               if(rep > 0)
               {
                  src = c;
                  rep = atoi(Buf);
               }
               else
               {
                  rep = 1;
               }

               if(org_dest - dest + rep > destlen - 3)
               {
                  break;
               }

               for(i = 0; i < rep; i++)
               {
                  *dest++ = ' ';
               }

               continue;
            }

            case 'n':
               *dest++ = '\n';
               *dest++ = '\r';
               src++;
               while(isspace(*src) != 0)
               {
                  src++;
               }
               continue;

            default:
            {
               slog(LOG_OFF, 0, "Illegal escape character, '&%c'", *src);
               break;
            }
         }
         if(*src != 0)
         {
            src++;
         }
         continue;
      } /* if ('&') */

      if(formatting != 0)
      {
         if(isspace(*src) != 0)
         {
            if(isspace(*(src + 1)) == 0)
            {
               *dest++ = ' ';
            }
            src++;
         }
         else
         {
            *dest++ = *src++;
         }
      }
      else
      {
         if(*src == '\n')
         {
            *dest++ = *src++;
            *dest++ = '\r';
         }
         else
         {
            *dest++ = *src++;
         }
      }
   }

   *dest = '\0';

   /* while (dest > org_dest)
      if (isspace(*(dest-1)))
      *--dest = '\0';
      else
      break; */

   return org_dest - dest;
}

auto str_escape_format(const char *src, int formatting) -> char *
{
   char buf[MAX_STRING_LENGTH] = "";
   str_escape_format(src, buf, sizeof(buf), formatting);

   return str_dup(buf);
}
