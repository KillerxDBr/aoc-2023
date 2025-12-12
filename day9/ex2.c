// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

#include <inttypes.h>

typedef int64_t i64;

typedef struct {
    i64 *items;
    size_t count;
    size_t capacity;
} DataLine;

typedef struct {
    DataLine *items;
    size_t count;
    size_t capacity;
} Data;

static inline bool IsDataLineZeroed(DataLine dl) {
    for (size_t i = 0; i < dl.count; ++i) {
        if (dl.items[i])
            return false;
    }
    return true;
}

static inline void reverseArray(DataLine *dl) {
    size_t start = 0;
    size_t end = dl->count - 1;
    i64 temp; // Temporary variable for swapping

    // Loop until the start index is less than the end index
    while (start < end) {
        // Swap the elements at the start and end positions
        temp = dl->items[start];
        dl->items[start] = dl->items[end];
        dl->items[end] = temp;

        // Move the pointers towards the center
        start++;
        end--;
    }
}

// #define SMALL
int main(int argc, char **argv) {
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

    Data data             = {};
    DataLine dl           = {};
    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        return 1;

    Nob_String_View sv = nob_sv_trim(nob_sb_to_sv(sb));
    while (sv.count) {
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        memset(&dl, 0, sizeof(dl));
        while (sv2.count) {
            sv2   = nob_sv_trim(sv2);
            i64 n = nob_sv_chop_i64(&sv2);
            nob_da_append(&dl, n);
        }
        reverseArray(&dl);
        nob_da_append(&data, dl);
    }

    nob_sb_free(sb);
    memset(&sb, 0, sizeof(sb));

    i64 finalResult = 0;
    Data ext        = {};
    for (size_t i = 0; i < data.count; ++i) {
        // puts("--------------------------------");
        ext.count = 0;
        nob_da_append(&ext, data.items[i]);
        assert(ext.count > 0);
        DataLine *last = &ext.items[ext.count - 1];
        while (true) {
            memset(&dl, 0, sizeof(dl));
            for (size_t j = 1; j < last->count; ++j) {
                i64 n = last->items[j] - last->items[j - 1];
                nob_da_append(&dl, n);
            }
            if (IsDataLineZeroed(dl)) {
                nob_da_free(dl);
                break;
            }
            nob_da_append(&ext, dl);
            last = &ext.items[ext.count - 1];
        }

        for (size_t j = 0; j < ext.count; ++j) {
            finalResult += ext.items[j].items[ext.items[j].count - 1];
            nob_da_free(ext.items[j]);
        }
    }

    nob_log(NOB_INFO, "Result: %" PRIi64, finalResult);

    nob_da_free(ext);
    nob_da_free(data);
    nob_sb_free(sb);

    return 0;
}
