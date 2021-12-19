#pragma once
/* *********************************************************************** *
 * File   : textutil.h                                Part of Valhalla MUD *
 * Version: 1.02                                                           *
 * Author : all                                                            *
 *                                                                         *
 * Purpose: String related headers.                                        *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

#include "values.h"

#define isaspace(c) ((c) == ' ')

auto str_escape_format(const char *str, char *dest, int destlen, int format = 1) -> int;
auto str_escape_format(const char *src, int formatting = 1) -> char *;

auto text_read_line(char *buf, char *res) -> char *;
auto text_skip_line(char *buf) -> char *;
auto catnames(char *s, const char **names) -> char *;
void str_insert(char *str, char *itxt);
void str_substitute(const char *old, const char *newstr, char *str);
void str_rem(char *s, char c);
void str_rem_punct(char *s);
void str_blank_punct(char *s);
void str_remspc(char *s);
void str_chraround(char *str, char c);

auto is_name_raw(const char *arg, const char *names[]) -> char *;
auto is_name(const char *arg, const char *names[]) -> char *;

auto str_lower(char *s) -> int;
auto str_lower(const char *s, char *d, int nBufSize) -> int;

auto spc(int n) -> char *;
auto str_line(const char *, char *) -> char *;
#ifdef DOS
   #define itoa(n) itoa_dos(n)
char *itoa_dos(int n);
#else
auto itoa(int n) -> char *;
#endif
auto ltoa(long n) -> char *;
auto str_ccmp(const char *s, const char *d) -> int;
auto str_nccmp(const char *s, const char *d, int n) -> int;
auto str_dup(const char *source) -> char *;
auto str_str(const char *cs, const char *ct) -> char *;
auto str_cstr(const char *cs, const char *ct) -> char *;
auto str_is_empty(const char *arg) -> bool;
auto str_is_number(const char *str) -> bool;
auto next_word_is_number(const char *str) -> bool;
auto one_argument(const char *argument, char *first_arg) -> char *;
auto search_block_abbrevs(const char *oarg, const char **list, const char **end) -> int;
auto search_block(const char *arg, const char **list, bool exact) -> int;
auto search_block_length(const char *arg, int length, const char **list, bool exact) -> int;
auto fill_word(const char *argument) -> int;
auto is_abbrev(const char *arg1, const char *arg2) -> bool;
auto str_cc(const char *s1, const char *s2) -> char *;
auto add_name(const char *name, char **namelist) -> char **;
auto del_name(const char *name, char **namelist) -> char **;
auto create_namelist() -> char **;
auto namestring_to_namelist(const char *str) -> char **;
void free_namelist(char **list);
auto str_ccmp_next_word(const char *buf, const char *next_word) -> char *;
auto copy_namelist(const char **source) -> char **;
auto str_next_word_copy(const char *argument, char *first_arg) -> char *;
auto str_next_word(const char *argument, char *first_arg) -> char *;
void split_fi_ref(const char *str, char *zone, char *name);
auto skip_spaces(const char *) -> char *;
auto skip_blanks(const char *string) -> char *;
void strip_trailing_spaces(char *str);
void strip_trailing_blanks(char *str);

//#ifdef toupper
//   #undef toupper
//#endif
//#define toupper(c) ((c) >= 'a' && (c) <= 'z' ? ((c) + ('A' - 'a')) : (c))
//
//#ifdef tolower
//   #undef tolower
//#endif
//#define tolower(c) ((c) >= 'A' && (c) <= 'Z' ? ((c) + ('a' - 'A')) : (c))

#define str_delete(str, nDel, nStrLen) memmove(str, str + nDel, nStrLen - nDel + 1)
