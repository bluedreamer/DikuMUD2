#pragma once
#include "cHook.h"
#include "essential.h"
#include "terminal_setup_type.h"

/* Use these Super Colour defines, so that we can easilly change the
   default colours */

#define COLOUR_MENU   CONTROL_FGB_WHITE
#define COLOUR_NORMAL CONTROL_FG_WHITE
#define COLOUR_COMM   CONTROL_FGB_WHITE
#define COLOUR_MOB    CONTROL_FG_CYAN
#define COLOUR_ATTN   CONTROL_FGB_RED

#define TERM_DUMB     0
#define TERM_TTY      1
#define TERM_ANSI     2
#define TERM_VT100    3
#define TERM_INTERNAL 4

auto read_to_queue(int fd, cQueue *q) -> int;
void protocol_send_close(cHook *Hook, uint16_t id);
void protocol_send_confirm(cHook *Hook, uint16_t id);
void protocol_send_request(cHook *Hook);
void protocol_send_text(cHook *Hook, uint16_t id, const char *text, uint8_t type);
void protocol_send_setup(cHook *Hook, uint16_t id, terminal_setup_type *setup);
void protocol_send_host(cHook *Hook, uint16_t id, char *host, uint16_t nPort, uint8_t nLine);

auto protocol_parse_incoming(cHook *Hook, uint16_t *pid, uint16_t *plen, char **str, uint8_t *text_type) -> int;

#define MULTI_UNIQUE_CHAR '\x01'
#define MULTI_UNIQUE_STR  "\x01"

#define MULTI_CONNECT_REQ_CHAR 'A'
#define MULTI_CONNECT_CON_CHAR 'B'
#define MULTI_TERMINATE_CHAR   'C'
#define MULTI_SETUP_CHAR       'D'
#define MULTI_HOST_CHAR        'E'
#define MULTI_TEXT_CHAR        'F'
#define MULTI_PAGE_CHAR        'G'
#define MULTI_PROMPT_CHAR      'H'

#define MULTI_CONNECT_REQ_STR "A"
#define MULTI_CONNECT_CON_STR "B"
#define MULTI_TERMINATE_STR   "C"
#define MULTI_SETUP_STR       "D"
#define MULTI_HOST_STR        "E"
#define MULTI_TEXT_STR        "F"
#define MULTI_PAGE_STR        "G"
#define MULTI_PROMPT_STR      "H"

#define MULTI_CONNECT_REQ MULTI_UNIQUE_STR MULTI_CONNECT_REQ_STR "\0\0\0\0"
#define MULTI_CONNECT_CON MULTI_UNIQUE_STR MULTI_CONNECT_CON_STR
#define MULTI_TERMINATE   MULTI_UNIQUE_STR MULTI_TERMINATE_STR "\0\0\0\0"
#define MULTI_SETUP       MULTI_UNIQUE_STR MULTI_SETUP_STR
#define MULTI_HOST        MULTI_UNIQUE_STR MULTI_HOST_STR "\0\0\0\0"

#define MULTI_TEXT   MULTI_UNIQUE_STR MULTI_TXT_STR
#define MULTI_PAGE   MULTI_UNIQUE_STR MULTI_TXT_STR
#define MULTI_PROMPT MULTI_UNIQUE_STR MULTI_TXT_STR

#define CONTROL_CHAR '\x1b'

#define CONTROL_ECHO_OFF_CHAR 'A'
#define CONTROL_ECHO_ON_CHAR  'B'
#define CONTROL_BOLD_CHAR     'C'
#define CONTROL_REVERSE_CHAR  'D'
#define CONTROL_RESET_CHAR    'E'
#define CONTROL_DIM_CHAR      'F'
#define CONTROL_HOME_CHAR     'H'

/* Must run in sequence +1 from fg black */

#define CONTROL_FG_BLACK_CHAR   'a'
#define CONTROL_FG_RED_CHAR     'b'
#define CONTROL_FG_GREEN_CHAR   'c'
#define CONTROL_FG_YELLOW_CHAR  'd'
#define CONTROL_FG_BLUE_CHAR    'e'
#define CONTROL_FG_MAGENTA_CHAR 'f'
#define CONTROL_FG_CYAN_CHAR    'g'
#define CONTROL_FG_WHITE_CHAR   'h'

