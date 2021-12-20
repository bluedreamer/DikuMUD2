#pragma once

#include <cstdint>

class terminal_setup_type
{
public:
   uint8_t redraw;         /* Redraw the users prompt? */
   uint8_t echo;           /* Echo chars to user?      */
   uint8_t width, height;  /* The colour of his nose   */
   uint8_t emulation;      /* TERM_XXX                 */
   uint8_t telnet;         /* Is telnet used?          */
   uint8_t colour_convert; /* Colour conversion (mapping) used */
};
