/* *********************************************************************** *
 * File   : dilexp.c                                  Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk & seifert@diku.dk				   *
 *                                                                         *
 *                                                                         *
 * Purpose: DIL Expressions.		       	                           *
 *									   *
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

/* Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun */
#include "dilexp.h"
#include "affect.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "db_file.h"
#include "dil.h"
#include "dilrun.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "money.h"
#include "movement.h"
#include "skills.h"
#include "spell_args.h"
#include "spell_info_type.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "tree_type.h"
#include "unit_fptr.h"
#include "unixshit.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>

/* ************************************************************************ */
/* DIL-expressions							    */
/* ************************************************************************ */

void dilfe_illegal(struct dilprg *p, class dilval *v)
{
   szonelog(UNIT_FI_ZONE(p->sarg->owner), "DIL %s@%s, Illegal Expression/Instruction Node.\n", UNIT_FI_NAME(p->sarg->owner),
            UNIT_FI_ZONENAME(p->sarg->owner));
   p->waitcmd = WAITCMD_QUIT;
   if(v != nullptr)
   {
      v->type = DILV_ERR; /* error value */
   }
}

void dilfe_atsp(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;
   class dilval v4;
   class dilval v5;

   eval_dil_exp(p, &v1); /* spell number */
   eval_dil_exp(p, &v2); /* caster */
   eval_dil_exp(p, &v3); /* medium */
   eval_dil_exp(p, &v4); /* target */
   eval_dil_exp(p, &v5); /* bonus */

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v2.val.ptr == nullptr) || !IS_CHAR((unit_data *)v2.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v3.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v4))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v4.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v5))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if((is_in(v1.val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) != 0) &&
      ((spell_info[v1.val.num].spell_pointer != nullptr) || (spell_info[v1.val.num].tmpl != nullptr)) && v->type == DILV_INT)
   {
      spell_args sa;

      set_spellargs(&sa, (unit_data *)v2.val.ptr, (unit_data *)v3.val.ptr, (unit_data *)v4.val.ptr, nullptr, 0);
      sa.pEffect = nullptr;

      /* cast the spell */
      v->val.num = spell_offensive(&sa, v1.val.num, v5.val.num);
      dil_test_secure(p);
   }
}

void dilfe_cast2(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;
   class dilval v4;
   class dilval v5;

   eval_dil_exp(p, &v1); /* spell number */
   eval_dil_exp(p, &v2); /* caster       */
   eval_dil_exp(p, &v3); /* medium       */
   eval_dil_exp(p, &v4); /* target       */
   eval_dil_exp(p, &v5); /* Display?     */

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v2.val.ptr == nullptr) || !IS_CHAR((unit_data *)v2.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v3.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v4))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v4.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v5))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT && (is_in(v1.val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) != 0) &&
      ((spell_info[v1.val.num].spell_pointer != nullptr) || (spell_info[v1.val.num].tmpl != nullptr)))
   {
      /* cast the spell */
      char mbuf[MAX_INPUT_LENGTH] = {0};
      v->val.num = spell_perform(v1.val.num, MEDIA_SPELL, (unit_data *)v2.val.ptr, (unit_data *)v3.val.ptr, (unit_data *)v4.val.ptr, mbuf,
                                 (char *)v5.val.ptr);
      dil_test_secure(p);
   }
}

void dilfe_rest(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   auto restore_unit(char *zonename, char *unitname)->unit_data *;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_UP;

   switch(dil_getval(&v1))
   {
      case DILV_SP:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_SP:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_UP)
   {
      v->val.ptr = restore_unit((char *)v1.val.ptr, (char *)v2.val.ptr);
      if(v->val.ptr == nullptr)
      {
         v->type = DILV_NULL;
      }
      else
      {
         unit_to_unit((unit_data *)v->val.ptr, p->owner);
      }
   }
}

void dilfe_opro(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      if((v1.val.num > 0) && (v2.val.num > 0) && (v2.val.num < v1.val.num / 2 - 1))
      {
         v->val.num = open_ended_roll(v1.val.num, v2.val.num);
      }
      else
      {
         v->val.num = 0;
      }
   }
}

void dilfe_eqpm(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_UP;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_UP)
   {
      v->val.ptr = equipment((unit_data *)v1.val.ptr, v2.val.num);
      if(v->val.ptr == nullptr)
      {
         v->type = DILV_NULL;
      }
   }
}

/* int meleeAttack(unit, unit, int, int) */
void dilfe_mel(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;
   class dilval v4;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v2.val.ptr == nullptr) || !IS_CHAR((unit_data *)v2.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v4))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      v->val.num = one_hit((unit_data *)v1.val.ptr, (unit_data *)v2.val.ptr, v3.val.num, v4.val.num);
      dil_test_secure(p);
   }
}

/* visible, some vs other */
void dilfe_visi(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v2.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      v->val.num = CHAR_CAN_SEE((unit_data *)v1.val.ptr, (unit_data *)v2.val.ptr);
   }
}

/* is unit opponent of other */
void dilfe_oppo(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v2.val.ptr == nullptr) || !IS_CHAR((unit_data *)v2.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      v->val.num =
         (CHAR_COMBAT((unit_data *)v1.val.ptr) ? CHAR_COMBAT((unit_data *)v1.val.ptr)->FindOpponent((unit_data *)v2.val.ptr) != nullptr
                                               : FALSE);
   }
}

