/* *********************************************************************** *
 * File   : bytestring.h                              Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Prototypes for reading/writing from/to buffers.                *
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

#ifndef _MUD_BYTESTRING_H
#define _MUD_BYTESTRING_H

#include "essential.h"

class CByteBuffer
{
public:
   CByteBuffer(ubit32 nSize = 1024);
   virtual ~CByteBuffer();

   // Informative functions

   inline auto GetLength() -> ubit32 const { return m_nLength; }
   inline auto GetAllocated() -> ubit32 const { return m_nAllocated; }
   inline auto GetReadPosition() -> ubit32 const { return m_nReadPos; }
   inline auto GetData() -> const ubit8 * { return m_pData; }

   void SetReadPosition(ubit32 nReadPosition);
   void SetLength(ubit32 nLen);
   void SetData(ubit8 *pData, ubit32 nLength);

   inline void Rewind() { m_nReadPos = 0; }
   inline void Clear()
   {
      m_nReadPos = 0;
      m_nLength  = 0;
   }

   // Public
   //
   auto FileRead(FILE *f, ubit32 nLength) -> int;
   auto FileRead(FILE *f, long offset, ubit32 length) -> int;
   auto FileWrite(FILE *f) -> int;

   // Public functions to read from a buffer
   //
   auto Read(ubit8 *pBuf, ubit32 nLen) -> int;

   auto ReadBlock(ubit8 **pData, ubit32 *nLen) -> int;

   auto Read8(ubit8 *pNum) -> int;
   auto Read16(ubit16 *pNum) -> int;
   auto Read32(ubit32 *pNum) -> int;
   auto Read8(sbit8 *pNum) -> int;
   auto Read16(sbit16 *pNum) -> int;
   auto Read32(sbit32 *pNum) -> int;
   auto ReadFloat(float *pFloat) -> int;
   auto ReadStringAlloc(char **pStr) -> int;
   auto ReadStringCopy(char *pStr, ubit32 nSize) -> int;
   auto ReadNames(char ***pppNames) -> int;

   auto Skip(int n) -> int;
   auto Skip8() -> int;
   auto Skip16() -> int;
   auto Skip32() -> int;
   auto SkipFloat() -> int;
   auto SkipString(char **ppStr = nullptr) -> int;
   auto SkipNames() -> int;

   // Public functions to write to a buffer
   //
   void Append(const ubit8 *pData, ubit32 nLen);

   void Append(CByteBuffer *pBuf);

   void AppendBlock(const ubit8 *pData, ubit32 nLen);

   void Append8(ubit8 i);
   void Append16(ubit16 i);
   void Append32(ubit32 i);
   void AppendFloat(float f);
   void AppendString(const char *pStr);
   void AppendDoubleString(const char *pStr);
   void AppendNames(const char *pNames[]);

protected:
   void SetSize(ubit32 nSize);
   void IncreaseSize(ubit32 nSize);

private:
   ubit32 m_nReadPos;
   ubit32 m_nLength;
   ubit32 m_nAllocated;
   ubit8 *m_pData;
};

auto bread_ubit8(ubit8 **buf) -> ubit8;
auto bread_ubit16(ubit8 **buf) -> ubit16;
auto bread_ubit32(ubit8 **buf) -> ubit32;
auto bread_float(ubit8 **buf) -> float;
auto bread_data(ubit8 **b, ubit32 *len) -> ubit8 *;
void bwrite_data(ubit8 **b, ubit8 *data, ubit32 len);
void bread_strcpy(ubit8 **b, char *str);
auto bread_str_alloc(ubit8 **buf) -> char *;
auto bread_str_skip(ubit8 **b) -> char *;
auto bread_nameblock(ubit8 **b) -> char **;

void bwrite_ubit8(ubit8 **b, ubit8 i);
void bwrite_ubit16(ubit8 **b, ubit16 i);
void bwrite_ubit32(ubit8 **b, ubit32 i);
void bwrite_float(ubit8 **b, float f);
void bwrite_string(ubit8 **b, const char *str);
void bwrite_double_string(ubit8 **b, char *str);
void bwrite_nameblock(ubit8 **b, char **nb);

#endif /* _MUD_BYTESTRING_H */
