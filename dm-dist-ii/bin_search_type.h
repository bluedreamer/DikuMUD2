#pragma once

/* This must be maintained as an array for use with binary search methods */
class bin_search_type
{
public:
   const char *compare; /* Points to the comparison string  */
   void       *block;   /* Points to the relevant block     */
};
