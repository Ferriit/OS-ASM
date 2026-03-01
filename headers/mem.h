#ifndef INT_MEM_H

#define INT_MEM_H
#include <stdlib.h>
#include <string.h>

static inline void *alloc(size_t size) {
    return malloc(size);
}

static inline void free_mem(void *ptr) {
    free(ptr);
}

static inline void *mem_memcpy(void *dest, const void *src, size_t n) {
    return memcpy(dest, src, n);
}

static inline int mem_strcmp(const char *s1, const char *s2) {
    return strcmp(s1, s2);
}


static inline size_t mem_strlen(const char *s) {
    return strlen(s);
}

#endif