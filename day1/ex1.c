// #define NOB_IMPLEMENTATION
#include "../include/nob.h"

#define BUF_SIZE 256

int main(int argc, char **argv) {
    Nob_String_Builder sb = {};

    const char *input;
    if (argc > 1)
        input = argv[1];
    else
        input = "input.txt";

    if (!nob_read_entire_file(input, &sb)) {
        return 1;
    }

    Nob_String_View content = nob_sv_trim(nob_sb_to_sv(sb));
    Nob_String_View line;

    size_t soma   = 0;
    size_t fdigit = 0;
    size_t ldigit = 0;

    while (content.count) {
        line = nob_sv_trim(nob_sv_chop_by_delim(&content, '\n'));
        for (size_t i = 0; i < line.count; ++i) {
            if (isdigit(line.data[i])) {
                if (fdigit == 0)
                    fdigit = line.data[i] - '0';
                ldigit = line.data[i] - '0';
            }
        }
        nob_log(NOB_INFO, "Linha: " SV_Fmt " fdigit: %zu ldigit: %zu", SV_Arg(line), fdigit, ldigit);
        soma += (fdigit * 10) + ldigit;
        fdigit = 0;
        ldigit = 0;
    }

    nob_log(NOB_INFO, "Resultado final: %zu", soma);
    nob_sb_free(sb);
    return 0;
}
