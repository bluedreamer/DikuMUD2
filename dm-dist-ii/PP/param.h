#pragma once

class param
{
public:
   param *p_link;       /* Next in list				*/
   char   p_flags;      /* Flags:				*/
#define PF_RQUOTES 0x01 /* Remove "" chars from parameter	*/
#define PF_PNLINES 0x02 /* Preserve '\n' char in parameter	*/
   char p_name[1];      /* Name is appended to struct	*/
};
