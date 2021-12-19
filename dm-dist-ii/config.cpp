#include "config.h"

#include <cstdio>

void ShowUsage(const char *name)
{
   fprintf(stderr,
           "Usage: %s [-h] [-d pathname] "
           "[-z pathname] [-c {players}]\n",
           name);
   fprintf(stderr, "  -h: This help screen.\n");
   fprintf(stderr, "  -c: List or Convert playerfile\n");
   fprintf(stderr, "  -d: Alternate lib/ dir\n");
   fprintf(stderr, "  -z: Alternate zon/ dir\n");
   fprintf(stderr, "  -p: Persistant containers list\n");
   fprintf(stderr, "Copyright (C) 1994 - 1996 by Valhalla.\n");
}
