#pragma once

#include "graph_vertice.h"
class hob
{
public:
   int             no;    /* No of entries in the Hob       */
   int             d;     /* d as in d-Hob (2...X)          */
   graph_vertice **array; /* Array of pointers to vertices  */
};
