// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

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
    enum PipeShape ps;
} Cell;

typedef struct {
    Cell *items;
    size_t count;
    size_t capacity;
} Cells;

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

    Cells mapRows = {};

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        nob_return_defer(1);

    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));
    while (sv.count) {
        Nob_String_View line = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        nob_log(NOB_INFO, SV_Fmt, SV_Arg(line));
    }

defer:
    nob_sb_free(sb);

    nob_da_free(mapRows);

    return result;
}
