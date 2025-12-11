// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

typedef struct {
    // char node[3];
    // char left[3];
    // char right[3];
    uint32_t node;
    uint32_t left;
    uint32_t right;
} Node;

typedef struct {
    Node *items;
    size_t count;
    size_t capacity;
} Nodes;

#define SMALL
int main(int argc, char **argv) {
    int result = 0;

    const char *input = NULL;
    if (argc > 1)
        input = argv[1];
    else {
        char *fullPath = GetFullPath(__FILE__, NULL, 0);
        if (fullPath != NULL) {
            fullPath[nob_path_name(fullPath) - fullPath] = '\0';
            if (!nob_set_current_dir(fullPath)) {
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

    Nodes nodes = {};

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        nob_return_defer(1);

    Nob_String_View sv           = nob_sv_trim(nob_sb_to_sv(sb));
    Nob_String_View instructions = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));

    // nob_log(NOB_INFO, "Instructions: " SV_Fmt, SV_Arg(instructions));
    sv = nob_sv_trim(sv);
    while (sv.count) {
        Node n = {};
        Nob_String_View a, b, c;
        Nob_String_View line = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n')); // AAA = (BBB, CCC)

        a = nob_sv_trim(nob_sv_chop_by_delim(&line, ' '));

        nob_sv_chop_by_delim(&line, '(');
        b = nob_sv_trim(nob_sv_chop_by_delim(&line, ','));

        nob_sv_chop_by_delim(&line, ' ');
        c = nob_sv_trim(nob_sv_chop_by_delim(&line, ')'));

        // nob_log(NOB_INFO, "a = " SV_Fmt " b = " SV_Fmt " c = " SV_Fmt, SV_Arg(a), SV_Arg(b), SV_Arg(c));
        memcpy(&n.node, a.data, a.count);
        // assert(n.node >= 'AAA' && n.node <= 'ZZZ');

        memcpy(&n.left, b.data, b.count);
        // assert(n.left >= 'AAA' && n.left <= 'ZZZ');

        memcpy(&n.right, c.data, c.count);
        // assert(n.right >= 'AAA' && n.right <= 'ZZZ');

        nob_da_append(&nodes, n);
    }

    for (size_t i = 0; i < nodes.count; ++i) {
        Node n = nodes.items[i];
        nob_log(NOB_INFO, "Node: %s Left: %s Right: %s", (char *)&n.node, (char *)&n.left, (char *)&n.right);
    }

    uint32_t pos  = *(uint32_t*)"AAA";
    size_t nMoves = 0;
    for (; pos != *(uint32_t*)"ZZZ"; ++nMoves) {
        Node n;
        for (size_t j = 0; j < nodes.count; ++j) {
            if (nodes.items[j].node == pos) {
                n = nodes.items[j];
                break;
            }
        }
        switch (instructions.data[nMoves % instructions.count]) {
        case 'R':
            nob_log(NOB_INFO, "%s -> %s", (char *)&pos, (char *)&n.right);
            pos = n.right;
            break;
        case 'L':
            nob_log(NOB_INFO, "%s -> %s", (char *)&pos, (char *)&n.left);
            pos = n.left;
            break;

        default:
            NOB_UNREACHABLE("Invalid Instruction");
            break;
        }
    }

    nob_log(NOB_INFO, "Found destination in %zu steps", nMoves);
defer:
    nob_sb_free(sb);
    nob_da_free(nodes);

    return result;
}
