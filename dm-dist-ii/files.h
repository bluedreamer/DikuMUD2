/* *********************************************************************** *
 * File   : files.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Various routines for administering files.                      *
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

#pragma once
#include "bytestring.h"

#define FCACHE_MAX 100

void fstrcpy(CByteBuffer *pBuf, FILE *f);
auto fread_line_commented(FILE *fl, char *buf, int max) -> char *;
auto fsize(FILE *f) -> long;
void touch_file(char *name);
auto fread_string(FILE *fl) -> char *;
auto fread_string_copy(FILE *fl, char *buf, int max) -> char *;
auto file_to_string(char *name, char *buf, int max_len) -> int;
auto config_file_to_string(char *name, char *buf, int max_len) -> int;
auto fread_num(FILE *fl) -> long;
auto fopen_cache(char *name, const char *mode) -> FILE *;
void fclose_cache();
auto file_exists(const char *name) -> ubit1;
