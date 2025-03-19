#define NOB_IMPLEMENTATION
#include "../include/nob.h"

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

int main(int argc, char **argv) {
    const char *program = nob_shift(argv, argc);

    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        return 1;
    }
#if SMALL
    const char *input = "small.txt";
#else
    const char *input = "input.txt";
#endif
    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    str strings = { 0 };
    char *tmp = malloc(256);
    while (fgets(tmp, 255, fd) != NULL) {
        tmp[LINE_SIZE] = 0;
        tmp = realloc(tmp, strlen(tmp) + 1);
        nob_da_append(&strings, tmp);
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
                    nob_da_append(&symbles, n);
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
                    nob_da_append(&digits, n);
                }
            }
            j += inc;
        }
    }

    size_t soma = 0, tmpSoma = 1;
    size_t nx, ny;

    bool *countedNumbers = NULL;
    for (size_t i = 0; i < symbles.count; i++) {
        // node_t *prevHead = NULL;
        countedNumbers = realloc(countedNumbers, digits.count);
        for (size_t b = 0; b < digits.count; b++)
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

    free(countedNumbers);

    for (size_t i = 0; i < strings.count; ++i)
        free((void *)strings.items[i]);

    nob_da_free(strings);

    nob_da_free(digits);
    nob_da_free(symbles);

    printf("Resultado final: %zu\n", soma);
    return 0;
}
