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

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

int main(int argc, char **argv) {
    int result = 0;

    const char *input = ProcessInput(argc, argv, __FILE__);
    assert(input != NULL);

    size_t startIndex = (size_t)-1;
    Cells map         = {};

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
                break;
            case PIPE_POS:
                assert(startIndex == ((size_t)-1));
                startIndex = map.count;
                break;
            default:
                nob_log(NOB_ERROR, "Invalid Cell Shape: " CELL_FMT, CELL_ARGS(c));
                nob_return_defer(1);
                break;
            }
            nob_da_append(&map, c);
        }

        lineCount++;
    }

    assert(startIndex < map.count);

    // Cell start = map.items[startIndex];
    Cell start = map.items[0];
    nob_log(NOB_INFO, "Starting Position: " CELL_FMT, CELL_ARGS(start));

    // for (size_t i = 0; i < map.count; ++i) {
    //     nob_log(NOB_INFO, CELL_FMT, CELL_ARGS(map.items[i]));
    // }

    const v2i32 mapSize = {map.items[map.count - 1].pos.x + 1, map.items[map.count - 1].pos.y + 1};
    assert(((size_t)mapSize.x * (size_t)mapSize.y) == map.count);

    nob_log(NOB_INFO, "Map Size: (%d, %d)", mapSize.x, mapSize.y);
    nob_log(NOB_INFO, "Number of Map Cells: %zu", map.count);

    {
        // arr2d[y][x]
        const Cell(*arr2d)[mapSize.x] = (void *)map.items;

        bool up = false;
        bool dw = false;
        bool lf = false;
        bool rt = false;

        const Cell *c = NULL;
        if (start.pos.x > 0) { // East - Left
            c = &arr2d[start.pos.y][start.pos.x - 1];
            if (c->shape == PIPE_EW || c->shape == PIPE_NE || c->shape == PIPE_SE)
                lf = true;
        }

        if (start.pos.x < (mapSize.x - 1)) { // West - Right
            c = &arr2d[start.pos.y][start.pos.x + 1];
            if (c->shape == PIPE_EW || c->shape == PIPE_NW || c->shape == PIPE_SW)
                rt = true;
        }

        if (start.pos.y > 0) { // South - Down
            c = &arr2d[start.pos.y + 1][start.pos.x];
            if (c->shape == PIPE_NS || c->shape == PIPE_SW || c->shape == PIPE_SE)
                dw = true;
        }

        if (start.pos.y < (mapSize.y - 1)) { // North - Up
            c = &arr2d[start.pos.y - 1][start.pos.x];
            if (c->shape == PIPE_NS || c->shape == PIPE_NE || c->shape == PIPE_NW) {
                up = true;
            }
        }

#define BOOLLOG(b) nob_log(NOB_INFO, "%s = %s", (#b), (b) ? "True" : "False")

        BOOLLOG(up);
        BOOLLOG(dw);
        BOOLLOG(lf);
        BOOLLOG(rt);
    }

#if 0
    for (size_t i = 0; i < map.count; ++i) {
        i      = startIndex;
        Cell c = map.items[i];
        nob_log(NOB_INFO, "Testing Cell: " CELL_FMT, CELL_ARGS(c));
        // ssize_t index = i + xoff + (yoff * mapSize.x);
        ssize_t index;
        for (i32 xoff = -1; xoff < 2; ++xoff) {
            if (xoff == 0)
                continue;
            index = i + xoff;

            if (index < 0 || index >= (ssize_t)map.count)
                continue;

            nob_log(NOB_INFO, "%c Adjacent Cell: " CELL_FMT, 'X', CELL_ARGS(map.items[index]));
        }

        for (i32 yoff = -1; yoff < 2; ++yoff) {
            if (yoff == 0)
                continue;
            index = i + (yoff * mapSize.x);

            if (index < 0 || index >= (ssize_t)map.count)
                continue;

            nob_log(NOB_INFO, "%c Adjacent Cell: " CELL_FMT, 'Y', CELL_ARGS(map.items[index]));
        }
        break;
    }
#endif

defer:
    nob_sb_free(sb);
    nob_da_free(map);

    return result;
}
