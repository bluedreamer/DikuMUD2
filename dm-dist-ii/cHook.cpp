#include "cHook.h"

#include "cCaptainHook.h"

#include <algorithm>
#include <cerrno>
#include <unistd.h>
/* ------------------------------------------------------------------- */
/*                                HOOK                                 */
/* ------------------------------------------------------------------- */

cHook::cHook()
{
   fd = -1;
}

cHook::~cHook()
{
   Unhook();
}

auto cHook::tfd() const -> int
{
   return fd;
}

auto cHook::IsHooked() const -> int
{
   return static_cast<int>(fd != -1);
}

void cHook::Unhook()
{
   if(IsHooked() != 0)
   {
      CaptainHook.Unhook(this);
   }

   fd = -1;
}

void cHook::PushWrite()
{
   int     sofar;
   int     len;
   int     thisround;
   uint8_t buf[1460];

   if(IsHooked() == 0)
   {
      return;
   }

   while(qTX.IsEmpty() == 0)
   {
      len = std::min(sizeof(buf), static_cast<unsigned long>(qTX.Bytes()));

      qTX.CutCopy(buf, len);

      sofar = 0;

      for(;;)
      {
         thisround = write(fd, buf + sofar, len - sofar);

         if(thisround == 0)
         {
            slog(LOG_ALL, 0, "PushWrite (%d): Write to socket EOF", fd);
            Unhook();
            return;
         }
         if(thisround < 0)
         {
            if(errno == EWOULDBLOCK) /* I'd rather this didn't happen */
            {
               qTX.Prepend(new cQueueElem(buf + sofar, len - sofar));
               return;
            }

            slog(LOG_ALL, 0, "PushWrite (%d): Write to socket, error %d", fd, errno);
            Unhook();
            return;
         }

         if(thisround < len - sofar)
         {
            sofar += thisround;
            qTX.Prepend(new cQueueElem(buf + sofar, len - sofar));
            return;
         }

         sofar += thisround;

         if(sofar >= len)
         {
            break;
         }
      }
   }
}

void cHook::Write(uint8_t *pData, uint32_t nLen, int bCopy)
{
   if(nLen <= 0)
   {
      return;
   }

   if(IsHooked() == 0)
   {
      if(bCopy == 0)
      {
         free(pData);
      }
      return;
   }

   qTX.Append(new cQueueElem(pData, nLen, bCopy));

   PushWrite();
}
