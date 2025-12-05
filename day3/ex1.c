// #define NOB_IMPLEMENTATION
#include "../include/nob.h"

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} str;

// typedef struct
// {
//     size_t x, y, size;
//     bool isSymble;
// } Digit;

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

int main(int argc, char **argv) {
    const char *input = "input.txt";
    if (argc >= 2)
        input = argv[1];

    FILE *fd = fopen(input, "rb");
    if (fd == NULL) {
        fprintf(stderr, "[ERROR] Couldn't open file '%s': %s\n", input, strerror(errno));
        return 1;
    }

    str strings = {};
    char *tmp   = malloc(256);
    while (fgets(tmp, 255, fd) != NULL) {
        tmp[255] = 0;
        tmp      = realloc(tmp, strlen(tmp) + 1);
        nob_da_append(&strings, tmp);
        tmp = malloc(256);
    }

    free(tmp);
    fclose(fd);

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

                if (!isdigit(n.c)) {
                    nob_da_append(&symbles, n);
                } else {

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

    size_t soma = 0;

    Nob_String_Builder sb = {};
    for (size_t i = 0; i < symbles.count; i++) {
        node_t *prevHead = NULL;
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
                            counting = true;
                        }
                        ptr = ptr->next;
                    }
                    if (counting && head != prevHead) {
                        prevHead = head;
                        sb.count = 0;
                        while (head != NULL) {
                            nob_sb_append_buf(&sb, &head->c, 1);
                            head = head->next;
                        }
                        nob_sb_append_null(&sb);
                        soma += strtoull(sb.items, NULL, 10);
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < strings.count; ++i)
        free((void *)strings.items[i]);

    nob_da_free(strings);

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
    nob_sb_free(sb);

    printf("Resultado final: %zu\n", soma);
    return 0;
}
