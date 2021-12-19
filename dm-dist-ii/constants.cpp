#include "skills.h"
#include "values.h"

#include <climits>

/* ---------------------------------------------------------------------- */
/*                         M O V E M E N T                                */
/* ---------------------------------------------------------------------- */

const char *char_pos[] = {
   "dead R.I.P.", "mortally-wounded", "incapacitated", "stunned", "sleeping", "resting", "sitting", "fighting", "standing", nullptr};

/* Moved to common.c: const char *dirs[] = */

const char *enter_dirs[]     = {"the north", "the east", "the south", "the west", "above", "below", nullptr};

 int   rev_dir[]        = {2, 3, 0, 1, 5, 4};

const char *room_landscape[] = {"inside",
                                "city",
                                "field",
                                "forest",
                                "hills",
                                "mountain",
                                "desert",
                                "swamp",

                                "water-swim",
                                "water-sail",
                                "under-water",
                                "snow",
                                "slush",
                                "ice",
                                nullptr};

 int   movement_loss[]  = {
   1,  /* Inside     */
   1,  /* City       */
   2,  /* Field      */
   3,  /* Forest     */
   4,  /* Hills      */
   6,  /* Mountains  */
   8,  /* Desert     */
   8,  /* Swamp      */
   4,  /* low-water  */
   50, /* deep-water */
   8,  /* underwater */
   8,  /* snow */
   6,  /* slush */
   10  /* ice   */
};

/* ---------------------------------------------------------------------- */
/*                               T I M E                                  */
/* ---------------------------------------------------------------------- */

const char *weekdays[MUD_WEEK]   = {"the Day of the Moon",
                                  "the Day of the Bull",
                                  "the Day of the Deception",
                                  "the Day of Thunder",
                                  "the Day of Freedom",
                                  "the Day of the Great Gods",
                                  "the Day of the Sun"};

const char *month_name[MUD_YEAR] = {
   "Month of Winter",          /* Winter 0 */
   "Month of the Winter Wolf", /* Winter 1 */
   "Month of the Frost Giant", /* Winter 2 */

   "Month of the Spring", /* Spring 3 */
   "Month of Futility",   /* Spring 4 */

   "Month of the Sun",  /* Summer 5 */
   "Month of the Heat", /* Summer 6 */

   "Month of the Long Shadows",    /* Fall   7 */
   "Month of the Ancient Darkness" /* Fall   8 */
};

int8_t time_light[SUN_SET + 1] = {
   -1, /* SUN_DARK  */
   0,  /* SUN_RISE  */
   1,  /* SUN_LIGHT */
   0   /* SUN_SET   */
};

/* ---------------------------------------------------------------------- */
/*                          C O N S U M A B L E                           */
/* ---------------------------------------------------------------------- */

/* char *drinks moved to common.c! */

const char *color_liquid[LIQ_MAX + 2] = {"clear",
                                         "brown",
                                         "clear",
                                         "brown",
                                         "dark",
                                         "golden",
                                         "red",
                                         "green",
                                         "clear",
                                         "light green",
                                         "white",
                                         "brown",
                                         "black",
                                         "red",
                                         "clear",
                                         "black",
                                         "clear",
                                         "golden",
                                         nullptr};

int         drink_aff[LIQ_MAX + 2][3] = {
   {0, 1, 10}, /* Water    */
   {3, 2, 5},  /* beer     */
   {5, 2, 5},  /* wine     */
   {2, 2, 5},  /* ale      */
   {1, 2, 5},  /* ale      */
   {6, 1, 4},  /* Whiskey  */
   {0, 1, 8},  /* lemonade */
   {10, 0, 0}, /* firebr   */
   {3, 3, 3},  /* local    */
   {0, 4, -8}, /* juice    */
   {0, 3, 6},  /* milk     */
   {0, 1, 6},  /* tea      */
   {0, 1, 6},  /* coffee   */
   {0, 2, -1}, /* blood    */
   {0, 1, -2}, /* salt wat */
   {0, 1, 5},  /* coke     */
   {10, 0, 0}, /* vodka    */
   {6, 1, 4}   /* Brandy  */
};

const char *fullness[] = {
   "less than half ", "about half ", "more than half ", "" /* nothing makes "full" appear */
};

/* ---------------------------------------------------------------------- */
/*                         I N F O R M A T I V E                          */
/* ---------------------------------------------------------------------- */

const char   *arm_text[ARM_PLATE + 1 + 1] = {"clothes base", "leather base", "hardened leather base", "chain base", "plate base", nullptr};

const char   *shi_text[SHIELD_LARGE + 2]  = {"small shield", "medium shield", "large shield", nullptr};

