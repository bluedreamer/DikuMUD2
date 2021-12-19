#pragma once

#include "cHook.h"

class cCaptainHook
{
   friend cHook;

public:
   cCaptainHook();
   ~cCaptainHook();

   void Close();
   void Hook(int nHandle, cHook *hook);
   auto Wait(struct timeval *timeout) -> int;

private:
   void Unhook(cHook *hook);

   fd_set read_set, write_set;

   cHook *pfHook[256];

   int nIdx[256];
   int nMax;
   int nTop;
};

extern cCaptainHook CaptainHook;
