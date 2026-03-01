#ifndef H_IO_H
#define H_IO_H

#include <stdio.h>
#include "dynamic_array.h"

static inline DynamicArray read(const char *filename) {
    DynamicArray result;
    init_array(&result, sizeof(char));

    FILE *file = fopen(filename, "r");
    if (!file)
        return result; // Return empty array on failure

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        for (size_t i = 0; buffer[i] != '\0'; i++) {
            push_back(&result, &buffer[i]);
        }
    }

    fclose(file);
    return result;
}

#endif