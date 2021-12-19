#pragma once
#include "trie_type.h"

auto search_trie(const char *s, trie_type *t) -> void *;
void set_triedata(const char *s, trie_type *t, void *p, bool verbose);
auto add_trienode(const char *s, trie_type *t) -> trie_type *;
void free_trie(trie_type *t, void (*free_data)(void *));
auto del_trie(char *s, trie_type **t, void (*)(void *)) -> bool;
void qsort_triedata(trie_type *t);
