#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Tree.h"
#include "HashMap.h"
#include "path_utils.h"
#include "err.h"

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
    Tree *t = NULL;
    void *node = &t;
    while (hmap_next(tree->sub_trees, &it, &key, node))
    {
        tree_free(t);
        t = NULL;
    }

    hmap_free(tree->sub_trees);
    free(tree->name);
    free(tree);
}

int tree_create(Tree *tree, const char *path)
{
    if (!is_path_valid(path))
        syserr("ENOENT");
    Tree *parent = tree;
    Tree *child = NULL;
    char component[MAX_FOLDER_NAME_LENGTH + 1];
    const char *subpath = path;
    while ((subpath = split_path(subpath, component)))
    {
        child = hmap_get(parent->sub_trees, component);
        if (child == NULL)
        {
            child = malloc(sizeof(Tree));
            child->name = calloc(256, sizeof(char));
            strcpy(child->name, component);
            child->sub_trees = hmap_new();
            hmap_insert(parent->sub_trees, component, child);
        }
        else if (strcmp(subpath, "/") == 0)
        {
            syserr("EEXIST");
        }
        parent = child;
        child = NULL;
    }

    return 0;
}