/* spellindex */
void dilfe_splx(struct dilprg *p, class dilval *v)
{
   class dilval v1;

   eval_dil_exp(p, &v1)

      v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         if(v1.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      int   i;
      char *c = (char *)v1.val.ptr;

      if(str_is_empty(c) != 0u)
      {
         v->val.num = -1;
      }
      else
      {
         v->val.num = search_block_abbrevs(c, spl_text, (const char **)&c);
      }
   }
}

/* spellinfo */
void dilfe_spli(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;
   class dilval v4;
   class dilval v5;
   class dilval v6;
   class dilval v7;
   class dilval v8;

   v->type = DILV_FAIL;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);
   eval_dil_exp(p, &v5);
   eval_dil_exp(p, &v6);
   eval_dil_exp(p, &v7);
   eval_dil_exp(p, &v8);

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_INT:
         if(is_in(v1.val.num, SPL_ALL, SPL_TREE_MAX - 1) == 0)
         {
            v->type = DILV_FAIL;
            return;
         }
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v2.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v3.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v4.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v5.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v6.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v7.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v8.type)
   {
      case DILV_FAIL:
      case DILV_NULL:
         return;

      case DILV_SINT4R:
      case DILV_UINT4R:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   /* pName := spellinfo(SPL_LIGHTNING_3, nRealm, nSphere, nMana, bOffensive,
      nResistType, bvMedia, bvTargets); */

   /* We're home free... */

   *((uint32_t *)v2.ref) = spell_info[v1.val.num].realm;
   *((uint32_t *)v3.ref) = spl_tree[v1.val.num].parent;
   *((uint32_t *)v4.ref) = spell_info[v1.val.num].usesmana;
   *((uint32_t *)v5.ref) = spell_info[v1.val.num].offensive;
   *((uint32_t *)v6.ref) = spell_info[v1.val.num].cast_type;
   *((uint32_t *)v7.ref) = spell_info[v1.val.num].media;
   *((uint32_t *)v8.ref) = spell_info[v1.val.num].targets;

   v->type    = DILV_SP;
   v->atyp    = DILA_EXP;
   v->val.ptr = str_dup(spl_text[v1.val.num] == nullptr ? "" : spl_text[v1.val.num]);
}

/* contents of purse */
void dilfe_purs(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v1.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         if((v2.val.ptr == nullptr) || (*((char *)v2.val.ptr) == 0))
         {
            v->type = DILV_FAIL;
         }
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      int i;

      v->val.num = 0;

      for(i = 0; i <= MAX_MONEY; i++)
      {
         if(strcmp((char *)v2.val.ptr, money_types[i].abbrev) == 0)
         {
            break;
         }
      }

      if(i <= MAX_MONEY)
      {
         /* Note down money-objects in from, and their values */
         for(unit_data *tmp = UNIT_CONTAINS((unit_data *)v1.val.ptr); tmp != nullptr; tmp = tmp->next)
         {
            if(IS_MONEY(tmp) && MONEY_TYPE(tmp) == i)
            {
               v->val.num = MONEY_AMOUNT(tmp);
               break;
            }
         }
      }
   }
}

/* money_string */
void dilfe_mons(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);

   v->type = DILV_SP;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_SP)
   {
      v->atyp    = DILA_EXP;
      v->val.ptr = str_dup(money_string(v1.val.num, DEF_CURRENCY, v2.val.num));
   }
}

/* pathto */
void dilfe_path(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v1.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v2.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      unit_data *u1;
      unit_data *u2;
      u1 = unit_room((unit_data *)v1.val.ptr);
      u2 = unit_room((unit_data *)v2.val.ptr);

      v->val.num = move_to(u1, u2);
   }
}

/* can_carry */
void dilfe_cary(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v2.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         if(v3.val.num < 1)
         {
            v->type = DILV_FAIL;
         }
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      if(char_can_carry_n((unit_data *)v1.val.ptr, v3.val.num) == 0)
      {
         v->val.num = 1;
      }
      else if(char_can_carry_w((unit_data *)v1.val.ptr, v3.val.num * UNIT_WEIGHT((unit_data *)v2.val.ptr)) == 0)
      {
         v->val.num = 2;
      }
      else
      {
         v->val.num = 0;
      }
   }
}

/* transfermoney */
void dilfe_trmo(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr != nullptr) && !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_NULL:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v2.val.ptr != nullptr) && !IS_CHAR((unit_data *)v2.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_NULL:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   v->val.num = 0;

   if(v->type == DILV_INT && ((v1.val.ptr != nullptr) || (v2.val.ptr != nullptr)))
   {
      if(v1.val.ptr == nullptr)
      {
         slog(LOG_ALL, 0, "%s was given %d old gold pieces by DIL %s@%s.", UNIT_NAME((unit_data *)v2.val.ptr), v3.val.num,
              UNIT_FI_NAME(p->sarg->owner), UNIT_FI_ZONENAME(p->sarg->owner));
         money_transfer(nullptr, (unit_data *)v2.val.ptr, v3.val.num, local_currency((unit_data *)v2.val.ptr));
         v->val.num = 1;
      }
      else if(v2.val.ptr == nullptr)
      {
         if(char_can_afford((unit_data *)v1.val.ptr, v3.val.num, local_currency((unit_data *)v1.val.ptr)) != 0u)
         {
            money_transfer((unit_data *)v1.val.ptr, nullptr, v3.val.num, local_currency((unit_data *)v1.val.ptr));
            v->val.num = 1;
         }
      }
      else
      {
         if(char_can_afford((unit_data *)v1.val.ptr, v3.val.num, local_currency((unit_data *)v2.val.ptr)) != 0u)
         {
            money_transfer((unit_data *)v1.val.ptr, (unit_data *)v2.val.ptr, v3.val.num, local_currency((unit_data *)v2.val.ptr));
            v->val.num = 1;
         }
      }
   }
}

