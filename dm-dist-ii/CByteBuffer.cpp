#include "CByteBuffer.h"

#include "essential.h"
#include "textutil.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

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