#define CONTROL_FGB_BLACK_CHAR   'i'
#define CONTROL_FGB_RED_CHAR     'j'
#define CONTROL_FGB_GREEN_CHAR   'k'
#define CONTROL_FGB_YELLOW_CHAR  'l'
#define CONTROL_FGB_BLUE_CHAR    'm'
#define CONTROL_FGB_MAGENTA_CHAR 'n'
#define CONTROL_FGB_CYAN_CHAR    'o'
#define CONTROL_FGB_WHITE_CHAR   'p'

#define CONTROL_BG_BLACK_CHAR   'q'
#define CONTROL_BG_RED_CHAR     'r'
#define CONTROL_BG_GREEN_CHAR   's'
#define CONTROL_BG_YELLOW_CHAR  't'
#define CONTROL_BG_BLUE_CHAR    'u'
#define CONTROL_BG_MAGENTA_CHAR 'v'
#define CONTROL_BG_CYAN_CHAR    'w'
#define CONTROL_BG_WHITE_CHAR   'x'

#define CONTROL_ECHO_OFF                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "A"
#define CONTROL_ECHO_ON                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "B"

#define CONTROL_BOLD                                                                                                                       \
   "\x1B"                                                                                                                                  \
   "C"
#define CONTROL_REVERSE                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "D"

#define CONTROL_RESET                                                                                                                      \
   "\x1B"                                                                                                                                  \
   "E"
#define CONTROL_DIM                                                                                                                        \
   "\x1B"                                                                                                                                  \
   "F"
#define CONTROL_HOME                                                                                                                       \
   "\x1B"                                                                                                                                  \
   "H"

#define CONTROL_FG_BLACK                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "a"
#define CONTROL_FG_RED                                                                                                                     \
   "\x1B"                                                                                                                                  \
   "b"
#define CONTROL_FG_GREEN                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "c"
#define CONTROL_FG_YELLOW                                                                                                                  \
   "\x1B"                                                                                                                                  \
   "d"
#define CONTROL_FG_BLUE                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "e"
#define CONTROL_FG_MAGENTA                                                                                                                 \
   "\x1B"                                                                                                                                  \
   "f"
#define CONTROL_FG_CYAN                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "g"
#define CONTROL_FG_WHITE                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "h"

#define CONTROL_FGB_BLACK                                                                                                                  \
   "\x1B"                                                                                                                                  \
   "i"
#define CONTROL_FGB_RED                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "j"
#define CONTROL_FGB_GREEN                                                                                                                  \
   "\x1B"                                                                                                                                  \
   "k"
#define CONTROL_FGB_YELLOW                                                                                                                 \
   "\x1B"                                                                                                                                  \
   "l"
#define CONTROL_FGB_BLUE                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "m"
#define CONTROL_FGB_MAGENTA                                                                                                                \
   "\x1B"                                                                                                                                  \
   "n"
#define CONTROL_FGB_CYAN                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "o"
#define CONTROL_FGB_WHITE                                                                                                                  \
   "\x1B"                                                                                                                                  \
   "p"

#define CONTROL_BG_BLACK                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "q"
#define CONTROL_BG_RED                                                                                                                     \
   "\x1B"                                                                                                                                  \
   "r"
#define CONTROL_BG_GREEN                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "s"
#define CONTROL_BG_YELLOW                                                                                                                  \
   "\x1B"                                                                                                                                  \
   "t"
#define CONTROL_BG_BLUE                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "u"
#define CONTROL_BG_MAGENTA                                                                                                                 \
   "\x1B"                                                                                                                                  \
   "v"
#define CONTROL_BG_CYAN                                                                                                                    \
   "\x1B"                                                                                                                                  \
   "w"
#define CONTROL_BG_WHITE                                                                                                                   \
   "\x1B"                                                                                                                                  \
   "x"
