#pragma once
#include "bytestring.h"
#include "dil.h"
#include "structs.h"

extern CByteBuffer g_FileBuffer; /* Defined in db_file.c */
extern int         g_nCorrupt;   /*          "           */

auto bread_dil(CByteBuffer *pBuf, unit_data *, uint8_t version, unit_fptr *fptr) -> void *;

auto bread_diltemplate(CByteBuffer *pBuf) -> diltemplate *;
auto bread_extra(CByteBuffer *pBuf, class extra_descr_data **ppExtra) -> int;

auto bread_swap(CByteBuffer *pBuf, unit_data *u) -> int;
auto bread_swap_skip(CByteBuffer *pBuf) -> int;
auto bread_affect(CByteBuffer *pBuf, unit_data *u, uint8_t nVersion) -> int;

auto bread_func(CByteBuffer *pBuf, uint8_t version, unit_data *owner) -> unit_fptr *;

void bread_block(FILE *datafile, long file_pos, int length, void *buffer);

void bwrite_extra_descr(CByteBuffer *pBuf, extra_descr_data *e);
void bwrite_swap(CByteBuffer *pBuf, unit_data *u);
void bwrite_affect(CByteBuffer *pBuf, unit_affected_type *af, uint8_t version);
void bwrite_func(CByteBuffer *pBuf, unit_fptr *fptr);
void bwrite_block(FILE *f, int length, void *buffer);
void bwrite_dil(CByteBuffer *pBuf, dilprg *prg);
void bwrite_diltemplate(CByteBuffer *pBuf, diltemplate *tmpl);

void write_unit(FILE *f, unit_data *u, char *fname);
void write_diltemplate(FILE *f, diltemplate *tmpl);
auto write_unit_string(CByteBuffer *pBuf, unit_data *u) -> int;
