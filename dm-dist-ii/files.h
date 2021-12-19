#pragma once
#include "bytestring.h"
#include "CByteBuffer.h"
#define FCACHE_MAX 100

void fstrcpy(CByteBuffer *pBuf, FILE *f);
auto fread_line_commented(FILE *fl, char *buf, int max) -> char *;
auto fsize(FILE *f) -> long;
void touch_file(char *name);
auto fread_string(FILE *fl) -> char *;
auto fread_string_copy(FILE *fl, char *buf, int max) -> char *;
auto file_to_string(char *name, char *buf, int max_len) -> int;
auto config_file_to_string(char *name, char *buf, int max_len) -> int;
auto fread_num(FILE *fl) -> long;
auto fopen_cache(char *name, const char *mode) -> FILE *;
void fclose_cache();
auto file_exists(const char *name) -> bool;
