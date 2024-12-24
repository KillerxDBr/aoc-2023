#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __max
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#endif

int main(void) {
    const char *input = "day2/input.txt";
    FILE *fd = fopen(input, "rt");
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
                r = __max(r, n);
            } else if (strcmp(color, "green") == 0) {
                g = __max(g, n);
            } else if (strcmp(color, "blue") == 0) {
                b = __max(b, n);
            }
            start = strtok(NULL, ",;");
        }
        soma += (r * g * b);
    }
    printf("Resultado: %zu\n", soma);
    return 0;
}
