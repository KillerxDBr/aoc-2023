#define NOB_IMPLEMENTATION
#include "../include/nob.h"

#define BUF_SIZE 256

int main(int argc, char **argv) {
    const char *program = nob_shift(argv, argc);

    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        return 1;
    }

    const char *input = "input.txt";
    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    char line[256];
    size_t soma = 0;
    size_t fdigit = 0;
    size_t ldigit = 0;
    while (fgets(line, 255, fd) != NULL) {
        for (size_t i = 0; i < strlen(line); ++i) {
            if (isdigit(line[i])) {
                if (fdigit == 0)
                    fdigit = line[i] - '0';
                ldigit = line[i] - '0';
            }
        }
        printf("Linha: %s fdigit: %zu ldigit: %zu\n", line, fdigit, ldigit);
        soma += (fdigit * 10) + ldigit;
        fdigit = 0;
        ldigit = 0;
    }
    printf("Resultado final: %zu\n", soma);
    return 0;
}
