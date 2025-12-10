// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

typedef union {
    uint32_t v;
    char c[4];
} NodeValue;

static_assert(sizeof(NodeValue) == sizeof(uint32_t), "Size of NodeValue is different of size of uint32");

typedef struct {
    NodeValue node;
    NodeValue left;
    NodeValue right;
} Node;

typedef struct {
    Node *items;
    size_t count;
    size_t capacity;
} Nodes;

typedef struct {
    NodeValue *items;
    size_t count;
    size_t capacity;
} nvArr;

// #define SMALL
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

    nvArr pos   = {};
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
        assert(n.node.v >= '000' && n.node.v <= 'ZZZ');

        memcpy(&n.left, b.data, b.count);
        assert(n.left.v >= '000' && n.left.v <= 'ZZZ');

        memcpy(&n.right, c.data, c.count);
        assert(n.right.v >= '000' && n.right.v <= 'ZZZ');

        nob_da_append(&nodes, n);
    }

    for (size_t i = 0; i < nodes.count; ++i) {
        Node n = nodes.items[i];
        nob_log(NOB_INFO, "Node: %s Left: %s Right: %s", n.node.c, n.left.c, n.right.c);
    }

    for (size_t i = 0; i < nodes.count; ++i) {
        nob_log(NOB_INFO, "%s -> %c", nodes.items[i].node.c, nodes.items[i].node.c[2]);
        if (nodes.items[i].node.c[2] == 'A') {
            nob_da_append(&pos, nodes.items[i].node);
        }
    }
    nob_log(NOB_INFO, "pos.count: %zu", pos.count);
    for (size_t i = 0; i < pos.count; ++i) {
        nob_log(NOB_INFO, "start pos: %s", pos.items[i].c);
    }

    size_t end    = 0;
    size_t nMoves = 0;
    for (; end != pos.count; ++nMoves) {
        nob_log(NOB_INFO, "Moves: %zu", nMoves);
        end = 0;
        for (size_t i = 0; i < pos.count; ++i) {
            NodeValue p = pos.items[i];
            for (size_t j = 0; j < nodes.count; ++j) {
                if (nodes.items[j].node.v == p.v) {
                    switch (instructions.data[nMoves % instructions.count]) {
                    case 'R':
                        // printf("Right\n");
                        p = nodes.items[j].right;
                        break;
                    case 'L':
                        // printf("Left\n");
                        p = nodes.items[j].left;
                        break;
                    default:
                        NOB_UNREACHABLE("Invalid Instruction");
                        break;
                    }
                    break;
                }
            }
            pos.items[i] = p;
            end += (pos.items[i].c[2] == 'Z');
        }
    }

    nob_log(NOB_INFO, "Found destination in %zu steps", nMoves);

defer:
    nob_sb_free(sb);
    nob_da_free(nodes);
    nob_da_free(pos);

    return result;
}
