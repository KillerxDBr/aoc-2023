#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define da_free(xs)                                                                                                                        \
    do {                                                                                                                                   \
        if ((xs)->count > 0) {                                                                                                             \
            free((xs)->items);                                                                                                             \
            (xs)->count = 0;                                                                                                               \
            (xs)->capacity = 0;                                                                                                            \
        }                                                                                                                                  \
    } while (0);

#define da_free_ptr(xs)                                                                                                                    \
    do {                                                                                                                                   \
        if ((xs)->count > 0) {                                                                                                             \
            for (int i = 0; i < (xs)->count; i++)                                                                                          \
                free((xs)->items[i]);                                                                                                      \
            free((xs)->items);                                                                                                             \
            (xs)->count = 0;                                                                                                               \
            (xs)->capacity = 0;                                                                                                            \
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

/*
typedef struct
{
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

#define SMALL 0
#if SMALL
#define LINE_SIZE 48
#else
#define LINE_SIZE 116
#endif

int main(void) {
#if SMALL
    const char *input = "small.txt";
#else
    const char *input = "input.txt";
#endif
    FILE *fd = fopen(input, "rt");
    if (fd == NULL) {
        fprintf(stderr, "Couldn't open file %s: ", input);
        perror(NULL);
        return 1;
    }
    Strings strings = { 0 };
    Numbers yourNumbers = { 0 };
    Numbers winningNumbers = { 0 };
    Numbers result = { 0 };

    char *tmp = malloc(256);
    while (fgets(tmp, 255, fd) != NULL) {
        tmp[LINE_SIZE] = 0;
        tmp = realloc(tmp, strlen(tmp) + 1);
        da_append(&strings, tmp);
        tmp = malloc(256);
    }
    fclose(fd);
    free(tmp);

    char *sep = NULL;
    size_t n, wsize, ysize;
    for (size_t i = 0; i < strings.count; i++) {
        wsize = 0;
        ysize = 0;

        tmp = strchr(strings.items[i], ':');
        sep = strchr(strings.items[i], '|');
        tmp++;

        while (tmp < sep - 1 && tmp != NULL) {
            sscanf(tmp, "%zu", &n);
            tmp++;
            tmp++;
            tmp = strchr(tmp, ' ');
            da_append(&yourNumbers, n);
            ysize++;
        }
        sep++;
        while (sep != NULL && sep < sep + strlen(strings.items[i])) {
            sscanf(sep, "%zu", &n);
            sep++;
            sep++;
            sep = strchr(sep, ' ');
            // printf("seu numero: %d\n",n);
            da_append(&winningNumbers, n);
            wsize++;
        }

        printf("%s\n", strings.items[i]);
    }

    size_t soma = 0, tmpSoma = 0;
    size_t wn, yn;

    puts("===================");
    for (size_t i = 0; i < winningNumbers.count / wsize; i++) {
        for (size_t j = 0; j < wsize; j++) {
            // if (i > j && j + i * wsize % wsize == 0)
            //     printf("\n");
            // printf("%02d ", winningNumbers.items[j + i * wsize]);
            wn = winningNumbers.items[j + i * wsize];

            for (size_t l = 0; l < ysize; l++) {
                yn = yourNumbers.items[l + ysize * i];
                if (yn != wn)
                    continue;
                if (tmpSoma == 0)
                    tmpSoma++;
                else
                    tmpSoma *= 2;
                break;
            }
        }
        da_append(&result, tmpSoma);
        tmpSoma = 0;
    }
    // puts("\n===================");
    for (size_t i = 0; i < result.count; i++) {
        soma += result.items[i];
        printf("Jogo %zu: %zu\n", i + 1, result.items[i]);
    }
    printf("Resultado final: %zu\n", soma);

    return 0;
}
