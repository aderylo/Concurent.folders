#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "Tree.h"
#include "HashMap.h"

struct Tree
{
    char *name;
    HashMap *sub_trees;
};

Tree *tree_new()
{
    Tree *tree = malloc(sizeof(Tree));
    if (!tree)
        return NULL;

    tree->sub_trees = hmap_new();
    tree->name = malloc(2 * sizeof(char));
    strcpy(tree->name, "/");
    return tree;
}

void tree_free(Tree *tree)
{

    HashMapIterator it = hmap_iterator(tree->sub_trees);
    const char *key;
    Tree *node = NULL;
    while (hmap_next(tree->sub_trees, &it, &key, &node))
    {
        tree_free(node);
    }

    hmap_free(tree->sub_trees);
    free(tree->name);
    free(tree);
}
