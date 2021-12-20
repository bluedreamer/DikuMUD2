#pragma once

class damage_chart_element_type
{
public:
   int offset;  /* When does damage start         */
   int basedam; /* The damage given at 'offset'   */
   int alpha;   /* Step size of damage as 1/alpha */
};
