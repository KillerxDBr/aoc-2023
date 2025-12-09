// #define NOBDEF static inline
// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

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
    const char **items;
    size_t count;
    size_t capacity;
} Strings;

void updateNumbers(Strings *strings, Numbers *yourNumbers, Numbers *winningNumbers, size_t index);
void updateStrings(Strings *strings, size_t ysize, size_t wsize, Numbers *yourNumbers, Numbers *winningNumbers,
                   size_t index, Numbers *result);

size_t originalSize;

// #define SMALL
#define BUFFER_SIZE 256

#ifdef _WIN32
static const char *strndup(const char *str, size_t n) {
    const char *rst = calloc(n + 1, 1);
    assert(rst != NULL);
    memcpy((void *)rst, str, n);
    return rst;
}
#endif

int main(int argc, char **argv) {
    const char *separator = "===================";

    const char *input;
    if (argc > 1)
        input = argv[1];
    else {
        char *fullPath = GetFullPath(__FILE__, NULL, 0);
        if (fullPath != NULL) {
            fullPath[nob_path_name(fullPath) - fullPath] = '\0';
            if (!nob_set_current_dir(fullPath)) {
                return 1;
            }

            free(fullPath);
        }

#ifdef SMALL
        input = "small.txt";
#else
        input = "input.txt";
#endif
    }

    Nob_String_Builder sb   = {};
    Nob_String_View content = {};
    if (!nob_read_entire_file(input, &sb))
        return 1;

    content = nob_sv_trim(nob_sb_to_sv(sb));

    Strings strings        = {};
    Numbers yourNumbers    = {};
    Numbers winningNumbers = {};
    Numbers result         = {};

    while (content.count) {
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&content, '\n'));
        const char *str     = strndup(sv2.data, sv2.count);
        // const char *str = nob_temp_sv_to_cstr(sv2);
        nob_da_append(&strings, str);
    }

    nob_sb_free(sb);

    originalSize = strings.count;
    size_t index = 0;
    updateNumbers(&strings, &yourNumbers, &winningNumbers, index);
    size_t wsize = winningNumbers.count / originalSize;
    size_t ysize = yourNumbers.count / originalSize;

    do {
        size_t tmpIndex = strings.count;
        updateStrings(&strings, ysize, wsize, &yourNumbers, &winningNumbers, index, &result);
        index = tmpIndex;
        updateNumbers(&strings, &yourNumbers, &winningNumbers, index);
    } while (index != strings.count);

    const size_t rst = strings.count;

    nob_da_free(yourNumbers);
    nob_da_free(winningNumbers);
    nob_da_free(result);

    for (size_t i = 0; i < strings.count; ++i)
        free((void *)strings.items[i]);
    nob_da_free(strings);

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

void updateStrings(Strings *strings, size_t ysize, size_t wsize, Numbers *yourNumbers, Numbers *winningNumbers,
                   size_t index, Numbers *result) {
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
            // dupString = nob_temp_strdup(strings->items[strToDup + j]);
            dupString = strdup(strings->items[strToDup + j]);
            nob_da_append(strings, dupString);
        }
    }
}
