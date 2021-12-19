#pragma once
auto parse_match(char *pData, char *pMatch) -> char *;
auto parse_name(char **pData) -> char *;
auto parse_numlist(char **pData, int *count) -> int *;
auto parse_num(char **pData, int *pNum) -> int;
auto parse_namelist(char **pData) -> char **;
auto parse_match_num(char **pData, const char *pMatch, int *pResult) -> int;
auto parse_match_numlist(char **pData, const char *pMatch, int *count) -> int *;
auto parse_match_name(char **pData, const char *pMatch) -> char *;
auto parse_match_namelist(char **pData, const char *pMatch) -> char **;
