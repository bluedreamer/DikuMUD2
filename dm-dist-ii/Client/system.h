/* *********************************************************************** *
 * File   : system.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "channel.h"

#define DIR_FILE ".filelist"

/*
   There is always exactly one system class at a fixed channel.

   To get the contents of the last status information update, call:

      Status();

   To update the Status(), call:

      RemoteOpen(SYS_STATUS);

   Status() will then quickly be updated with the response.

*/

class cSystem : public cChannel
{
public:
   cSystem(uint8_t nChn);
   ~cSystem(void);

   void        Receive(uint8_t *data, uint32_t len);
   const char *Status(void);

   void SetPriority(void);
   void RemoteOpen(int nServiceType, uint8_t nChn = 0, char *pStr = NULL);
   void RemoteClose(uint8_t nChn);

   uint32_t PriorityDelay(void) { return nDelay; }
   uint16_t PriorityQueue(void) { return nQueue; }
   uint16_t PrioritySize(void) { return nSize; }

private:
   void SystemGatherInfo(uint8_t *data, int len);
   void SystemNormal(uint8_t *data, int len);
   void SendDir(void);
   void CD(char *path);

   char     StatusBuf[1024];
   uint32_t nState;
   uint8_t  nPriorityQueue;
   uint8_t  nPriorityDelay;
   uint8_t  nPrioritySize;

   uint16_t nQueue;
   uint32_t nSize;
   uint32_t nDelay;
};

extern class cSystem *System;

#define SYS_STATUS    (3)
#define SYS_INFO      (4)
#define SYS_UPLOAD    (5)
#define SYS_DOWNLOAD  (6)
#define SYS_DIRECTORY (7)
#define SYS_CD        (8)
#define SYS_FILEINFO  (9)
#define SYS_PRIORITY  (10)
#define SYS_LOGINFO   (11)
#define SYS_RTT       (12)
#define SYS_LONGINFO  (14)

#endif
