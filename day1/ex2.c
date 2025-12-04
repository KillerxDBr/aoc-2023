#define NOMINMAX

// #define NOB_IMPLEMENTATION
#include "../include/nob.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int main(int argc, char **argv) {
    const char *program = nob_shift(argv, argc);

    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        return 1;
    }

    const char *numeros[]
        = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    size_t numerosCount = sizeof(numeros) / sizeof(numeros[0]);
    const char *input = "input.txt";
    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    char line[256];

    size_t soma = 0, n1 = 0, n2 = 0;
    while (fgets(line, 255, fd) != NULL) {
        line[strlen(line) - 1] = '\0';
        printf("%s -> (%p)\n", line, line);
        size_t minDist = UINT64_MAX;
        size_t maxDist = 0;
        for (size_t i = 0; i < numerosCount; i++) {
            char *tmp = strstr(line, numeros[i]);
            if (tmp != NULL) {
                while (tmp != NULL) {
                    maxDist = max(maxDist, (size_t)(tmp - line));
                    minDist = min(minDist, (size_t)(tmp - line));
                    printf("minDist: %zu\n", minDist);
                    printf("maxDist: %zu\n", maxDist);
                    tmp++;
                    tmp = strstr(tmp, numeros[i]);
                }
            }
        }
        n1 = 0;
        n2 = 0;
        char final[6] = { 0 };
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
    fclose(fd);
    printf("Resultado final: %zu\n", soma);
    return 0;
}
