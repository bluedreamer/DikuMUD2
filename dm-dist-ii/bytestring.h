#pragma once
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



#include "essential.h"

class CByteBuffer
{
public:
   explicit CByteBuffer(uint32_t nSize = 1024);
   virtual ~CByteBuffer();

   // Informative functions
   [[nodiscard]] inline auto GetLength() const -> uint32_t  { return m_nLength; }
   [[nodiscard]] inline auto GetAllocated() const -> uint32_t  { return m_nAllocated; }
   [[nodiscard]] inline auto GetReadPosition() const -> uint32_t  { return m_nReadPos; }
   inline auto GetData() -> const uint8_t * { return m_pData; }

   void SetReadPosition(uint32_t nReadPosition);
   void SetLength(uint32_t nLen);
   void SetData(uint8_t *pData, uint32_t nLength);

   inline void Rewind() { m_nReadPos = 0; }
   inline void Clear()
   {
      m_nReadPos = 0;
      m_nLength  = 0;
   }

   // Public
   //
   auto FileRead(FILE *f, uint32_t nLength) -> int;
   auto FileRead(FILE *f, long offset, uint32_t length) -> int;
   auto FileWrite(FILE *f) -> int;

   // Public functions to read from a buffer
   //
   auto Read(uint8_t *pBuf, uint32_t nLen) -> int;

   auto ReadBlock(uint8_t **pData, uint32_t *nLen) -> int;

   auto Read8(uint8_t *pNum) -> int;
   auto Read16(uint16_t *pNum) -> int;
   auto Read32(uint32_t *pNum) -> int;
   auto Read8(int8_t *pNum) -> int;
   auto Read16(int16_t *pNum) -> int;
   auto Read32(int32_t *pNum) -> int;
   auto ReadFloat(float *pFloat) -> int;
   auto ReadStringAlloc(char **pStr) -> int;
   auto ReadStringCopy(char *pStr, uint32_t nSize) -> int;
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
   void Append(const uint8_t *pData, uint32_t nLen);

   void Append(CByteBuffer *pBuf);

   void AppendBlock(const uint8_t *pData, uint32_t nLen);

   void Append8(uint8_t i);
   void Append16(uint16_t i);
   void Append32(uint32_t i);
   void AppendFloat(float f);
   void AppendString(const char *pStr);
   void AppendDoubleString(const char *pStr);
   void AppendNames(const char *pNames[]);

protected:
   void SetSize(uint32_t nSize);
   void IncreaseSize(uint32_t nSize);

private:
   uint32_t m_nReadPos{0};
   uint32_t m_nLength{0};
   uint32_t m_nAllocated{0};
   uint8_t *m_pData{nullptr};
};

auto bread_uint8_t(uint8_t **buf) -> uint8_t;
auto bread_uint16_t(uint8_t **buf) -> uint16_t;
auto bread_uint32_t(uint8_t **buf) -> uint32_t;
auto bread_float(uint8_t **buf) -> float;
auto bread_data(uint8_t **b, uint32_t *len) -> uint8_t *;
void bwrite_data(uint8_t **b, uint8_t *data, uint32_t len);
void bread_strcpy(uint8_t **b, char *str);
auto bread_str_alloc(uint8_t **buf) -> char *;
auto bread_str_skip(uint8_t **b) -> char *;
auto bread_nameblock(uint8_t **b) -> char **;

void bwrite_uint8_t(uint8_t **b, uint8_t i);
void bwrite_uint16_t(uint8_t **b, uint16_t i);
void bwrite_uint32_t(uint8_t **b, uint32_t i);
void bwrite_float(uint8_t **b, float f);
void bwrite_string(uint8_t **b, const char *str);
void bwrite_double_string(uint8_t **b, char *str);
void bwrite_nameblock(uint8_t **b, char **nb);
