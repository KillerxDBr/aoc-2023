#define NOB_IMPLEMENTATION
#include "../include/nob.h"

#define SV_IMPLEMENTATION
#include "../include/sv.h"

typedef Nob_File_Paths Strings;

const char *labels[] = {
    "Time:",
    "Distance:",
};

typedef struct {
    size_t *items;
    size_t count, capacity;
} stArr;

uint64_t kxd_sv_chop_u64(Nob_String_View *sv);

// #define SMALL
int main(void) {
#ifdef SMALL
    const char *input = "day6/small.txt";
#else
    const char *input = "day6/input.txt";
#endif
    int result = 0;
    Nob_String_Builder sb = { 0 };

    if (!nob_read_entire_file(input, &sb)) {
        nob_sb_free(sb);
        return 1;
    }
    Nob_String_View sv = nob_sb_to_sv(sb);

    Nob_String_View tLine = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
    Nob_String_View dLine = nob_sv_trim(sv);

    sv_chop_by_sv(&tLine, nob_sv_from_cstr(labels[0]));
    sv_chop_by_sv(&dLine, nob_sv_from_cstr(labels[1]));

    tLine = nob_sv_trim(tLine);
    dLine = nob_sv_trim(dLine);

    printf("------------------------------------\n");
    printf("tLine: '" SV_Fmt "'\n", SV_Arg(tLine));
    printf("dLine: '" SV_Fmt "'\n", SV_Arg(dLine));

    size_t t = kxd_sv_chop_u64(&tLine);
    size_t d = kxd_sv_chop_u64(&dLine);
    size_t rst = 0;

    printf("t: %zu\n", t);
    printf("d: %zu\n", d);

    for (int vel = 1; vel < t; ++vel) {
        size_t dt = vel * (t - vel);
        // printf("vel: %d\n", vel);
        // printf("result: %d\n", dt);
        if (dt > d) {
            rst++;
        }
    }

    printf("Resultado: %zu\n", rst);

    // nob_da_free(rst);
    nob_sb_free(sb);

    return result;
}

uint64_t kxd_sv_chop_u64(Nob_String_View *sv) {
    uint64_t result = 0;
    while (sv->count > 0) {
        if (isdigit(*sv->data))
            result = result * 10 + *sv->data - '0';
        sv->count -= 1;
        sv->data += 1;
    }
    return result;
}
