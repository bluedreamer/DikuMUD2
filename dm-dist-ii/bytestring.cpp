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

CByteBuffer::CByteBuffer(ubit32 nSize)
{
   m_nLength    = 0;
   m_nReadPos   = 0;
   m_nAllocated = nSize;
   m_pData      = (ubit8 *)calloc(nSize, 1);

   assert(m_pData != nullptr);
}

CByteBuffer::~CByteBuffer()
{
   free(m_pData);
}

void CByteBuffer::SetReadPosition(ubit32 nReadPosition)
{
   m_nReadPos = nReadPosition;
}

void CByteBuffer::SetLength(ubit32 nLen)
{
   m_nLength = nLen;
}

void CByteBuffer::SetSize(ubit32 nSize)
{
   m_pData = (ubit8 *)realloc(m_pData, nSize);
   assert(m_pData);
   m_nAllocated = nSize;
}

void CByteBuffer::SetData(ubit8 *pData, ubit32 nSize)
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

void CByteBuffer::IncreaseSize(ubit32 nAdd)
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

auto CByteBuffer::FileRead(FILE *f, ubit32 nLength) -> int
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

auto CByteBuffer::FileRead(FILE *f, long nOffset, ubit32 nLength) -> int
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

auto CByteBuffer::Read(ubit8 *pBuf, ubit32 nLen) -> int
{
   if(m_nReadPos + nLen > m_nLength)
   {
      return 1;
   }

   memcpy(pBuf, m_pData + m_nReadPos, nLen);
   m_nReadPos += nLen;

   return 0;
}

auto CByteBuffer::Read8(ubit8 *pNum) -> int
{
   return Read(pNum, sizeof(ubit8));
}

auto CByteBuffer::Read8(sbit8 *pNum) -> int
{
   return Read((ubit8 *)pNum, sizeof(ubit8));
}

auto CByteBuffer::Read16(ubit16 *pNum) -> int
{
   return Read((ubit8 *)pNum, sizeof(ubit16));
}

auto CByteBuffer::Read16(sbit16 *pNum) -> int
{
   return Read((ubit8 *)pNum, sizeof(sbit16));
}

auto CByteBuffer::Read32(ubit32 *pNum) -> int
{
   return Read((ubit8 *)pNum, sizeof(ubit32));
}

auto CByteBuffer::Read32(sbit32 *pNum) -> int
{
   return Read((ubit8 *)pNum, sizeof(sbit32));
}

auto CByteBuffer::ReadFloat(float *pFloat) -> int
{
   return Read((ubit8 *)pFloat, sizeof(float));
}

auto CByteBuffer::ReadStringCopy(char *pStr, ubit32 nMaxSize) -> int
{
   ubit32 nLen = 1 + strlen((char *)m_pData + m_nReadPos);

   if(nLen > nMaxSize)
   {
      pStr[0] = 0;
      return 1;
   }

   return Read((ubit8 *)pStr, nLen);
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

   return Read((ubit8 *)*ppStr, nLen);
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

auto CByteBuffer::ReadBlock(ubit8 **ppData, ubit32 *pnLen) -> int
{
   *ppData = nullptr;

   if(Read32(pnLen) != 0)
   {
      return 1;
   }

   *ppData = (ubit8 *)malloc(*pnLen);

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
   return Skip(sizeof(ubit8));
}

auto CByteBuffer::Skip16() -> int
{
   return Skip(sizeof(ubit16));
}

auto CByteBuffer::Skip32() -> int
{
   return Skip(sizeof(ubit32));
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

void CByteBuffer::Append(const ubit8 *pData, ubit32 nLen)
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

void CByteBuffer::Append8(ubit8 i)
{
   Append(&i, sizeof(i));
}

void CByteBuffer::Append16(ubit16 i)
{
   Append((ubit8 *)&i, sizeof(i));
}

void CByteBuffer::Append32(ubit32 i)
{
   Append((ubit8 *)&i, sizeof(i));
}

void CByteBuffer::AppendFloat(float f)
{
   Append((ubit8 *)&f, sizeof(f));
}

void CByteBuffer::AppendBlock(const ubit8 *pData, ubit32 nLen)
{
   Append32(nLen);
   Append(pData, nLen);
}

void CByteBuffer::AppendString(const char *pStr)
{
   if(pStr != nullptr)
   {
      Append((ubit8 *)pStr, strlen(pStr) + 1);
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

auto bread_ubit8(ubit8 **b) -> ubit8
{
   ubit8 i;

   memcpy((ubit8 *)&i, *b, sizeof(ubit8));
   *b += sizeof(ubit8);

   return i;
}

auto bread_ubit16(ubit8 **b) -> ubit16
{
   ubit16 i;

   memcpy((ubit8 *)&i, *b, sizeof(ubit16));
   *b += sizeof(ubit16);

   return i;
}

auto bread_ubit32(ubit8 **b) -> ubit32
{
   ubit32 i;

   memcpy((ubit8 *)&i, *b, sizeof(ubit32));
   *b += sizeof(ubit32);

   return i;
}

auto bread_float(ubit8 **b) -> float
{
   float f;

   memcpy((ubit8 *)&f, *b, sizeof(float));
   *b += sizeof(float);

   return f;
}

auto bread_data(ubit8 **b, ubit32 *plen) -> ubit8 *
{
   ubit32 len;
   ubit8 *data;

   data = nullptr;
   len  = bread_ubit32(b);
   if(plen != nullptr)
   {
      *plen = len;
   }

   if(len > 0)
   {
      CREATE(data, ubit8, len);
      memcpy(data, *b, len);
      *b += len;
   }

   return data;
}

/* Stored: as Null terminated string            */
/* Copy string from **b into *str               */
void bread_strcpy(ubit8 **b, char *str)
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
auto bread_str_alloc(ubit8 **b) -> char *
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
auto bread_str_skip(ubit8 **b) -> char *
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
auto bread_nameblock(ubit8 **b) -> char **
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

void bwrite_ubit8(ubit8 **b, ubit8 i)
{
   **b = i;
   *b += sizeof(ubit8);
}

void bwrite_ubit16(ubit8 **b, ubit16 i)
{
   memcpy(*b, (ubit8 *)&i, sizeof(ubit16));
   *b += sizeof(ubit16);
}

void bwrite_ubit32(ubit8 **b, ubit32 i)
{
   memcpy(*b, (ubit8 *)&i, sizeof(ubit32));
   *b += sizeof(ubit32);
}

void bwrite_float(ubit8 **b, float f)
{
   memcpy(*b, (ubit8 *)&f, sizeof(float));
   *b += sizeof(float);
}

void bwrite_data(ubit8 **b, ubit8 *data, ubit32 len)
{
   bwrite_ubit32(b, len);
   if(len > 0)
   {
      memcpy(*b, data, len);
      *b += len;
   }
}

/* String is stored as Null terminated string   */
/* Space is NOT allocated if string is 0 length */
/* but NIL is returned                          */
void bwrite_string(ubit8 **b, const char *str)
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
void bwrite_double_string(ubit8 **b, char *str)
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
void bwrite_nameblock(ubit8 **b, char **nb)
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
