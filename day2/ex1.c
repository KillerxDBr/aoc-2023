#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED_CUBE 12
#define GRN_CUBE 13
#define BLU_CUBE 14

#define CUBE_QTD 100

typedef struct cubes_s {
    size_t id, plays;
    bool notValid;
} cubes_t;

size_t getId(const char *line);
size_t getPlays(const char *line);
void calcCubes(cubes_t *cubes, const char *line);

int main(void) {
    const char *input = "input.txt";
    FILE *fd = fopen(input, "rt");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    cubes_t cubes[CUBE_QTD] = { 0 };
    char line[256] = { 0 };
    int i = 0;
    while (fgets(line, 255, fd) != NULL) {
        // printf("%s", line);
        cubes[i].plays = getPlays(line);
        // puts("getPlays OK!\n");
        cubes[i].id = getId(line);
        // puts("getId OK!\n");
        calcCubes(&cubes[i], line);
        // puts("calcCubes OK!");
        printf("==========\n Cube: %d\n    Id:%zu\n    Plays: %zu\n", i + 1, cubes[i].id, cubes[i].plays);
        if (cubes[i].notValid)
            printf("    Cube %d is not valid\n", i + 1);
        else
            printf("    Cube %d is valid\n", i + 1);
        i++;
    }
    fclose(fd);
    puts("==========\nCalculando: ");
    size_t soma = 0;
    for (int i = 0; i < CUBE_QTD; i++) {
        if (cubes[i].id == 0)
            continue;

        if (!cubes[i].notValid) {
            soma += cubes[i].id;
        }
    }
    printf("Resultado: %zu\n", soma);
    return 0;
}

size_t getId(const char *line) {
    char rst[5] = { 0 };
    char *delim = strchr(line, ':');
    // delim = strtok(NULL,":");
    for (int i = 5; i < delim - line; i++) {
        sprintf(rst, "%s%c", rst, line[i]);
        // printf("%d -> %c", i, line[i]);
    }
    return strtoull(rst, NULL, 10);
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
void calcCubes(cubes_t *cubes, const char *line) {
    size_t qtd = 0;
    char *start = strchr(line, ':');
    start++;
    char color[6] = { 0 };
    start = strtok(start, ",;");
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
