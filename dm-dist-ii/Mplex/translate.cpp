#include "mplex.h"
#include "ttydef.h"

#include <arpa/telnet.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <strings.h>

static uint8_t default_colours[3][24] = {
   /* Default (no change in table) */
   {CONTROL_FG_BLACK_CHAR,  CONTROL_FG_RED_CHAR,      CONTROL_FG_GREEN_CHAR,  CONTROL_FG_YELLOW_CHAR,
    CONTROL_FG_BLUE_CHAR,   CONTROL_FG_MAGENTA_CHAR,  CONTROL_FG_CYAN_CHAR,   CONTROL_FG_WHITE_CHAR,

    CONTROL_FGB_BLACK_CHAR, CONTROL_FGB_RED_CHAR,     CONTROL_FGB_GREEN_CHAR, CONTROL_FGB_YELLOW_CHAR,
    CONTROL_FGB_BLUE_CHAR,  CONTROL_FGB_MAGENTA_CHAR, CONTROL_FGB_CYAN_CHAR,  CONTROL_FGB_WHITE_CHAR,

    CONTROL_BG_BLACK_CHAR,  CONTROL_BG_RED_CHAR,      CONTROL_BG_GREEN_CHAR,  CONTROL_BG_YELLOW_CHAR,
    CONTROL_BG_BLUE_CHAR,   CONTROL_BG_MAGENTA_CHAR,  CONTROL_BG_CYAN_CHAR,   CONTROL_BG_WHITE_CHAR},

   {CONTROL_FG_BLACK_CHAR,  CONTROL_FG_RED_CHAR,      CONTROL_FG_YELLOW_CHAR,  CONTROL_FG_YELLOW_CHAR,
    CONTROL_FG_BLUE_CHAR,   CONTROL_FG_MAGENTA_CHAR,  CONTROL_FG_BLUE_CHAR,    CONTROL_FG_BLACK_CHAR,

    CONTROL_FGB_BLACK_CHAR, CONTROL_FGB_RED_CHAR,     CONTROL_FGB_YELLOW_CHAR, CONTROL_FGB_YELLOW_CHAR,
    CONTROL_FGB_BLUE_CHAR,  CONTROL_FGB_MAGENTA_CHAR, CONTROL_FGB_BLUE_CHAR,   CONTROL_FGB_BLACK_CHAR,

    CONTROL_BG_WHITE_CHAR,  CONTROL_BG_RED_CHAR,      CONTROL_BG_GREEN_CHAR,   CONTROL_BG_YELLOW_CHAR,
    CONTROL_BG_BLUE_CHAR,   CONTROL_BG_MAGENTA_CHAR,  CONTROL_BG_CYAN_CHAR,    CONTROL_BG_WHITE_CHAR},

   {CONTROL_FG_BLACK_CHAR,  CONTROL_FG_MAGENTA_CHAR,  CONTROL_FG_GREEN_CHAR,  CONTROL_FG_YELLOW_CHAR,
    CONTROL_FG_BLUE_CHAR,   CONTROL_FG_MAGENTA_CHAR,  CONTROL_FG_CYAN_CHAR,   CONTROL_FG_WHITE_CHAR,

    CONTROL_FGB_BLACK_CHAR, CONTROL_FGB_MAGENTA_CHAR, CONTROL_FGB_GREEN_CHAR, CONTROL_FGB_YELLOW_CHAR,
    CONTROL_FGB_BLUE_CHAR,  CONTROL_FGB_MAGENTA_CHAR, CONTROL_FGB_CYAN_CHAR,  CONTROL_FGB_WHITE_CHAR,

    CONTROL_BG_BLUE_CHAR,   CONTROL_BG_RED_CHAR,      CONTROL_BG_GREEN_CHAR,  CONTROL_BG_YELLOW_CHAR,
    CONTROL_BG_BLUE_CHAR,   CONTROL_BG_MAGENTA_CHAR,  CONTROL_BG_CYAN_CHAR,   CONTROL_BG_WHITE_CHAR}};

static void (*control_code[5][256])(class cConHook *con, char **b, uint8_t code);

#define TELNET_SEND_GA(buf)  sprintf(buf, "%c%c", IAC, GA)
#define TELNET_ECHO_OFF(buf) sprintf(buf, "%c%c%c", IAC, WILL, TELOPT_ECHO)
#define TELNET_ECHO_ON(buf)  sprintf(buf, "%c%c%c", IAC, WONT, TELOPT_ECHO)

/* ----------------------------------------------------------------------- */

