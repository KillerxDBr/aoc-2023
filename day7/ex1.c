#define NOB_IMPLEMENTATION
#include "../include/nob.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

typedef enum {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_KIND,
    FULL_HOUSE,
    FOUR_KIND,
    FIVE_KIND,
} Type;

const char cards[] = "23456789TJQKA";
#define CARDS_SIZE (sizeof(cards) - 1)

typedef struct {
    char c[5];
    uint8_t count[5];
} Tokens;

typedef struct {
    char cards[5];
    uint64_t bid;
    Type type;
    Tokens tokens;
} Hand;

typedef struct {
    Hand *items;
    size_t count, capacity;
} Hands;

void addValue(char *s, char c);

#define SMALL

#ifndef SMALL
#define INPUT_FILE "input.txt"
#else
#define INPUT_FILE "exemple.txt"
#endif // SMALL

int main(int argc, char **argv) {
    int result = 0;

    const char *program = nob_shift(argv, argc);
    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        nob_return_defer(1);
    }

    nob_temp_reset();
    Nob_String_Builder sb = { 0 };

    if (!nob_read_entire_file(INPUT_FILE, &sb)) {
        nob_return_defer(1);
    }

    Hand h = { 0 };
    Hands hands = { 0 };
    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));

    while (sv.count) {
        Nob_String_View line = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        memcpy(h.cards, line.data, 5);
        nob_sv_chop_by_delim(&line, ' ');

        line = nob_sv_trim(line);
        h.bid = nob_sv_to_u64(line);

        nob_da_append(&hands, h);
        memset(&h, 0, sizeof(h));
    }

    nob_sb_free(sb);
    memset(&sb, 0, sizeof(sb));
    memset(&sv, 0, sizeof(sv));

    // uint64_t b = 0;
    // for (size_t i = 0; i < hands.count; ++i)
    //     b = max(b, hands.items[i].bid);

    // const int isz = snprintf(NULL, 0, "%zu", hands.count);
    // const int bsz = snprintf(NULL, 0, "%zu", b);

    // nob_log(NOB_INFO, "Number of entries: %zu", hands.count);
    // for (size_t i = 0; i < hands.count; ++i) {
    //     nob_log(NOB_INFO, "Hand %*zu: " SV_Fmt " | Bid: %*zu", isz, i + 1, 5, hands.items[i].cards, bsz, hands.items[i].bid);
    // }

    char cs[5];
    for (size_t i = 0; i < hands.count; ++i) {
        memset(cs, 0, sizeof(cs));
        h = hands.items[i];

        printf("----------------------------\n");
        printf("Cards: " SV_Fmt "\n", 5, h.cards);
        printf("Bid: %zu\n", h.bid);

        for (size_t j = 0; j < 5; ++j) {
            char c = h.cards[j];
            int count = 0;

            if (memchr(cs, c, 5))
                continue;
            else
                addValue(cs, c);

            for (size_t k = j; k < 5; ++k)
                if (c == h.cards[k])
                    count++;

            assert(count <= 5);
            h.tokens.c[j] = c;
            h.tokens.count[j] = count;
        }

        for (size_t j = 0; j < 5; ++j) {
            if (h.tokens.count[j])
                printf("'%c' -> %d %s\n", h.tokens.c[j], h.tokens.count[j], (h.tokens.count[j] > 1) ? "times" : "time");
        }

        hands.items[i] = h;
        printf("Tokens -> " SV_Fmt "\n", 5, cs);
    }

    nob_da_free(hands);

defer:
    return result;
}

void addValue(char *s, char c) {
    while (*s != 0) s++;
    *s = c;
}
