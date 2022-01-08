#include <errno.h>
#include <string.h>

#include "Tree.h"

struct Tree
{
    char *name;
};

Tree *tree_new()
{
    Tree *tree = malloc(sizeof(Tree));
    if (!tree)
        return NULL;

    tree->name = malloc(2 * sizeof(char));
    strcpy(tree->name, "/");
    return tree;
}

void tree_free(Tree *tree)
{
    free(tree->name);
    free(tree);
}
