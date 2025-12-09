#include "include/utils.h"

#include <stdlib.h>

char *GetFullPath(const char *path, char *buff, size_t buffSize) {
#ifdef _WIN32
    return _fullpath(buff, path, buffSize);
#else
    (void)buffSize;
    return realpath(path, buff);
#endif
}
