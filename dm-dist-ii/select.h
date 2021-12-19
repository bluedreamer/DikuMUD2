/* *********************************************************************** *
 * File   : select.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: headers                                                        *
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

#ifndef SELECT_H
#define SELECT_H

#include "essential.h"
#include "queue.h"

extern int bPipeSignal;

auto PipeRead(int fd, char *buf, size_t count) -> int;
auto PipeWrite(int fd, char *buf, size_t count) -> int;

#define SELECT_READ   0x01
#define SELECT_WRITE  0x02
#define SELECT_EXCEPT 0x04

class cHook;


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

#endif
