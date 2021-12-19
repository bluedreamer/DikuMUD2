#pragma once

#include "blk_file_type.h"
#include "essential.h"

#include <cstdio>

#define BLK_NULL -3 /* Use this constant to locally mark when a handle  is free */
using BLK_FILE = struct blk_file_type;

void blk_delete(BLK_FILE *bf, blk_handle index);
auto blk_read(BLK_FILE *bf, blk_handle index, blk_length *blen) -> void *;
auto blk_read_reserved(BLK_FILE *bf, blk_length *blen) -> void *;
auto blk_write(BLK_FILE *bf, const void *data, blk_length len) -> blk_handle;
void blk_write_reserved(BLK_FILE *bf, const void *data, blk_length len);
auto blk_open(const char *name, blk_length block_size) -> BLK_FILE *;
void blk_close(BLK_FILE *bf);
