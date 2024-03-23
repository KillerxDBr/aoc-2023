#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tgmath.h>

int main(void)
{
    const char *numeros[] = {
        "one", "two", "three", "four",
        "five", "six", "seven", "eight", "nine",
        "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    size_t numerosCount = sizeof(numeros) / sizeof(char *);
    // printf("%zu\n",numerosCount);
    const char *input = "input.txt";
    FILE *fd = fopen(input, "rt");
    if (fd == NULL)
    {
        fprintf(stderr, "Couldn't open file %s\n", input);
        return 1;
    }

    char line[256];

    // int rst[1024][1024];
    // for (int i = 0; i < 1024; i++)
    // {
    //     for (int j = 0; j < 1024; j++)
    //     {
    //         rst[i][j] = -1;
    //     }
    // }
    size_t soma = 0, n1 = 0, n2 = 0;
    while (fgets(line, 255, fd) != NULL)
    {
        line[strlen(line) - 1] = '\0';
        // assert(line == "eighttkbtzjz6nineeight");
        printf("%s -> (%p)\n", line, line);
        // puts("OK!");
        size_t minDist = -1, maxDist = -1;
        // strcpy(line, "seven9fourpdseven1four2eight\n");
        for (size_t i = 0; i < numerosCount; i++)
        {
            char *tmp = strstr(line, numeros[i]);
            if (tmp != NULL)
            {
                while (tmp != NULL)
                {
                    if (minDist == -1)
                        minDist = tmp - line;
                    if (maxDist == -1)
                        maxDist = tmp - line;
                    maxDist = fmax(maxDist, tmp - line);
                    minDist = fmin(minDist, tmp - line);
                    printf("minDist: %zu\nmaxDist: %zu\n",minDist,maxDist);
                    tmp++;
                    tmp = strstr(tmp, numeros[i]);
                }
            }
        }
        n1 = 0;
        n2 = 0;
        char final[6] = {0};
        // strncpy(final, fdigit, 6);
        strncpy(final, line + minDist, 6);
        final[5] = 0;
        for (int i = 0; i < numerosCount; i++)
        {
            if (strncmp(final, numeros[i], strlen(numeros[i])) == 0)
            {
                if (i > 8)
                {
                    n1 = (final[0] - '0') * 10;
                    break;
                }
                else
                {
                    n1 = (i + 1) * 10;
                    break;
                }
            }
        }
        // strncpy(final, ldigit, 6);
        strncpy(final, line + maxDist, 6);
        final[5] = 0;
        for (int i = 0; i < numerosCount; i++)
        {
            if (strncmp(final, numeros[i], strlen(numeros[i])) == 0)
            {
                if (i > 8)
                {
                    n2 = final[0] - '0';
                    break;
                }
                else
                {
                    n2 = i + 1;
                    break;
                }
            }
        }
        printf("String %s adicionando %zu\n\tn1 = %zu\n\tn2 = %zu\n============\n", line, (n1 + n2), n1, n2);
        soma += (n1 + n2);
        // break;
    }
    fclose(fd);
    printf("Resultado final: %zu\n", soma);
    return 0;
}