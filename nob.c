#define NOB_IMPLEMENTATION
#include "include/nob.h"

bool buildExes(void);
bool clearExes(void);

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *command;
    const char *program = nob_shift(argv, argc);

    if (argc > 0)
        command = nob_shift(argv, argc);
    else
        command = "build";

    // nob_log(NOB_INFO, "Command: '%s'", command);
    if ((strcmp(command, "build") == 0) || (strcmp(command, "b") == 0)) {
        nob_log(NOB_INFO, "Building...");
        return !buildExes();
    }

    if ((strcmp(command, "clear") == 0) || (strcmp(command, "c") == 0)) {
        nob_log(NOB_INFO, "Cleaning...");
        return !clearExes();
    }
    nob_log(NOB_ERROR, "Unknown command: '%s'", command);
    printf("[USAGE] %s <[b]uild, [c]lear>", program);
    return 1;
}

bool buildExes(void) {
    bool result = true;

    Nob_Procs procs = { 0 };
    Nob_Cmd cmd = { 0 };
    nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-O1");
    const size_t cmdBkp = cmd.count;

    for (size_t day = 0; true; day++) {
        const size_t exCP = nob_temp_save();
        for (size_t exN = 0; true; exN++) {
            cmd.count = cmdBkp;
            const char *srcPath = nob_temp_sprintf("day%zu/ex%zu.c", day + 1, exN + 1);

            if (nob_file_exists(srcPath) < 1)
                break;

            const char *exePath = nob_temp_sprintf("day%zu/ex%zu.exe", day + 1, exN + 1);

            // nob_log(NOB_INFO, "src: \"%s\"", srcPath);
            // nob_log(NOB_INFO, "exe: \"%s\"", exePath);

            if (nob_needs_rebuild1(exePath, srcPath) != 0) {
                nob_cmd_append(&cmd, "-o", exePath, srcPath);

                nob_da_append(&procs, nob_cmd_run_async(cmd));
            }
        }
        nob_temp_rewind(exCP);
        if (nob_file_exists(nob_temp_sprintf("day%zu/", day + 1)) < 1)
            break;
    }

    if (!nob_procs_wait(procs))
        nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    return result;
}

bool clearExes(void) { NOB_TODO("clearExes"); }
