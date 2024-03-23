#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 256

int main(void)
{
    const char *input = "input.txt";
    FILE *fd = fopen(input, "rt");
    if (fd == NULL)
    {
        fprintf(stderr, "Couldn't open file %s\n", input);
        return 1;
    }
    size_t soma = 0;
    char line[256];
    size_t fdigit = 0;
    size_t ldigit = 0;
    while (fgets(line, 255, fd) != NULL)
    {
        for (int i = 0; i < strlen(line); ++i)
        {
            if (isdigit(line[i]))
            {
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