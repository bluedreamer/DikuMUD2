/* *********************************************************************** *
 * File   : dijkstra.h                                Part of Valhalla MUD *
 * Version: 1.42                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Shortest-path info.                                            *
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

#ifndef _MUD_DIJKSTRA_H
#define _MUD_DIJKSTRA_H

auto npc_move(const unit_data *npc, const unit_data *to) -> int;
auto move_to(const unit_data *from, const unit_data *to) -> int;

#endif
