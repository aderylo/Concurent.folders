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

char *tree_list(Tree *tree, const char *path)
{
    if (!is_path_valid(path))
        return NULL;

    Tree *parent = tree;
    Tree *child = NULL;
    char component[MAX_FOLDER_NAME_LENGTH + 1];
    const char *subpath = path;
    if (strcmp(subpath, "/") == 0)
    {
        return make_map_contents_string(parent->sub_trees);
    }
    else
    {
        while ((subpath = split_path(subpath, component)))
        {
            child = hmap_get(parent->sub_trees, component);
            if (child == NULL)
                return NULL;

            if (strcmp(subpath, "/") == 0)
            {
                return make_map_contents_string(child->sub_trees);
            }

            parent = child;
            child = NULL;
        }
    }

    return NULL;
}
// add enoet if creating more than one folder at once
int tree_create(Tree *tree, const char *path)
{
    if (!is_path_valid(path))
        syserr("EINVAL");

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

int tree_remove(Tree *tree, const char *path)
{
    if (!is_path_valid(path))
        syserr("EINVAL");

    if (strcmp(path, "/") == 0)
        syserr("EBUSY");

    Tree *parent = tree;
    Tree *child = NULL;
    char component[MAX_FOLDER_NAME_LENGTH + 1];
    const char *subpath = path;
    while ((subpath = split_path(subpath, component)))
    {
        child = hmap_get(parent->sub_trees, component);
        if (child == NULL)
            syserr("ENOENT");

        if (strcmp(subpath, "/") == 0)
        {
            if (hmap_size(child->sub_trees) > 0)
                syserr("ENOTEMPTY");
            else
            {
                hmap_remove(parent->sub_trees, component);
                tree_free(child);
                child = NULL;
                break;
            }
        }
        parent = child;
        child = NULL;
    }

    return 0;
}

Tree *tree_detach(Tree *tree, const char *path)
{
    if (!is_path_valid(path))
        syserr("EINVAL");

    Tree *parent = tree;
    Tree *child = NULL;
    char component[MAX_FOLDER_NAME_LENGTH + 1];
    const char *subpath = path;
    if (strcmp(subpath, "/") == 0)
        syserr("EBUSY");
    else
    {
        while ((subpath = split_path(subpath, component)))
        {
            child = hmap_get(parent->sub_trees, component);
            if (child == NULL)
                syserr("ENOENT");

            if (strcmp(subpath, "/") == 0)
            {
                hmap_remove(parent->sub_trees, component);
                return child;
            }

            parent = child;
            child = NULL;
        }
    }
    return NULL;
}

void tree_rename(Tree **tree, const char *new_name)
{

    free((*tree)->name);
    (*tree)->name = calloc(256, sizeof(char));
    strcpy((*tree)->name, new_name);
}

/** Attach subtree to the given path in tree;
 * if given tree has follwing paths: /a/b/ , /b/;
 * Then attaching to it a subtree /c/d/ under path /b/;
 * would result in tree : /a/b/ /b/c/d/
 *
 */

int tree_attach(Tree *tree, Tree *subtree, const char *path)
{
    if (!is_path_valid(path))
        syserr("EINVAL");

    if (strcmp(path, "/") == 0)
        syserr("EBUSY");

    Tree *parent = tree;
    Tree *child = NULL;
    char component[MAX_FOLDER_NAME_LENGTH + 1];
    char dir_name[MAX_FOLDER_NAME_LENGTH + 1];
    const char *subpath = path;
    char *trash = make_path_to_parent(subpath, dir_name);
    free(trash);
    tree_rename(&subtree, dir_name);

    if (strcmp(subpath, "/") == 0)
    {
        hmap_insert(tree->sub_trees, dir_name, subtree);
    }
    else
    {
        while ((subpath = split_path(subpath, component)))
        {

            if (strcmp(component, dir_name) == 0)
            {
                hmap_insert(parent->sub_trees, dir_name, subtree);
            }
            else
            {
                child = hmap_get(parent->sub_trees, component);
                if (child == NULL)
                    syserr("ENOENT");

                parent = child;
                child = NULL;
            }
        }
    }
    return 0;
}

int tree_move(Tree *tree, const char *source, const char *target)
{

    // check if source and target is okay then procide
    Tree *moving = tree_detach(tree, source);
    char component[MAX_FOLDER_NAME_LENGTH + 1];
    const char *subpath = target;
    subpath = make_path_to_parent(subpath, component);

    free(moving->name);
    moving->name = NULL;
    moving->name = calloc(256, sizeof(char));
    strcpy(moving->name, component);

    tree_attach(tree, moving, subpath);
    return 0;
}
