#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/da.h"

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} str;

typedef struct vector2 {
    size_t x, y;
} vector2;

typedef struct node {
    char c;
    vector2 pos;
    struct node *next;
} node_t;

typedef struct {
    node_t *items;
    size_t count;
    size_t capacity;
} Digits;

typedef struct {
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
int main(void) {
#if SMALL
    const char *input = "day3/small.txt";
#else
    const char *input = "day3/input.txt";
#endif
    FILE *fd = fopen(input, "rt");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    str strings = { 0 };
    char *tmp = malloc(256);
    while (fgets(tmp, 255, fd) != NULL) {
        tmp[LINE_SIZE] = 0;
        tmp = realloc(tmp, strlen(tmp) + 1);
        da_append(&strings, tmp);
        tmp = malloc(256);
    }
    free(tmp);

    Digits digits = { 0 };
    Symbles symbles = { 0 };

    for (size_t i = 0; i < strings.count; i++) {
        size_t j = 0;
        while (strings.items[i][j] != 0) {
            size_t inc = 1;
            if (strings.items[i][j] != '.' && strings.items[i][j] != '\n') {
                node_t n = { 0 };
                n.pos.x = i;
                n.pos.y = j;
                n.c = strings.items[i][j];
                n.next = NULL;

                while (isdigit(n.c) && isdigit(strings.items[i][j + inc]))
                    inc++;

                if (n.c == '*') {
                    da_append(&symbles, n);
                } else if (isdigit(n.c)) {

                    node_t *prev = &n;
                    for (size_t k = 1; k < inc; k++) {
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

    size_t soma = 0, tmpSoma = 1;
    int nx, ny;

    for (size_t i = 0; i < symbles.count; i++) {
        // node_t *prevHead = NULL;
        bool countedNumbers[digits.count];
        for (int b = 0; b < digits.count; b++)
            countedNumbers[b] = false;
        size_t count = 0;
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; k++) {
                nx = symbles.items[i].pos.x + j;
                ny = symbles.items[i].pos.y + k;
                for (size_t l = 0; l < digits.count; l++) {
                    node_t *head = &digits.items[l];
                    node_t *ptr = head;
                    bool counting = false;
                    while (ptr != NULL) {
                        if (ptr->pos.x == nx && ptr->pos.y == ny) {
                            if (countedNumbers[l])
                                goto brklp;
                            countedNumbers[l] = true;
                            counting = true;
                        }
                        if (counting) {
                            char digitsToConvert[10] = { 0 };
                            node_t *ptr2 = head;
                            while (ptr2 != NULL) {
                                digitsToConvert[strlen(digitsToConvert)] = ptr2->c;
                                ptr2 = ptr2->next;
                            }
                            printf("%s\n", digitsToConvert);
                            tmpSoma *= strtoull(digitsToConvert, NULL, 10);
                            count++;
                            counting = false;
                        }
                        ptr = ptr->next;
                    }
                brklp:;
                }
                if (count == 2 && tmpSoma != 1) {
                    printf("Somando %zu a %zu\n", tmpSoma, soma);
                    soma += tmpSoma;
                    tmpSoma = 1;
                }
            }
        }
        printf("Simbolo %zu tem %zu numeros\n", i + 1, count);
        tmpSoma = 1;
    }

    da_free_ptr(&strings);
    da_free(&digits);
    da_free(&symbles);

    printf("Resultado final: %zu\n", soma);
    return 0;
}
