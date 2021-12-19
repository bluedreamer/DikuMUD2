#include "system.h"

#include "account.h"
#include "ban.h"
#include "cCaptainHook.h"
#include "cMultiMaster.h"
#include "comm.h"
#include "common.h"
#include "CServerConfiguration.h"
#include "db.h"
#include "db_file.h"
#include "files.h"
#include "handler.h"
#include "main.h"
#include "pcsave.h"
#include "protocol.h"
#include "str_parse.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <arpa/inet.h>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

/* extern vars */

class cMotherHook : public cHook
{
public:
   void Input(int nFlags) override;
   void Close();
};

class cMotherHook MotherHook;

void              MplexSendSetup(descriptor_data *d)
{
   assert(d);

   protocol_send_setup(d->multi, d->id, &PC_SETUP(d->character));
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                      Descriptor Routines                          */
/*                                                                   */
/* ----------------------------------------------------------------- */

/* Call only once when creating a new char (guest)    */
void init_char(unit_data *ch)
{
   int            i;
   int            init_skills = 0;

   extern int32_t player_id;

   if(g_cServerConfig.m_bBBS != 0)
   {
      PC_SETUP_ECHO(ch)      = static_cast<uint8_t>(TRUE);
      PC_SETUP_REDRAW(ch)    = static_cast<uint8_t>(TRUE);
      PC_SETUP_EMULATION(ch) = TERM_ANSI;
      PC_SETUP_TELNET(ch)    = static_cast<uint8_t>(FALSE);

      SET_BIT(PC_FLAGS(ch), PC_INFORM);
   }
   else
   {
      PC_SETUP_ECHO(ch)      = static_cast<uint8_t>(FALSE);
      PC_SETUP_REDRAW(ch)    = static_cast<uint8_t>(FALSE);
      PC_SETUP_EMULATION(ch) = TERM_TTY;
      PC_SETUP_TELNET(ch)    = static_cast<uint8_t>(TRUE);
   }

   PC_SETUP_WIDTH(ch)    = 80;
   PC_SETUP_HEIGHT(ch)   = 15;
   PC_SETUP_COLOUR(ch)   = 0;
   PC_CRACK_ATTEMPTS(ch) = 0;

   account_defaults(ch);

   PC_INFO(ch)          = nullptr;
   PC_QUEST(ch)         = nullptr;
   UNIT_EXTRA_DESCR(ch) = nullptr;
   PC_HOME(ch)          = nullptr;
   PC_GUILD(ch)         = nullptr;

   CHAR_POS(ch)         = POSITION_STANDING;
   CHAR_SPEED(ch)       = SPEED_DEFAULT;
   CHAR_RACE(ch)        = RACE_HUMAN;
   CHAR_SEX(ch)         = SEX_MALE;

   PC_TIME(ch).connect = PC_TIME(ch).birth = PC_TIME(ch).creation = time(nullptr);
   PC_TIME(ch).played                                             = 0;
   PC_LIFESPAN(ch)                                                = 100;

   CHAR_EXP(ch)                                                   = 0;
   CHAR_LEVEL(ch)                                                 = 0;
   PC_ID(ch)                                                      = -1;
   PC_CRIMES(ch)                                                  = 0;

   PC_ABILITY_POINTS(ch)                                          = 0;
   PC_SKILL_POINTS(ch)                                            = 0;

   /* *** if this is our first player --- he be God *** */
   if(player_id == -7)
   {
      slog(LOG_ALL, 0, "The very first GOD is created!");
      CHAR_EXP(ch)   = required_xp(255);
      CHAR_LEVEL(ch) = 255;

      init_skills    = 200;

      CHAR_RACE(ch)  = RACE_HUMAN;
      CHAR_SEX(ch)   = SEX_MALE; /* God is male ;-) */

      player_id      = 1;

      PC_ID(ch)      = new_player_id();
   }

   for(i = 0; i < SPL_TREE_MAX; i++)
   {
      PC_SPL_SKILL(ch, i) = init_skills;
      PC_SPL_LVL(ch, i)   = 0;
      PC_SPL_COST(ch, i)  = 0;
   }
   for(i = 0; i < WPN_TREE_MAX; i++)
   {
      PC_WPN_SKILL(ch, i) = init_skills;
      PC_WPN_LVL(ch, i)   = 0;
      PC_WPN_COST(ch, i)  = 0;
   }
   for(i = 0; i < SKI_TREE_MAX; i++)
   {
      PC_SKI_SKILL(ch, i) = init_skills;
      PC_SKI_LVL(ch, i)   = 0;
      PC_SKI_COST(ch, i)  = 0;
   }
   for(i = 0; i < ABIL_TREE_MAX; i++)
   {
      CHAR_ABILITY(ch, i) = init_skills;
      PC_ABI_LVL(ch, i)   = 0;
      PC_ABI_COST(ch, i)  = 0;
   }

   /* CHAR_HAND_QUALITY(ch) = 15;  15% */
   CHAR_ATTACK_TYPE(ch)    = WPN_FIST;
   CHAR_NATURAL_ARMOUR(ch) = ARM_CLOTHES;

   /* UNIT_TGH(ch) = 0; */
   UNIT_HIT(ch) = UNIT_MAX_HIT(ch) = 1;

   CHAR_MANA(ch)                   = mana_limit(ch);
   CHAR_ENDURANCE(ch)              = move_limit(ch);
   CHAR_LAST_ROOM(ch)              = nullptr;

   CHAR_FLAGS(ch)                  = 0;
   SET_BIT(CHAR_FLAGS(ch), CHAR_PROTECTED);

   for(i = 0; i < 3; i++)
   {
      PC_COND(ch, i) = (CHAR_LEVEL(ch) >= 200 ? 48 : 24);
   }

   PC_COND(ch, DRUNK) = 0;

   set_title(ch);
}

int  no_connections     = 0; /* No of used descriptors                    */
int  max_no_connections = 0; /* Statistics                                */

/* Pass the multi-fd which is to be associated with this new descriptor */
/* Note that id zero signifies that mplex descriptor has no mplex'er    */
auto descriptor_new(class cMultiHook *pe) -> descriptor_data *
{
   return new descriptor_data(pe);
}

/* Flush should be set to true, when there is noone to receive the  */
/* data, i.e. when for example the mplex'er is down or if the       */
/* mplex'er has sent a terminate request to the server. Nothing     */
/* bad happens if you mess up, but the mplex'er receives data to    */
/* an unknown id...                                                 */
void descriptor_close(descriptor_data *d, int bSendClose)
{
   descriptor_data  *tmp;
   class cMultiHook *multi = nullptr;

   void              unsnoop(unit_data * ch, int mode);
   void              unswitchbody(unit_data * npc);

   assert(d->character);

   /* Descriptor must be either in the game (UNIT_IN) or in menu.  */
   /* If unit has been extracted, then all his data is half erased */
   /* (affects, etc) and he shall not be saved!                    */
   if(UNIT_IN(d->character) == nullptr) /* In menu - extract completely */
   {
      assert(!UNIT_CONTAINS(d->character));
      assert(!UNIT_IN(d->character));
      assert(!d->character->gnext);
      assert(!d->character->gprevious);

      /* Important that we set to NULL before calling extract,
         otherwise we just go to the menu... ... ... */
      CHAR_DESCRIPTOR(d->character) = nullptr;
      extract_unit(d->character);
      d->character = nullptr;
      /* Too much log slog(LOG_ALL, "Losing descriptor from menu."); */
   }
   else
   {
      if(d->localstr != nullptr)
      {
         free(d->localstr);
      }

      d->localstr = nullptr;
      d->postedit = nullptr;
      d->editing  = nullptr;
      d->editref  = nullptr;

      if(CHAR_IS_SNOOPING(d->character) || CHAR_IS_SNOOPED(d->character))
      {
         unsnoop(d->character, 1);
      }

      if(CHAR_IS_SWITCHED(d->character))
      {
         unswitchbody(d->character);
      }

      assert(!d->snoop.snooping && !d->snoop.snoop_by);
      assert(!d->original);

      act("$1n has lost $1s link.", A_HIDEINV, d->character, nullptr, nullptr, TO_ROOM);
      slog(LOG_BRIEF, UNIT_MINV(d->character), "Closing link and making link dead: %s.", UNIT_NAME(d->character));

      if(is_destructed(DR_UNIT, d->character) == 0)
      {
         void disconnect_game(unit_data * pc);

         disconnect_game(d->character);

         if(!PC_IS_UNSAVED(d->character))
         {
            /* We need to save player to update his time status! */
            save_player(d->character); /* Save non-guests */
            save_player_contents(d->character, static_cast<int>(TRUE));

            create_fptr(d->character, SFUN_LINK_DEAD, 0, SFB_CMD, nullptr);
         }
         else
         {
            extract_unit(d->character); /* We extract guests */
         }
      }

      /* Important we set tp null AFTER calling save - otherwise
         time played does not get updated. */
      CHAR_DESCRIPTOR(d->character) = nullptr;
      d->character                  = nullptr;
   }

   if((bSendClose != 0) && (d->multi->IsHooked() != 0))
   {
      protocol_send_close(d->multi, d->id);
   }

   if(next_to_process == d)
   { /* to avoid crashing the process loop */
      next_to_process = next_to_process->next;
   }

   if(d == descriptor_list)
   { /* this is the head of the list */
      descriptor_list = descriptor_list->next;
   }
   else /* This is somewhere inside the list */
   {
      /* Locate the previous element */
      for(tmp = descriptor_list; (tmp != nullptr) && (tmp->next != d); tmp = tmp->next)
      {
         ;
      }
      tmp->next = d->next;
   }

   delete d;
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                        Multi-Protocol-Procedures                  */
/*                                                                   */
/* ----------------------------------------------------------------- */

void multi_clear()
{
   descriptor_data *nextd;
   descriptor_data *d;

   for(d = descriptor_list; d != nullptr; d = nextd)
   {
      nextd = d->next;
      if(d->multi->IsHooked() == 0)
      {
         descriptor_close(d);
      }
   }
}

void multi_close_all()
{
   int i;

   slog(LOG_BRIEF, 0, "Closing all multi connections.");

   for(i = 0; i < MAX_MULTI; i++)
   {
      Multi.Multi[i].Close();
   }

   multi_clear();
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                         MotherHook                                */
/*                                                                   */
/* ----------------------------------------------------------------- */
#define SELECT_READ   0x01
#define SELECT_WRITE  0x02
#define SELECT_EXCEPT 0x04

void cMotherHook::Input(int nFlags)
{
   if((nFlags & SELECT_EXCEPT) != 0)
   {
      slog(LOG_ALL, 0, "Mother connection closed down.");
      Close();
   }

   if((nFlags & SELECT_READ) != 0)
   {
      struct sockaddr_in isa;
      int                i;
      int                t;
      socklen_t          len; // MS2020

      len = sizeof(isa);

      if((t = accept(this->tfd(), (struct sockaddr *)&isa, &len)) < 0)
      {
         slog(LOG_ALL, 0, "Mother accept error %d", errno);
         return;
      }

      if(g_cServerConfig.ValidMplex(&isa) == 0)
      {
         slog(LOG_ALL, 0, "Mplex not from trusted host, terminating.");
         close(t);
         return;
      }

      i = fcntl(t, F_SETFL, FNDELAY);

      if(i == -1)
      {
         error(HERE, "Noblock");
      }

      int n;
      n = setsockopt(t, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
      if(n == -1)
      {
         error(HERE, "No Setsockopt()");
         exit(1);
      }

      for(i = 0; i < MAX_MULTI; i++)
      {
         if(Multi.Multi[i].IsHooked() == 0)
         {
            break;
         }
      }

      if((i >= MAX_MULTI) || (Multi.Multi[i].IsHooked() != 0))
      {
         slog(LOG_ALL, 0, "No more multi connections allowed");
         close(t);
         return;
      }

      CaptainHook.Hook(t, &Multi.Multi[i]);

      Multi.nCount++;

      slog(LOG_ALL, 0, "A multi-host has connected to the game.");
   }
}

void cMotherHook::Close()
{
   multi_close_all();

   Unhook();
}

void init_mother(int nPort)
{
   int                n;
   int                fdMother;
   struct linger      ld;
   struct sockaddr_in server_addr;

   memset(&server_addr, 0, sizeof(struct sockaddr_in));
   server_addr.sin_family      = AF_INET;
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   server_addr.sin_port        = htons(nPort);

   fdMother                    = socket(AF_INET, SOCK_STREAM, 0);

   if(fdMother == -1)
   {
      slog(LOG_OFF, 0, "Can't open Mother Connection");
      exit(0);
   }

   n = 1;
   if(setsockopt(fdMother, SOL_SOCKET, SO_REUSEADDR, (char *)&n, sizeof(n)) < 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "OpenMother setsockopt REUSEADDR");
      exit(0);
   }

   ld.l_onoff  = 0;
   ld.l_linger = 1000;

   if(setsockopt(fdMother, SOL_SOCKET, SO_LINGER, (char *)&ld, sizeof(ld)) < 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "OpenMother setsockopt LINGER");
      exit(0);
   }

   n = bind(fdMother, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));

   if(n != 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Can't bind Mother Connection port %d (errno %d).", nPort, errno);
      exit(0);
   }

   n = listen(fdMother, 5);

   if(n != 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Can't listen on Mother Connection.");
      exit(0);
   }

   n = fcntl(fdMother, F_SETFL, FNDELAY);

   if(n == -1)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Non blocking set error.");
      exit(1);
   }

   int i;
   n = setsockopt(fdMother, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
   if(n == -1)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Setsockopt TCP_NODELAY failed.");
      exit(1);
   }

   CaptainHook.Hook(fdMother, &MotherHook);
}

/* Returns how much memory is in use by system. This may help spotting
   possible fragmentation of memory. */
void system_memory(unit_data *ch)
{
#ifdef LINUX
   struct rusage rusage_data;
   int           n;
   char          Buf[1024];

   n = getrusage(RUSAGE_CHILDREN, &rusage_data);

   if(n != 0)
      slog(LOG_ALL, 0, "System memory status error.");
   else
   {
      sprintf(Buf,
              "Vol. Switches       %8ld\n\r"
              "Max RSS             %8ld\n\r"
              "Shared memory size  %8ld\n\r"
              "Unshared data size  %8ld\n\r"
              "Unshared stack size %8ld\n\r\n\r",
              rusage_data.ru_nvcsw,
              rusage_data.ru_maxrss,
              rusage_data.ru_ixrss,
              rusage_data.ru_isrss,
              rusage_data.ru_idrss);
      send_to_char(Buf, ch);
   }
#endif
}