/* transfermoney */
void dilfe_fits(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   v->type = DILV_SP;

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_NULL:
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v2.val.ptr == nullptr) || !IS_OBJ((unit_data *)v2.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_NULL:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_SP)
   {
      auto obj_wear_size(unit_data * ch, unit_data * obj, int keyword)->char *;

      char *c = obj_wear_size((unit_data *)v1.val.ptr, (unit_data *)v2.val.ptr, v3.val.num);

      v->atyp    = DILA_EXP;
      v->val.ptr = str_dup(c == nullptr ? "" : c);
   }
}

void dilfe_intr(struct dilprg *p, class dilval *v)
{
   /* add interrupt to current frame */
   uint16_t intnum;
   uint8_t *lab;

   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   lab = p->sp->pc;
   eval_dil_exp(p, &v2); /* ignore */

   intnum = dil_intr_insert(p, lab, v1.val.num);

   if(v != nullptr)
   {
      v->val.num = intnum;
      v->type    = DILV_INT;
      v->atyp    = DILA_NONE;
   }

   bread_uint32_t(&(p->sp->pc)); /* skip label */
}

void dilfe_not(struct dilprg *p, class dilval *v)
{
   /* Negation of integers (and booleans, etc.) */
   class dilval v1;

   eval_dil_exp(p, &v1);
   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = static_cast<int64_t>(dil_getbool(&v1)) == 0;
}

void dilfe_umin(struct dilprg *p, class dilval *v)
{
   /* Unary minus */
   class dilval v1;

   eval_dil_exp(p, &v1);

   switch(dil_getval(&v1))
   {
      case DILV_INT:
         v->atyp    = DILA_NONE;
         v->type    = DILV_INT;
         v->val.num = -v1.val.num;
         break;
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }
}

void dilfe_itoa(struct dilprg *p, class dilval *v)
{
   /* Conversion of integers to strings */
   class dilval v1;

   eval_dil_exp(p, &v1);
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type    = DILV_SP;
         v->atyp    = DILA_EXP;
         v->val.ptr = str_dup(itoa(v1.val.num));
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }
}

void dilfe_atoi(struct dilprg *p, class dilval *v)
{
   /* Conversion of strings to integers */
   class dilval v1;

   eval_dil_exp(p, &v1);
   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL; /* failed */
         break;
      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            v->atyp    = DILA_NONE;
            v->type    = DILV_INT;
            v->val.num = atoi((char *)v1.val.ptr);
         }
         else
         {
            v->type = DILV_FAIL; /* failed */
         }
         break;
      default:
         v->type = DILV_ERR; /* Wrong type */
         return;
   }
}

void dilfe_len(struct dilprg *p, class dilval *v)
{
   /* length of strings or stringlists */
   class dilval v1;

   eval_dil_exp(p, &v1);
   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL; /* failed */
         break;
      case DILV_SP:
         v->atyp = DILA_NONE;
         v->type = DILV_INT;
         if(v1.val.ptr != nullptr)
         {
            v->val.num = strlen((char *)v1.val.ptr);
         }
         else
         {
            v->val.num = 0;
         }
         break;
      case DILV_SLP:
         v->atyp = DILA_NONE;
         v->type = DILV_INT;
         if(v1.val.ptr != nullptr)
         {
            v->val.num = ((cNamelist *)v1.val.ptr)->Length();
         }
         else
         {
            v->val.num = 0;
         }
         break;
      default:
         v->type = DILV_ERR; /* Wrong type */
         return;
   }
}

/* textformat */

void dilfe_txf(struct dilprg *p, class dilval *v)
{
   class dilval v1;

   eval_dil_exp(p, &v1); /* string */

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL; /* failed */
         break;

      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            char dest[4096];

            v->atyp = DILA_EXP;
            v->type = DILV_SP;
            str_escape_format((char *)v1.val.ptr, dest, sizeof(dest), TRUE);

            v->val.ptr = str_dup(dest);
         }
         else
         {
            v->type = DILV_FAIL; /* NULL string */
         }
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }
}

/* asctime */
void dilfe_ast(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   char        *c;

   eval_dil_exp(p, &v1); /* integer */

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL; /* failed */
         break;

      case DILV_INT:
         c = ctime((time_t *)&v1.val.num);
         assert(strlen(c) >= 1);

         v->atyp          = DILA_EXP;
         v->type          = DILV_SP;
         c[strlen(c) - 1] = 0;
         v->val.ptr       = str_dup(c);
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }
}

void dilfe_getw(struct dilprg *p, class dilval *v)
{
   /* Get first word of a string */
   class dilval v1;
   char        *c;
   char         buf1[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1); /* string */

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL; /* failed */
         break;
      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            v->atyp    = DILA_EXP;
            v->type    = DILV_SP;
            c          = str_next_word_copy((char *)v1.val.ptr, buf1);
            c          = skip_spaces(c);
            v->val.ptr = str_dup(buf1);

            if(v1.atyp == DILA_NORM && v1.type == DILV_SPR)
            {
               memmove(*(char **)v1.ref, c, strlen(c) + 1);
            }
            else if(p->sarg->arg == v1.val.ptr)
            {
               p->sarg->arg = c; /* Update argument as in findunit :) */
            }
         }
         else
         {
            v->type = DILV_FAIL; /* NULL string */
         }
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }
}

