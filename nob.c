#define NOB_IMPLEMENTATION
#include "include/nob.h"

bool buildExes(void);
bool clearExes(void);
bool runExes(void);

#ifdef _WIN32
#define WIN32_EXE ".exe"
#else
#define WIN32_EXE ""
#endif // _WIN32

Nob_File_Paths fp = { 0 };

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "include/nob.h");

    const char *program = nob_shift(argv, argc);

    bool build = false;
    bool clear = false;
    bool run = false;

    if (!argc)
        build = true;
    else {
        for (int i = 0; i < argc; ++i) {
            if ((strcmp(argv[i], "build") == 0) || (strcmp(argv[i], "b") == 0)) {
                build = true;
                continue;
            }
            if ((strcmp(argv[i], "clear") == 0) || (strcmp(argv[i], "c") == 0)) {
                clear = true;
                continue;
            }
            if ((strcmp(argv[i], "run") == 0) || (strcmp(argv[i], "r") == 0)) {
                run = true;
                continue;
            }
            if (build && clear && run)
                break;
        }

        if (run)
            build = true;
    }

    if (clear) {
        nob_log(NOB_INFO, "Cleaning...");
        bool result = clearExes();

        if (!build || !result)
            return !result;
    }

    if (build) {
        nob_log(NOB_INFO, "Building...");
        if (!buildExes())
            return 1;
        if (run) {
            nob_log(NOB_INFO, "Running Executables...");
            runExes();
        }
        for (size_t i = 0; i < fp.count; ++i) {
            free((void *)fp.items[i]);
        }
        nob_da_free(fp);
        return 0;
    }

    Nob_String_Builder sb = { 0 };
    Nob_String_View sv;

    for (int i = 0; i < argc; ++i) {
        nob_sb_append_cstr(&sb, argv[i]);
        nob_sb_append_cstr(&sb, " ");
    }
    sv = nob_sv_trim(nob_sb_to_sv(sb));
    if (sv.count)
        nob_log(NOB_ERROR, "Unknown command: '" SV_Fmt "'", SV_Arg(sv));
    nob_sb_free(sb);

    fprintf(stderr, "[USAGE] %s <[b]uild, [c]lear, [r]un>\n", program);
    return 1;
}

bool buildExes(void) {
    bool result = true;

    Nob_Cmd cmd = { 0 };

    if (nob_needs_rebuild("libwinsane.o", (const char *[]){ "winsane.c", "manifest.rc", "utf8.xml" }, 3) > 0) {
        nob_log(NOB_INFO, "Rebuilding libWinsane...");

        nob_cmd_append(&cmd, "gcc", "-c", "winsane.c");
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);

        nob_cmd_append(&cmd, "windres", "-o", "manifest.o", "manifest.rc");
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);

        nob_cmd_append(&cmd, "ld.exe", "-relocatable", "-o", "libwinsane.o", "winsane.o", "manifest.o");
        if (!nob_cmd_run_sync_and_reset(&cmd))
            nob_return_defer(false);
    }

    nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-Og", "-g3", "-ggdb3", "-lm");
#ifndef _WIN32
    nob_cmd_append(&cmd, "-fsanitize=address");
#endif // !_WIN32
    const size_t cmdBkp = cmd.count;

    bool continueComp = true;

    for (size_t day = 0; continueComp; day++) {
        const size_t exCP = nob_temp_save();
        for (size_t exN = 0; continueComp; exN++) {
            cmd.count = cmdBkp;
            const char *srcPath = nob_temp_sprintf("day%zu/ex%zu.c", day + 1, exN + 1);

            if (nob_file_exists(srcPath) < 1)
                break;

            const char *exePath = nob_temp_sprintf("day%zu/ex%zu" WIN32_EXE, day + 1, exN + 1);

            if (nob_needs_rebuild(exePath, (const char *[]){ srcPath, "libwinsane.o" }, 2) != 0) {
                nob_cmd_append(&cmd, "-o", exePath, srcPath, "libwinsane.o");

                continueComp = continueComp & nob_cmd_run_sync(cmd);
            }
            char *s = strdup(exePath);
            nob_da_append(&fp, s);
        }
        nob_temp_rewind(exCP);
        if (nob_file_exists(nob_temp_sprintf("day%zu/", day + 1)) < 1)
            break;
    }

    if (!continueComp)
        nob_return_defer(false);

defer:
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
        if ((strcmp(children.items[i], ".") == 0) || (strcmp(children.items[i], "..") == 0))
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
    for (size_t day = 0; true; day++) {
        const size_t exCP = nob_temp_save();
        for (size_t exN = 0; exN < 2; exN++) {
            sb.count = 0;
            const char *exePath = nob_temp_sprintf("day%zu/ex%zu", day + 1, exN + 1);
            nob_sb_append_cstr(&sb, exePath);
            Nob_String_Builder sb2 = sb;
            nob_sb_append_cstr(&sb, ".exe");

            nob_sb_append_null(&sb);
            if (nob_file_exists(sb.items) == 1) {
                nob_log(NOB_INFO, "Deleting '%s'", sb.items);
                if (remove(sb.items) != 0)
                    nob_log(NOB_ERROR, "Could not remove file '%s': %s", sb.items, strerror(errno));
            }

            nob_sb_append_null(&sb2);
            if (nob_file_exists(sb2.items) == 1) {
                nob_log(NOB_INFO, "Deleting '%s'", sb2.items);
                if (remove(sb2.items) != 0)
                    nob_log(NOB_ERROR, "Could not remove file '%s': %s", sb2.items, strerror(errno));
            }

            // printf("sb:  '"SB_Fmt"'\n", SB_Arg(sb));
            // printf("sb2: '"SB_Fmt"'\n", SB_Arg(sb2));
            // printf("sb:  0x%p\n", sb.items);
            // printf("sb2: 0x%p\n", sb2.items);
        }
        nob_temp_rewind(exCP);
        if (nob_file_exists(nob_temp_sprintf("day%zu/", day + 1)) < 1)
            break;
    }
    nob_sb_free(sb);

    return true;
}

bool runExes(void) {
    bool result = true;

    Nob_Cmd cmd = { 0 };

    for (size_t i = 0; i < fp.count; ++i) {
        nob_log(NOB_INFO, "Running %s...", fp.items[i]);
        nob_cmd_append(&cmd, fp.items[i]);
        if (!nob_cmd_run_sync_and_reset(&cmd)) {
            nob_log(NOB_ERROR, "Could not run '%s'", fp.items[i]);
            nob_return_defer(false);
        }
    }

defer:
    nob_cmd_free(cmd);
    return result;
}
