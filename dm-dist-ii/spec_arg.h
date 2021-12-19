#pragma once

#include "unit_data.h"

class command_info;

struct spec_arg
{
   unit_data    *owner;     /* Who is this?                       */
   unit_data    *activator; /* Who performed the operation        */
   unit_data    *medium;    /* Possibly what is used in operation */
   unit_data    *target;    /* Possible target of operation       */
   command_info *cmd;
   unit_fptr    *fptr; /* The fptr is allowed to be modified, destroyed */
   int          *pInt; /* Potential int to modify */
   const char   *arg;
   uint16_t      mflags; /* Would like to make constant, but then can't define.. */
};
