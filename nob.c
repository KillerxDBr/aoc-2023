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
    nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-Og");
#ifndef _WIN32
    nob_cmd_append(&cmd, "-g", "-fsanitize=address", "-lm");
#endif // _WIN32
    const size_t cmdBkp = cmd.count;

    for (size_t day = 0; true; day++) {
        const size_t exCP = nob_temp_save();
        for (size_t exN = 0; true; exN++) {
            cmd.count = cmdBkp;
            const char *srcPath = nob_temp_sprintf("day%zu/ex%zu.c", day + 1, exN + 1);

            if (nob_file_exists(srcPath) < 1)
                break;

            const char *exePath = nob_temp_sprintf("day%zu/ex%zu.exe", day + 1, exN + 1);

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
    nob_da_free(procs);
    nob_cmd_free(cmd);
    return result;
}

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} EmbedFiles;

void recurse_dir(Nob_String_Builder *sb, EmbedFiles *eb) {
    Nob_File_Paths children = { 0 };

    if (sb->items[sb->count - 1] != 0)
        nob_sb_append_null(sb);

    nob_read_entire_dir(sb->items, &children);
    if (sb->items[sb->count - 1] == 0)
        sb->count--;

    for (size_t i = 0; i < children.count; i++) {
        if (strcmp(children.items[i], ".") == 0)
            continue;
        if (strcmp(children.items[i], "..") == 0)
            continue;

        const size_t dir_qtd = sb->count;

        // nob_log(NOB_INFO, "Last Char: '%c'(count - 1)", sb->items[sb->count - 1]);
        if (sb->items[sb->count - 1] != '\\' && sb->items[sb->count - 1] != '/') {
            // nob_log(NOB_INFO, "Appending '/' to sb");
            nob_sb_append_cstr(sb, "/");
        }

        nob_sb_append_cstr(sb, children.items[i]);
        nob_sb_append_null(sb);
        Nob_File_Type rst = nob_get_file_type(sb->items);
        // printf("File: %s | Type '%d'\n", sb->items, rst);
        switch (rst) {
        case NOB_FILE_DIRECTORY:
            sb->count--;
            recurse_dir(sb, eb);
            sb->count = dir_qtd;
            break;
        case NOB_FILE_REGULAR:
            // printf("%s\n",sb->items);
            nob_da_append(eb, strdup(sb->items));
            // sb->count--;
            sb->count = dir_qtd;
            break;
        default:
            continue;
        }
        // break;
    }
    free(children.items);
}

bool clearExes(void) {
    Nob_String_Builder sb = { 0 };
    EmbedFiles eb = { 0 };

    for (size_t day = 0; true; day++) {
        const char *df = nob_temp_sprintf("day%zu/", day + 1);
        // nob_log(NOB_INFO, "Testing '%s' directory", df);
        if (nob_file_exists(df) < 1)
            break;

        sb.count = 0;
        nob_sb_append_cstr(&sb, df);
        recurse_dir(&sb, &eb);
    }

    for (size_t i = 0; i < eb.count; i++) {
        if (nob_sv_end_with(nob_sv_from_cstr(eb.items[i]), ".exe")) {
            nob_log(NOB_INFO, "Deleting '%s'", eb.items[i]);
            if (remove(eb.items[i]) != 0)
                nob_log(NOB_ERROR, "Could not remove file '%s': ", eb.items[i], strerror(errno));
        }
    }

    for (size_t i = 0; i < eb.count; i++)
        NOB_FREE(eb.items[i]);

    NOB_FREE(eb.items);

    nob_sb_free(sb);

    return true;
}
