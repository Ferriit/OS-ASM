#ifndef HASHMAP_H
#define HASHMAP_H

#include "dynamic_array.h"
#include "mem.h"

typedef struct {
    char *key;
    void *value;
} HashMapEntry;

typedef struct {
    DynamicArray *buckets;
    size_t num_buckets;
} HashMap;

// djb2 hash function
static inline unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

static inline int init_hashmap(HashMap *map, size_t num_buckets) {
    map->num_buckets = num_buckets;
    map->buckets = (DynamicArray*)alloc(num_buckets * sizeof(DynamicArray));
    if (!map->buckets)
        return 1;

    for (size_t i = 0; i < num_buckets; i++) {
        if (init_array(&map->buckets[i], sizeof(HashMapEntry)) != 0)
            return 1; // Failed to init bucket
    }

    return 0; // Success
}

static inline int hashmap_put(HashMap *map, const char *key, void *value) {
    unsigned long h = hash(key);
    size_t index = h % map->num_buckets;

    DynamicArray *bucket = &map->buckets[index];

    for (size_t i = 0; i < bucket->size; i++) {
        HashMapEntry *entry = (HashMapEntry *)get(bucket, i);
        if (entry && mem_strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 0;
        }
    }

    HashMapEntry new_entry;
    new_entry.key = (char*)alloc(strlen(key) + 1);
    if (!new_entry.key)
        return 1;
    mem_memcpy(new_entry.key, key, strlen(key) + 1);
    new_entry.value = value;

    if (push_back(bucket, &new_entry) != 0)
        return 1;

    return 0;
}

static inline void *hashmap_get(HashMap *map, const char *key) {
    unsigned long h = hash(key);
    size_t index = h % map->num_buckets;

    DynamicArray *bucket = &map->buckets[index];

    for (size_t i = 0; i < bucket->size; i++) {
        HashMapEntry *entry = (HashMapEntry *)get(bucket, i);
        if (entry && mem_strcmp(entry->key, key) == 0)
            return entry->value;
    }

    return nullptr;
}

static inline void free_hashmap(HashMap *map) {
    if (!map->buckets)
        return;

    for (size_t i = 0; i < map->num_buckets; i++) {
        DynamicArray *bucket = &map->buckets[i];
        for (size_t j = 0; j < bucket->size; j++) {
            HashMapEntry *entry = (HashMapEntry *)get(bucket, j);
            if (entry)
                free_mem(entry->key);
        }
        free_array(bucket);
    }

    free_mem(map->buckets);
    map->buckets = NULL;
    map->num_buckets = 0;
}

static inline int hashmap_keys(HashMap *map, DynamicArray *out_keys) {
    if (init_array(out_keys, sizeof(char *)) != 0)
        return 1;

    for (size_t i = 0; i < map->num_buckets; i++) {
        DynamicArray *bucket = &map->buckets[i];
        for (size_t j = 0; j < bucket->size; j++) {
            HashMapEntry *entry = (HashMapEntry *)get(bucket, j);
            if (entry) {
                char *key_ptr = entry->key;
                if (push_back(out_keys, &key_ptr) != 0)
                    return 1;
            }
        }
    }

    return 0; // Success
}

static inline int hashmap_values(HashMap *map, DynamicArray *out_values) {
    if (init_array(out_values, sizeof(void *)) != 0)
        return 1;

    for (size_t i = 0; i < map->num_buckets; i++) {
        DynamicArray *bucket = &map->buckets[i];
        for (size_t j = 0; j < bucket->size; j++) {
            HashMapEntry *entry = (HashMapEntry *)get(bucket, j);
            if (entry) {
                void *value_ptr = entry->value;
                if (push_back(out_values, &value_ptr) != 0)
                    return 1;
            }
        }
    }

    return 0; // Success
}

#endif