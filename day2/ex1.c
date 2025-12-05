// #define NOB_IMPLEMENTATION
#include "../include/nob.h"

#include <inttypes.h>

#define RED_CUBE 12
#define GRN_CUBE 13
#define BLU_CUBE 14

typedef struct cube {
    size_t id, plays;
    bool notValid;
} Cube;

typedef struct {
    Cube *items;
    size_t count;
    size_t capacity;
} Cubes;

size_t getId(const char *line);
size_t getPlays(const char *line);
void calcCubes(Cube *cubes, const char *line);

Nob_String_Builder sb2 = {};

int main(int argc, char **argv) {
    const char *input = "input.txt";
    if (argc > 1) {
        input = argv[1];
    }

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        return 1;

    Nob_String_View sv      = nob_sv_trim(nob_sb_to_sv(sb));
    Cubes cubes = {};
    while (sv.count) {
        Cube c = {};
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        const char *line    = nob_temp_sv_to_cstr(sv2);
        // printf("%s", line);
        c.plays = getPlays(line);
        // puts("getPlays OK!\n");
        c.id = getId(line);
        // puts("getId OK!\n");
        calcCubes(&c, line);
        // puts("calcCubes OK!");
        printf("==========\n Cube: %zu\n    Id:%zu\n    Plays: %zu\n", cubes.count + 1, c.id, c.plays);
        if (c.notValid)
            printf("    Cube %zu is not valid\n", cubes.count + 1);
        else
            printf("    Cube %zu is valid\n", cubes.count + 1);
        nob_da_append(&cubes, c);
    }

    puts("==========\nCalculando: ");
    size_t soma = 0;
    for (size_t i = 0; i < cubes.count; i++) {
        if (cubes.items[i].id == 0)
            continue;

        if (!cubes.items[i].notValid) {
            soma += cubes.items[i].id;
        }
    }
    printf("cubes.count: %zu\n", cubes.count);
    printf("Resultado: %zu\n", soma);

    nob_da_free(cubes);
    nob_sb_free(sb);
    nob_sb_free(sb2);

    return 0;
}

size_t getId(const char *line) {
    sb2.count   = 0;
    char *delim = strchr(line, ':');
    for (int i = 5; i < delim - line; i++) {
        nob_sb_append_buf(&sb2, &line[i], 1);
    }
    nob_sb_append_null(&sb2);
    return strtoull(sb2.items, NULL, 10);
}

size_t getPlays(const char *line) {
    size_t i = 0, rst = 0;
    while (line[i] != 0) {
        if (line[i] == ';')
            rst++;
        i++;
    }
    return rst + 1;
}
void calcCubes(Cube *cubes, const char *line) {
    size_t qtd  = 0;
    char *start = strchr(line, ':');
    start++;
    char color[6] = {};
    start         = strtok(start, ",;");
    while (start != NULL) {
        sscanf(start, "%zu %s", &qtd, color);
        if (strcmp(color, "red") == 0 && qtd > RED_CUBE) {
            cubes->notValid = true;
            return;
        } else if (strcmp(color, "green") == 0 && qtd > GRN_CUBE) {
            cubes->notValid = true;
            return;
        } else if (strcmp(color, "blue") == 0 && qtd > BLU_CUBE) {
            cubes->notValid = true;
            return;
        }
        start = strtok(NULL, ",;");
    }
    cubes->notValid = false;
}
