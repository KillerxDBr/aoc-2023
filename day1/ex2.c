#define NOMINMAX

// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int main(int argc, char **argv) {
    const char *numeros[] = {
        "one",  "two", "three", "four",          //
        "five", "six", "seven", "eight", "nine", //
        "1",    "2",   "3",     "4",             //
        "5",    "6",   "7",     "8",     "9"     //
    };
    const size_t numerosCount = sizeof(numeros) / sizeof(numeros[0]);

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

        input = "input.txt";
    }

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        return 1;

    // char line[256];
    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));
    Nob_String_View sv2;

    size_t soma = 0;
    size_t n1   = 0;
    size_t n2   = 0;

    while (sv.count) {
        sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        printf("sv2: " SV_Fmt "\n", SV_Arg(sv2));
        size_t minDist   = (size_t)~0;
        size_t maxDist   = 0;
        const char *line = nob_temp_sv_to_cstr(sv2);
        for (size_t i = 0; i < numerosCount; i++) {
            char *tmp = strstr(line, numeros[i]);
            // if (tmp != NULL) {
            while (tmp != NULL) {
                maxDist = max(maxDist, (size_t)(tmp - line));
                minDist = min(minDist, (size_t)(tmp - line));
                printf("minDist: %zu\n", minDist);
                printf("maxDist: %zu\n", maxDist);
                tmp++;
                tmp = strstr(tmp, numeros[i]);
            }
            // }
        }
        n1            = 0;
        n2            = 0;
        char final[6] = {};
        strncpy(final, line + minDist, 6);
        final[5] = 0;
        for (size_t i = 0; i < numerosCount; i++) {
            if (strncmp(final, numeros[i], strlen(numeros[i])) == 0) {
                if (i > 8) {
                    n1 = (final[0] - '0') * 10;
                    break;
                } else {
                    n1 = (i + 1) * 10;
                    break;
                }
            }
        }
        strncpy(final, line + maxDist, 6);
        final[5] = 0;
        for (size_t i = 0; i < numerosCount; i++) {
            if (strncmp(final, numeros[i], strlen(numeros[i])) == 0) {
                if (i > 8) {
                    n2 = final[0] - '0';
                    break;
                } else {
                    n2 = i + 1;
                    break;
                }
            }
        }
        printf("String %s adicionando %zu\n\tn1 = %zu\n\tn2 = %zu\n============\n", line, (n1 + n2), n1, n2);
        soma += (n1 + n2);
    }
    printf("Resultado final: %zu\n", soma);
    fflush(stdout);
    nob_sb_free(sb);
    return 0;
}
