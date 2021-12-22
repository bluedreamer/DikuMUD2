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

void event_enq(int when, void (*func)(void *, void *), void *arg1, void *arg2);
void event_deenq(void (*func)(void *, void *), void *arg1, void *arg2);
void event_deenq_relaxed(void (*func)(void *, void *), void *arg1, void *arg2);

#endif
