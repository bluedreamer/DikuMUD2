#pragma once

#include <cstdint>
/*
 *  An external reference.
 *  For each external reference, the name and cooresponding
 *  argument and return types will be saved
 */
struct dilxref
{
   char    *name; /* func/proc name [@ zone] */
   uint8_t  rtnt; /* return type */
   uint8_t  argc; /* number of arguments (min 1) */
   uint8_t *argt; /* argument types */
};
