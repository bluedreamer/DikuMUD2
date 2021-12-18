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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "bytestring.h"
#include "essential.h"
#include "textutil.h"

/* =================================================================== */
/* =================================================================== */

CByteBuffer::CByteBuffer(uint32_t nSize)
{
   m_nLength    = 0;
   m_nReadPos   = 0;
   m_nAllocated = nSize;
   m_pData      = (uint8_t *)calloc(nSize, 1);

   assert(m_pData != nullptr);
}

CByteBuffer::~CByteBuffer()
{
   free(m_pData);
}

void CByteBuffer::SetReadPosition(uint32_t nReadPosition)
{
   m_nReadPos = nReadPosition;
}

void CByteBuffer::SetLength(uint32_t nLen)
{
   m_nLength = nLen;
}

void CByteBuffer::SetSize(uint32_t nSize)
{
   m_pData = (uint8_t *)realloc(m_pData, nSize);
   assert(m_pData);
   m_nAllocated = nSize;
}

void CByteBuffer::SetData(uint8_t *pData, uint32_t nSize)
{
   if(m_pData != nullptr)
   {
      free(m_pData);
   }
   Clear();

   m_pData = pData;
   assert(m_pData);

   m_nAllocated = nSize;
   m_nLength    = nSize;
}

void CByteBuffer::IncreaseSize(uint32_t nAdd)
{
   if(nAdd > m_nAllocated)
   {
      m_nAllocated += nAdd + 128;
   }
   else
   {
      m_nAllocated *= 2;
   }

   SetSize(m_nAllocated);
}

auto CByteBuffer::FileRead(FILE *f, uint32_t nLength) -> int
{
   Clear();

   if(m_nAllocated < nLength)
   {
      IncreaseSize(nLength - m_nAllocated + 1);
   }

   int n = fread(m_pData, 1, nLength, f);

   if(n > 0)
   {
      m_nLength = n;
   }

   return n;
}

auto CByteBuffer::FileWrite(FILE *f) -> int
{
   int n;

   if(m_nLength > 0)
   {
      return fwrite(m_pData, 1, m_nLength, f);
   }
   return 0;
}

auto CByteBuffer::FileRead(FILE *f, long nOffset, uint32_t nLength) -> int
{
   Clear();

   if(m_nAllocated < nLength)
   {
      IncreaseSize(nLength - m_nAllocated + 1);
   }

   if(fseek(f, nOffset, SEEK_SET) != 0)
   {
      return -1;
   }

   int n = fread(m_pData, 1, nLength, f);

   if(n > 0)
   {
      m_nLength = nLength;
   }

   return n;
}

auto CByteBuffer::Read(uint8_t *pBuf, uint32_t nLen) -> int
{
   if(m_nReadPos + nLen > m_nLength)
   {
      return 1;
   }

   memcpy(pBuf, m_pData + m_nReadPos, nLen);
   m_nReadPos += nLen;

   return 0;
}

auto CByteBuffer::Read8(uint8_t *pNum) -> int
{
   return Read(pNum, sizeof(uint8_t));
}

auto CByteBuffer::Read8(int8_t *pNum) -> int
{
   return Read((uint8_t *)pNum, sizeof(uint8_t));
}

auto CByteBuffer::Read16(uint16_t *pNum) -> int
{
   return Read((uint8_t *)pNum, sizeof(uint16_t));
}

auto CByteBuffer::Read16(int16_t *pNum) -> int
{
   return Read((uint8_t *)pNum, sizeof(int16_t));
}

auto CByteBuffer::Read32(uint32_t *pNum) -> int
{
   return Read((uint8_t *)pNum, sizeof(uint32_t));
}

auto CByteBuffer::Read32(int32_t *pNum) -> int
{
   return Read((uint8_t *)pNum, sizeof(int32_t));
}

auto CByteBuffer::ReadFloat(float *pFloat) -> int
{
   return Read((uint8_t *)pFloat, sizeof(float));
}

auto CByteBuffer::ReadStringCopy(char *pStr, uint32_t nMaxSize) -> int
{
   uint32_t nLen = 1 + strlen((char *)m_pData + m_nReadPos);

   if(nLen > nMaxSize)
   {
      pStr[0] = 0;
      return 1;
   }

   return Read((uint8_t *)pStr, nLen);
}

