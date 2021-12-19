/* *********************************************************************** *
 * File   : channel.h                                 Part of Valhalla MUD *
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

#ifndef CHANNELS_H
#define CHANNELS_H

#include "essential.h"
#include "slide.h"

#include <assert.h>

extern cSlw *Slw;

// Reserved channels.
//
#define SERVICE_SYSTEM   1
#define SERVICE_TEXT     2
#define SERVICE_UPLOAD   3
#define SERVICE_DOWNLOAD 4

#define CHN_MAX_DATA (SLW_MAX_DATA - 1)

// This class defines the basic elements that are in a single Channel.
// Namely a send, a receive, and the number of the channel.
//
class cChannel
{
public:
   cChannel(uint8_t n);
   virtual ~cChannel();

   uint8_t             ChannelNo(void) { return nChannel; }
   int32_t             Send(uint8_t *data, uint32_t len);
   virtual void        Receive(uint8_t *data, uint32_t len) = NULL;
   virtual void        Thread(void);
   virtual const char *Status(void);

private:
   uint8_t nChannel;
};

// This is the class that maintains the channels. It consists of 256
// base channels (above).
//
class cChannelManager
{
   friend class cChannel;

public:
   cChannelManager(uint8_t bArg);
   cChannel *Set(uint8_t nService, uint8_t nChn, char *pData = NULL);

   cChannel *Alloc(uint8_t nService, char *pData = NULL);

   void      Close(void);
   void      Free(uint8_t nChannel);

   int32_t   MngrTX(uint8_t *data, uint32_t len, uint8_t nChn);
   void      MngrRX(uint8_t *data, uint32_t len);
   void      MngrRX(uint8_t *data, uint32_t len, uint8_t nChannel); // For direct connect
   void      WriteQueue(void);

   void      Threads(void);

   cChannel *ChannelPtr(uint8_t nChn);

private:
   uint8_t         bServer; // TRUE if server (host), FALSE if client (caller)
   class cChannel *BaseChannel[256];
   cQueue          qTx;
};

extern cChannelManager ChannelManager;

#endif
