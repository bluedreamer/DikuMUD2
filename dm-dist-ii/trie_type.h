#pragma once

#include "trie_entry.h"

#include <cstdint>

struct trie_type
{
   void       *data;
   uint8_t     size;
   trie_entry *nexts;
};
