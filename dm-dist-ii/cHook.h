#pragma once
#include "essential.h"
#include "queue.h"
#include <cstdint>

class cCaptainHook;
class cQueue;

class cHook
{
   friend cCaptainHook;

public:
   cHook();
   virtual ~cHook();

   [[nodiscard]] auto tfd() const -> int;
   [[nodiscard]] auto IsHooked() const -> int;
   void               Write(uint8_t *pData, uint32_t nLen, int bCopy = static_cast<int>(TRUE));

   cQueue qRX;

   void Unhook();

protected:
   void PushWrite();

   virtual void Input(int nFlags) = 0;

   cQueue qTX;

private:
   int fd;
   int id;
};
