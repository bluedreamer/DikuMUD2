#pragma once

#include "cHook.h"

class cMultiHook : public cHook
{
public:
   cMultiHook();
   void Input(int nFlags) override;

   void Close();
   auto Read() -> int;

   int  succ_err; /* Number of successive errors */
};
