#ifndef DA_H
#define DA_H

#include <stdlib.h>
#include <string.h>

#define da_free(xs)                                                                                                                        \
    do {                                                                                                                                   \
        if ((xs)->capacity > 0) {                                                                                                          \
            free((xs)->items);                                                                                                             \
            memset((xs), 0, sizeof(*(xs)));                                                                                                \
        }                                                                                                                                  \
    } while (0);

#define da_free_ptr(xs)                                                                                                                    \
    do {                                                                                                                                   \
        if ((xs)->capacity > 0) {                                                                                                          \
            for (size_t i = 0; i < (xs)->count; i++)                                                                                       \
                free((xs)->items[i]);                                                                                                      \
            free((xs)->items);                                                                                                             \
            memset((xs), 0, sizeof(*(xs)));                                                                                                \
        }                                                                                                                                  \
    } while (0);

#define da_append(xs, x)                                                                                                                   \
    do {                                                                                                                                   \
        if ((xs)->count >= (xs)->capacity) {                                                                                               \
            if ((xs)->capacity == 0)                                                                                                       \
                (xs)->capacity = 256;                                                                                                      \
            else                                                                                                                           \
                (xs)->capacity *= 2;                                                                                                       \
            (xs)->items = realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items));                                                     \
        }                                                                                                                                  \
        (xs)->items[(xs)->count++] = (x);                                                                                                  \
    } while (0)

#define da_array(name, dtype)                                                                                                              \
    typedef struct {                                                                                                                       \
        dtype *items;                                                                                                                      \
        size_t count, capacity;                                                                                                            \
    } name

#endif // DA_H
