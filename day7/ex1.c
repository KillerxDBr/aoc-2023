// #define NOB_IMPLEMENTATION
#include "../include/nob.h"

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
    char c;
    uint8_t count;
} Token;

typedef struct {
    char cards[5];

    uint64_t bid;
    int64_t rank;
    Type type;

    Token tokens[5];
    uint8_t ntok;
} Hand;

typedef struct {
    Hand *items;
    size_t count, capacity;
} Hands;

void addValue(char *s, char ch, size_t len);

// #define SMALL

#ifndef SMALL
#define INPUT_FILE "input.txt"
#else
#define INPUT_FILE "exemple.txt"
#endif // SMALL

int sort_tokens(const void *a, const void *b) {
    //    return (*(int*)a - *(int*)b);
    return (((Token *)b)->count - ((Token *)a)->count);
}

int sort_ranks(const void *a, const void *b) {
    Hand h1 = *(Hand *)b;
    Hand h2 = *(Hand *)a;
    // printf("----------------------------\n");
    // printf("h1 -> " SV_Fmt "\nh2 ->" SV_Fmt "\n", 5, h1.cards, 5, h2.cards);

    int result = h1.type - h2.type;

    if (result == 0) {
        for (size_t i = 0; i < 5; ++i) {
            result = strchr(cards, h1.cards[i]) - strchr(cards, h2.cards[i]);
            if (result != 0)
                break;
        }
        // printf("Result: %d\n", result);
    }

    return result;
}

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
    uint64_t totalWinnings = 0;

    while (sv.count) {
        Nob_String_View line = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        memcpy(h.cards, line.data, 5);
        nob_sv_chop_by_delim(&line, ' ');

        line = nob_sv_trim(line);
        h.bid = nob_sv_to_u64(line);
        h.rank = -1;

        nob_da_append(&hands, h);
        memset(&h, 0, sizeof(h));
    }

    nob_sb_free(sb);
    memset(&sb, 0, sizeof(sb));
    memset(&sv, 0, sizeof(sv));

    char cs[5];
    for (size_t i = 0; i < hands.count; ++i) {
        memset(cs, 0, sizeof(cs));
        h = hands.items[i];

        for (size_t j = 0; j < 5; ++j) {
            char c = h.cards[j];
            uint8_t count = 0;

            if (memchr(cs, c, 5))
                continue;

            addValue(cs, c, sizeof(cs));

            for (size_t k = j; k < 5; ++k)
                if (c == h.cards[k])
                    count++;

            assert(count <= 5);
            h.tokens[j].c = c;
            h.tokens[j].count = count;
        }

        qsort(h.tokens, 5, sizeof(Token), sort_tokens);

        for (size_t j = 0; j < 5; ++j) {
            if (h.tokens[j].count && h.tokens[j].c) {
                ++h.ntok;
            }
        }

        hands.items[i] = h;
    }

    for (size_t i = 0; i < hands.count; ++i) {
        h = hands.items[i];

        switch (h.ntok) {
        case 5:
            h.type = HIGH_CARD;
            break;
        case 4:
            h.type = ONE_PAIR;
            break;
        case 3:
            if ((h.tokens[0].count == 3) && (h.tokens[1].count == 1)) { // Three of a kind -> TTT98
                h.type = THREE_KIND;
            } else if ((h.tokens[0].count == 2) && (h.tokens[1].count == 2)) { // Two pair -> 23432
                h.type = TWO_PAIR;
            } else
                NOB_UNREACHABLE("switch(h.ntok) case 3");
            break;
        case 2:
            if ((h.tokens[0].count == 3) && (h.tokens[1].count == 2)) { // Full House -> 23332
                h.type = FULL_HOUSE;
            } else if ((h.tokens[0].count == 4) && (h.tokens[1].count == 1)) { // Four of a kind -> AA8AA
                h.type = FOUR_KIND;
            } else {
                NOB_UNREACHABLE("switch(h.ntok) case 2");
            }
            break;
        case 1:
            h.type = FIVE_KIND;
            break;
        default:
            NOB_UNREACHABLE("switch(h.ntok)");
            break;
        }
        // h.rank = rank--;

        hands.items[i] = h;

        // printf("rank: %d\n", h.rank);
        // totalWinnings += (h.rank * h.bid);
    }

    qsort(hands.items, hands.count, sizeof(Hand), sort_ranks);
    printf("----------------------------\n");
    size_t rank = hands.count;
    for (size_t i = 0; i < hands.count; ++i) {
        h = hands.items[i];
        h.rank = rank--;

        printf("----------------------------\n");
        printf("Cards: " SV_Fmt "\n", 5, h.cards);
        printf("Bid: %zu\n", h.bid);

        for (size_t j = 0; j < 5; ++j) {
            if (!h.tokens[j].count)
                break;
            printf("'%c' -> %d %s\n", h.tokens[j].c, h.tokens[j].count, (h.tokens[j].count > 1) ? "times" : "time");
        }

        printf("Tokens -> " SV_Fmt " (%d)\n", (int)sizeof(cs), cs, h.ntok);
        printf("Rank -> %zd\n", h.rank);
        printf("Calc: %zu\n", (h.bid * h.rank));
        totalWinnings += (h.bid * h.rank);
    }
    printf("----------------------------\n");
    printf("Total Winnings: %zu\n", totalWinnings);

#ifdef SMALL
    if (totalWinnings != 6440) {
        printf("ERROR: totalWinnings (%llu) != 6440\n", totalWinnings);
    }
#endif

    fflush(stdout);

    nob_da_free(hands);

defer:
    return result;
}

void addValue(char *s, char ch, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (s[i] == 0) {
            s[i] = ch;
            return;
        }
    }
    NOB_UNREACHABLE("addValue");
}
