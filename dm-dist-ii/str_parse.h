#pragma once
/* *********************************************************************** *
 * File   : str_parse.h                               Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Headers.                                                       *
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

auto parse_match(char *pData, char *pMatch) -> char *;
auto parse_name(char **pData) -> char *;
auto parse_numlist(char **pData, int *count) -> int *;
auto parse_num(char **pData, int *pNum) -> int;
auto parse_namelist(char **pData) -> char **;
auto parse_match_num(char **pData, const char *pMatch, int *pResult) -> int;
auto parse_match_numlist(char **pData, const char *pMatch, int *count) -> int *;
auto parse_match_name(char **pData, const char *pMatch) -> char *;
auto parse_match_namelist(char **pData, const char *pMatch) -> char **;
