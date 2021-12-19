#include "files.h"

#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cstdio>

/* Returns the size of a file in bytes */
auto fsize(FILE *f) -> long
{
   long oldpos;
   long size;

   oldpos = ftell(f);

   if(fseek(f, 0L, SEEK_END) != 0)
   { /* Seek to end of file */
      assert(FALSE);
   }

   size = ftell(f);

   if(fseek(f, oldpos, SEEK_SET) != 0)
   { /* Seek to end of file */
      assert(FALSE);
   }

   return size;
}

/* check if a file exists */
auto file_exists(const char *name) -> bool
{
   FILE *fp;

   if((fp = fopen(name, "r")) == nullptr)
   {
      return FALSE;
   }

   fclose(fp);
   return TRUE;
}

/* create a file if it doesn't exist. if error, terminate */
void touch_file(const char *name)
{
   FILE *fp;

   if(file_exists(name) != false)
   {
      return;
   }

   if((fp = fopen(name, "w")) == nullptr)
   {
      fprintf(stderr, "touch_file(): Couldn't create %s...\n", name);
      assert(false);
   }
   fclose(fp);
}

auto fread_line_commented(FILE *fl, char *buf, int max) -> char *
{
   char *s;

   for(;;)
   {
      s = fgets(buf, max, fl);

      if(s == nullptr)
      {
         break;
      }

      if(*skip_spaces(buf) != '#')
      {
         break;
      }
   }

   return s;
}

/* read and allocate space for a '~'-terminated string from a given file */
auto fread_string_copy(FILE *fl, char *buf, int max) -> char *
{
   char *obuf;
   char *point;
   int   flag;
   int   total;

   memset(buf, '\0', max);
   total = 0;
   obuf  = buf;

   do
   {
      if(fgets(buf, max - total, fl) == nullptr)
      {
         error(HERE, "fread_string_copy");
      }

      total += strlen(buf);

      if(total >= max)
      {
         error(HERE, "fread_string_copy: string too large (db.c)");
      }

      for(point = buf + strlen(buf) - 2; point >= buf && (isspace(*point) != 0); point--)
      {
         ;
      }

      if((flag = static_cast<int>(*point == '~')) != 0)
      {
         if(*(buf + strlen(buf) - 3) == '\n')
         {
            *(buf + strlen(buf) - 2) = '\r';
            *(buf + strlen(buf) - 1) = '\0';
         }
         else
         {
            *(buf + strlen(buf) - 2) = '\0';
         }
      }
      else
      {
         *(buf + strlen(buf) + 1) = '\0';
         *(buf + strlen(buf))     = '\r';
      }
      TAIL(buf);
   } while(flag == 0);

   return obuf;
}

/* read and allocate space for a '~'-terminated string from a given file */
auto fread_string(FILE *fl) -> char *
{
   char buf[MAX_STRING_LENGTH];

   fread_string_copy(fl, buf, MAX_STRING_LENGTH);

   if(strlen(buf) > 0)
   {
      return str_dup(buf);
   }
   return nullptr;
}

/* Read contents of a file, but skip all remark lines and blank lines. */
auto config_file_to_string(char *name, char *buf, int max_len) -> int
{
   FILE *fl;
   char  tmp[500];

   *buf = '\0';

   if((fl = fopen(name, "r")) == nullptr)
   {
      DEBUG("File-to-string error.\n");
      *buf = '\0';
      return (-1);
   }

   do
   {
      if(fgets(tmp, sizeof(tmp) - 1, fl) != nullptr)
      {
         if(tmp[0] == '#')
         {
            continue;
         }

         if(tmp[0] == 0)
         {
            continue;
         }

         if(strlen(buf) + strlen(tmp) + 2 > (uint32_t)max_len)
         {
            DEBUG("fl->strng: string too big (db.c, file_to_string)\n");
            strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
            fclose(fl);
            return (-1);
         }

         strcat(buf, tmp);
         *(buf + strlen(buf) + 1) = '\0';
         *(buf + strlen(buf))     = '\r';
      }
   } while(feof(fl) == 0);

   fclose(fl);

   return (0);
}

/* read contents of a text file, and place in buf */
auto file_to_string(char *name, char *buf, int max_len) -> int
{
   FILE *fl;
   char  tmp[500];

   *buf = '\0';

   if((fl = fopen(name, "r")) == nullptr)
   {
      slog(LOG_ALL, 0, "File-to-string error");
      *buf = '\0';
      return (-1);
   }

   do
   {
      if(fgets(tmp, sizeof(tmp) - 1, fl) != nullptr)
      {
         if(strlen(buf) + strlen(tmp) + 2 > (uint32_t)max_len)
         {
            slog(LOG_ALL, 0, "fl->strng: string too big (db.c, file_to_string)");
            strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
            return -1;
         }

         strcat(buf, tmp);
         *(buf + strlen(buf) + 1) = '\0';
         *(buf + strlen(buf))     = '\r';
      }
   } while(feof(fl) == 0);

   fclose(fl);

   return 0;
}

