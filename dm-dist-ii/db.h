#pragma once
#include "bin_search_type.h"
#include "bytestring.h"
#include "config.h"
#include "dbfind.h"

#include <string>

const std::string MUD_NAME{"Valhalla"}; /* For ease of localization */

const std::string DFLT_DIR{"lib/"};        // Default data directory
const std::string ZONE_DIR{"zon/"};        // Default zone directory
const std::string PLAY_DIR{"ply/"};        // Default player directory
const std::string UNIT_DIR{"units/"};      // Directory for DIL store / restore units
const std::string PERSIST_DIR{"persist/"}; // Directory for persistant units
const std::string BBS_DIR{"bbs/"};         // Default BBS shared directory

const std::string BASIS_ZONE{"basis"};

/* Data files (LIB) used by the game system */

const std::string RACE_DEFS{"races.dat"};
const std::string ABILITY_DEFS{"abilities.dat"};
const std::string SPELL_DEFS{"spells.dat"};
const std::string WEAPON_DEFS{"weapons.dat"};
const std::string SKILL_DEFS{"skills.dat"};

const std::string SERVER_CONFIG{"server.cfg"};

const std::string EXECUTE_FILE{"execute.dat"};
const std::string STATISTIC_FILE{"connectionsnew.dat"};
const std::string COMMAND_FILE{"commands"};
const std::string COMPETITION_FILE{"competition"};
const std::string ACCOUNT_FILE{"account"};
const std::string ACCOUNT_LOG{"account.log"};
const std::string CREDITFILE_LOG{"ccinfo.log"};
const std::string STATISTICS_FILE{"statis.dat"};
const std::string SLIME_FILE{"slime"};
const std::string CRIME_NUM_FILE{"crime_nr"};
const std::string CRIME_ACCUSE_FILE{"crime"};
const std::string PLAYER_ID_NAME{"players.id"};
const std::string MAIL_FILE_NAME{"mailbox.idx"};
const std::string MAIL_BLOCK_NAME{"mailbox.blk"};
const std::string CREDITS_FILE{"credits"}; /* for the 'credits' command  */
const std::string NEWS_FILE{"news"};       /* for the 'news' command     */
const std::string MOTD_FILE{"motd"};       /* messages of today          */
const std::string LOGO_FILE{"logo"};
const std::string WELCOME_FILE{"welcome"};
const std::string GOODBYE_FILE{"goodbye"};
const std::string NEWBIE_FILE{"newbie"};
const std::string IDEA_FILE{"ideas"};            /* for the 'idea'-command     */
const std::string TYPO_FILE{"typos"};            /*         'typo'             */
const std::string BUG_FILE{"bugs"};              /*         'bug'              */
const std::string MESS_FILE{"messages"};         /* damage message             */
const std::string SOCMESS_FILE{"actions"};       /* messgs for social acts     */
const std::string HELP_FILE{"help"};             /* for HELP <keyword>         */
const std::string HELP_FILE_WIZ{"help.wiz"};     /* for HELP <keyword>         */
const std::string HELP_FILE_LOCAL{"help.local"}; /* for HELP <keyword>         */
const std::string WIZLIST_FILE{"wizlist"};       /* for WIZLIST                */
const std::string POSEMESS_FILE{"poses"};        /* for 'pose'-command         */
const std::string MONEYDEF_FILE{"money"};        /* For definition of money    */
const std::string ZONE_FILE_LIST{"zonelist"};

auto                  read_unit_string(CByteBuffer *pBuf, int type, int len, int bSwapin, char *whom) -> unit_data *;
void                  read_unit_file(file_index_type *org_fi, CByteBuffer *pBuf);
auto                  read_unit(file_index_type *fi) -> unit_data *;
void                  free_unit(unit_data *ch);
void                  free_extra_descr(extra_descr_data *ex);
void                  free_extra_descr_list(extra_descr_data *ex);

auto                  create_extra_descr() -> extra_descr_data *;
auto                  create_unit(uint8_t type) -> unit_data *;

/* --- The globals of db.c --- */

extern int            room_number;
extern unit_data     *unit_list;
