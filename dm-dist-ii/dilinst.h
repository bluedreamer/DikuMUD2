#pragma once
/* *********************************************************************** *
 * File   : dilinst.h                                 Part of Valhalla MUD *
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

#include "dilprg.h"
#include "dilval.h"
void dil_insterr(dilprg *p, char *where);

void dilfi_folo(dilprg *, dilval *);
void dilfi_lcri(dilprg *, dilval *);
void dilfi_sete(dilprg *, dilval *);
void dilfi_amod(dilprg *, dilval *);
void dilfi_stor(dilprg *, dilval *);
void dilfi_chas(dilprg *, dilval *);
void dilfi_setf(dilprg *, dilval *);
void dilfi_ass(dilprg *, dilval *);
void dilfi_lnk(dilprg *, dilval *);
void dilfi_exp(dilprg *, dilval *);
void dilfi_if(dilprg *, dilval *);
void dilfi_set(dilprg *, dilval *);
void dilfi_uset(dilprg *, dilval *);
void dilfi_adl(dilprg *, dilval *);
void dilfi_sul(dilprg *, dilval *);
void dilfi_ade(dilprg *, dilval *);
void dilfi_sue(dilprg *, dilval *);
void dilfi_dst(dilprg *, dilval *);
void dilfi_walk(dilprg *, dilval *);
void dilfi_exec(dilprg *, dilval *);
void dilfi_wit(dilprg *, dilval *);
void dilfi_act(dilprg *, dilval *);
void dilfi_goto(dilprg *, dilval *);
void dilfi_sua(dilprg *, dilval *);
void dilfi_ada(dilprg *, dilval *);
void dilfi_pri(dilprg *, dilval *);
void dilfi_npr(dilprg *, dilval *);
void dilfi_snd(dilprg *, dilval *);
void dilfi_snt(dilprg *, dilval *);
void dilfi_snta(dilprg *, dilval *);
void dilfi_log(dilprg *, dilval *);
void dilfi_sec(dilprg *, dilval *);
void dilfi_use(dilprg *, dilval *);
void dilfi_eqp(dilprg *, dilval *);
void dilfi_ueq(dilprg *, dilval *);
void dilfi_quit(dilprg *, dilval *);
void dilfi_blk(dilprg *, dilval *);
void dilfi_pup(dilprg *, dilval *);
void dilfi_cast(dilprg *, dilval *);
void dilfi_rproc(dilprg *, dilval *);
void dilfi_rfunc(dilprg *, dilval *);
void dilfi_rts(dilprg *, dilval *);
void dilfi_rtf(dilprg *, dilval *);
void dilfi_dlc(dilprg *, dilval *);
void dilfi_on(dilprg *, dilval *);
void dilfi_rsproc(dilprg *, dilval *);
void dilfi_rsfunc(dilprg *, dilval *);
void dilfi_swt(dilprg *, dilval *);
void dilfi_sbt(dilprg *, dilval *);
void dilfi_cli(dilprg *, dilval *);
void dilfi_foe(dilprg *, dilval *);
void dilfi_fon(dilprg *, dilval *);
void dilfi_sntadil(dilprg *, dilval *);
