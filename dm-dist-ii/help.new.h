#pragma once
/* *********************************************************************** *
 * File   : help.h                                    Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for the help stuff.                                     *
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

class help_index_type
{
public:
   help_index_type();
   ~help_index_type();

   char *keyword;
   long  pos;
};

class help_file_type
{
public:
   help_file_type();
   ~help_file_type();

   void generate_help_idx(const char *filename);

   struct help_index_type *help_idx;       /* the help keyword list */
   int                     elements, size; /* info about the list   */
   char                   *filename;
};
