#pragma once

#include "trie_entry.h"

#include <cstdint>

class trie_type
{
public:
   void       *data;
   uint8_t     size;
   trie_entry *nexts;
};
