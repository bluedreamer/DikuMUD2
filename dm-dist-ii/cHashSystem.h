#pragma once

#include "cStringConstant.h"

#include <cstdint>

#define HASH_SIZE 1511 /* MUST be prime! */

class cHashSystem
{
public:
   cHashSystem();
   static auto Hash(const char *str) -> uint32_t;
   void        Insert(cStringConstant *p, uint32_t nHash);
   void        Remove(cStringConstant *p);
   auto        Lookup(const char *str, uint32_t nLen, uint32_t nHash) -> cStringConstant *;
   auto        MaxDepth(uint32_t        */*slots*/) -> uint32_t;

private:
   cStringConstant *str_table[HASH_SIZE];
};
extern cHashSystem Hash;
