#pragma once

/*
 *	Predefined symbol entry.
 */

class ppdir
{
public:
   const char *pp_name;                     /* #function name		*/
   char        pp_ifif;                     /* FALSE if ! to do on false #if*/
   void (*pp_func)(int, int, const char *); /* Address of function		*/
   int pp_arg;                              /* Argument to function		*/
};
