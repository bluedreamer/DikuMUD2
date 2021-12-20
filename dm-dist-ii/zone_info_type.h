#pragma once
#include "bin_search_type.h"
#include "zone_type.h"

class zone_info_type
{
public:
   int              no_of_zones; /* Total number of zones       */
   zone_type       *zone_list;   /* The Linked List of Zones    */
   bin_search_type *ba;          /* Array of structs for search */
   void           **spmatrix;    /* Inter zone shortest paths   */
};

extern zone_info_type zone_info;
