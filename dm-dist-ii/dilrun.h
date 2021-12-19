#pragma once
/* *********************************************************************** *
 * File   : dilrun.h                                  Part of Valhalla MUD *
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

#include "unit_data.h"

struct dil_func_type
{
   void (*func)(struct dilprg *, struct dilval *);
};

extern struct dil_func_type dilfe_func[];

/* Maximum number of consecutive instructions allowed in one tick */
#define WAITCMD_FINISH    -100 /* Stop program execution until next event */
#define WAITCMD_STOP      -150 /* Suspend execution                       */
#define WAITCMD_QUIT      -200 /* Quit the DIL program entirely	   */
#define WAITCMD_DESTROYED -500 /* The DIL program was destroyed	   */

/* MS: The ONLY reason for having TWO almost identical evals, is that I
   want to initialize the result variables as error by default! */

#define eval_dil_exp(prg, v)                                                                                                               \
   {                                                                                                                                       \
      (prg)->sp->pc++;                                                                                                                     \
      (v)->type = DILV_ERR;                                                                                                                \
      (dilfe_func[*(prg->sp->pc - 1)].func((prg), (v)));                                                                                   \
      assert((prg)->sp->pc <= &((prg)->sp->tmpl->core[(prg)->sp->tmpl->coresz]));                                                          \
   }

void DeactivateDil(unit_data *pc);
void ActivateDil(unit_data *pc);

auto dil_direct_init(struct spec_arg *sarg) -> int;
auto dil_init(struct spec_arg *sarg) -> int;

void dil_activate(struct dilprg *prg);
void dil_init_vars(int varc, struct dilframe *frm);
void dil_free_var(struct dilvar *var);

void dil_free_frame(struct dilframe *frame);

auto dil_getbool(struct dilval *v) -> char;
auto dil_getval(struct dilval *v) -> int;
void dil_add_secure(struct dilprg *prg, unit_data *sup, uint8_t *lab);
#ifdef __cplusplus
void dil_sub_secure(struct dilframe *frm, unit_data *sup, int bForeach = static_cast<int>(FALSE));
#endif
auto same_environment(unit_data *u1, unit_data *u2) -> int;
void unhash_str(char **s);
void dil_clear_non_secured(struct dilprg *prg);
void dil_clear_lost_reference(struct dilframe *frm, void *ptr);
void dil_test_secure(struct dilprg *prg);
auto dil_destroy(char *name, unit_data *u) -> int;

/* NULL fptr creates one...  */
auto dil_copy_template(struct diltemplate *tmpl, unit_data *u, unit_fptr **pfptr) -> struct dilprg *;
auto dil_copy(char *name, unit_data *u) -> struct dilprg *;

auto dil_find(const char *name, unit_data *u) -> unit_fptr *;

void dil_typeerr(struct dilprg *p, const char *where);

#define FAIL_NULL     1
#define TYPEFAIL_NULL 2

auto dil_type_check(const char *f, struct dilprg *p, int tot, ...) -> int;
void dil_free_prg(struct dilprg *prg);

void dil_intr_remove(struct dilprg *p, int idx);
auto dil_intr_insert(struct dilprg *p, uint8_t *lab, uint16_t flags) -> int;
