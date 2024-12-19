#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/da.h"

#define NOB_IMPLEMENTATION
#include "../include/nob.h"

#define SV_IMPLEMENTATION
#include "../include/sv.h"

da_array(Strings, char *);

typedef struct Range {
    size_t dest, src, len;
} Range;

da_array(Ranges, Range);
da_array(Seeds, size_t);

void parseSeeds(Nob_String_View sv, Seeds *seeds);
size_t parseRange(Strings *strings, Ranges *arrRange, size_t index);
void parseRange2(Nob_String_View sv, Ranges *arrRange);

const char *labels[] = {
    "seeds: ",
    "seed-to-soil map:",
    "soil-to-fertilizer map:",
    "fertilizer-to-water map:",
    "water-to-light map:",
    "light-to-temperature map:",
    "temperature-to-humidity map:",
    "humidity-to-location map:",
};

// #define SMALL 1

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

int main(void) {

#if SMALL
    const char *input = "day5/small.txt";
#else
    const char *input = "day5/input.txt";
#endif
    int result = 0;
    Strings strings = { 0 };
    Nob_String_Builder sb = { 0 };

    if (!nob_read_entire_file(input, &sb)) {
        nob_return_defer(1);
    }

    Nob_String_View sv = nob_sb_to_sv(sb);

    // Nob_String_View sv2;
    while (sv.count > 0) {
        Nob_String_View sv2 = nob_sv_trim(nob_sv_chop_by_delim(&sv, '\n'));
        if (sv2.count == 0)
            continue;

        char *tmp = malloc(sv2.count + 1);
        if (tmp == NULL) {
            nob_log(NOB_ERROR, "Could not allocate memory: %s", strerror(errno));
            nob_return_defer(1);
        }
        // memcpy(tmp, sv2.data, sv2.count);
        if (sprintf(tmp, SV_Fmt, SV_Arg(sv2)) != sv2.count) {
            nob_log(NOB_ERROR, "Could not copy string: %s", strerror(errno));
            nob_return_defer(1);
        }
        nob_da_append(&strings, tmp);
    }

    // for (size_t i = 0; i < strings.count; i++)
    //     printf("%02zu: '%s'\n", i + 1, strings.items[i]);

    // nob_return_defer(0);

    Seeds seeds = { 0 };

    Ranges SoilRange = { 0 };
    Ranges FertRange = { 0 };
    Ranges WaterRange = { 0 };
    Ranges LightRange = { 0 };
    Ranges TempRange = { 0 };
    Ranges HumidRange = { 0 };
    Ranges LocRange = { 0 };

    int mode = 0;

    for (size_t i = 0; i < strings.count; i++) {
        // printf("Testing String: '%s'\n", strings.items[i]);
        Nob_String_View sv_tmp = nob_sv_from_cstr(strings.items[i]);

        if (nob_sv_eq(nob_sv_from_parts(sv_tmp.data, strlen(labels[0])), nob_sv_from_cstr(labels[0]))) {
            mode = SEEDS;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[1]))) {
            mode = SOIL;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[2]))) {
            mode = FERT;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[3]))) {
            mode = WATER;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[4]))) {
            mode = LIGHT;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[5]))) {
            mode = TEMP;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[6]))) {
            mode = HUMID;
            continue;
        } else if (nob_sv_eq(sv_tmp, nob_sv_from_cstr(labels[7]))) {
            mode = LOC;
            continue;
        }

        switch (mode) {
        case SEEDS:
            // printf("SEEDS: '%s'\n", strings.items[i]);
            parseSeeds(sv_tmp, &seeds);
            break;
        case SOIL:
            // printf("SOIL: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &SoilRange);
            break;

        case FERT:
            // printf("FERT: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &FertRange);
            break;

        case WATER:
            // printf("WATER: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &WaterRange);
            break;

        case LIGHT:
            // printf("LIGHT: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &LightRange);
            break;

        case TEMP:
            // printf("TEMP: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &TempRange);
            break;

        case HUMID:
            // printf("HUMID: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &HumidRange);
            break;

        case LOC:
            // printf("LOC: '%s'\n", strings.items[i]);
            parseRange2(sv_from_cstr(strings.items[i]), &LocRange);
            break;
        default:
            printf("Mode: %d\n", mode);
            NOB_UNREACHABLE("switch (mode)");
            break;
        }
    }
    fflush(stdout);

    printf("-----------------------------\n");
    for (size_t i = 0; i < seeds.count; ++i)
        printf("Seeds      %02zu: %010zu\n", i + 1, seeds.items[i]);
    printf("-----------------------------\n");
    for (size_t i = 0; i < SoilRange.count; ++i)
        printf("SoilRange  %02zu: \"%010zu %010zu %010zu\"\n", i + 1, SoilRange.items[i].dest, SoilRange.items[i].src, SoilRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < FertRange.count; ++i)
        printf("FertRange  %02zu: \"%010zu %010zu %010zu\"\n", i + 1, FertRange.items[i].dest, FertRange.items[i].src, FertRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < WaterRange.count; ++i)
        printf("WaterRange %02zu: \"%010zu %010zu %010zu\"\n", i + 1, WaterRange.items[i].dest, WaterRange.items[i].src, WaterRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < LightRange.count; ++i)
        printf("LightRange %02zu: \"%010zu %010zu %010zu\"\n", i + 1, LightRange.items[i].dest, LightRange.items[i].src, LightRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < TempRange.count; ++i)
        printf("TempRange  %02zu: \"%010zu %010zu %010zu\"\n", i + 1, TempRange.items[i].dest, TempRange.items[i].src, TempRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < HumidRange.count; ++i)
        printf("HumidRange %02zu: \"%010zu %010zu %010zu\"\n", i + 1, HumidRange.items[i].dest, HumidRange.items[i].src, HumidRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < LocRange.count; ++i)
        printf("LocRange   %02zu: \"%010zu %010zu %010zu\"\n", i + 1, LocRange.items[i].dest, LocRange.items[i].src, LocRange.items[i].len);

    fflush(stdout);

defer:
    da_free_ptr(&strings);

    da_free(&seeds);
    da_free(&SoilRange);
    da_free(&FertRange);
    da_free(&WaterRange);
    da_free(&LightRange);
    da_free(&TempRange);
    da_free(&HumidRange);
    da_free(&LocRange);

    nob_sb_free(sb);

    return result;
}

size_t parseRange(Strings *strings, Ranges *arrRange, size_t index) {
    index++;
    Range range = { 0 };
    while (index < strings->count && strlen(strings->items[index]) != 0) {
        sscanf(strings->items[index], "%zu %zu %zu", &range.dest, &range.src, &range.len);
        da_append(arrRange, range);
        index++;
    }
    return index;
}

void parseRange2(Nob_String_View sv, Ranges *arrRange) {
    Range r = {
        .dest = sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        .src  = sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        .len  = sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
    };

    assert(sv.count == 0);

    nob_da_append(arrRange, r);
}

void parseSeeds(Nob_String_View sv, Seeds *seeds) {
    nob_sv_chop_by_delim(&sv, ' ');
    while (sv.count > 0) {
        nob_da_append(seeds, sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')));
    }
}
