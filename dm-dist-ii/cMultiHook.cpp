#include "cMultiHook.h"
#include "comm.h"
#include "config.h"
#include "protocol.h"
#include "select.h"
#include "system.h"
#include <cstring>

extern class cMultiMaster Multi;
extern descriptor_data   *descriptor_list;
auto                      descriptor_new(class cMultiHook *pe) -> descriptor_data *;

cMultiHook::cMultiHook()
{
   succ_err = 0;
}

void cMultiHook::Input(int nFlags)
{
   if((nFlags & SELECT_EXCEPT) != 0)
   {
      slog(LOG_ALL, 0, "Freaky multi!");
      Close();
   }
   if((nFlags & SELECT_READ) != 0)
   {
      int n;

      for(;;)
      {
         n = Read();
         if((n == 0) || (n == -1))
         {
            break;
         }
      }

      if(n == -1)
      {
         slog(LOG_OFF, 0, "ERROR READING FROM MPLEX.");
         Close();
      }
      else if(n == -2)
      {
         slog(LOG_OFF, 0, "MPLEX PROTOCOL ERROR.");
      }
   }
}

void cMultiHook::Close()
{
   if(IsHooked() == 0)
   {
      return;
   }

   slog(LOG_ALL, 0, "Closing connection to multi host.");

   Unhook();

   Multi.nCount--;
}

auto cMultiHook::Read() -> int
{
   descriptor_data *d = nullptr;
   int              p;
   int              n;
   uint16_t         id;
   uint16_t         len;
   char            *data;
   uint8_t          text_type;

   extern char *logo;

   p = protocol_parse_incoming(this, &id, &len, &data, &text_type);

   if(p <= 0)
   {
      return p;
   }

   if(id != 0)
   {
      for(d = descriptor_list; d != nullptr; d = d->next)
      {
         if(d->id == id)
         {
            assert(d->multi == this);
            break;
         }
      }

      if(d == nullptr)
      {
         /* This could perhaps occur if a connected player issues a command
            simultaneously with the server throwing the person out.
            I will simply test if d == NULL in all cases below and then
            ignore the message. */

         slog(LOG_ALL, 0, "No destination ID (%d).", id);

         if(succ_err++ > 5)
         {
            Close();
            slog(LOG_ALL, 0, "Lost track of multi stream.");
            if(data != nullptr)
            {
               free(data);
            }
            return -1;
         }
      }
      else
      {
         succ_err = 0;
      }
   }

   switch(p)
   {
      case MULTI_TERMINATE_CHAR:
         /* This is very nice, but it prevents descriptor_close to send
            a connection_close to the mplex'er */
         if(d != nullptr)
         {
            descriptor_close(d, FALSE);
         }
         if(data != nullptr)
         {
            free(data);
         }
         break;

      case MULTI_CONNECT_REQ_CHAR:
         d = descriptor_new(this);
         protocol_send_confirm(this, d->id);
         send_to_descriptor(g_cServerConfig.m_pLogo, d);
         send_to_descriptor("By what name do they call you? ", d);
         break;

      case MULTI_HOST_CHAR:
         if((d != nullptr) && (data != nullptr))
         {
            auto *b = (uint8_t *)data;

            d->nPort = bread_uint16_t(&b);
            d->nLine = bread_uint8_t(&b);
            strncpy(d->host, (char *)b, sizeof(d->host));
            d->host[sizeof(d->host) - 1] = 0;
         }
         if(data != nullptr)
         {
            free(data);
         }
         break;

      case MULTI_TEXT_CHAR:
         if(d != nullptr)
         {
            d->qInput.Append(new cQueueElem(data, FALSE));
         }
         /* Kept in queue */
         break;

      default:
         slog(LOG_ALL, 0, "Illegal unexpected unique multi character.");
         Close();
         if(data != nullptr)
         {
            free(data);
         }
         return -1;
   }

   return p;
}
