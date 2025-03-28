#define NOB_IMPLEMENTATION
#include "../include/nob.h"

/*
typedef struct {
    <type> *items;
    size_t count;
    size_t capacity;
} <name>;
*/

typedef struct {
    size_t *items;
    size_t count;
    size_t capacity;
} Numbers;

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Strings;

void updateNumbers(Strings *strings, Numbers *yourNumbers, Numbers *winningNumbers, size_t index);
void updateStrings(Strings *strings, size_t ysize, size_t wsize, Numbers *yourNumbers, Numbers *winningNumbers, size_t index,
                   Numbers *result);

size_t originalSize;

#define SMALL 0
#define BUFFER_SIZE 256

int main(int argc, char **argv) {
    const char *program = nob_shift(argv, argc);

    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        return 1;
    }

    const char *separator = "===================";
#if SMALL
    const char *input = "small.txt";
#else
    const char *input = "input.txt";
#endif
    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    Strings strings = { 0 };
    Numbers yourNumbers = { 0 };
    Numbers winningNumbers = { 0 };
    Numbers result = { 0 };

    char *tmp = malloc(BUFFER_SIZE);
    while (fgets(tmp, BUFFER_SIZE - 1, fd) != NULL) {
        char *endLine = strchr(tmp, '\n');
        if (endLine != NULL)
            endLine[0] = '\0';
        tmp = realloc(tmp, strlen(tmp) + 1);
        nob_da_append(&strings, tmp);
        tmp = malloc(BUFFER_SIZE);
    }
    fclose(fd);
    free(tmp);

    originalSize = strings.count;
    size_t tmpIndex;
    size_t index = 0;
    updateNumbers(&strings, &yourNumbers, &winningNumbers, index);
    size_t wsize = winningNumbers.count / originalSize, ysize = yourNumbers.count / originalSize;

    do {
        tmpIndex = strings.count;
        updateStrings(&strings, ysize, wsize, &yourNumbers, &winningNumbers, index, &result);
        index = tmpIndex;
        updateNumbers(&strings, &yourNumbers, &winningNumbers, index);
    } while (index != strings.count);

    size_t rst = strings.count;

    nob_da_free(yourNumbers);
    nob_da_free(winningNumbers);
    nob_da_free(result);

    for (size_t i = 0; i < strings.count; ++i)
        free((void *)strings.items[i]);

    printf("%s\nResultado: %zu\n", separator, rst);
    return 0;
}

void updateNumbers(Strings *strings, Numbers *yourNumbers, Numbers *winningNumbers, size_t index) {
    size_t n;
    char *str = NULL;
    char *sep = NULL;

    for (size_t i = index; i < strings->count; i++) {

        str = strchr(strings->items[i], ':');
        sep = strchr(strings->items[i], '|');
        str++;

        while (str < sep - 1 && str != NULL) {
            sscanf(str, "%zu", &n);
            str++;
            str++;
            str = strchr(str, ' ');
            nob_da_append(winningNumbers, n);
        }
        sep++;

        while (sep != NULL && sep < sep + strlen(strings->items[i])) {
            sscanf(sep, "%zu", &n);
            sep++;
            sep++;
            sep = strchr(sep, ' ');
            nob_da_append(yourNumbers, n);
        }
    }
}

void updateStrings(Strings *strings, size_t ysize, size_t wsize, Numbers *yourNumbers, Numbers *winningNumbers, size_t index,
                   Numbers *result) {
    size_t wn, yn;
    size_t n = 0;

    for (size_t i = index; i < strings->count; i++) {
        for (size_t j = 0; j < wsize; j++) {
            wn = winningNumbers->items[j + i * wsize];
            for (size_t l = 0; l < ysize; l++) {
                yn = yourNumbers->items[l + ysize * i];
                if (yn == wn)
                    n++;
            }
        }
        nob_da_append(result, n);
        n = 0;
    }

    char *dupString;

    for (size_t i = index; i < result->count; i++) {
        if (result->items[i] == 0)
            continue;
        for (size_t j = 1; j <= result->items[i]; j++) {
            size_t strToDup = originalSize + 1;
            for (size_t k = 0; k < originalSize; k++) {
                if (strcmp(strings->items[i], strings->items[k]) == 0) {
                    strToDup = k;
                    break;
                }
            }
            // printf("%zu\n", strToDup);
            assert(strToDup < originalSize);
            dupString = strdup(strings->items[strToDup + j]);
            nob_da_append(strings, dupString);
        }
    }
}
