// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "out.h"
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

enum Directions {
    DIR_N = (1 << 1),
    DIR_S = (1 << 2),
    DIR_E = (1 << 3),
    DIR_W = (1 << 4),

    DIR_NE = (DIR_N | DIR_E),
    DIR_NW = (DIR_N | DIR_W),
    DIR_SE = (DIR_S | DIR_E),
    DIR_SW = (DIR_S | DIR_W),
    DIR_EW = (DIR_E | DIR_W),
    DIR_NS = (DIR_N | DIR_S),
};

struct buf out;

static void test(Nob_Log_Level level, const char *fmt, va_list args) {
    switch (level) {
    case NOB_INFO:
        APPEND_STR(&out, "[INFO] ");
        break;
    case NOB_WARNING:
        APPEND_STR(&out, "[WARNING] ");
        break;
    case NOB_ERROR:
        APPEND_STR(&out, "[ERROR] ");
        break;
    case NOB_NO_LOGS:
        return;
    default:
        NOB_UNREACHABLE("test");
    }

    appendv(&out, (unsigned char *)fmt, args);
    append_byte(&out, '\n');
}

int main(int argc, char **argv) {
    unsigned char mem[1 << 16];
    OSFD fd;
#ifdef _WIN32
    fd = GetStdHandle(STD_OUTPUT_HANDLE);
    assert(fd != INVALID_HANDLE_VALUE);
#else
    fd = 1;
#endif
    out = (struct buf)FDBUF(fd, mem, sizeof(mem));

    nob_set_log_handler(test);

    int result = 0;

    const char *input = ProcessInput(argc, argv, __FILE__);
    assert(input != NULL);

    size_t startIndex = (size_t)-1;
    Cells map         = {};

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb)) nob_return_defer(1);

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

    // for (size_t i = 0; i < map.count; ++i) {
    //     nob_log(NOB_INFO, CELL_FMT, CELL_ARGS(map.items[i]));
    // }

    const v2i32 mapSize = {map.items[map.count - 1].pos.x + 1, map.items[map.count - 1].pos.y + 1};
    assert(((size_t)mapSize.x * (size_t)mapSize.y) == map.count);

    nob_log(NOB_INFO, "Map Size: (%d, %d)", mapSize.x, mapSize.y);
    nob_log(NOB_INFO, "Number of Map Cells: %zu", map.count);

    Cell start = map.items[startIndex];
    nob_log(NOB_INFO, "Starting Position: " CELL_FMT, CELL_ARGS(start));
    {
        // arr2d[y][x]
        const Cell(*arr2d)[mapSize.x] = (void *)map.items;

        enum DIR {
            DIR_NONE,
            DIR_N,
            DIR_E,
            DIR_S,
            DIR_W,
            DIR_COUNT,
        } dir = DIR_NONE;

        const char *dirLabels[] = {
            [DIR_NONE] = "DIR_NONE", [DIR_N] = "DIR_N", [DIR_E] = "DIR_E",
            [DIR_S] = "DIR_S",       [DIR_W] = "DIR_W", [DIR_COUNT] = "DIR_COUNT",
        };

        const Cell *c    = NULL;
        const Cell *next = NULL;

        if (start.pos.x > 0) { // East - Left
            c = &arr2d[start.pos.y][start.pos.x - 1];
            nob_log(NOB_INFO, "Testing %5s Position: " CELL_FMT, "Left", CELL_ARGS(*c));
            if (c->shape == PIPE_EW || c->shape == PIPE_NE || c->shape == PIPE_SE) {
                next = c;
                dir  = DIR_W;
            }
        }

        if (next == NULL) {
            if (start.pos.x < (mapSize.x - 1)) { // West - Right
                c = &arr2d[start.pos.y][start.pos.x + 1];
                nob_log(NOB_INFO, "Testing %5s Position: " CELL_FMT, "Right", CELL_ARGS(*c));
                if (c->shape == PIPE_EW || c->shape == PIPE_NW || c->shape == PIPE_SW) {
                    next = c;
                    dir  = DIR_E;
                }
            }
        }

        if (next == NULL) {
            if (start.pos.y > 0) { // North - Up
                c = &arr2d[start.pos.y - 1][start.pos.x];
                nob_log(NOB_INFO, "Testing %5s Position: " CELL_FMT, "Up", CELL_ARGS(*c));
                if (c->shape == PIPE_NS || c->shape == PIPE_SE || c->shape == PIPE_SW) {
                    next = c;
                    dir  = DIR_N;
                }
            }
        }

        if (next == NULL) {
            if (start.pos.y < (mapSize.y - 1)) { // South - Down
                c = &arr2d[start.pos.y + 1][start.pos.x];
                nob_log(NOB_INFO, "Testing %5s Position: " CELL_FMT, "Down", CELL_ARGS(*c));
                if (c->shape == PIPE_NS || c->shape == PIPE_NE || c->shape == PIPE_NW) {
                    next = c;
                    dir  = DIR_S;
                }
            }
        }

        assert(next != NULL);
        assert(dir > DIR_NONE && dir < DIR_COUNT);

        size_t steps = 0;
        nob_log(NOB_INFO, "Next Position: " CELL_FMT, CELL_ARGS(*next));
        while (next->shape != PIPE_POS) {
#if 0
            NOB_TODO("Count Cells");
#else
            switch (dir) {
            case DIR_N:
                switch (next->shape) {
                case PIPE_NS:
                    next = &arr2d[next->pos.y - 1][next->pos.x];
                    break;
                case PIPE_SE:
                    next = &arr2d[next->pos.y][next->pos.x + 1];
                    dir  = DIR_E;
                    break;
                case PIPE_SW:
                    next = &arr2d[next->pos.y][next->pos.x - 1];
                    dir  = DIR_W;
                    break;
                default:
                    NOB_UNREACHABLE("Pipe Shape North");
                    break;
                }
                break;
            case DIR_S:
                switch (next->shape) {
                case PIPE_NS:
                    next = &arr2d[next->pos.y + 1][next->pos.x];
                    break;
                case PIPE_NE:
                    next = &arr2d[next->pos.y][next->pos.x + 1];
                    dir  = DIR_W;
                    break;
                case PIPE_NW:
                    next = &arr2d[next->pos.y][next->pos.x - 1];
                    dir  = DIR_E;
                    break;
                default:
                    NOB_UNREACHABLE("Pipe Shape South");
                    break;
                }
                break;
            case DIR_E:
                switch (next->shape) {
                case PIPE_EW:
                    next = &arr2d[next->pos.y][next->pos.x + 1];
                    break;
                case PIPE_NW:
                    next = &arr2d[next->pos.y - 1][next->pos.x];
                    dir  = DIR_N;
                    break;
                case PIPE_SW:
                    next = &arr2d[next->pos.y + 1][next->pos.x];
                    dir  = DIR_S;
                    break;
                default:
                    NOB_UNREACHABLE("Pipe Shape East");
                    break;
                }
                break;
            case DIR_W:
                switch (next->shape) {
                case PIPE_EW:
                    next = &arr2d[next->pos.y][next->pos.x - 1];
                    break;
                case PIPE_NE:
                    next = &arr2d[next->pos.y - 1][next->pos.x];
                    dir  = DIR_N;
                    break;
                case PIPE_SE:
                    next = &arr2d[next->pos.y + 1][next->pos.x];
                    dir  = DIR_S;
                    break;
                default:
                    NOB_UNREACHABLE("Pipe Shape West");
                    break;
                }
                break;
            default:
                NOB_UNREACHABLE("Direction");
                break;
            }
            nob_log(NOB_INFO, "Next Position: " CELL_FMT, CELL_ARGS(*next));
            nob_log(NOB_INFO, "New Direction: %s", dirLabels[dir]);
            steps++;
            // break;
#endif
        }
        size_t resposta = (steps / 2) + (steps % 2);
        nob_log(NOB_INFO, "Resultado: %zu", resposta);
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

    flush(&out);

    return result;
}
