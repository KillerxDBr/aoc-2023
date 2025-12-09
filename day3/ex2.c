// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} str;

typedef struct vector2 {
    ssize_t x, y;
} vector2;

typedef struct node node_t;
struct node {
    char c;
    vector2 pos;
    node_t *next;
};

typedef struct {
    node_t *items;
    size_t count;
    size_t capacity;
} Nodes;

#define SMALL 0
#if SMALL
#define LINE_SIZE 10
#else
#define LINE_SIZE 140
#endif

int main(int argc, char **argv) {
    const char *input;
    if (argc > 1)
        input = argv[1];
    else {
        char *fullPath = GetFullPath(__FILE__, NULL, 0);
        if (fullPath != NULL) {
            if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(fullPath) - fullPath), fullPath))) {
                return 1;
            }

            free(fullPath);
        }

#ifdef SMALL
        input = "small.txt";
#else
        input = "input.txt";
#endif
    }

    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    str strings = {};
    char *tmp   = malloc(256);
    while (fgets(tmp, 255, fd) != NULL) {
        tmp[LINE_SIZE] = 0;
        tmp            = realloc(tmp, strlen(tmp) + 1);
        nob_da_append(&strings, tmp);
        tmp = malloc(256);
    }
    free(tmp);

    Nodes digits  = {};
    Nodes symbles = {};

    for (size_t i = 0; i < strings.count; i++) {
        size_t j = 0;
        while (strings.items[i][j] != 0) {
            size_t inc = 1;
            if (strings.items[i][j] != '.' && strings.items[i][j] != '\n') {
                node_t n = {};
                n.pos.x  = i;
                n.pos.y  = j;
                n.c      = strings.items[i][j];
                n.next   = NULL;

                while (isdigit(n.c) && isdigit(strings.items[i][j + inc]))
                    inc++;

                if (n.c == '*') {
                    nob_da_append(&symbles, n);
                } else if (isdigit(n.c)) {

                    node_t *prev = &n;
                    for (size_t k = 1; k < inc; k++) {
                        node_t *nextNode = calloc(1, sizeof(node_t));

                        nextNode->pos.x = n.pos.x;
                        nextNode->pos.y = n.pos.y + k;
                        nextNode->c     = strings.items[i][j + k];
                        nextNode->next  = NULL;

                        prev->next = nextNode;
                        prev       = nextNode;
                    }
                    nob_da_append(&digits, n);
                }
            }
            j += inc;
        }
    }

    size_t soma    = 0;
    size_t tmpSoma = 1;

    bool *countedNumbers  = NULL;
    Nob_String_Builder sb = {};
    for (size_t i = 0; i < symbles.count; i++) {
        // node_t *prevHead = NULL;
        countedNumbers = realloc(countedNumbers, digits.count);
        memset(countedNumbers, 0, digits.count);
        size_t count = 0;
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; k++) {
                ssize_t nx = symbles.items[i].pos.x + j;
                ssize_t ny = symbles.items[i].pos.y + k;
                for (size_t l = 0; l < digits.count; l++) {
                    node_t *head  = &digits.items[l];
                    node_t *ptr   = head;
                    bool counting = false;
                    while (ptr != NULL) {
                        if (ptr->pos.x == nx && ptr->pos.y == ny) {
                            if (countedNumbers[l])
                                break;
                            countedNumbers[l] = true;
                            counting          = true;
                        }
                        if (counting) {
                            node_t *ptr2 = head;
                            while (ptr2 != NULL) {
                                nob_sb_append_buf(&sb, &head->c, 1);
                                ptr2 = ptr2->next;
                            }
                            nob_sb_append_null(&sb);
                            printf("%s\n", sb.items);
                            tmpSoma *= strtoull(sb.items, NULL, 10);
                            count++;
                            counting = false;
                        }
                        ptr = ptr->next;
                    }
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
    nob_sb_free(sb);

    node_t *p1 = NULL;
    node_t *p2 = NULL;
    for (size_t i = 0; i < digits.count; ++i) {
        p1 = digits.items[i].next;

        if (p1 == NULL) {
            continue;
        }

        if (p1->next != NULL) {
            p2 = p1->next;
            while (p2->next) {
                p2 = p1->next;
                free(p1);
                p1 = p2;
                p2 = p2->next;
            }
            free(p2);
        }
        free(p1);
    }

    nob_da_free(digits);
    nob_da_free(symbles);

    printf("Resultado final: %zu\n", soma);
    return 0;
}
