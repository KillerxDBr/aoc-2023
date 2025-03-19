#define NOB_IMPLEMENTATION
#include "../include/nob.h"
#include <minwindef.h>

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

int main(int argc, char **argv) {
    int result = 0;

    const char *program = nob_shift(argv, argc);
    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        nob_return_defer(1);
    }

    Hands hands = { 0 };
    Nob_String_Builder sb = { 0 };
    Nob_String_View sv = { 0 };

    if (!nob_read_entire_file("exemple.txt", &sb)) {
        nob_return_defer(1);
    }

    sv = nob_sv_trim(nob_sb_to_sv(sb));

    Hand h;
    while (sv.count) {
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        memcpy(h.cards, sv2.data, 5);
        nob_sv_chop_by_delim(&sv2, ' ');

        sv2 = nob_sv_trim(sv2);
        h.bid = nob_sv_to_u64(sv2);

        nob_da_append(&hands, h);
    }

    nob_sb_free(sb);
    memset(&sb, 0, sizeof(sb));
    memset(&sv, 0, sizeof(sv));

    nob_log(NOB_INFO, "Number of entries: %zu", hands.count);
    for (size_t i = 0; i < hands.count; ++i) {
        h = hands.items[i];
        nob_log(NOB_INFO, "Hand: " SV_Fmt " | Bid %zu", 5, h.cards, h.bid);
    }

defer:
    nob_da_free(hands);
    return result;
}
