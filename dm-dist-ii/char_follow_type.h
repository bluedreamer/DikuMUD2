#pragma once

class unit_data;

class char_follow_type
{
public:
   unit_data        *follower; /* Must be a char */
   char_follow_type *next;
};
