#pragma once
/* *********************************************************************** *
 * File   : extra.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Extra descriptions                                             *
 * Bugs   : None known.                                                    *
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
#include "essential.h"
#include "namelist.h"

class extra_descr_data
{
public:
   extra_descr_data();
   ~extra_descr_data();

   void AppendBuffer(CByteBuffer *pBuf);

   auto find_raw(const char *name) -> class extra_descr_data *;
   auto add(const char *name, const char *descr) -> class extra_descr_data *;
   auto add(const char **names, const char *descr) -> class extra_descr_data *;
   auto remove(class extra_descr_data *exd) -> class extra_descr_data *;
   auto remove(const char *name) -> class extra_descr_data *;

   void free_list();

   cNamelist         names; /* Keyword in look/examine          */
   cStringInstance   descr; /* What to see                      */
   extra_descr_data *next;  /* Next in list                     */
};

auto unit_find_extra_string(unit_data *ch, char *word, unit_data *list) -> const char *;
auto char_unit_find_extra(unit_data *ch, unit_data **target, char *word, unit_data *list) -> extra_descr_data *;
auto unit_find_extra(const char *word, unit_data *unit) -> extra_descr_data *;
