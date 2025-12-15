#pragma once
#ifndef KXD_UTILS_H_
#define KXD_UTILS_H_
#include <stddef.h>

char *GetFullPath(const char *path, char *buff, size_t buffSize);
const char *ProcessInput(int argc, char **argv, const char *src);
char *KxD_strndup(const char *data, size_t count);

#endif // KXD_UTILS_H_