void dilfe_getws(struct dilprg *p, class dilval *v)
{
   /* Get first word of a string */
   class dilval v1;
   char        *tmp;
   char        *c;

   eval_dil_exp(p, &v1); /* string */

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL; /* failed */
         break;

      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            auto *words = new cNamelist;

            v->atyp = DILA_EXP;
            v->type = DILV_SLP;

            tmp = str_dup((char *)v1.val.ptr);
            for(c = strtok(tmp, " "); (c != nullptr) && (*c != 0); c = strtok(nullptr, " "))
            {
               words->AppendName(c);
            }
            free(tmp);

            v->val.ptr = words;
         }
         else
         {
            v->type = DILV_FAIL; /* NULL string */
         }
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }
}

void dilfe_load(struct dilprg *p, class dilval *v)
{
   /* Load a unit from database */
   class dilval v1;

   eval_dil_exp(p, &v1); /* unit name */

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            v->val.ptr = read_unit(str_to_file_index((char *)v1.val.ptr));
            if(v->val.ptr != nullptr)
            {
               if(IS_MONEY((unit_data *)v->val.ptr))
               {
                  set_money((unit_data *)v->val.ptr, MONEY_AMOUNT((unit_data *)v->val.ptr));
               }

               unit_to_unit((unit_data *)v->val.ptr, p->sarg->owner);
               v->atyp = DILA_NORM;
               v->type = DILV_UP;
            }
            else
            {
               v->type = DILV_NULL; /* Unit not found */
            }
         }
         else
         {
            v->type = DILV_FAIL; /* NULL string */
         }
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }
}

void dilfe_plus(struct dilprg *p, class dilval *v)
{
   /* Addition of strings or integers */

   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         return;

      case DILV_INT:
         v->type = DILV_INT;
         break;

      case DILV_SP:
         v->type = DILV_SP;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         return;

      case DILV_SP:
         if(v->type != DILV_SP)
         {
            v->type = DILV_ERR; /* wrong type */
         }
         break;

      case DILV_INT:
         if(v->type != DILV_INT)
         {
            v->type = DILV_ERR; /* wrong type */
         }
         break;

      case DILV_NULL:
         if(v->type != DILV_SP)
         {
            v->type = DILV_ERR; /* wrong type */
         }
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(v->type)
   {
      case DILV_SP:
         v->atyp    = DILA_EXP;
         v->val.ptr = str_dup(str_cc(STR((char *)v1.val.ptr), STR((char *)v2.val.ptr)));
         break;

      case DILV_INT:
         v->atyp    = DILA_NONE;
         v->val.num = v1.val.num + v2.val.num;
         break;
   }
}

void dilfe_dld(struct dilprg *p, class dilval *v)
{
   /* Destruction of DIL programs */

   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1); /* string  */
   eval_dil_exp(p, &v2); /* unitptr */

   v->type = DILV_INT;

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         return;

      case DILV_SP:
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v2))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         return;

      case DILV_UP:
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if(v->type == DILV_INT)
   {
      if((v1.val.ptr != nullptr) && (v2.val.ptr != nullptr))
      {
         v->atyp    = DILA_NONE;
         v->val.num = dil_destroy((char *)v1.val.ptr, (unit_data *)v2.val.ptr);
      }
   }
}

void dilfe_dlf(struct dilprg *p, class dilval *v)
{
   /* Detection of DIL programs (TRUE/FALSE) */

   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         return;

      case DILV_UP:
         v->type = DILV_INT;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         return;

      case DILV_SP:
         v->type = DILV_INT; /* wrong type */
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if((v1.val.ptr != nullptr) && (v2.val.ptr != nullptr))
   {
      v->atyp = DILA_NONE;
      if(dil_find((char *)v1.val.ptr, (unit_data *)v2.val.ptr) != nullptr)
      {
         v->val.num = TRUE;
      }
      else
      {
         v->val.num = FALSE;
      }
   }
}

void dilfe_min(struct dilprg *p, class dilval *v)
{
   /* Subtraction of integers */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_INT:
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = v1.val.num - v2.val.num;
   }
}

void dilfe_mul(struct dilprg *p, class dilval *v)
{
   /* Multiplication of integers */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = v1.val.num * v2.val.num;
   }
}

void dilfe_div(struct dilprg *p, class dilval *v)
{
   /* Division of integers */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if(v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      if(v2.val.num == 0)
      {
         v->type = DILV_FAIL;
      }
      else
      {
         v->val.num = v1.val.num / v2.val.num;
      }
   }
}

void dilfe_mod(struct dilprg *p, class dilval *v)
{
   /* Modulo of integers */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if(v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      if(v2.val.num == 0)
      {
         v->type = DILV_NULL;
      }
      else
      {
         v->val.num = v1.val.num % v2.val.num;
      }
   }
}

void dilfe_and(struct dilprg *p, class dilval *v)
{
   /* And two integers (or booleans) */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   v->atyp = DILA_NONE;

   if((dil_getval(&v1) == DILV_INT) && (dil_getval(&v2) == DILV_INT))
   {
      v->val.num = v1.val.num & v2.val.num;
   }
   else
   {
      v->type = DILV_FAIL;
   }
}

void dilfe_land(struct dilprg *p, class dilval *v)
{
   /* And two integers (or booleans) */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = (static_cast<int64_t>(dil_getbool(&v1) != 0) && (dil_getbool(&v2)) != 0);
}

