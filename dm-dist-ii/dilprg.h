#pragma once

#include "dilframe.h"
#include "spec_arg.h"

#include <cstdint>

struct dilprg
{
   uint8_t    flags;   /* Recall, copy, etc. */
   uint16_t   varcrc;  /* variable crc from compiler (saved) */
   uint16_t   corecrc; /* core crc from compiler (saved) */
   uint16_t   stacksz; /* stack size */
   dilframe  *sp;      /* stack and pointer */
   dilframe  *stack;   /* stack frames, #0 saved */
   spec_arg  *sarg;
   unit_data *owner;
   int16_t    waitcmd; /* Command countdown */
   dilprg    *next;    /* For global dilprg list (sendtoalldil) */
};

extern dilprg *dil_list;
extern dilprg *dil_list_nextdude;