const char   *bodyparts[]                 = {"arms",       "left arm",  "right arm", "legs",      "left leg",   "right leg", "head", "feet",
                           "right foot", "left foot", "toes",      "hands",     "right hand", "left hand", "face", "hair",
                           "neck",       "butt",      "eyes",      "right eye", "left eye",   "mouth",     "ears", "right ear",
                           "left ear",   "teeth",     "tounge",    "nose",      "nostrils",   nullptr};

 uint8_t bodyparts_cover[31][5]      = {
   {WEAR_ARMS, WEAR_UNUSED},                                    /* arms */
   {WEAR_ARMS, WEAR_UNUSED},                                    /* left arm */
   {WEAR_ARMS, WEAR_UNUSED},                                    /* right arm */
   {WEAR_LEGS, WEAR_UNUSED},                                    /* legs */
   {WEAR_LEGS, WEAR_UNUSED},                                    /* left leg */
   {WEAR_LEGS, WEAR_UNUSED},                                    /* right leg */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* head */
   {WEAR_FEET, WEAR_UNUSED},                                    /* feet */
   {WEAR_FEET, WEAR_UNUSED},                                    /* right foot */
   {WEAR_FEET, WEAR_UNUSED},                                    /* left foot */
   {WEAR_FEET, WEAR_UNUSED},                                    /* toes */
   {WEAR_HANDS, WEAR_UNUSED},                                   /* hands */
   {WEAR_HANDS, WEAR_UNUSED},                                   /* right hand */
   {WEAR_HANDS, WEAR_UNUSED},                                   /* left hand */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* face */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* hair */
   {WEAR_NECK_1, WEAR_NECK_2, WEAR_UNUSED},                     /* neck */
   {WEAR_ABOUT, WEAR_BODY, WEAR_LEGS, WEAR_WAIST, WEAR_UNUSED}, /* butt */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* eyes */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* right eye */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* left eye */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* mouth */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* ears */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* right ear */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* left ear */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* teeth */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* tounge */
   {WEAR_HEAD, WEAR_UNUSED},                                    /* nose */
   {WEAR_HEAD, WEAR_UNUSED}                                     /* nostrils */
};

const char    *where[]           = {"<ILLEGAL POSITION>   ", /* Position 0 is not a position */
                       "<ILLEGAL POSITION>   ", /* Position light is not used   */
                       "<worn on finger>     ", "<worn on finger>     ", "<worn around neck>   ", "<worn around neck>   ",
                       "<worn on body>       ", "<worn on head>       ", "<worn on legs>       ", "<worn on feet>       ",
                       "<worn on hands>      ", "<worn on arms>       ", "<worn as shield>     ", "<worn about body>    ",
                       "<worn about waist>   ", "<worn around wrist>  ", "<worn around wrist>  ", "<wielded>            ",
                       "<held>               ", "<worn across chest>  ", "<worn on back>       ", "<worn on left ear>   ",
                       "<worn on right ear>  ", "<worn on ankle>      ", "<worn on ankle>      ", nullptr};

/* ---------------------------------------------------------------------- */
/*                      P O I N T   S Y S T E M                           */
/* ---------------------------------------------------------------------- */

const char    *npc_class_types[] = {"Normal", "Undead", nullptr};

skill_interval how_good[]        = {{-20, "horrible"},
                             {-15, "very bad"},
                             {-10, "bad"},
                             {-5, "worse than average"},
                             {0, "average"},
                             {5, "a little better than average"},
                             {10, "better than average"},
                             {15, "good"},
                             {20, "very good"},
                             {25, "supreme"},
                             {-1, nullptr}};

skill_interval weapon_skills[]   = {{0, "utterly hopeless"},
                                  {10, "impossible"},
                                  {25, "poor"},
                                  {40, "good"},
                                  {50, "skilled"},
                                  {70, "specialized"},
                                  {90, "supreme"},
                                  {100, "expert"},
                                  {150, "godly"},
                                  {-1, nullptr}};

skill_interval skill_skills[]    = {{0, "utterly hopeless"},
                                 {10, "have heard about"},
                                 {25, "have tried a little"},
                                 {40, "have practiced"},
                                 {50, "are good at"},
                                 {70, "are an expert at"},
                                 {90, "master"},
                                 {100, "are expert at"},
                                 {150, "are godly at"},
                                 {-1, nullptr}};

skill_interval spell_skills[]    = {{10, "might remember"},
                                 {25, "can almost remember"},
                                 {40, "can remember"},
                                 {50, "have practiced"},
                                 {70, "have learned"},
                                 {90, "master"},
                                 {100, "are expert with"},
                                 {150, "are godly with"},
                                 {-1, nullptr}};

#ifdef SUSPEKT
const char *weapon_skills[] = {"impossible", "poor", "good", "skilled", "specialized", "supreme", "godly", NULL};

const char *skill_skills[]  = {
   "have heard about", "have tried a little", "have practiced", "are good at", "are an expert at", "master", "are godly at", NULL};

const char *spell_skills[] = {
   "might remember", "can almost remember", "can remember", "have practiced", "have learned", "master", "are godly with", NULL};
#endif

/* ---------------------------------------------------------------------- */
/*                         B I T - V E C T O R S                          */
/* ---------------------------------------------------------------------- */