void dilfe_or(struct dilprg *p, class dilval *v)
{
   /* Or two integers (or booleans) */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   v->atyp = DILA_NONE;

   if((dil_getval(&v1) == DILV_INT) && (dil_getval(&v2) == DILV_INT))
   {
      v->val.num = v1.val.num | v2.val.num;
   }
   else
   {
      v->type = DILV_FAIL;
   }
}

void dilfe_lor(struct dilprg *p, class dilval *v)
{
   /* Or two integers (or booleans) */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = (static_cast<int64_t>(dil_getbool(&v1) != 0) || (dil_getbool(&v2)) != 0);
}

void dilfe_isa(struct dilprg *p, class dilval *v)
{
   /* Test if unit is affected by affect */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v1.val.ptr != nullptr)
         {
            v->type = DILV_INT;
         }
         else
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(affected_by_spell((unit_data *)v1.val.ptr, v2.val.num) != nullptr);
   }
}

void dilfe_rnd(struct dilprg *p, class dilval *v)
{
   /* Random in an integer range */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = number(v1.val.num, v2.val.num);
   }
}

void dilfe_fndr(struct dilprg *p, class dilval *v)
{
   /* Find a room */
   class dilval v1;
   char         buf1[MAX_STRING_LENGTH];
   char         buf2[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1);
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         v->atyp = DILA_NORM;
         v->type = DILV_UP;
         *buf1   = '\0';
         *buf2   = '\0';
         if(v1.val.ptr != nullptr)
         {
            split_fi_ref((char *)v1.val.ptr, buf1, buf2);
            v->val.ptr = world_room(buf1, buf2);
            if(v->val.ptr == nullptr)
            {
               v->type = DILV_NULL; /* not found */
            }
         }
         else
         {
            v->type = DILV_NULL; /* not found */
         }

         break;
      case DILV_NULL:
         v->type = DILV_NULL; /* not found */
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }
}

void dilfe_fnds2(struct dilprg *p, class dilval *v)
{
   /* Find a symbolic unit */
   class dilval v1;
   class dilval v2;
   class dilval v3;

   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_UP:
         if(v1.val.ptr != nullptr)
         {
            v->type = DILV_UP;
         }
         else
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_NULL:
         v->type = DILV_NULL; /* not found */
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_SP:
         if(v2.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_NULL:
         v->type = DILV_NULL; /* not found */
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_INT:
         break;

      case DILV_NULL:
         v->type = DILV_NULL; /* not found */
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_UP)
   {
      v->atyp = DILA_NORM;
      *buf1   = '\0';
      *buf2   = '\0';
      split_fi_ref((char *)v2.val.ptr, buf1, buf2);
      v->val.ptr = find_symbolic_instance_ref((unit_data *)v1.val.ptr, find_file_index(buf1, buf2), v3.val.num);
      if(v->val.ptr == nullptr)
      {
         v->type = DILV_NULL; /* not found */
      }
   }
}

void dilfe_fnds(struct dilprg *p, class dilval *v)
{
   /* Find a symbolic unit */
   class dilval v1;
   char         buf1[MAX_STRING_LENGTH];
   char         buf2[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1);
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_SP:
         v->atyp = DILA_NORM;
         v->type = DILV_UP;
         *buf1   = '\0';
         *buf2   = '\0';
         split_fi_ref((char *)v1.val.ptr, buf1, buf2);
         v->val.ptr = find_symbolic(buf1, buf2);
         if(v->val.ptr == nullptr)
         {
            v->type = DILV_NULL; /* not found */
         }
         break;
      case DILV_NULL:
         v->type = DILV_NULL; /* not found */
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }
}

void dilfe_gt(struct dilprg *p, class dilval *v)
{
   /* Greater Than operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.num > v2.val.num);
   }
}

void dilfe_lt(struct dilprg *p, class dilval *v)
{
   /* Less Than operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.num < v2.val.num);
   }
}

void dilfe_ge(struct dilprg *p, class dilval *v)
{
   /* Greater or Equal operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.num >= v2.val.num);
   }
}

void dilfe_le(struct dilprg *p, class dilval *v)
{
   /* Less or Equal operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.num <= v2.val.num);
   }
}

void dilfe_eq(struct dilprg *p, class dilval *v)
{
   /* Equal operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.num == v2.val.num);
   }
}

void dilfe_se(struct dilprg *p, class dilval *v)
{
   /* String equal operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         v->type = DILV_INT;
         break;
      case DILV_NULL:
         v->type = DILV_NULL; /* null pointer */
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_SP:
         break;
      case DILV_NULL:
         if((v->type != DILV_ERR) && (v->type != DILV_FAIL))
         {
            v->type = DILV_NULL; /* null pointer */
         }
         break;
      default:
         v->type = DILV_ERR;
         break;
   }

   switch(v->type)
   {
      case DILV_INT:
         v->atyp = DILA_NONE;
         if((str_is_empty((char *)v1.val.ptr) != 0u) || (str_is_empty((char *)v2.val.ptr) != 0u))
         {
            v->val.num = static_cast<int64_t>((str_is_empty((char *)v1.val.ptr) != 0u) && (str_is_empty((char *)v2.val.ptr) != 0u));
         }
         else
         {
            v->val.num = static_cast<int64_t>(str_ccmp((char *)v1.val.ptr, (char *)v2.val.ptr)) == 0;
         }
         break;
      case DILV_NULL:
         v->atyp    = DILA_NONE;
         v->type    = DILV_INT;
         v->val.num = FALSE;
         break;
   }
}

