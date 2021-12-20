#pragma once

#include "dilintr.h"
#include "dilsecure.h"
#include "diltemplate.h"
#include "dilvar.h"

#include <cstdint>

/*
 *  A stack frame for a DIL call of function or procedure.
 *  The frame contains runtime values for proc/func execution,
 *  including the return variable number for calling proc/func.
 *  Uppon call, a new stackframe is created from the called
 *  template. The needed memory is allocated in one chunk.
 */
class dilframe
{
public:
   uint16_t     ret;         /* return variable # (not saved) */
   diltemplate *tmpl;        /* current template */
   dilvar      *vars;        /* variables */
   uint8_t     *pc;          /* program counter */
   uint16_t     securecount; /* number of secures (not saved) */
   dilsecure   *secure;      /* secured vars (not saved) */
   uint16_t     intrcount;   /* number of interrupts */
   dilintr     *intr;        /* interrupts */
};
