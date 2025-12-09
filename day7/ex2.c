// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

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

    Nob_String_Builder sb = {};
    if (!nob_read_entire_file(input, &sb))
        nob_return_defer(1);

defer:
    nob_sb_free(sb);
    return result;
}