void dilfe_sne(struct dilprg *p, class dilval *v)
{
   /* String not equal operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_NULL:
      case DILV_SP:
         v->type = DILV_INT;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_NULL:
      case DILV_SP:
         break;

      default:
         v->type = DILV_ERR;
         break;
   }

   switch(v->type)
   {
      case DILV_INT:
         v->atyp = DILA_NONE;
         if((str_is_empty((char *)v1.val.ptr) != 0u) || (str_is_empty((char *)v2.val.ptr) != 0u))
         {
            v->val.num = static_cast<int64_t>((str_is_empty((char *)v1.val.ptr) == 0u) || (str_is_empty((char *)v2.val.ptr) == 0u));
         }
         else
         {
            v->val.num = static_cast<int64_t>(str_ccmp((char *)v1.val.ptr, (char *)v2.val.ptr) != 0);
         }
         break;
   }
}

void dilfe_pe(struct dilprg *p, class dilval *v)
{
   /* Pointer Equality operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_NULL:
      case DILV_SLP:
      case DILV_EDP:
      case DILV_UP:
      case DILV_SP:
         v->type = DILV_INT;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_NULL:
      case DILV_SP:
      case DILV_SLP:
      case DILV_EDP:
      case DILV_UP:
         v->type = DILV_INT;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.ptr == v2.val.ptr);
   }
}

void dilfe_pne(struct dilprg *p, class dilval *v)
{
   /* Pointer Equality operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_NULL:
      case DILV_SLP:
      case DILV_EDP:
      case DILV_UP:
      case DILV_SP:
         v->type = DILV_INT;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_NULL:
      case DILV_SP:
      case DILV_SLP:
      case DILV_EDP:
      case DILV_UP:
         v->type = DILV_INT;
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         return;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.ptr != v2.val.ptr);
   }
}

void dilfe_ne(struct dilprg *p, class dilval *v)
{
   /* Not Equal operator */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = static_cast<int64_t>(v1.val.num != v2.val.num);
   }
}

void dilfe_iss(struct dilprg *p, class dilval *v)
{
   /* Test if bits is set in bitfield */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_INT:
         v->type = DILV_INT;
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_INT)
   {
      v->atyp    = DILA_NONE;
      v->val.num = IS_SET(v1.val.num, v2.val.num);
   }
}

void dilfe_in(struct dilprg *p, class dilval *v)
{
   /* Test if string in string, stringlist or extra description */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_SLP:
         if(v2.val.ptr != nullptr)
         {
            v->type = DILV_SLP;
         }
         else
         {
            v->type = DILV_NULL; /* not found */
         }
         break;

      case DILV_EDP:
         if(v2.val.ptr != nullptr)
         {
            v->type = DILV_EDP;
         }
         else
         {
            v->type = DILV_NULL; /* not found */
         }
         break;

      case DILV_SP:
         if(v2.val.ptr != nullptr)
         {
            v->type = DILV_INT;
         }
         else
         {
            v->type = DILV_NULL; /* not found */
         }
         break;

      case DILV_NULL:
         v->type = DILV_NULL; /* not found */
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            break;
         }
         else
         {
            v->type = DILV_FAIL;
         }

      case DILV_NULL:
         v->type    = DILV_NULL;
         v->val.num = 0;
         break;

      default:
         v->type = DILV_ERR;
         return;
   }

   switch(v->type)
   {
      case DILV_SLP:
         v->atyp    = DILA_NONE;
         v->type    = DILV_INT;
         v->val.num = 1 + (((cNamelist *)v2.val.ptr)->IsNameIdx(skip_spaces((char *)v1.val.ptr)));
         break;

      case DILV_EDP:
         v->atyp    = DILA_NORM;
         v->type    = DILV_EDP;
         v->val.ptr = ((class extra_descr_data *)v2.val.ptr)->find_raw(skip_spaces((char *)v1.val.ptr));
         break;

      case DILV_INT:
         v->atyp    = DILA_NONE;
         v->type    = DILV_INT;
         v->val.num = static_cast<int64_t>(str_cstr((char *)v2.val.ptr, (char *)v1.val.ptr) != nullptr);
         break;
   }
}

void dilfe_fndu(struct dilprg *p, class dilval *v)
{
   /* Find a unit */
   class dilval v1;
   class dilval v2;
   class dilval v3;
   class dilval v4;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;

      case DILV_UP:
         if(v1.val.ptr != nullptr)
         {
            v->type = DILV_UP;
         }
         else
         {
            v->type = DILV_NULL; /* not applicable UP */
         }
         break;

      case DILV_NULL:
         v->type = DILV_NULL;
         break;

      default:
         v->type = DILV_ERR;
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_SP:
         if(v->type != DILV_ERR)
         {
            if(v2.val.ptr == nullptr)
            {
               v->type = DILV_NULL; /* not applicable SP */
            }
            else if(*((char *)v2.val.ptr) == 0)
            {
               v->type = DILV_NULL; /* not applicable SP */
            }
         }
         break;

      case DILV_NULL:
         if((v->type != DILV_ERR) && (v->type != DILV_FAIL))
         {
            v->type = DILV_NULL; /* not applicable */
         }
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_INT:
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v4))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_UP:
         break;

      case DILV_NULL: /* null pointer */
         if((v->type != DILV_ERR) && (v->type != DILV_FAIL))
         {
            v4.val.ptr = nullptr;
         }
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if((v->type == DILV_UP) && IS_CHAR((unit_data *)v1.val.ptr))
   {
      char *c = (char *)v2.val.ptr;

      v->atyp    = DILA_NORM;
      v->val.ptr = find_unit((unit_data *)v1.val.ptr, &c, (unit_data *)v4.val.ptr, v3.val.num);

      if(v2.atyp == DILA_NORM && v2.type == DILV_SPR)
      {
         memmove(v2.val.ptr, c, strlen(c) + 1);
      }
      else if(p->sarg->arg == v2.val.ptr)
      {
         p->sarg->arg = c; /* Update argument if argument is argument! :) */
      }

      if(v->val.ptr == nullptr)
      {
         v->type = DILV_NULL;
      }
   }
   else
   {
      v->type = DILV_FAIL;
   }
}

