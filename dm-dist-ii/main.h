/* *********************************************************************** *
 * File   : main.h                                    Part of Valhalla MUD *
 * Version: 1.05                                                           *
 * Author : seifert@diku.dk and quinn@freja.diku.dk                        *
 *                                                                         *
 * Purpose: Header for the basis stuff.                                    *
 *                                                                         *
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

#ifndef _MUD_MAIN_H
#define _MUD_MAIN_H

#include <functional>

void event_enq(int when, std::function<void(void*,void*)> func, void *arg1, void *arg2);
void event_deenq(std::function<void(void*,void*)> func, void *arg1, void *arg2);
void event_deenq_relaxed(std::function<void(void*,void*)> func, void *arg1, void *arg2);

#endif
