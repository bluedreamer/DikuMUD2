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

#ifndef _MUD_TRIE_H
#define _MUD_TRIE_H

struct trie_entry
{
   char              c;
   struct trie_type *t;
};

struct trie_type
{
   void              *data;
   uint8_t              size;
   struct trie_entry *nexts;
};

auto  search_trie(const char *s, struct trie_type *t) -> void *;
void  set_triedata(const char *s, struct trie_type *t, void *p, bool verbose);
auto  add_trienode(const char *s, struct trie_type *t) -> struct trie_type *;
void  free_trie(struct trie_type *t, void (*free_data)(void *));
bool del_trie(char *s, struct trie_type **t, void (*)(void *));
void  qsort_triedata(struct trie_type *t);

#endif /* _MUD_TRIE_H */
