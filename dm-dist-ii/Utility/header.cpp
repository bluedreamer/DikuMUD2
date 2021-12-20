#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_SIZE 8192

float version       = 1.0;
char  file[1024]    = "UNKNOWN";
char  author[1024]  = " * Author : Unknown.                                                       *";
char  purpose[1024] = " * Purpose: Unknown.                                                       *";
char  bugs[1024]    = " * Bugs   : Unknown.                                                       *";
char  status[1024]  = " * Status : Unpublished.                                                   *";

const char *notice1 = " * Copyright (C) Valhalla (This work is unpublished).                      *"
                      "\n"
                      " *                                                                         *"
                      "\n"
                      " * This work is a property of:                                             *"
                      "\n"
                      " *                                                                         *"
                      "\n"
                      " *        Valhalla I/S                                                     *"
                      "\n"
                      " *        Noerre Soegade 37A, 4th floor                                    *"
                      "\n"
                      " *        1370 Copenhagen K.                                               *"
                      "\n"
                      " *        Denmark                                                          *"
                      "\n"
                      " *                                                                         *"
                      "\n"
                      " * This is an unpublished work containing Valhalla confidential and        *"
                      "\n"
                      " * proprietary information. Disclosure, use or reproduction without        *"
                      "\n"
                      " * authorization of Valhalla is prohobited.                                *";

const char *notice2 = " * Copyright (C) 1994 - 1996 by Valhalla (This work is published).         *"
                      "\n"
                      " *                                                                         *"
                      "\n"
                      " * This work is a property of:                                             *"
                      "\n"
                      " *                                                                         *"
                      "\n"
                      " *        Valhalla I/S                                                     *"
                      "\n"
                      " *        Noerre Soegade 37A, 4th floor                                    *"
                      "\n"
                      " *        1370 Copenhagen K.                                               *"
                      "\n"
                      " *        Denmark                                                          *"
                      "\n"
                      " *                                                                         *"
                      "\n"
                      " * This work is copyrighted. No part of this work may be copied,           *"
                      "\n"
                      " * reproduced, translated or reduced to any medium without the prior       *"
                      "\n"
                      " * written consent of Valhalla.                                            *";

char notice[4096];

void write_header(FILE *output)
{
   fprintf(output,
           "/* *********************************************************************** *"
           "\n"
           " * File   : %-40s  Part of Valhalla MUD *"
           "\n"
           " * Version: %1.2f                                                           *"
           "\n"
           "%s"
           "\n"
           " *                                                                         *"
           "\n"
           "%s"
           "\n"
           "%s"
           "\n"
           "%s"
           "\n"
           " *                                                                         *"
           "\n"
           "%s"
           "\n"
           " * *********************************************************************** */"
           "\n",
           file,
           version,
           author,
           purpose,
           bugs,
           status,
           notice);
}

auto main(int argc, char *argv[]) -> int
{
   char  buf[MAX_SIZE];
   char *start;
   char *end;
   char  c;
   int   n;
   FILE *input;
   FILE *output;

   strcpy(notice, notice1);

   input  = stdin;
   output = stdout;

   if(argc == 2)
   {
      input = fopen(argv[1], "rb");
      if(input == nullptr)
      {
         printf("Error: No such file '%s'.\n", argv[1]);
         exit(1);
      }

      output = fopen("tmp_hdr", "wb");
      if(input == nullptr)
      {
         printf("Error: Can't make temp file 'tmpheader'.\n");
         exit(1);
      }

      strcpy(file, argv[1]);
   }

   n          = fread(buf, sizeof(char), MAX_SIZE, input);
   c          = buf[n - 1];
   buf[n - 1] = 0;
   start      = strstr(buf, "/*");
   end        = strstr(buf, "*/");
   if((start == nullptr) || (end == nullptr) || (start[40] != '*'))
   {
      buf[n - 1] = c;
      version    = 1.0;
      write_header(output);
      fwrite(buf, sizeof(char), n, output);
   }
   else
   {
      char *pstr;
      char *estr;

      pstr = strstr(buf, "Version:");
      if(pstr != nullptr)
      {
         sscanf(pstr + 9, " %f ", &version);
      }

      pstr = strstr(buf, " * Author : ");
      if(pstr != nullptr)
      {
         estr = strstr(pstr + 11, "*\n");
         if(estr != nullptr)
         {
            strncpy(author, pstr, estr - pstr);
            author[estr - pstr + 1] = 0;
         }

         author[75] = '*';
         author[76] = 0;
      }

      pstr = strstr(buf, " * Status : ");
      if(pstr != nullptr)
      {
         if(strstr(pstr + 11, "Unpublished") != nullptr)
         {
            strcpy(notice, notice1);
         }
         else if(strstr(pstr + 11, "Published") != nullptr)
         {
            strcpy(notice, notice2);
         }

         estr = strstr(pstr + 11, "*\n");
         if(estr != nullptr)
         {
            strncpy(status, pstr, estr - pstr);
            status[estr - pstr + 1] = 0;
         }

         status[75] = '*';
         status[76] = 0;
      }

      pstr = strstr(buf, " * Purpose:");
      if(pstr != nullptr)
      {
         estr = strstr(pstr + 11, "\n * Bugs");
         if(estr != nullptr)
         {
            strncpy(purpose, pstr, estr - pstr);
            purpose[estr - pstr + 1] = 0;
         }
      }

      pstr = strstr(buf, " * Bugs   :");
      if(pstr != nullptr)
      {
         estr = strstr(pstr + 11, "\n * Valhalla ");
         if(estr != nullptr)
         {
            strncpy(bugs, pstr, estr - pstr);
            bugs[estr - pstr + 1] = 0;
         }
      }

      buf[n - 1] = c;
      write_header(output);
      fwrite(end + 3, sizeof(char), n - (end - buf) - 3, output);
   }

   while(feof(input) == 0)
   {
      n = fread(buf, sizeof(char), MAX_SIZE, input);
      if(n > 0)
      {
         fwrite(buf, sizeof(char), n, output);
      }
   }

   n = rename("tmp_hdr", file);
   assert(n == 0);

   return 0;
}
