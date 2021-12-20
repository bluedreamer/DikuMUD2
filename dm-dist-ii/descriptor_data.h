#pragma once
#include "cMultiHook.h"
#include "snoop_data.h"

#include <cstdint>
#include <ctime>

class unit_data;

class descriptor_data
{
public:
   descriptor_data(cMultiHook *pe);
   ~descriptor_data();

   void CreateBBS() const;
   void RemoveBBS() const;

   time_t      logon; /* Time of last connect              */
   cMultiHook *multi; /* Multi element pointer             */
   uint16_t    id;    /* The ID for the multi              */
   void (*fptr)(descriptor_data *, const char *);
   int      state;    /* Locally used in each fptr         */
   char     host[50]; /* hostname                          */
   uint16_t nPort;    /* Mplex port                        */
   uint8_t  nLine;    /* Serial Line                       */
   int      wait;     /* wait for how many loops           */
   uint16_t timer;    /* num of hours idleness for mortals */
   uint32_t replyid;  /* Used for 'tell reply'             */

   /* For the 'modify-string' system.       */
   char *localstr; /* This string is expanded while editing */

   void (*postedit)(descriptor_data *);
   unit_data *editing;
   void      *editref; /* pointer to "where we are editing"     */
                       /* when using (volatile) extras + boards */

   int        prompt_mode;                    /* control of prompt-printing       */
   char       last_cmd[MAX_INPUT_LENGTH + 1]; /* the last entered cmd_str         */
   char       history[MAX_INPUT_LENGTH + 1];  /* simple command history           */
   cQueue     qInput;                         /* q of unprocessed input           */
   unit_data *character;                      /* linked to char                   */
   unit_data *original;                       /* original char                    */
   snoop_data snoop;                          /* to snoop people.                 */

   descriptor_data *next; /* link to next descriptor          */
};