const char *sfb_flags[] = {
   "Priority", "Time_Variation", "Normal_Cmd", "Tick", "Death", "Combat", "Message", "Save", "Aware", "Activate", "PRE", "Done", nullptr};

/* UNIT_MANIPULATE */
const char *unit_manipulate[] = {"take",  "finger", "neck",  "body", "head",  "legs", "feet", "hands", "arms",  "shield", "about-body",
                                 "waist", "wrist",  "wield", "hold", "enter", "ear",  "back", "chest", "ankle", nullptr};

const char *unit_open_flags[] = {"open-close", "closed", "locked", "pickproof", "inside-open-close-lock-unlock", "hidden", nullptr};

/* UNIT_FLAGS */
const char *unit_flags[]      = {"private",
                            "invisible",
                            "can-be-buried",
                            "is-buried",
                            "no-teleport",
                            "sacred",
                            "no-mobile",
                            "no-weather",
                            "indoors",
                            "invisible-mortals",
                            "transparant",
                            "is-edited",
                            "no-save",
                            "magic",
                            nullptr};

/* OBJ_FLAGS */
const char *obj_flags[]       = {"unequip", "two-hands", "nocover", "nodual", nullptr};

/* ROOM_FLAGS */
const char *room_flags[]      = {"save",
                            "unused",
                            "no-save",

                            nullptr};

/* CHAR_FLAGS but ought to be CHAR_FLAGS */
const char *char_flags[]      = {
   "law-protected",     "legal-target",     "outlaw",       "group",         "blind",         "hide",         "mute",        "sneak",
   "detect-alignments", "detect-invisible", "detect-magic", "detect-poison", "detect-undead", "detect-curse", "detect-life", "wimpy",
   "not-used",          "self-defence",     "peaceful",     nullptr};

/* PC_FLAGS */
const char *pc_flags[]        = {"brief",
                          "PK relaxed",
                          "information",
                          "compact",
                          "nowiz",
                          "spirit",
                          "expert",
                          "prompt",
                          "GA Telnet",
                          "echo-say",
                          "ansi-terminal",
                          "dmserver-terminal",
                          "noshouting",
                          "notelling",
                          "noshout",
                          "notell",
                          nullptr};

/* NPC FLAGS */
const char *npc_flags[]       = {"ILLEGAL-BIT", "sentinel", "scavenger", "nice-thief", "agressive", "stay-zone", nullptr};

/* ---------------------------------------------------------------------- */
/*                             O T H E R (stat)                           */
/* ---------------------------------------------------------------------- */

const char *unit_status[]     = {"NPC", "PC", "ROOM", "OBJECT", nullptr};

const char *char_sex[]        = {"neutral", "male", "female", nullptr};

const char *obj_types[]       = {"ILLEGAL OBJECT TYPE",
                           "light",
                           "scroll",
                           "wand",
                           "staff",
                           "weapon",
                           "fire-weapon",
                           "missile",
                           "treasure",
                           "armor",
                           "potion",
                           "worn",
                           "other",
                           "trash",
                           "trap",
                           "container",
                           "note",
                           "liquid-container",
                           "key",
                           "food",
                           "money",
                           "pen",
                           "boat",
                           "spell-page",
                           "spell-book",
                           "shield",
                           nullptr};

const char *equip_pos[]       = {"In inventory/Not equipped",
                           "ILLEGAL EQUIP POSITION - REPORT!",
                           "Worn on right finger",
                           "Worn on left finger",
                           "First worn around Neck",
                           "Second worn around Neck",
                           "Worn on body",
                           "Worn on head",
                           "Worn on legs",
                           "Worn on feet",
                           "Worn on hands",
                           "Worn on arms",
                           "Worn as shield",
                           "Worn about body",
                           "Worn around waist",
                           "Worn around right wrist",
                           "Worn around left wrist",
                           "Wielded",
                           "Held",
                           nullptr};

const char *obj_pos[]         = {"inventory", "ILLEGAL EQUIP POSITION - REPORT!",
                         "rfinger",   "lfinger",
                         "1neck",     "2neck",
                         "body",      "head",
                         "legs",      "feet",
                         "hands",     "arms",
                         "shield",    "body",
                         "waist",     "rwrist",
                         "lwrist",    "wielded",
                         "held",      "chest",
                         "back",      "ear1",
                         "ear2",      "ankle1",
                         "ankle2",    nullptr};

const char *connected_types[] = {"Playing",
                                 "Get Name",
                                 "Confirm Name",
                                 "Read Pwd",
                                 "New Pwd",
                                 "New Pwd Chk",
                                 "Message Of Today",
                                 "Menu Select",
                                 "Extra Description",
                                 "Redefine Pwd",
                                 "Redefine Pwd Chk",
                                 "Waiting to cut connection.",
                                 "Thrown off",
                                 "New select Sex",
                                 "New select Race",
                                 "New select Guild",
                                 "Confirm player deletion",
                                 nullptr};