void dilfe_fndru(struct dilprg *p, class dilval *v)
{
   /* Find a unit */
   class dilval v1;
   class dilval v2;
   class dilval v3;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v1.val.ptr != nullptr)
         {
            v->type = DILV_UP;
         }
         else
         {
            v->type = DILV_NULL; /* not applicable UP */
         }
         break;
      case DILV_NULL:
         v->type = DILV_NULL;
         break;
      default:
         v->type = DILV_ERR;
         break;
   }

   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;

      case DILV_NULL:
         if((v->type != DILV_ERR) && (v->type != DILV_FAIL))
         {
            v->type = DILV_NULL; /* not applicable */
         }
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   switch(dil_getval(&v3))
   {
      case DILV_FAIL:
         if(v->type != DILV_ERR)
         {
            v->type = DILV_FAIL;
         }
         break;
      case DILV_INT:
         break;
      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }

   if(v->type == DILV_UP)
   {
      v->atyp    = DILA_NORM;
      v->val.ptr = random_unit((unit_data *)v1.val.ptr, v2.val.num, v3.val.num);

      if(v->val.ptr == nullptr)
      {
         v->type = DILV_NULL;
      }
   }
   else
   {
      v->type = DILV_FAIL;
   }
}

void dilfe_fs(struct dilprg *p, class dilval *v)
{
   /* A Fixed String */
   v->type = DILV_SP;
   v->atyp = DILA_NORM;

   /* Reference directly into the core */
   /* assumes bwrite_string is same format */
   /* saves LOTS of time! */
   v->val.ptr = p->sp->pc;

   /* just skip single string */
   TAIL(p->sp->pc);

   (p->sp->pc)++; /* ready for next */
}

void dilfe_fsl(struct dilprg *p, class dilval *v)
{
   auto *namelist = new cNamelist;
   /* A Fixed String list */

   v->atyp = DILA_EXP; /* no other way, copy namelist from core */
   v->type = DILV_SLP;

   namelist->bread(&(p->sp->pc));
   v->val.ptr = namelist;
}

void dilfe_var(struct dilprg *p, class dilval *v)
{
   /* A variable */
   int varno;

   varno = bread_uint16_t(&(p->sp->pc));

   switch(p->sp->vars[varno].type)
   {
      /* normal variable */
      case DILV_UP: /* unit pointer */
         v->atyp = DILA_NORM;
         v->type = DILV_UPR;
         v->ref  = &(p->sp->vars[varno].val.unitptr);
         break;
      case DILV_SP: /* string pointer */
         v->atyp = DILA_NORM;
         v->type = DILV_SPR;
         v->ref  = &(p->sp->vars[varno].val.string);
         break;
      case DILV_SLP: /* string list pointer */
         v->atyp = DILA_NORM;
         v->type = DILV_SLPR;
         v->ref  = p->sp->vars[varno].val.namelist;
         break;
      case DILV_EDP: /* extra description pointer */
         v->atyp = DILA_NORM;
         v->type = DILV_EDPR;
         v->ref  = &(p->sp->vars[varno].val.extraptr);
         break;
      case DILV_INT: /* integer */
         v->atyp = DILA_NONE;
         v->type = DILV_SINT4R;
         v->ref  = &(p->sp->vars[varno].val.integer);
         break;
      default:
         assert(FALSE);
   }
}

void dilfe_weat(struct dilprg *p, class dilval *v)
{
   /* Self */

   v->type    = DILV_INT;
   v->atyp    = DILA_NORM;
   v->val.num = unit_zone(p->sarg->owner)->weather.sky;
}

void dilfe_self(struct dilprg *p, class dilval *v)
{
   /* Self */

   v->type    = DILV_UP;
   v->atyp    = DILA_NORM;
   v->val.ptr = p->sarg->owner;
}

void dilfe_hrt(struct dilprg *p, class dilval *v)
{
   /* Heartbeat */

   v->type = DILV_SINT2R;
   v->atyp = DILA_NONE;
   v->ref  = (int16_t *)&(p->sarg->fptr->heart_beat);
}

void dilfe_tho(struct dilprg *p, class dilval *v)
{
   /* MudHour */

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = mud_date(time(nullptr)).hours;
}

void dilfe_tda(struct dilprg *p, class dilval *v)
{
   /* MudDay */

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = mud_date(time(nullptr)).day;
}

void dilfe_tmd(struct dilprg *p, class dilval *v)
{
   /* MudMonth */

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = mud_date(time(nullptr)).month;
}

void dilfe_tye(struct dilprg *p, class dilval *v)
{
   /* MudYear */

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = mud_date(time(nullptr)).year;
}

void dilfe_rti(struct dilprg *p, class dilval *v)
{
   /* RealTime */

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = time(nullptr);
}

void dilfe_acti(struct dilprg *p, class dilval *v)
{
   /* Activator */

   v->type    = DILV_UP;
   v->atyp    = DILA_NORM;
   v->val.ptr = p->sarg->activator;
}

