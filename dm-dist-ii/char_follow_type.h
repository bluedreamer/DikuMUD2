#pragma once

class unit_data;

struct char_follow_type
{
   unit_data        *follower; /* Must be a char */
   char_follow_type *next;
};
