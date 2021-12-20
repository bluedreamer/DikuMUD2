#include "cCaptainHook.h"

#include <cerrno>
#include <csignal>
#include <cstring>

cCaptainHook CaptainHook;
#define SELECT_READ   0x01
#define SELECT_WRITE  0x02
#define SELECT_EXCEPT 0x04

cCaptainHook::cCaptainHook()
{
   signal(SIGPIPE, SIG_IGN); // Or else pipe fucks the whole thing up...

   for(auto &i : pfHook)
   {
      i = nullptr;
   }

   nTop = 0;
   nMax = 0;
}

cCaptainHook::~cCaptainHook()
{
   Close();
}

void cCaptainHook::Close()
{
   for(auto &i : pfHook)
   {
      if(i != nullptr)
      {
         i->Unhook();
      }
   }

   nTop = 0;
   nMax = 0;
}

void cCaptainHook::Hook(int nHandle, cHook *hook)
{
   static int newid = 0;

   assert(pfHook[nHandle] == nullptr);
   assert(hook->fd == -1);

   pfHook[nHandle] = hook;
   hook->fd        = nHandle;
   hook->id        = newid++;

   hook->qTX.Flush();
   hook->qRX.Flush();

   nIdx[nTop] = nHandle;

   nTop++;

   if(nHandle > nMax)
   {
      nMax = nHandle;
   }

   // DEBUG("HOOKED: Fd %d, Flags %d, Max %d\n", nHandle, nFlag, nMax);
}

void cCaptainHook::Unhook(cHook *hook)
{
   int i;
   int nHandle = hook->fd;

   assert(pfHook[nHandle] == hook);

   i = close(nHandle);
   if(i < 0)
   {
      slog(LOG_ALL, 0, "Captain Hook: Close error %d.", errno);
   }

   for(i = 0; i < nTop; i++)
   {
      if(nIdx[i] == nHandle)
      {
         nIdx[i] = nIdx[nTop - 1];
         nTop--;
         break;
      }
   }

   pfHook[nHandle]->id = -1;
   pfHook[nHandle]     = nullptr;

   nMax = 0;
   for(i = 0; i < nTop; i++)
   {
      if(nIdx[i] > nMax)
      {
         nMax = nIdx[i];
      }
   }
}

// If an unhook is performed during the wait, the possible select may be
// postponed until the next call to wait().
//
auto cCaptainHook::Wait(struct timeval *timeout) -> int
{
   int n;
   int lmax;

   /* The following two are used in Wait() because the Input & Write
      can cause any descriptor to become unhooked. It is then the job
      of the Unhook to update this table of descriptors accordingly */

   int nTable[256];
   int nId[256];
   int nTableTop;

   memcpy(nTable, nIdx, sizeof(int) * nTop);
   nTableTop = nTop;

   FD_ZERO(&read_set);
   FD_ZERO(&write_set);

   for(int i = 0; i < nTableTop; i++)
   {
      nId[i] = pfHook[nTable[i]]->id;

      assert(nId[i] != -1);

      FD_SET(nTable[i], &read_set);

      if(pfHook[nTable[i]]->qTX.IsEmpty() == 0)
      {
         FD_SET(nTable[i], &write_set);
      }
   }

   n = select(nMax + 1, &read_set, &write_set, nullptr, timeout);

   if(n == -1)
   {
      // Do not set to zero, it means that a timeout occurred.
      //
      if(errno == EAGAIN)
      {
         n = 1;
      }
      else if(errno == EINTR)
      {
         // slog(LOG_ALL, 0, "CaptainHook: Select Interrupted.\n");
         n = 1;
      }
      else
      {
         slog(LOG_ALL, 0, "CaptainHook: Select error %d.\n", errno);
      }
   }
   else if(n > 0)
   {
      int nFlag;
      int tmpfd;
      int i;

      /* We need to do this the hard way, because nTable[] can be
         changed radically by any sequence of read or write */

      for(i = 0; i < nTableTop; i++)
      {
         nFlag = 0;
         tmpfd = nTable[i];

         if(FD_ISSET(tmpfd, &read_set))
         {
            SET_BIT(nFlag, SELECT_READ);
         }

         if(FD_ISSET(tmpfd, &write_set))
         {
            SET_BIT(nFlag, SELECT_WRITE);
         }

         cHook *pfTmpHook = pfHook[tmpfd];

         /* It could have been unhooked by any previous sequence of
            Input() or Write() sequences */

         if((nFlag != 0) && (pfTmpHook != nullptr))
         {
            int nTmpid = pfTmpHook->id;

            if((pfTmpHook == pfHook[tmpfd]) && (pfTmpHook->id == nId[i]))
            {
               if((nFlag & (SELECT_READ | SELECT_EXCEPT)) != 0)
               {
                  pfTmpHook->Input(nFlag);
               }
            }

            if((pfTmpHook == pfHook[tmpfd]) && (pfTmpHook->id == nId[i]))
            {
               if((nFlag & SELECT_WRITE) != 0)
               {
                  pfTmpHook->PushWrite();
               }
            }
         }
      }
   }

   return n;
}
