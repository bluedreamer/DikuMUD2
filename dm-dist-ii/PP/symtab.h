#pragma once

#include "param.h"
class symtab
{
public:
   symtab *s_link;    /* Next in list for this chain		*/
   char    disable;   /* TRUE to disable recognition for now	*/
   char   *s_body;    /* Body of definition			*/
   param  *s_params;  /* List of parameters		*/
   char    s_name[1]; /* Name is appended to structure*/
};
