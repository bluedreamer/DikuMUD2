#pragma once
/* *********************************************************************** *
 * File   : dilexp.h                                  Part of Valhalla MUD *
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

/* Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun (dilfe_pck) */

#include "dil.h"
#include "dilval.h"

void dilfe_rti(dilprg *, dilval *);
void dilfe_txf(dilprg *, dilval *);
void dilfe_ast(dilprg *, dilval *);
void dilfe_splx(dilprg *, dilval *);
void dilfe_spli(dilprg *, dilval *);
void dilfe_mons(dilprg *, dilval *);
void dilfe_path(dilprg *, dilval *);
void dilfe_cary(dilprg *, dilval *);
void dilfe_fits(dilprg *, dilval *);
void dilfe_rest(dilprg *, dilval *);
void dilfe_opro(dilprg *, dilval *);
void dilfe_eqpm(dilprg *, dilval *);
void dilfe_mel(dilprg *, dilval *);
void dilfe_cast2(dilprg *, dilval *);
void dilfe_atsp(dilprg *, dilval *);
void dilfe_weat(dilprg *p, dilval *v);
void dilfe_illegal(dilprg *, dilval *);
void dilfe_plus(dilprg *, dilval *);
void dilfe_min(dilprg *, dilval *);
void dilfe_mul(dilprg *, dilval *);
void dilfe_div(dilprg *, dilval *);
void dilfe_mod(dilprg *, dilval *);
void dilfe_and(dilprg *, dilval *);
void dilfe_or(dilprg *, dilval *);
void dilfe_not(dilprg *, dilval *);
void dilfe_atoi(dilprg *, dilval *);
void dilfe_rnd(dilprg *, dilval *);
void dilfe_fndu(dilprg *, dilval *);
void dilfe_fndru(dilprg *, dilval *);
void dilfe_fndr(dilprg *, dilval *);
void dilfe_load(dilprg *, dilval *);
void dilfe_gt(dilprg *, dilval *);
void dilfe_lt(dilprg *, dilval *);
void dilfe_ge(dilprg *, dilval *);
void dilfe_le(dilprg *, dilval *);
void dilfe_eq(dilprg *, dilval *);
void dilfe_pe(dilprg *, dilval *);
void dilfe_ne(dilprg *, dilval *);
void dilfe_iss(dilprg *, dilval *);
void dilfe_in(dilprg *, dilval *);
void dilfe_getw(dilprg *, dilval *);
void dilfe_getws(dilprg *, dilval *);
void dilfe_fld(dilprg *, dilval *);
void dilfe_var(dilprg *, dilval *);
void dilfe_fs(dilprg *, dilval *);
void dilfe_int(dilprg *, dilval *);
void dilfe_umin(dilprg *, dilval *);
void dilfe_self(dilprg *, dilval *);
void dilfe_acti(dilprg *, dilval *);
void dilfe_argm(dilprg *, dilval *);
void dilfe_cmst(dilprg *, dilval *);
void dilfe_itoa(dilprg *, dilval *);
void dilfe_len(dilprg *, dilval *);
void dilfe_se(dilprg *, dilval *);
void dilfe_fsl(dilprg *, dilval *);
void dilfe_hrt(dilprg *, dilval *);
void dilfe_isa(dilprg *, dilval *);
void dilfe_tho(dilprg *, dilval *);
void dilfe_tda(dilprg *, dilval *);
void dilfe_tmd(dilprg *, dilval *);
void dilfe_tye(dilprg *, dilval *);
void dilfe_cmds(dilprg *, dilval *);
void dilfe_fnds(dilprg *, dilval *);
void dilfe_fnds2(dilprg *, dilval *);
void dilfe_sne(dilprg *, dilval *);
void dilfe_pne(dilprg *, dilval *);
void dilfe_null(dilprg *, dilval *);
void dilfe_dlf(dilprg *, dilval *);
void dilfe_dld(dilprg *, dilval *);
void dilfe_lor(dilprg *, dilval *);
void dilfe_land(dilprg *, dilval *);
void dilfe_intr(dilprg *, dilval *);
void dilfe_visi(dilprg *, dilval *);
void dilfe_oppo(dilprg *, dilval *);
void dilfe_purs(dilprg *, dilval *);
void dilfe_medi(dilprg *, dilval *);
void dilfe_targ(dilprg *, dilval *);
void dilfe_powe(dilprg *, dilval *);
void dilfe_trmo(dilprg *, dilval *);
void dilfe_pck(dilprg *, dilval *);
void dilfe_act(dilprg *, dilval *);