#define PROTOCOL_TRANSLATE(con, code, b)                                                                                                   \
   if(control_code[(con)->m_sSetup.emulation][code])                                                                                       \
      (*control_code[(con)->m_sSetup.emulation][code])((con), (b), (code));

void protocol_translate(class cConHook *con, uint8_t code, char **b)
{
   assert(b && *b);

   if(code >= CONTROL_FG_BLACK_CHAR && code <= CONTROL_BG_WHITE_CHAR)
   {
      code = default_colours[con->m_sSetup.colour_convert][code - CONTROL_FG_BLACK_CHAR];
   }

   PROTOCOL_TRANSLATE(con, code, b);
}

/* ======================================================================= */
/*                             A N S I                                     */
/* ======================================================================= */

static void Control_ANSI_Fg_Black(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_BLACK);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_Red(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_RED);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_Green(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_GREEN);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_Yellow(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_YELLOW);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_Blue(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_BLUE);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_Magenta(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_MAGENTA);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_Cyan(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_CYAN);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fg_White(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FG_WHITE);
   TAIL(*b);

   if((con->m_nBgColor >= (uint8_t)CONTROL_BG_BLACK_CHAR) && (con->m_nBgColor <= (uint8_t)CONTROL_BG_WHITE_CHAR))
      PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

static void Control_ANSI_Fgb_Black(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_BLACK);
   TAIL(*b);
}

static void Control_ANSI_Fgb_Red(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_RED);
   TAIL(*b);
}

static void Control_ANSI_Fgb_Green(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_GREEN);
   TAIL(*b);
}

static void Control_ANSI_Fgb_Yellow(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_YELLOW);
   TAIL(*b);
}

static void Control_ANSI_Fgb_Blue(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_BLUE);
   TAIL(*b);
}

static void Control_ANSI_Fgb_Magenta(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_MAGENTA);
   TAIL(*b);
}

static void Control_ANSI_Fgb_Cyan(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_CYAN);
   TAIL(*b);
}

static void Control_ANSI_Fgb_White(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_FGB_WHITE);
   TAIL(*b);
}

static void Control_ANSI_Bg_Black(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_BLACK);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_BLACK_CHAR;
}

static void Control_ANSI_Bg_Red(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_RED);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_RED_CHAR;
}

static void Control_ANSI_Bg_Green(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_GREEN);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_GREEN_CHAR;
}

static void Control_ANSI_Bg_Yellow(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_YELLOW);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_YELLOW_CHAR;
}

static void Control_ANSI_Bg_Blue(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_BLUE);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_BLUE_CHAR;
}

static void Control_ANSI_Bg_Magenta(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_MAGENTA);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_MAGENTA_CHAR;
}

static void Control_ANSI_Bg_Cyan(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_CYAN);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_CYAN_CHAR;
}

static void Control_ANSI_Bg_White(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BG_WHITE);
   TAIL(*b);
   con->m_nBgColor = CONTROL_BG_WHITE_CHAR;
}

static void Control_ANSI_Echo_Off(class cConHook *con, char **b, uint8_t code)
{
   if(con->m_sSetup.telnet != 0u)
   {
      TELNET_ECHO_OFF(*b);
      TAIL(*b);
   }

   strcpy(*b, ANSI_CONCEALED);
   TAIL(*b);
}

static void Control_ANSI_Reset(class cConHook *con, char **b, uint8_t code)
{
   if(con->m_sSetup.telnet != 0u)
   {
      TELNET_ECHO_ON(*b);
      TAIL(*b);
   }

   strcpy(*b, ANSI_RESET);
   TAIL(*b);

   Control_ANSI_Fg_White(con, b, code);
}

static void Control_ANSI_Home(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_HOME);
   TAIL(*b);
   con->m_nPromptLen = 0;
}

static void Control_ANSI_Bold(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_BOLD);
   TAIL(*b);
}

static void Control_ANSI_Reverse(class cConHook *con, char **b, uint8_t code)
{
   strcpy(*b, ANSI_REVERSE);
   TAIL(*b);
}

static void Control_TTY_Echo_Off(class cConHook *con, char **b, uint8_t code)
{
   if(con->m_sSetup.telnet != 0u)
   {
      TELNET_ECHO_OFF(*b);
      TAIL(*b);
   }
}

static void Control_TTY_Echo_On(class cConHook *con, char **b, uint8_t code)
{
   if(con->m_sSetup.telnet != 0u)
   {
      TELNET_ECHO_ON(*b);
      TAIL(*b);
   }
}

static void Control_Copy(class cConHook *con, char **b, uint8_t code)
{
   **b = CONTROL_CHAR;
   (*b)++;
   **b = code;
   (*b)++;
}

