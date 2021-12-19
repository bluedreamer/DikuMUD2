#pragma once

#include <ctime>

struct ban_t
{
   char  *site;
   char   type;
   time_t until;
   char  *textfile;
   ban_t *next;
};
