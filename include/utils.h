#pragma once
#ifndef KXD_UTILS_H_
#define KXD_UTILS_H_

#include <stdlib.h>
#include <string.h>

char *GetFullPath(const char *path, char *buff, size_t buffSize);
const char *ProcessInput(int argc, char **argv, const char *src);

static inline char *KxD_strndup(const char *data, size_t count) {
#if (!defined(_WIN32) && !defined(__WINE__)) || (defined(POSIX_C_SOURCE) && POSIX_C_SOURCE >= 200809L) ||              \
    (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 700) || defined(_GNU_SOURCE)
    return strndup(data, count);
#else
    char *result = (char *)malloc(count + 1);
    if (result != NULL) {
        memcpy(result, data, count);
        result[count] = '\0';
    }
    return result;
#endif
}

#endif // KXD_UTILS_H_
