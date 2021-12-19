/* *********************************************************************** *
 * File   : bytestring.c                              Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Routines for writing and reading bytestrings.                  *
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "bytestring.h"
#include "essential.h"
#include "textutil.h"

/* =================================================================== */
/* =================================================================== */


/* =================================================================== */
/* =================================================================== */

auto bread_uint8_t(uint8_t **b) -> uint8_t
{
   uint8_t i;

   memcpy((uint8_t *)&i, *b, sizeof(uint8_t));
   *b += sizeof(uint8_t);

   return i;
}

auto bread_uint16_t(uint8_t **b) -> uint16_t
{
   uint16_t i;

   memcpy((uint8_t *)&i, *b, sizeof(uint16_t));
   *b += sizeof(uint16_t);

   return i;
}

auto bread_uint32_t(uint8_t **b) -> uint32_t
{
   uint32_t i;

   memcpy((uint8_t *)&i, *b, sizeof(uint32_t));
   *b += sizeof(uint32_t);

   return i;
}

auto bread_float(uint8_t **b) -> float
{
   float f;

   memcpy((uint8_t *)&f, *b, sizeof(float));
   *b += sizeof(float);

   return f;
}

auto bread_data(uint8_t **b, uint32_t *plen) -> uint8_t *
{
   uint32_t len;
   uint8_t *data;

   data = nullptr;
   len  = bread_uint32_t(b);
   if(plen != nullptr)
   {
      *plen = len;
   }

   if(len > 0)
   {
      CREATE(data, uint8_t, len);
      memcpy(data, *b, len);
      *b += len;
   }

   return data;
}

/* Stored: as Null terminated string            */
/* Copy string from **b into *str               */
void bread_strcpy(uint8_t **b, char *str)
{
   for(; (*str++ = **b) != 0; (*b)++)
   {
      ;
   }
   (*b)++;
}

/*  Stored: as Null terminated string
 *  Will allocate space for string, if the read
 *  string is one or more characters, and return
 *  pointer to allocated string (or 0)
 */
auto bread_str_alloc(uint8_t **b) -> char *
{
   if(**b != 0u)
   {
      char *c;
      char *t;
      t = (char *)*b;

      c = str_dup(t);

      *b += strlen(c) + 1;
      return c;
   }

   (*b)++;
   return nullptr;
}

/* Returns pointer to the string and skips past the end to next
   point in buffer */
auto bread_str_skip(uint8_t **b) -> char *
{
   char *o = (char *)*b;

   TAIL(*b);
   (*b)++;

   return o;
}

/* Stored: As 'N' strings followed by the empty */
/* string ("")                                  */
/* Returns * to nameblock, nameblock may be     */
/* but is never null ({""}).                    */
auto bread_nameblock(uint8_t **b) -> char **
{
   char   buf[MAX_STRING_LENGTH];
   char **nb;

   nb = create_namelist();

   for(;;)
   {
      bread_strcpy(b, buf);
      if(*buf != 0)
      {
         nb = add_name(buf, nb);
      }
      else
      {
         break;
      }
   }

   return nb;
}

void bwrite_uint8_t(uint8_t **b, uint8_t i)
{
   **b = i;
   *b += sizeof(uint8_t);
}

void bwrite_uint16_t(uint8_t **b, uint16_t i)
{
   memcpy(*b, (uint8_t *)&i, sizeof(uint16_t));
   *b += sizeof(uint16_t);
}

void bwrite_uint32_t(uint8_t **b, uint32_t i)
{
   memcpy(*b, (uint8_t *)&i, sizeof(uint32_t));
   *b += sizeof(uint32_t);
}

void bwrite_float(uint8_t **b, float f)
{
   memcpy(*b, (uint8_t *)&f, sizeof(float));
   *b += sizeof(float);
}

void bwrite_data(uint8_t **b, uint8_t *data, uint32_t len)
{
   bwrite_uint32_t(b, len);
   if(len > 0)
   {
      memcpy(*b, data, len);
      *b += len;
   }
}

/* String is stored as Null terminated string   */
/* Space is NOT allocated if string is 0 length */
/* but NIL is returned                          */
void bwrite_string(uint8_t **b, const char *str)
{
   if(str != nullptr)
   {
      for(; *str != 0; str++, (*b)++)
      {
         **b = *str;
      }

      **b = '\0';
      *b += 1;
   }
   else
   {
      **b = '\0';
      *b += 1;
   }
}

/* Write a string of the format:  ssss\0ssss\0 */
void bwrite_double_string(uint8_t **b, char *str)
{
   int i;

   if(str != nullptr)
   {
      for(i = 0; i < 2; str++, (*b)++, (*str != 0 ? 0 : i++))
      {
         **b = *str;
      }

      **b = '\0';
      *b += 1;
   }
   else
   {
      bwrite_string(b, "");
      bwrite_string(b, "");
   }
}

/* Stored: As 'N' strings followed by the empty string ("") */
void bwrite_nameblock(uint8_t **b, char **nb)
{
   if(nb != nullptr)
   {
      for(; (*nb != nullptr) && (**nb != 0); nb++)
      {
         bwrite_string(b, *nb);
      }
   }

   bwrite_string(b, "");
}