void dilfe_medi(struct dilprg *p, class dilval *v)
{
   /* Medium */

   v->type    = DILV_UP;
   v->atyp    = DILA_NORM;
   v->val.ptr = p->sarg->medium;
}

void dilfe_targ(struct dilprg *p, class dilval *v)
{
   /* Target */

   v->type    = DILV_UP;
   v->atyp    = DILA_NORM;
   v->val.ptr = p->sarg->target;
}

void dilfe_powe(struct dilprg *p, class dilval *v)
{
   /* Power */

   static int dummy = 0;

   v->type = DILV_SINT4R;
   v->atyp = DILA_NONE;

   if(p->sarg->pInt != nullptr)
   {
      v->ref = (int32_t *)p->sarg->pInt;
   }
   else
   {
      v->ref = (int *)&dummy;
   }
}

void dilfe_cmst(struct dilprg *p, class dilval *v)
{
   /* cmdstr */

   v->type = DILV_SP;
   v->atyp = DILA_NORM;

   if(p->sarg->cmd->cmd_str != nullptr)
   {
      v->val.ptr = (char *)p->sarg->cmd->cmd_str;
   }
   else
   {
      v->type    = DILV_NULL;
      v->val.ptr = nullptr;
   }
}

void dilfe_argm(struct dilprg *p, class dilval *v)
{
   /* Argument */

   v->type = DILV_SP;
   v->atyp = DILA_NORM;
   if(p->sarg->arg != nullptr)
   {
      v->val.ptr = (char *)p->sarg->arg;
   }
   else
   {
      v->type    = DILV_NULL;
      v->val.ptr = nullptr;
   }
}

void dilfe_null(struct dilprg *p, class dilval *v)
{
   /* Pointer value null */

   v->type    = DILV_NULL;
   v->atyp    = DILA_NONE;
   v->val.ptr = nullptr;
}

void dilfe_int(struct dilprg *p, class dilval *v)
{
   /* Fixed integer */

   v->type    = DILV_INT;
   v->atyp    = DILA_NONE;
   v->val.num = (int32_t)bread_uint32_t(&(p->sp->pc));
}

void dilfe_cmds(struct dilprg *p, class dilval *v)
{
   /* Check if the input command might the supplied argument */
   class dilval v1;

   eval_dil_exp(p, &v1);

   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_SP:
         if(v1.val.ptr != nullptr)
         {
            v->type    = DILV_INT;
            v->atyp    = DILA_NONE;
            v->val.num = is_command(p->sarg->cmd, (char *)v1.val.ptr);
         }
         else
         {
            v->type = DILV_FAIL;
         }
         break;

      case DILV_INT:
         v->type    = DILV_INT;
         v->val.num = static_cast<int64_t>(p->sarg->cmd->no == v1.val.num);
         break;

      default:
         v->type = DILV_ERR; /* wrong type */
         break;
   }
}

/* visible, some vs other */
void dilfe_pck(struct dilprg *p, class dilval *v)
{
   extern auto  pay_point_charlie(unit_data * ch, unit_data * to)->int; /* from act_movement.c */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1) eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   switch(dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if((v1.val.ptr == nullptr) || !IS_CHAR((unit_data *)v1.val.ptr))
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }
   switch(dil_getval(&v2))
   {
      case DILV_FAIL:
      case DILV_NULL:
         v->type = DILV_FAIL;
         break;
      case DILV_UP:
         if(v2.val.ptr == nullptr)
         {
            v->type = DILV_FAIL;
         }
         break;
      default:
         v->type = DILV_ERR;
         return;
   }

   if(v->type == DILV_INT)
   {
      v->val.num = pay_point_charlie((unit_data *)v1.val.ptr, (unit_data *)v2.val.ptr);
   }
}

void dilfe_act(struct dilprg *p, class dilval *v)
{
   char buf[1024];
   /* Conversion of integers to strings */
   class dilval v1;
   class dilval v2;
   class dilval v3;
   class dilval v4;
   class dilval v5;
   class dilval v6;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);
   eval_dil_exp(p, &v5);
   eval_dil_exp(p, &v6);

   v->type = DILV_FAIL;

   if(dil_type_check("act", p, 6, &v1, TYPEFAIL_NULL, 1, DILV_SP, &v2, TYPEFAIL_NULL, 1, DILV_INT, &v3, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP,
                     DILV_NULL, &v4, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL, &v5, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL, &v6,
                     TYPEFAIL_NULL, 1, DILV_INT) < 0)
   {
      return;
   }

   switch(v6.val.num)
   {
      case TO_CHAR:
      case TO_ROOM:
      case TO_ALL:
      case TO_REST:
         /* these require 1st argument */
         if(v3.val.ptr != nullptr)
         {
            act_generate(buf, (char *)v1.val.ptr, v2.val.num, v3.val.ptr, v4.val.ptr, v5.val.ptr, v6.val.num, (unit_data *)v3.val.ptr);
         }
         v->type    = DILV_SP;
         v->atyp    = DILA_EXP;
         v->val.ptr = str_dup(buf);
         break;

      case TO_VICT:
      case TO_NOTVICT:
         if(v5.val.ptr != nullptr)
         {
            act_generate(buf, (char *)v1.val.ptr, v2.val.num, v3.val.ptr, v4.val.ptr, v5.val.ptr, v6.val.num, (unit_data *)v5.val.ptr);
         }
         v->type    = DILV_SP;
         v->atyp    = DILA_EXP;
         v->val.ptr = str_dup(buf);
         break;
   }
}
