#pragma once
/* *********************************************************************** *
 * File   : trie.h                                    Part of Valhalla MUD *
 * Version: 1.02                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Prototypes...                                                  *
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

#include "trie_type.h"

auto search_trie(const char *s, trie_type *t) -> void *;
void set_triedata(const char *s, trie_type *t, void *p, bool verbose);
auto add_trienode(const char *s, trie_type *t) -> trie_type *;
void free_trie(trie_type *t, void (*free_data)(void *));
auto del_trie(char *s, trie_type **t, void (*)(void *)) -> bool;
void qsort_triedata(trie_type *t);
