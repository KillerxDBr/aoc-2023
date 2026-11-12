// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "utils.h"

#include <inttypes.h>

#define DA_ARRAY(name, dtype)                                                                                          \
    typedef struct {                                                                                                   \
        dtype *items;                                                                                                  \
        size_t count, capacity;                                                                                        \
    } name

DA_ARRAY(Strings, Nob_String_View);

typedef struct {
    uint64_t dest, src, len;
} Range;

typedef struct {
    uint64_t start, range;
} Seed;

DA_ARRAY(Ranges, Range);
DA_ARRAY(Seeds, Seed);

#ifndef _WIN32
#define __stdcall
#endif

void parseSeeds(Nob_String_View sv, Seeds *seeds);
void parseRange2(Nob_String_View sv, Ranges *arrRange);
uint64_t __stdcall checkRange(uint64_t n, Ranges ranges);

const Nob_String_View labels[] = {
    NOB_SVLIT_STATIC("seeds: "),
    NOB_SVLIT_STATIC("seed-to-soil map:"),
    NOB_SVLIT_STATIC("soil-to-fertilizer map:"),
    NOB_SVLIT_STATIC("fertilizer-to-water map:"),
    NOB_SVLIT_STATIC("water-to-light map:"),
    NOB_SVLIT_STATIC("light-to-temperature map:"),
    NOB_SVLIT_STATIC("temperature-to-humidity map:"),
    NOB_SVLIT_STATIC("humidity-to-location map:"),
};

