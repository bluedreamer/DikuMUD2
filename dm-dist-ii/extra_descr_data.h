#pragma once

#include "CByteBuffer.h"
#include "cNamelist.h"

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
