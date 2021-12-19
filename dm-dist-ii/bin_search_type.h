#pragma once

/* This must be maintained as an array for use with binary search methods */
struct bin_search_type
{
   const char *compare; /* Points to the comparison string  */
   void       *block;   /* Points to the relevant block     */
};
