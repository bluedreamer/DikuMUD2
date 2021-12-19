#include "descriptor_data.h"
#include "account.h"
#include "comm.h"
#include "db.h"
#include "nanny.h"
#include "queue.h"
#include "utils.h"

extern int              no_connections;
extern int              max_no_connections;
extern descriptor_data *descriptor_list;

descriptor_data::descriptor_data(cMultiHook *pe)
{
   static int nid = 0;

   no_connections++;
   if(no_connections > max_no_connections)
   {
      max_no_connections = no_connections;
   }

   /* init desc data */
   multi = pe;
   id    = ++nid;

   state       = 0;
   fptr        = reinterpret_cast<void (*)(descriptor_data *, const char *)>(nanny_get_name);
   wait        = 1;
   timer       = 0;
   prompt_mode = PROMPT_SENT;
   *last_cmd = *history = '\0';

   host[0] = 0;
   nPort   = 0;
   nLine   = 255;

   localstr = nullptr;
   postedit = nullptr;
   editing  = nullptr;
   editref  = nullptr;

   original       = nullptr;
   snoop.snooping = nullptr;
   snoop.snoop_by = nullptr;
   replyid        = (uint32_t)-1;

   /* Make a new PC struct */
   character = new(unit_data)(UNIT_ST_PC);
   init_char(character);
   CHAR_DESCRIPTOR(character) = this;

   /* prepend to list */
   next            = descriptor_list;
   descriptor_list = this;
}

void descriptor_data::RemoveBBS() const
{
   if(nLine != 255)
   {
      char buf[512];

      sprintf(buf, BBS_DIR "%d.%d", nPort, nLine);
      remove(buf);
   }
}

void descriptor_data::CreateBBS() const
{
   if(nLine != 255)
   {
      char  buf[512];
      FILE *f;

      sprintf(buf, BBS_DIR "%d.%d", nPort, nLine);

      if(character == nullptr)
      {
         slog(LOG_ALL, 0, "No character in %s.", buf);
         return;
      }

      f = fopen(buf, "wb");

      if(f == nullptr)
      {
         slog(LOG_ALL, 0, "Could not create %s.", buf);
         return;
      }

      if(account_is_overdue(this->character) != 0)
      {
         fprintf(f, "1\n");
      }
      else
      {
         fprintf(f, "0\n");
      }

      fprintf(f, "%d\n", PC_ACCOUNT(this->character).total_credit);
      fprintf(f, "%s\n", PC_FILENAME(this->character));
      fprintf(f, "%d\n", CHAR_LEVEL(this->character));
      fprintf(f, "%d\n", PC_ACCESS_LEVEL(this->character));

      fclose(f);
   }
}

descriptor_data::~descriptor_data()
{
   RemoveBBS();
   nLine = 255;
   no_connections--;
}
