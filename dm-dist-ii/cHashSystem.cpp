#include <algorithm>
#include <cassert>
#include <cstring>
#include "cHashSystem.h"

cHashSystem Hash;

cHashSystem::cHashSystem()
{
   int i;

   for(i = 0; i < HASH_SIZE; i++)
   {
      str_table[i] = nullptr;
   }
}

/*
 * Hash function hashpjw, from Aho et al's
 * "Compilers: Principles, Techniques and Tools", page 436
 */

auto cHashSystem::MaxDepth(uint32_t *slots) -> uint32_t
{
   cStringConstant *tmp;
   uint32_t               depth;
   uint32_t               i;
   uint32_t               tmpd;

   if(slots != nullptr)
   {
      *slots = 0;
   }

   for(depth = i = 0; i < HASH_SIZE; ++i)
   {
      if(str_table[i] != nullptr)
      {
         if(slots != nullptr)
         {
            ++(*slots);
         }
         for(tmpd = 0, tmp = str_table[i]; tmp != nullptr; tmp = tmp->Next())
         {
            ++tmpd;
         }
         depth = std::max(depth, tmpd);
      }
   }

   return depth;
}

auto cHashSystem::Hash(const char *str) -> uint32_t
{
   uint32_t h = 0;
   uint32_t g;

   for(; *str != 0; ++str)
   {
      if((g = ((h = (h << 4) + *str) & 0xf0000000)) != 0U)
      {
         h = (h ^ (g >> 24)) ^ g;
      }
   }

   return h % HASH_SIZE;
}

auto cHashSystem::Lookup(const char *str, uint32_t nLen, uint32_t nHash) -> cStringConstant *
{
   cStringConstant *lookup;

   assert(nHash < HASH_SIZE);

   for(lookup = str_table[nHash]; lookup != nullptr; lookup = lookup->pNext)
   {
      if(nLen == lookup->nStrLen && memcmp(str, lookup->String(), nLen) == 0)
      {
         return lookup;
      }
   }

   return nullptr;
}

void cHashSystem::Insert(cStringConstant *p, uint32_t nHash)
{
   p->pNext         = str_table[nHash];
   str_table[nHash] = p;
}

void cHashSystem::Remove(cStringConstant *r)
{
   uint32_t               h;
   cStringConstant *o;
   cStringConstant *p;

   h = Hash(r->pStr); // Can improve speed by storing in struct.

   if(r == str_table[h])
   {
      str_table[h] = str_table[h]->pNext;
   }
   else
   {
      for(p = str_table[h], o = p->pNext; o != nullptr; p = o, o = o->pNext)
      {
         if(r == o)
         {
            p->pNext = o->pNext;
            break;
         }
      }

      assert(r == o);
   }
}
