#ifndef INT_DYNAMIC_ARRAY_H
#define INT_DYNAMIC_ARRAY_H

#include "mem.h"

typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t elem_size;
} DynamicArray;

static inline int init_array(DynamicArray *arr, size_t elem_size) {
    arr->data = alloc(8 * elem_size);
    if (!arr->data)
        return 1;   // Allocation failed

    arr->size = 0;
    arr->capacity = 8;
    arr->elem_size = elem_size;
    return 0;       // Success
}

static inline void free_array(DynamicArray *arr) {
    free_mem(arr->data);
    arr->data = nullptr;
    arr->size = 0;
    arr->capacity = 0;
    arr->elem_size = 0;
}

static inline int push_back(DynamicArray *arr, void *element) {
    if (arr->size == arr->capacity) {
        size_t new_capacity = arr->capacity * 2;

        void *new_data = alloc(new_capacity * arr->elem_size);
        if (!new_data)
            return 1;   // Allocation failed

        // Copy old data into new buffer
        mem_memcpy(new_data,
               arr->data,
               arr->size * arr->elem_size);

        free_mem(arr->data);

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    // Compute address of insertion point
    void *dest = (char *)arr->data + (arr->size * arr->elem_size);

    // Copy element bytes into array
    mem_memcpy(dest, element, arr->elem_size);

    arr->size++;

    return 0;   // Success
}

static inline int pop_back(DynamicArray *arr) {
    if (arr->size == 0)
        return 1;   // Empty
    
    if (arr->size == (arr->capacity / 4) && arr->capacity > 8) {
        size_t new_capacity = arr->capacity / 2;

        void *new_data = alloc(new_capacity * arr->elem_size);
        if (!new_data)
            return 1;   // Allocation failed

        // Copy old data into new buffer
        mem_memcpy(new_data,
               arr->data,
               arr->size * arr->elem_size);

        free_mem(arr->data);

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    arr->size--;   // Logical removal only
    return 0;
}

static inline void *get(DynamicArray *arr, size_t index) {
    if (index >= arr->size)
        return nullptr;    // Out of bounds

    return (char *)arr->data + (index * arr->elem_size);
}

static inline void *set(DynamicArray *arr, size_t index, void *element) {
    if (index >= arr->capacity) {
        size_t new_capacity = arr->capacity;
        while (new_capacity <= index)
            new_capacity *= 2;

        void *new_data = alloc(new_capacity * arr->elem_size);
        if (!new_data)
            return nullptr;    // Allocation failed

        mem_memcpy(new_data, arr->data, arr->size * arr->elem_size);

        free_mem(arr->data);
        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    if (index >= arr->size)
        arr->size = index + 1;

    void *dest = (char *)arr->data + index * arr->elem_size;
    mem_memcpy(dest, element, arr->elem_size);

    return dest;
}

#endif