typedef enum {
    SEEDS,
    SOIL,
    FERT,
    WATER,
    LIGHT,
    TEMP,
    HUMID,
    LOC,
} Mode;

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define ST_FMT "%010" PRIu64
int main(int argc, char **argv) {
    const char *input = ProcessInput(argc, argv, __FILE__);
    assert(input != NULL);

    int result = 0;

    Strings strings = {};
    Seeds seeds     = {};

    Ranges SoilRange  = {};
    Ranges FertRange  = {};
    Ranges WaterRange = {};
    Ranges LightRange = {};
    Ranges TempRange  = {};
    Ranges HumidRange = {};
    Ranges LocRange   = {};

    Nob_String_Builder sb = {};

    if (!nob_read_entire_file(input, &sb)) {
        nob_sb_free(sb);
        nob_return_defer(1);
    }

    Nob_String_View content = nob_sb_to_sv(sb);

    while (content.count > 0) {
        Nob_String_View sv = nob_sv_trim(nob_sv_chop_by_delim(&content, '\n'));
        if (sv.count == 0) continue;

        nob_da_append(&strings, sv);
    }

    int mode = 0;

    for (size_t i = 0; i < strings.count; i++) {
        // printf("Testing String: '%s'\n", strings.items[i]);
        Nob_String_View sv = strings.items[i];

        if (nob_sv_starts_with(sv, labels[0])) {
            mode = SEEDS;
        } else if (nob_sv_eq(sv, labels[1])) {
            mode = SOIL;
            continue;
        } else if (nob_sv_eq(sv, labels[2])) {
            mode = FERT;
            continue;
        } else if (nob_sv_eq(sv, labels[3])) {
            mode = WATER;
            continue;
        } else if (nob_sv_eq(sv, labels[4])) {
            mode = LIGHT;
            continue;
        } else if (nob_sv_eq(sv, labels[5])) {
            mode = TEMP;
            continue;
        } else if (nob_sv_eq(sv, labels[6])) {
            mode = HUMID;
            continue;
        } else if (nob_sv_eq(sv, labels[7])) {
            mode = LOC;
            continue;
        }

        switch (mode) {
        case SEEDS:
            // printf("SEEDS: '%s'\n", strings.items[i]);
            parseSeeds(sv, &seeds);
            break;
        case SOIL:
            // printf("SOIL: '%s'\n", strings.items[i]);
            parseRange2(sv, &SoilRange);
            break;

        case FERT:
            // printf("FERT: '%s'\n", strings.items[i]);
            parseRange2(sv, &FertRange);
            break;

        case WATER:
            // printf("WATER: '%s'\n", strings.items[i]);
            parseRange2(sv, &WaterRange);
            break;

        case LIGHT:
            // printf("LIGHT: '%s'\n", strings.items[i]);
            parseRange2(sv, &LightRange);
            break;

        case TEMP:
            // printf("TEMP: '%s'\n", strings.items[i]);
            parseRange2(sv, &TempRange);
            break;

        case HUMID:
            // printf("HUMID: '%s'\n", strings.items[i]);
            parseRange2(sv, &HumidRange);
            break;

        case LOC:
            // printf("LOC: '%s'\n", strings.items[i]);
            parseRange2(sv, &LocRange);
            break;
        default:
            printf("Mode: %d\n", mode);
            NOB_UNREACHABLE("switch (mode)");
            break;
        }
    }
    fflush(stdout);
    // clang-format off
    assert(seeds.count      > 0);
    assert(SoilRange.count  > 0);
    assert(FertRange.count  > 0);
    assert(WaterRange.count > 0);
    assert(LightRange.count > 0);
    assert(TempRange.count  > 0);
    assert(HumidRange.count > 0);
    assert(LocRange.count   > 0);
    // clang-format on
    printf("-----------------------------\n");
    for (size_t i = 0; i < seeds.count; ++i)
        printf("Seed       %02zu: \"" ST_FMT " -> " ST_FMT "\"\n", i + 1, seeds.items[i].start,
               seeds.items[i].start + seeds.items[i].range - 1);
    printf("-----------------------------\n");
    for (size_t i = 0; i < SoilRange.count; ++i)
        printf("SoilRange  %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, SoilRange.items[i].dest,
               SoilRange.items[i].src, SoilRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < FertRange.count; ++i)
        printf("FertRange  %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, FertRange.items[i].dest,
               FertRange.items[i].src, FertRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < WaterRange.count; ++i)
        printf("WaterRange %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, WaterRange.items[i].dest,
               WaterRange.items[i].src, WaterRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < LightRange.count; ++i)
        printf("LightRange %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, LightRange.items[i].dest,
               LightRange.items[i].src, LightRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < TempRange.count; ++i)
        printf("TempRange  %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, TempRange.items[i].dest,
               TempRange.items[i].src, TempRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < HumidRange.count; ++i)
        printf("HumidRange %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, HumidRange.items[i].dest,
               HumidRange.items[i].src, HumidRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < LocRange.count; ++i)
        printf("LocRange   %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, LocRange.items[i].dest,
               LocRange.items[i].src, LocRange.items[i].len);

    fflush(stdout);

#ifdef __INTELLISENSE__
#undef UINT64_MAX
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFllu
#endif

    uint64_t n;
    uint64_t lowestLocation = UINT64_MAX;

    for (size_t i = 0; i < seeds.count; ++i) {
        n = 0;
        for (uint64_t j = 0; j < seeds.items[i].range; ++j) {
            n = seeds.items[i].start + j;
            n = checkRange(n, SoilRange);
            n = checkRange(n, FertRange);
            n = checkRange(n, WaterRange);
            n = checkRange(n, LightRange);
            n = checkRange(n, TempRange);
            n = checkRange(n, HumidRange);
            n = checkRange(n, LocRange);
            // nob_da_append(&mappings, m);
            lowestLocation = min(lowestLocation, n);
        }
    }

    printf("Final Result ------------------------\n");
    printf("Lowest Location Number: %" PRIu64 "\n", lowestLocation);

defer:
    nob_da_free(strings);

    nob_da_free(seeds);
    nob_da_free(SoilRange);
    nob_da_free(FertRange);
    nob_da_free(WaterRange);
    nob_da_free(LightRange);
    nob_da_free(TempRange);
    nob_da_free(HumidRange);
    nob_da_free(LocRange);

    nob_sb_free(sb);

    return result;
}

void parseRange2(Nob_String_View sv, Ranges *arrRange) {
    Range r = {
        .dest = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        .src  = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        .len  = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
    };

    assert(sv.count == 0);

    nob_da_append(arrRange, r);
}

void parseSeeds(Nob_String_View sv, Seeds *seeds) {
    nob_sv_chop_by_delim(&sv, ' ');
    while (sv.count > 0) {
        Seed s = {
            .start = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
            .range = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        };
        nob_da_append(seeds, s);
    }
}

uint64_t __stdcall checkRange(uint64_t n, Ranges ranges) {
    for (size_t i = 0; i < ranges.count; ++i) {
        const Range *r = &ranges.items[i];
        if ((n >= r->src) && (n <= (r->src + r->len - 1))) {
            return n - r->src + r->dest;
        }
    }
    return n;
}
