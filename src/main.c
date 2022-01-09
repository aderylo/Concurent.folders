#include "HashMap.h"
#include "Tree.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void print_map(HashMap *map)
{
    const char *key = NULL;
    void *value = NULL;
    printf("Size=%zd\n", hmap_size(map));
    HashMapIterator it = hmap_iterator(map);
    while (hmap_next(map, &it, &key, &value))
    {
        printf("Key=%s Value=%p\n", key, value);
    }
    printf("\n");
}

int main(void)
{
    HashMap *map = hmap_new();
    hmap_insert(map, "a", hmap_new());
    print_map(map);

    HashMap *child = (HashMap *)hmap_get(map, "a");
    hmap_free(child);
    hmap_remove(map, "a");
    print_map(map);

    hmap_free(map);

    Tree *tree = tree_new();
    tree_create(tree, "/bin/python/");
    tree_create(tree, "/bin/cython/");

    char *n = tree_list(tree, "/bin/");
    printf("%s\n", n);
    free(n);
    tree_free(tree);

    return 0;
}