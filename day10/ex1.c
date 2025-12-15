// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

#include <inttypes.h>

typedef int32_t i32;
typedef uint32_t u32;

enum PipeShape {
    PIPE_NULL = 0,
    PIPE_NS   = '|',
    PIPE_EW   = '-',
    PIPE_NE   = 'L',
    PIPE_NW   = 'J',
    PIPE_SW   = '7',
    PIPE_SE   = 'F',
    PIPE_GND  = '.',
    PIPE_POS  = 'S',
};

typedef struct {
    i32 x, y;
} v2i32;

typedef struct {
    v2i32 pos;
    enum PipeShape shape;
} Cell;

#define CELL_FMT     "(%" PRId32 ", %" PRId32 ") = '%c' (%d)"
#define CELL_ARGS(c) (c).pos.x, (c).pos.y, (c).shape, (c).shape

typedef struct {
    Cell *items;
    size_t count;
    size_t capacity;
} Cells;

int main(int argc, char **argv) {
    int result = 0;

    const char *input = ProcessInput(argc, argv, __FILE__);
    assert(input != NULL);

    Cells mapRows = {};

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        nob_return_defer(1);

    size_t lineCount   = 0;
    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));
    while (sv.count) {
        Nob_String_View line = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));

        for (size_t i = 0; i < line.count; ++i) {
            Cell c = {
                .pos   = (v2i32){.x = i, .y = lineCount},
                .shape = line.data[i],
            };
            switch (c.shape) {
            case PIPE_NS:
            case PIPE_EW:
            case PIPE_NE:
            case PIPE_NW:
            case PIPE_SW:
            case PIPE_SE:
            case PIPE_GND:
            case PIPE_POS:
                break;
            default:
                nob_log(NOB_ERROR, "Invalid Cell Shape: " CELL_FMT, CELL_ARGS(c));
                nob_return_defer(1);
                break;
            }
            nob_da_append(&mapRows, c);
        }

        lineCount++;
    }

    for (size_t i = 0; i < mapRows.count; ++i) {
        nob_log(NOB_INFO, CELL_FMT, CELL_ARGS(mapRows.items[i]));
    }

defer:
    nob_sb_free(sb);
    nob_da_free(mapRows);

    return result;
}