auto fread_num(FILE *fl) -> long
{
   long tmp;

   int  ms2020 = fscanf(fl, " %ld ", &tmp);
   return (tmp);
}

/* Read a null terminated string from file into str */
void fstrcpy(CByteBuffer *pBuf, FILE *f)
{
   int c;

   pBuf->Clear();

   while(((c = fgetc(f)) != 0) && (c != EOF))
   {
      pBuf->Append8(c);
   }

   pBuf->Append8(0);
}

/************************************************************************
 *  General purpose file Handling Procedures Below this line            *
 ********************************************************************** */

/* fopen_cache below this comment */

struct fcache_type
{
   int   hits;
   int   name_s;
   char *name;
   FILE *file;
};

static struct fcache_type fcache[FCACHE_MAX] = {
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
   {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}};

auto enl_strcpy(char *dest, char *source, int *dest_size) -> char *
{
   int len = strlen(source);

   if((dest == nullptr) || (*dest_size < len + 1))
   {
      if(dest != nullptr)
      {
         RECREATE(dest, char, len + 1);
      }
      else
      {
         CREATE(dest, char, 1 + len);
      }

      *dest_size = len + 1;
   }

   return strcpy(dest, source);
}

/* The mode is interpreted as follows:

   If mode contains a 'w' then the file is always opened as "w+"
   If mode contains an 'r' then the file is initially opened as "r+"
   If mode contains an 'a' then the file is initially opened as "a+"

Then succeeding calls to fopen cache with any mode (but 'w') will cause
a fseek to the end or start (for 'a' or 'r'). Thus performance is gained,
BUT 'read-only' files may be written to!

*/

auto fopen_cache(char *name, const char *mode) -> FILE *
{
   int        i;
   int        min_i;
   int        hit_i;
   static int pure_hits = 0;
   static int purge     = 0;

   min_i                = number(0, FCACHE_MAX - 1); /* In case many are equal */
   hit_i                = -1;

   for(i = 0; i < FCACHE_MAX; i++)
   {
      if(fcache[i].hits < fcache[min_i].hits)
      {
         min_i = i;
      }
      if((fcache[i].name != nullptr) && (strcmp(name, fcache[i].name) == 0))
      {
         hit_i = i;
      }
      fcache[i].hits--;
   }

   if(hit_i == -1)
   {
      if(fcache[min_i].file != nullptr)
      {
         if(fclose(fcache[min_i].file) != 0)
         {
            error(HERE, "Error on fcache fclose() on file [%s].", fcache[min_i].name);
         }
         purge++;
      }
      fcache[min_i].name = enl_strcpy(fcache[min_i].name, name, &fcache[min_i].name_s);
      fcache[min_i].hits = 0;

      if(strchr(mode, 'w') != nullptr)
      {
         fcache[min_i].file = fopen(name, "w+b");
      }
      else if(strchr(mode, 'a') != nullptr)
      {
         fcache[min_i].file = fopen(name, "a+b");
      }
      else if(strchr(mode, 'r') != nullptr)
      {
         fcache[min_i].file = fopen(name, "r+b");
      }
      else
      {
         error(HERE, "Bad file mode [%s] for file [%s]", mode, name);
      }

      return fcache[min_i].file;
   }

   if(strchr(mode, 'w') != nullptr)
   {
      fcache[hit_i].file = freopen(name, "w+b", fcache[hit_i].file);
   }
   else if(strchr(mode, 'a') != nullptr)
   {
      fseek(fcache[hit_i].file, 0L, SEEK_END);
   }
   else if(strchr(mode, 'r') != nullptr)
   {
      fseek(fcache[hit_i].file, 0L, SEEK_SET);
   }
   else
   {
      error(HERE, "Bad file mode [%s] for file [%s]", mode, name);
   }

   pure_hits++;

   fcache[hit_i].hits = 0;

   /* Consider fflush(fcache[hit_i].file); */

   return fcache[hit_i].file;
}

void fclose_cache()
{
   int i;

   /* closes all files in the cache! */

   for(i = 0; i < FCACHE_MAX; i++)
   {
      if(fcache[i].file != nullptr)
      {
         if(fclose(fcache[i].file) != 0)
         {
            slog(LOG_ALL, 0, "fcache close failed on file %s.", fcache[i].name);
            return;
         }

         fcache[i].file = nullptr;
      }
   }
}
