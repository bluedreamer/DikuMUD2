/* *********************************************************************** *
 * File   : trie.c                                    Part of Valhalla MUD *
 * Version: 1.11                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: To make searching commands from a given input very fast.       *
 *          Time to locate command from input is O(|input|).               *
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
/* Sep 1992: gnort:  Added del_trie                                        */
/* Jul 1994: gnort:  Added free_trie                                       */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "structs.h"
#include "trie.h"
#include "utility.h"
#include "utils.h"

static int trie_size = 0, trie_nodes = 0;

auto trie_src_cmp(const void *keyval, const void *datum) -> int
{
   return (((char *)keyval)[0] - ((struct trie_entry *)datum)->c);
}

auto trie_sort_cmp(const void *keyval, const void *datum) -> int
{
   return (((struct trie_entry *)keyval)->c - ((struct trie_entry *)datum)->c);
}

auto triebindex(char c, struct trie_type *t) -> struct trie_entry *
{
   if(t->nexts != nullptr)
   {
      return (struct trie_entry *)bsearch(&c, t->nexts, t->size, sizeof(struct trie_entry), trie_src_cmp);
   }
   return NULL;
}

void qsort_triedata(struct trie_type *t)
{
   int i;

   qsort(t->nexts, t->size, sizeof(struct trie_entry), trie_sort_cmp);

   for(i = 0; i < t->size; i++)
   {
      qsort_triedata(t->nexts[i].t);
   }
}

auto trie_index(char c, struct trie_type *t) -> int
{
   int i;

   /* See if the char already exists in array */
   for(i = 0; i < t->size; i++)
   {
      if(t->nexts[i].c == c)
      {
         break;
      }
   }

   return i < t->size ? i : -1;
}

auto add_trienode(const char *s, struct trie_type *t) -> struct trie_type *
{
   int i;

   /* If no node is given, create one */
   if(t == nullptr)
   {
      trie_size += sizeof(struct trie_type);
      trie_nodes++;

      CREATE(t, struct trie_type, 1);
      t->data  = nullptr;
      t->nexts = nullptr;
      t->size  = 0;
   }

   if(*s != 0)
   {
      i = trie_index(*s, t);

      if(i == -1)
      {
         trie_size += sizeof(struct trie_entry);

         if(t->size == 0)
         {
            CREATE(t->nexts, struct trie_entry, 1);
         }
         else
         {
            RECREATE(t->nexts, struct trie_entry, t->size + 1);
         }

         t->size++;

         t->nexts[t->size - 1].c = *s;
         t->nexts[t->size - 1].t = add_trienode(s + 1, nullptr);
      }
      else
      {
         add_trienode(s + 1, t->nexts[i].t);
      }
   }

   return t;
}

void set_triedata(const char *s, struct trie_type *t, void *p, bool nonabbrev)
{
   int i;

   while((*s != 0) && (t != nullptr))
   {
      if(t->data == nullptr && (nonabbrev == 0u))
      {
         t->data = p;
      }

      i = trie_index(*s, t);

      if(i == -1)
      {
         slog(LOG_ALL, 0, "Bad, unlikely thing in set_triedata!");
         return; /* This is bad! */
      }

      t = t->nexts[i].t;
      s++;
   }

   if((t != nullptr) && t->data == nullptr)
   {
      t->data = p;
   }
}

/* It runs in nothing less than O(|s|), returns the data pointer */
auto search_trie(const char *s, struct trie_type *t) -> void *
{
   struct trie_entry *te;

   for(; (*s != 0) && (t != nullptr); t = te->t, ++s)
   {
      if((te = triebindex(*s, t)) == nullptr)
      { /* O(1) since always < 256 */
         return nullptr;
      }
   }

   return t->data;
}

/*  The following two procedures work by being supplied another procedure
 *  which takes care of erasing the data at the nodes.
 *  Use at own risk! :)
 */

/* Free an entire trie */
void free_trie(struct trie_type *t, void (*free_data)(void *))
{
   uint8_t i;

   /* If there's any data at this node, let the supplied method free it */
   if(t->data != nullptr)
   {
      (*free_data)(t->data);
   }

   /* Subtract size of free'ed info */
   trie_size -= (t->size * sizeof(struct trie_entry) + sizeof(struct trie_type));
   --trie_nodes;

   /* Walk through node-array, and call recursively */
   for(i = 0; i < t->size; i++)
   {
      if(t->nexts[i].t != nullptr)
      {
         free_trie(t->nexts[i].t, free_data);
      }
   }

   /* Clean up the last bits */
   if(t->size > 0)
   {
      free(t->nexts);
   }

   free(t);
}

/*  The deletion of trie-entries is ONLY possible if they're created in
 *  nonabbrev mode!
 *  The boolean returned is entirely for internal usage.
 *
 *  Overcommented due to non-trivialism :-)
 */
auto del_trie(char *s, struct trie_type **t, void (*free_data)(void *)) -> bool
{
   /* Any more string of keyword to delete? */
   if(*s != 0)
   { /* Yep, let's find next chars index */
      int i = trie_index(*s, *t);

      /* Found one, did deletion of rest of keyword delete node below? */
      if(i >= 0 && (del_trie(s + 1, &((*t)->nexts[i].t), free_data) != 0u))
      {
         if((*t)->size == 1) /* Yes.  Are we alone at this node? */
         {                   /* Yep, delete and confirm */
            trie_size -= (sizeof(struct trie_entry) + sizeof(struct trie_type));
            trie_nodes--;

            free((*t)->nexts);
            free(*t);
            *t = nullptr;
            return TRUE;
         }
         /* No, so we have to clean up carefully */
         trie_size -= sizeof(struct trie_entry);

         (*t)->size--;
         for(; i < (*t)->size; i++)
            (*t)->nexts[i] = (*t)->nexts[i + 1];
         RECREATE((*t)->nexts, struct trie_entry, (*t)->size);
      }
   }
   /* No more string.  Is there data at this node? */
   else if((*t)->data != nullptr)
   { /* Yep, delete */
      (*free_data)((*t)->data);
      (*t)->data = nullptr;

      if((*t)->size == 0) /* Is this a leaf? */
      {                   /* Yep, delete it, and confirm */
         trie_nodes--;
         trie_size -= sizeof(struct trie_type);
         free(*t);
         *t = nullptr;
         return TRUE;
      }
   }
   /* We didn't get to delete a node (sulk) */
   return FALSE;
}

/*
 * This is how a build trie ought to look
 * Refer to interpreter.c for an example in use.

struct trie_type *build_trie(char *strs[])
{
  struct trie_type *trie = NULL;
  int i;

  for (i = 0; *strs[i]; i++)
    trie = add_trienode(strs[i], trie);

  qsort_triedata(trie);

  return trie;
}

*/
