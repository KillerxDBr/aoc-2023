#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define da_free(xs)             \
    do                          \
    {                           \
        if ((xs)->count > 0)    \
        {                       \
            free((xs)->items);  \
            (xs)->count = 0;    \
            (xs)->capacity = 0; \
        }                       \
    } while (0)

#define da_free_ptr(xs)                              \
    do                                               \
    {                                                \
        if ((xs)->count > 0)                         \
        {                                            \
            for (size_t i = 0; i < (xs)->count; i++) \
                free((xs)->items[i]);                \
            free((xs)->items);                       \
            (xs)->count = 0;                         \
            (xs)->capacity = 0;                      \
        }                                            \
    } while (0)

#define da_append(xs, x)                                                               \
    do                                                                                 \
    {                                                                                  \
        if ((xs)->count >= (xs)->capacity)                                             \
        {                                                                              \
            if ((xs)->capacity == 0)                                                   \
                (xs)->capacity = 256;                                                  \
            else                                                                       \
                (xs)->capacity *= 2;                                                   \
            (xs)->items = realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items)); \
        }                                                                              \
        (xs)->items[(xs)->count++] = (x);                                              \
    } while (0)

typedef struct
{
    char **items;
    size_t count;
    size_t capacity;
} str;

// typedef struct
// {
//     size_t x, y, size;
//     bool isSymble;
// } Digit;

typedef struct vector2
{
    size_t x, y;
} vector2;

typedef struct node
{
    char c;
    vector2 pos;
    struct node *next;
} node_t;

typedef struct
{
    node_t *items;
    size_t count;
    size_t capacity;
} Digits;

typedef struct
{
    node_t *items;
    size_t count;
    size_t capacity;
} Symbles;

#define SMALL 0
#if SMALL
#define LINE_SIZE 10
#else
#define LINE_SIZE 140
#endif
int main(void)
{
#if SMALL
    const char *input = "small.txt";
#else
    const char *input = "input.txt";
#endif
    FILE *fd = fopen(input, "rt");
    if (fd == NULL)
    {
        fprintf(stderr, "Couldn't open file %s: ", input);
        perror(NULL);
        return 1;
    }

    str strings = {0};
    char *tmp = malloc(256);
    while (fgets(tmp, 255, fd) != NULL)
    {
        tmp[LINE_SIZE] = 0;
        tmp = realloc(tmp, strlen(tmp) + 1);
        da_append(&strings, tmp);
        tmp = malloc(256);
    }
    free(tmp);

    Digits digits = {0};
    Symbles symbles = {0};

    for (size_t i = 0; i < strings.count; i++)
    {
        size_t j = 0;
        while (strings.items[i][j] != 0)
        {
            size_t inc = 1;
            if (strings.items[i][j] != '.' && strings.items[i][j] != '\n')
            {
                node_t n = {0};
                n.pos.x = i;
                n.pos.y = j;
                n.c = strings.items[i][j];
                n.next = NULL;

                while (isdigit(n.c) && isdigit(strings.items[i][j + inc]))
                    inc++;

                if (!isdigit(n.c))
                {
                    da_append(&symbles, n);
                }
                else
                {

                    node_t *prev = &n;
                    for (size_t k = 1; k < inc; k++)
                    {
                        node_t *nextNode = malloc(sizeof(node_t));
                        nextNode->pos.x = n.pos.x;
                        nextNode->pos.y = n.pos.y + k;
                        nextNode->c = strings.items[i][j + k];
                        nextNode->next = NULL;
                        prev->next = nextNode;
                        prev = nextNode;
                    }
                    da_append(&digits, n);
                }
            }
            j += inc;
        }
    }

    size_t soma = 0;
    int nx, ny;

    for (size_t i = 0; i < symbles.count; i++)
    {
        node_t *prevHead = NULL;
        for (int j = -1; j < 2; j++)
        {
            for (int k = -1; k < 2; k++)
            {
                nx = symbles.items[i].pos.x + j;
                ny = symbles.items[i].pos.y + k;

                for (size_t l = 0; l < digits.count; l++)
                {
                    node_t *head = &digits.items[l];
                    node_t *ptr = head;
                    size_t len = 0;
                    bool counting = false;
                    while (ptr != NULL)
                    {
                        if (ptr->pos.x == nx && ptr->pos.y == ny)
                        {
                            counting = true;
                        }
                        len++;
                        ptr = ptr->next;
                    }
                    if (counting && head != prevHead)
                    {
                        prevHead = head;
                        char *tmp = calloc(len + 1, sizeof(char));
                        while (head != NULL)
                        {
                            sprintf(tmp, "%s%c", tmp, head->c);
                            head = head->next;
                        }
                        soma += strtoull(tmp, NULL, 10);
                    }
                }
            }
        }
    }

    da_free_ptr(&strings);
    da_free(&digits);
    da_free(&symbles);

    printf("Resultado final: %zu\n", soma);
    return 0;
}
