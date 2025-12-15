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
    char **items;
    size_t count;
    size_t capacity;
} Strings;

int main(int argc, char **argv) {
    const char *input = ProcessInput(argc, argv, __FILE__);
    assert(input != NULL);

    Nob_String_View sv;
    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        return 1;

    sv = nob_sv_trim(nob_sb_to_sv(sb));

    Strings strings        = {};
    Numbers yourNumbers    = {};
    Numbers winningNumbers = {};
    Numbers result         = {};

    while (sv.count) {
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        char *str           = KxD_strndup(sv2.data, sv2.count);
        assert(str != NULL);
        nob_da_append(&strings, str);
    }

    nob_sb_free(sb);

    size_t n;
    size_t wsize = 0;
    size_t ysize = 0;
    for (size_t i = 0; i < strings.count; i++) {
        wsize = 0;
        ysize = 0;

        char *tmp = strchr(strings.items[i], ':');
        char *sep = strchr(strings.items[i], '|');

        assert(tmp != NULL);
        assert(sep != NULL);

        tmp++;

        while (tmp < sep - 1 && tmp != NULL) {
            sscanf(tmp, "%zu", &n);
            tmp++;
            tmp++;
            tmp = strchr(tmp, ' ');
            nob_da_append(&yourNumbers, n);
            ysize++;
        }
        sep++;
        while (sep != NULL && sep < sep + strlen(strings.items[i])) {
            sscanf(sep, "%zu", &n);
            sep++;
            sep++;
            sep = strchr(sep, ' ');
            // printf("seu numero: %d\n",n);
            nob_da_append(&winningNumbers, n);
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
        nob_da_append(&result, tmpSoma);
        tmpSoma = 0;
    }
    // puts("\n===================");
    for (size_t i = 0; i < result.count; i++) {
        soma += result.items[i];
        printf("Jogo %zu: %zu\n", i + 1, result.items[i]);
    }
    for (size_t i = 0; i < strings.count; ++i)
        free((void *)strings.items[i]);
    nob_da_free(strings);
    nob_da_free(yourNumbers);
    nob_da_free(winningNumbers);
    nob_da_free(result);

    printf("Resultado final: %zu\n", soma);

    return 0;
}
