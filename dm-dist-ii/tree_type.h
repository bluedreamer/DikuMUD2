#pragma once

#include "values.h"

#include <cstdint>

#define TREE_PARENT(tree, node)      (tree[node].parent)
#define TREE_GRANDPARENT(tree, node) (tree[tree[node].parent].parent)
#define TREE_ISROOT(tree, node)      ((node) == (tree[node].parent))
#define TREE_ISLEAF(tree, node)      (tree[node].isleaf)

class tree_type
{
public:
   int     parent;
   uint8_t isleaf;
};

extern tree_type abil_tree[ABIL_TREE_MAX + 1];
extern tree_type ski_tree[SKI_TREE_MAX + 1];
extern tree_type spl_tree[SPL_TREE_MAX + 1];
extern tree_type wpn_tree[WPN_TREE_MAX + 1];
