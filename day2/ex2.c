// #define NOB_IMPLEMENTATION
#include "../include/nob.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int main(void) {
    // const char *program = nob_shift(argv, argc);

    // if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
    //     return 1;
    // }

    const char *input = "day2/input.txt";
    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    size_t soma = 0;
    char line[256];
    int r, g, b;
    while (fgets(line, 255, fd) != NULL) {
        r = 1;
        g = 1;
        b = 1;
        int n = 0;
        char color[6] = { 0 };
        char *start = strchr(line, ':');
        start++;
        start = strtok(start, ",;");
        while (start != NULL) {
            sscanf(start, "%d %s", &n, color);
            if (strcmp(color, "red") == 0) {
                r = max(r, n);
            } else if (strcmp(color, "green") == 0) {
                g = max(g, n);
            } else if (strcmp(color, "blue") == 0) {
                b = max(b, n);
            }
            start = strtok(NULL, ",;");
        }
        soma += (r * g * b);
    }
    fclose(fd);
    printf("Resultado: %zu\n", soma);
    return 0;
}