auto CByteBuffer::ReadStringAlloc(char **ppStr) -> int
{
   int nLen = 1 + strlen((char *)m_pData + m_nReadPos);

   if(nLen == 1)
   {
      *ppStr = nullptr;
      return Skip8();
   }

   *ppStr = (char *)malloc(nLen);

   assert(*ppStr);

   return Read((uint8_t *)*ppStr, nLen);
}

auto CByteBuffer::ReadNames(char ***pppStr) -> int
{
   char *c;
   *pppStr = create_namelist();
   assert(*pppStr);

   for(;;)
   {
      if(SkipString(&c) != 0)
      {
         return 1;
      }

      if(*c != 0)
      {
         *pppStr = add_name(c, *pppStr);
      }
      else
      {
         break;
      }
   }

   return 0;
}

auto CByteBuffer::ReadBlock(uint8_t **ppData, uint32_t *pnLen) -> int
{
   *ppData = nullptr;

   if(Read32(pnLen) != 0)
   {
      return 1;
   }

   *ppData = (uint8_t *)malloc(*pnLen);

   if(ppData == nullptr)
   {
      return 1;
   }

   return Read(*ppData, *pnLen);
}

auto CByteBuffer::Skip(int nLen) -> int
{
   if(m_nReadPos + nLen > m_nLength)
   {
      return 1;
   }

   m_nReadPos += nLen;

   return 0;
}

auto CByteBuffer::Skip8() -> int
{
   return Skip(sizeof(uint8_t));
}

auto CByteBuffer::Skip16() -> int
{
   return Skip(sizeof(uint16_t));
}

auto CByteBuffer::Skip32() -> int
{
   return Skip(sizeof(uint32_t));
}

auto CByteBuffer::SkipFloat() -> int
{
   return Skip(sizeof(float));
}

auto CByteBuffer::SkipString(char **ppStr) -> int
{
   if(ppStr != nullptr)
   {
      *ppStr = (char *)m_pData + m_nReadPos;
   }

   return Skip(1 + strlen((char *)m_pData + m_nReadPos));
}

auto CByteBuffer::SkipNames() -> int
{
   char *c;

   for(;;)
   {
      if(SkipString(&c) != 0)
      {
         return 1;
      }

      if(c[0] == 0)
      {
         break;
      }
   }

   return 0;
}

void CByteBuffer::Append(const uint8_t *pData, uint32_t nLen)
{
   if(nLen + m_nLength > m_nAllocated)
   {
      IncreaseSize(nLen);
   }

   memcpy(m_pData + m_nLength, pData, nLen);

   m_nLength += nLen;
}

void CByteBuffer::Append(CByteBuffer *pBuf)
{
   Append(pBuf->GetData(), pBuf->GetLength());
}

void CByteBuffer::Append8(uint8_t i)
{
   Append(&i, sizeof(i));
}

void CByteBuffer::Append16(uint16_t i)
{
   Append((uint8_t *)&i, sizeof(i));
}

void CByteBuffer::Append32(uint32_t i)
{
   Append((uint8_t *)&i, sizeof(i));
}

void CByteBuffer::AppendFloat(float f)
{
   Append((uint8_t *)&f, sizeof(f));
}

void CByteBuffer::AppendBlock(const uint8_t *pData, uint32_t nLen)
{
   Append32(nLen);
   Append(pData, nLen);
}

void CByteBuffer::AppendString(const char *pStr)
{
   if(pStr != nullptr)
   {
      Append((uint8_t *)pStr, strlen(pStr) + 1);
   }
   else
   {
      AppendString("");
   }
}

void CByteBuffer::AppendDoubleString(const char *pStr)
{
   if(pStr != nullptr)
   {
      AppendString(pStr);
      TAIL(pStr);
      AppendString(pStr + 1);
   }
   else
   {
      AppendString("");
      AppendString("");
   }
}

void CByteBuffer::AppendNames(const char **ppNames)
{
   if(ppNames != nullptr)
   {
      for(; (*ppNames != nullptr) && (**ppNames != 0); ppNames++)
      {
         AppendString(*ppNames);
      }
   }

   AppendString("");
}

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
