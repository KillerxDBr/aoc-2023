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
    int *items;
    size_t count, capacity;
} intArr;

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

    intArr ta = { 0 };
    intArr da = { 0 };
    size_t rst = 0;
    size_t finalResult = 1;

    int n;
    while (tLine.count > 0) {
        tLine = nob_sv_trim(tLine);
        n = (int)sv_chop_u64(&tLine);
        // printf("tn: %d\n",n);
        nob_da_append(&ta, n);
    }

    while (dLine.count > 0) {
        dLine = nob_sv_trim(dLine);
        n = (int)sv_chop_u64(&dLine);
        // printf("dn: %d\n",n);
        nob_da_append(&da, n);
    }
    printf("------------------------------------\n");

    if (ta.count != da.count) {
        nob_log(NOB_ERROR, "Time count:     %zu", ta.count);
        nob_log(NOB_ERROR, "Distance count: %zu", da.count);
        nob_return_defer(1);
    }

    for (size_t i = 0; i < da.count; ++i) {
        rst = 0;
        int t = ta.items[i];
        int d = da.items[i];

        // printf("Time: %d\n",t);
        for (int vel = 1; vel < ta.items[i]; ++vel) {
            int dt = vel * (t - vel);
            // printf("vel: %d\n", vel);
            // printf("result: %d\n", dt);
            if (dt > d)
                rst++;
        }
        printf("Corrida[%zu]: %zu\n", i + 1, rst);
        finalResult *= rst;
        // printf("-----------------\n");
    }

    printf("Resultado final: %zu\n", finalResult);

defer:
    nob_da_free(ta);
    nob_da_free(da);
    nob_sb_free(sb);
    return result;
}