void translate_init()
{
   int i;
   int j;

   for(j = 0; j < 4; j++)
   {
      for(i = 0; i < 256; i++)
      {
         control_code[j][i] = nullptr;
      }
   }

   control_code[TERM_TTY][CONTROL_ECHO_OFF_CHAR]     = Control_TTY_Echo_Off;
   control_code[TERM_TTY][CONTROL_ECHO_ON_CHAR]      = Control_TTY_Echo_On;
   control_code[TERM_TTY][CONTROL_RESET_CHAR]        = Control_TTY_Echo_On;

   control_code[TERM_ANSI][CONTROL_ECHO_OFF_CHAR]    = Control_ANSI_Echo_Off;
   control_code[TERM_ANSI][CONTROL_ECHO_ON_CHAR]     = Control_ANSI_Reset;
   control_code[TERM_ANSI][CONTROL_RESET_CHAR]       = Control_ANSI_Reset;
   control_code[TERM_ANSI][CONTROL_HOME_CHAR]        = Control_ANSI_Home;

   control_code[TERM_ANSI][CONTROL_BOLD_CHAR]        = Control_ANSI_Bold;
   control_code[TERM_ANSI][CONTROL_REVERSE_CHAR]     = Control_ANSI_Reverse;

   control_code[TERM_ANSI][CONTROL_FG_BLACK_CHAR]    = Control_ANSI_Fg_Black;
   control_code[TERM_ANSI][CONTROL_FG_RED_CHAR]      = Control_ANSI_Fg_Red;
   control_code[TERM_ANSI][CONTROL_FG_GREEN_CHAR]    = Control_ANSI_Fg_Green;
   control_code[TERM_ANSI][CONTROL_FG_YELLOW_CHAR]   = Control_ANSI_Fg_Yellow;
   control_code[TERM_ANSI][CONTROL_FG_BLUE_CHAR]     = Control_ANSI_Fg_Blue;
   control_code[TERM_ANSI][CONTROL_FG_MAGENTA_CHAR]  = Control_ANSI_Fg_Magenta;
   control_code[TERM_ANSI][CONTROL_FG_CYAN_CHAR]     = Control_ANSI_Fg_Cyan;
   control_code[TERM_ANSI][CONTROL_FG_WHITE_CHAR]    = Control_ANSI_Fg_White;

   control_code[TERM_ANSI][CONTROL_FGB_BLACK_CHAR]   = Control_ANSI_Fgb_Black;
   control_code[TERM_ANSI][CONTROL_FGB_RED_CHAR]     = Control_ANSI_Fgb_Red;
   control_code[TERM_ANSI][CONTROL_FGB_GREEN_CHAR]   = Control_ANSI_Fgb_Green;
   control_code[TERM_ANSI][CONTROL_FGB_YELLOW_CHAR]  = Control_ANSI_Fgb_Yellow;
   control_code[TERM_ANSI][CONTROL_FGB_BLUE_CHAR]    = Control_ANSI_Fgb_Blue;
   control_code[TERM_ANSI][CONTROL_FGB_MAGENTA_CHAR] = Control_ANSI_Fgb_Magenta;

   control_code[TERM_ANSI][CONTROL_FGB_CYAN_CHAR]    = Control_ANSI_Fgb_Cyan;
   control_code[TERM_ANSI][CONTROL_FGB_WHITE_CHAR]   = Control_ANSI_Fgb_White;

   control_code[TERM_ANSI][CONTROL_BG_BLACK_CHAR]    = Control_ANSI_Bg_Black;
   control_code[TERM_ANSI][CONTROL_BG_RED_CHAR]      = Control_ANSI_Bg_Red;
   control_code[TERM_ANSI][CONTROL_BG_GREEN_CHAR]    = Control_ANSI_Bg_Green;
   control_code[TERM_ANSI][CONTROL_BG_YELLOW_CHAR]   = Control_ANSI_Bg_Yellow;
   control_code[TERM_ANSI][CONTROL_BG_BLUE_CHAR]     = Control_ANSI_Bg_Blue;
   control_code[TERM_ANSI][CONTROL_BG_MAGENTA_CHAR]  = Control_ANSI_Bg_Magenta;
   control_code[TERM_ANSI][CONTROL_BG_CYAN_CHAR]     = Control_ANSI_Bg_Cyan;
   control_code[TERM_ANSI][CONTROL_BG_WHITE_CHAR]    = Control_ANSI_Bg_White;

   for(i = 0; i < 256; i++)
   {
      control_code[TERM_INTERNAL][i] = Control_Copy;
   }
}
