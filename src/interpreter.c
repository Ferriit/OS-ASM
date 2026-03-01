#include "../headers/mem.h"
#include "../headers/dynamic_array.h"
#include "../headers/hashmap.h"
#include <stdio.h>

int main() {
    DynamicArray arr;
    if (init_array(&arr, sizeof(int)) != 0) {
        return 1;
    }

    for (int i = 0; i < 100; i++) {
        if (push_back(&arr, &i) != 0) {
            free_array(&arr);
            printf("Failed to push element %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < arr.size; i++) {
        int *value = (int *)get(&arr, i);
        if (value) {
            printf("%d ", *value);
        }
    }
    printf("\n");

    free_array(&arr);

    HashMap map;
    if (init_hashmap(&map, 16) != 0) {
        printf("Failed to initialize hashmap\n");
        return 1;
    }
    hashmap_put(&map, "key1", "value1");
    hashmap_put(&map, "key2", "value2");

    printf("key1: %s\n", (char *)hashmap_get(&map, "key1"));
    printf("key2: %s\n", (char *)hashmap_get(&map, "key2"));

    DynamicArray keys;
    if (hashmap_keys(&map, &keys) != 0) {
        printf("Failed to get hashmap keys\n");
        free_hashmap(&map);
        return 1;
    }

    printf("keys: ");
    for (size_t i = 0; i < keys.size; i++) {
        char **key_ptr = (char **)get(&keys, i);
        if (key_ptr && *key_ptr) {
            printf("%s ", *key_ptr);
        }
    }
    printf("\n");

    free_array(&keys);
    free_hashmap(&map);

    return 0;
}