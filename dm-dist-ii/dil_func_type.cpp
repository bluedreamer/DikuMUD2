#include "dil_func_type.h"

#include "dilexp.h"
#include "dilinst.h"
/* ************************************************************************ */
/* Evaluating DIL-expressions/instructions				    */
/* ************************************************************************ */

dil_func_type dilfe_func[DILI_MAX + 1] = {
   {dilfe_illegal}, /* 0 */
   {dilfe_plus},    {dilfe_min},  {dilfe_mul},  {dilfe_div},    {dilfe_mod},     {dilfe_and},  {dilfe_or},   {dilfe_not},  {dilfe_gt},
   {dilfe_lt}, /* 10 */
   {dilfe_ge},      {dilfe_le},   {dilfe_eq},   {dilfe_pe},     {dilfe_ne},      {dilfe_in},   {dilfe_umin}, {dilfe_se},   {dilfe_null},

   {dilfe_fld},                                                               /* 20 */
   {dilfe_atoi},    {dilfe_itoa}, {dilfe_rnd},  {dilfe_fndu},   {dilfe_fndr}, /* 25 */
   {dilfe_load},    {dilfe_iss},  {dilfe_getw}, {dilfe_isa},    {dilfe_cmds}, /* 30 */
   {dilfe_fnds},

   {dilfe_acti},    {dilfe_argm}, {dilfe_tho},  {dilfe_tda},    {dilfe_tmd},     {dilfe_tye},  {dilfe_hrt},  {dilfe_self},

   {dilfe_var}, /* 40 */
   {dilfe_fs},      {dilfe_fsl},  {dilfe_int},  {dilfe_len},

   {dilfi_ass},     {dilfi_lnk},  {dilfi_exp},  {dilfi_cast},   {dilfi_if},      {dilfi_set}, /* 50 */
   {dilfi_uset},    {dilfi_adl},  {dilfi_sul},  {dilfi_ade},    {dilfi_sue},     {dilfi_dst},  {dilfi_walk}, {dilfi_exec}, {dilfi_wit},
   {dilfi_act}, /* 60 */
   {dilfi_goto},    {dilfi_sua},  {dilfi_ada},  {dilfi_pri},    {dilfi_npr},     {dilfi_snd},  {dilfi_snt},  {dilfi_sec},  {dilfi_use},
   {dilfi_foe}, /* 70 */
   {dilfi_fon},     {dilfi_eqp},  {dilfi_ueq},  {dilfe_weat},   {dilfe_oppo},    {dilfi_quit},

   {dilfi_blk},     {dilfi_pup},

   {dilfe_getws},   {dilfi_snta}, /* 80 */
   {dilfi_log},     {dilfe_pne},  {dilfe_sne},  {dilfi_rproc},  {dilfi_rfunc},   {dilfi_rts},  {dilfi_rtf},  {dilfe_dld},  {dilfe_dlf},
   {dilfi_dlc}, /* 90 */
   {dilfe_lor},     {dilfe_land}, {dilfi_on},   {dilfi_rsproc}, {dilfi_rsfunc},  {dilfe_intr}, {dilfi_cli},  {dilfi_sbt},  {dilfi_swt},
   {dilfe_fndru},                                                                /* 100 */
   {dilfe_visi},    {dilfe_atsp}, {dilfe_purs}, {dilfi_chas},   {dilfi_setf},    /* 105 */
   {dilfe_medi},    {dilfe_targ}, {dilfe_powe}, {dilfe_trmo},   {dilfi_sntadil}, /* 110 */
   {dilfe_cast2},   {dilfe_mel},  {dilfe_eqpm}, {dilfe_cmst},   {dilfe_opro},    /* 115 */
   {dilfe_rest},    {dilfi_stor}, {dilfi_amod}, {dilfi_sete},   {dilfi_folo},    /* 120 */
   {dilfi_lcri},    {dilfe_fits}, {dilfe_cary}, {dilfe_fnds2},  {dilfe_path},    /* 125  */
   {dilfe_mons},    {dilfe_splx}, {dilfe_spli}, {dilfe_rti},    {dilfe_txf},     /* 130  */
   {dilfe_ast},     {dilfe_visi}, {dilfe_act}};
