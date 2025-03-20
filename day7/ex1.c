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
    char cards[5];
    uint64_t bid;
    Type type;
} Hand;

typedef struct {
    Hand *items;
    size_t count, capacity;
} Hands;

#define SMALL

#ifdef SMALL
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

    Hands hands = { 0 };
    Nob_String_Builder sb = { 0 };
    Nob_String_View sv = { 0 };

    if (!nob_read_entire_file(INPUT_FILE, &sb)) {
        nob_return_defer(1);
    }

    sv = nob_sv_trim(nob_sb_to_sv(sb));

    Hand h = { 0 };
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

    uint64_t b = 0;
    for (size_t i = 0; i < hands.count; ++i)
        b = max(b, hands.items[i].bid);

    const int isz = snprintf(NULL, 0, "%zu", hands.count - 1);
    // printf("Max Bid: %zu\n", b);
    const int bsz = snprintf(NULL, 0, "%zu", b);

    nob_log(NOB_INFO, "Number of entries: %zu", hands.count);
    for (size_t i = 0; i < hands.count; ++i) {
        nob_log(NOB_INFO, "Hand %*zu: " SV_Fmt " | Bid: %*zu", isz, i, 5, hands.items[i].cards, bsz, hands.items[i].bid);
    }

defer:
    nob_da_free(hands);
    return result;
}
