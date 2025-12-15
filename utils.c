#include "include/utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nob.h"

char *GetFullPath(const char *path, char *buff, size_t buffSize) {
#ifdef _WIN32
    return _fullpath(buff, path, buffSize);
#else
    (void)buffSize;
    return realpath(path, buff);
#endif
}

bool GetDirAndChange(const char *src) {
    bool result = true;

    char *fullPath = GetFullPath(src, NULL, 0);

    if (fullPath == NULL)
        nob_return_defer(false);

    fullPath[nob_path_name(fullPath) - fullPath] = '\0';
    if (!nob_set_current_dir(fullPath)) {
        nob_return_defer(false);
    }

defer:
    if (fullPath)
        free(fullPath);

    return result;
}

const char *ProcessInput(int argc, char **argv, const char *src) {
    const char *input = NULL;

    enum inputType {
        INPUT_NULL,
        INPUT_NORMAL,
        INPUT_SMALL,
        INPUT_FILE,
        INPUT_COUNT,
    } inputType = INPUT_NULL;

    if (argc == 1) {
        if (!GetDirAndChange(src))
            return NULL;
        inputType = INPUT_NORMAL;
        input     = "input.txt";
    } else {
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "-s") == 0) {
                if (!GetDirAndChange(src))
                    return NULL;
                if (nob_file_exists("small.txt") > 0) {
                    inputType = INPUT_SMALL;
                    input     = "small.txt";
                } else {
                    inputType = INPUT_NORMAL;
                    input     = "input.txt";
                }
                break;
            } else {
                input     = argv[i];
                inputType = INPUT_FILE;
                break;
            }
        }
    }

    assert(inputType > INPUT_NULL && inputType < INPUT_COUNT);

    return input;
}

char *KxD_strndup(const char *data, size_t count) {
#if !defined(_WIN32) || (defined(POSIX_C_SOURCE) && POSIX_C_SOURCE >= 200809L) ||                                      \
    (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 700) || defined(_GNU_SOURCE)
    return strndup(data, count);
#else
    char *result = calloc(count + 1, 1);
    if (result != NULL)
        memcpy(result, data, count);
    return result;
#endif
}
