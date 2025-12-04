#define NOBDEF static inline
#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "include/nob.h"

#ifndef CC
#if defined(__GNUC__)
#define CC "gcc"
#elif defined(__clang__)
#define CC "clang"
#elif defined(_MSC_VER)
#define CC "msvc"
#else
#define CC "cc"
#endif
#endif // nob_cc

#ifndef PLAT
#if defined(_WIN32)
#if defined(_WIN64)
#define PLAT "w64"
#else
#define PLAT "w32"
#endif
#elif defined(__linux__)
#define PLAT "linux"
#else
#define PLAT "unk"
#endif
#endif

#ifndef CC_STR
#define CC_STR CC "_" PLAT
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define OBJ_EXT ".obj"
#else
#define OBJ_EXT ".o"
#endif

#ifndef nob_cc_obj_output
#if defined(_MSC_VER) && !defined(__clang__)
#define nob_cc_obj_output(cmd, output_path) nob_cmd_append(cmd, nob_temp_sprintf("/Fo:%s", (output_path)))
#else
#define nob_cc_obj_output(cmd, output_path) nob_cc_output(cmd, output_path)
#endif
#endif // nob_cc_obj_output

#ifndef nob_cc_define
#if defined(_MSC_VER) && !defined(__clang__)
#define nob_cc_define(cmd, def) nob_cmd_append(cmd, "/D", (def))
#else
#define nob_cc_define(cmd, def) nob_cmd_append(cmd, "-D", (def))
#endif
#endif

#ifdef _WIN32
#define EXE_EXT ".exe"
#else
#define EXE_EXT ""
#endif

static inline const char *src_fmt(int day, int ex) { return temp_sprintf("day%d/ex%d.c", day, ex); }
static inline const char *exe_fmt(int day, int ex) { return temp_sprintf("build/d%de%d" EXE_EXT, day, ex); }

static const char *nob_obj = "build/nob_" CC_STR OBJ_EXT;

Cmd cmd = {};
String_Builder sb = {};
Nob_Procs procs = {};

int main(int argc, char **argv) {
    int result = 0;

#ifdef _WIN32
    if (!IsDebuggerPresent())
#endif
        NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "include/nob.h");

    // const char *program = nob_shift(argv, argc);

    bool force = false;
    bool clear = false;
    bool run = false;

    if (argc > 1) {
        for (int i = 0; i < argc; ++i) {
            if (!clear && ((strcmp(argv[i], "clear") == 0) || (strcmp(argv[i], "c") == 0))) {
                clear = true;
                continue;
            }
            if (!run && ((strcmp(argv[i], "run") == 0) || (strcmp(argv[i], "r") == 0))) {
                run = true;
                continue;
            }
            if (!force && ((strcmp(argv[i], "force") == 0) || (strcmp(argv[i], "f") == 0))) {
                force = true;
                continue;
            }
        }
    }

    if (clear) {
        if (file_exists("build/") > 0) {
            File_Paths fp = {};
            if (!read_entire_dir("build/", &fp)) {
                da_free(fp);
                return_defer(1);
            }
            for (size_t i = 2; i < fp.count; ++i) {
                delete_file(temp_sprintf("build/%s", fp.items[i]));
            }
            da_free(fp);
            return_defer(0);
        }
    }

    if (!mkdir_if_not_exists("build/"))
        return_defer(1);

    if (force || needs_rebuild1(nob_obj, "include/nob.h") > 0) {
        nob_cc(&cmd);

        nob_cc_obj_output(&cmd, nob_obj);
        nob_cc_define(&cmd, "NOB_IMPLEMENTATION");
        nob_cc_inputs(&cmd, "-xc", "-c", "include/nob.h", "-O2");
        nob_cc_flags(&cmd);

        cmd_run(&cmd);
    }

    char folder[6];
    for (int day = 1; true; ++day) {
        snprintf(folder, sizeof(folder), "day%d", day);
        if (file_exists(folder) < 1)
            break;
        for (int ex = 1; ex < 3; ++ex) {
            const char *srcFile = src_fmt(day, ex);
            const char *output = exe_fmt(day, ex);

            if (file_exists(srcFile) < 1)
                continue;

            const char *deps[] = {
                srcFile,
                nob_obj,
            };

            if (force || needs_rebuild(output, (void*)deps, ARRAY_LEN(deps)) > 0) {
                nob_cc(&cmd);
                nob_cc_output(&cmd, output);
                nob_cc_inputs(&cmd, srcFile, nob_obj);
                nob_cc_flags(&cmd);

                cmd_run(&cmd, .async = &procs);
            }
        }
    }

    if (!procs_flush(&procs))
        return_defer(1);

defer:
    cmd_free(cmd);
    sb_free(sb);
    da_free(procs);

    return result;
}
