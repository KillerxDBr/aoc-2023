#define NOB_IMPLEMENTATION
#include "../include/nob.h"

#define DA_ARRAY(name, dtype)                                                                                                              \
    typedef struct {                                                                                                                       \
        dtype *items;                                                                                                                      \
        size_t count, capacity;                                                                                                            \
    } name

DA_ARRAY(Strings, char *);

typedef struct {
    size_t dest, src, len;
} Range;

typedef struct {
    size_t seed;
    size_t soil;
    size_t fert;
    size_t water;
    size_t light;
    size_t temp;
    size_t humid;
    size_t loc;
} Mapping;

DA_ARRAY(Ranges, Range);
DA_ARRAY(Seeds, size_t);
DA_ARRAY(Mappings, Mapping);

void parseSeeds(Nob_String_View sv, Seeds *seeds);
size_t parseRange(Strings *strings, Ranges *arrRange, size_t index);
void parseRange2(Nob_String_View sv, Ranges *arrRange);
size_t checkRange(size_t n, Ranges *ranges);

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

// #define SMALL
int main(int argc, char **argv) {
    const char *program = nob_shift(argv, argc);

    if (!nob_set_current_dir(nob_temp_sprintf(SV_Fmt, (int)(nob_path_name(program) - program), program))) {
        return 1;
    }

#ifdef SMALL
    const char *input = "small.txt";
#define ST_FMT "%02zu"
#else
    const char *input = "input.txt";
#define ST_FMT "%010zu"
#endif
    int result = 0;
    Strings strings = { 0 };
    Nob_String_Builder sb = { 0 };

    if (!nob_read_entire_file(input, &sb)) {
        nob_sb_free(sb);
        return 1;
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
    // clang-format off
    Ranges SoilRange  = { 0 };
    Ranges FertRange  = { 0 };
    Ranges WaterRange = { 0 };
    Ranges LightRange = { 0 };
    Ranges TempRange  = { 0 };
    Ranges HumidRange = { 0 };
    Ranges LocRange   = { 0 };
    // clang-format on
    Mappings mappings = { 0 };

    int mode = 0;

    for (size_t i = 0; i < strings.count; i++) {
        // printf("Testing String: '%s'\n", strings.items[i]);
        Nob_String_View sv_tmp = nob_sv_from_cstr(strings.items[i]);

        // if (nob_sv_eq(nob_sv_from_parts(strings.items[i], strlen(labels[0])), nob_sv_from_cstr(labels[0]))) {
        if (nob_sv_starts_with(sv_tmp, nob_sv_from_cstr(labels[0]))) {
            mode = SEEDS;
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
            parseRange2(nob_sv_from_cstr(strings.items[i]), &SoilRange);
            break;

        case FERT:
            // printf("FERT: '%s'\n", strings.items[i]);
            parseRange2(nob_sv_from_cstr(strings.items[i]), &FertRange);
            break;

        case WATER:
            // printf("WATER: '%s'\n", strings.items[i]);
            parseRange2(nob_sv_from_cstr(strings.items[i]), &WaterRange);
            break;

        case LIGHT:
            // printf("LIGHT: '%s'\n", strings.items[i]);
            parseRange2(nob_sv_from_cstr(strings.items[i]), &LightRange);
            break;

        case TEMP:
            // printf("TEMP: '%s'\n", strings.items[i]);
            parseRange2(nob_sv_from_cstr(strings.items[i]), &TempRange);
            break;

        case HUMID:
            // printf("HUMID: '%s'\n", strings.items[i]);
            parseRange2(nob_sv_from_cstr(strings.items[i]), &HumidRange);
            break;

        case LOC:
            // printf("LOC: '%s'\n", strings.items[i]);
            parseRange2(nob_sv_from_cstr(strings.items[i]), &LocRange);
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
        printf("Seed       %02zu: \"" ST_FMT "\"\n", i + 1, seeds.items[i]);
    printf("-----------------------------\n");
    for (size_t i = 0; i < SoilRange.count; ++i)
        printf("SoilRange  %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, SoilRange.items[i].dest, SoilRange.items[i].src,
               SoilRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < FertRange.count; ++i)
        printf("FertRange  %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, FertRange.items[i].dest, FertRange.items[i].src,
               FertRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < WaterRange.count; ++i)
        printf("WaterRange %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, WaterRange.items[i].dest, WaterRange.items[i].src,
               WaterRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < LightRange.count; ++i)
        printf("LightRange %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, LightRange.items[i].dest, LightRange.items[i].src,
               LightRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < TempRange.count; ++i)
        printf("TempRange  %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, TempRange.items[i].dest, TempRange.items[i].src,
               TempRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < HumidRange.count; ++i)
        printf("HumidRange %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, HumidRange.items[i].dest, HumidRange.items[i].src,
               HumidRange.items[i].len);
    printf("-----------------------------\n");
    for (size_t i = 0; i < LocRange.count; ++i)
        printf("LocRange   %02zu: \"" ST_FMT " " ST_FMT " " ST_FMT "\"\n", i + 1, LocRange.items[i].dest, LocRange.items[i].src,
               LocRange.items[i].len);

    fflush(stdout);

    for (size_t i = 0; i < seeds.count; ++i) {
        Mapping m = { 0 };
        m.seed = seeds.items[i];
        // clang-format off
        m.soil  = checkRange(m.seed,  &SoilRange);
        m.fert  = checkRange(m.soil,  &FertRange);
        m.water = checkRange(m.fert,  &WaterRange);
        m.light = checkRange(m.water, &LightRange);
        m.temp  = checkRange(m.light, &TempRange);
        m.humid = checkRange(m.temp,  &HumidRange);
        m.loc   = checkRange(m.humid, &LocRange);
        // clang-format on
        nob_da_append(&mappings, m);
    }

    size_t lowestLocation = (size_t)-1;

    assert(mappings.count > 0);

    printf("-----------------------------\n");
    for (size_t i = 0; i < mappings.count; ++i) {
        Mapping m = mappings.items[i];
        // clang-format off
        printf("Seed %02zu: "ST_FMT", Soil: "ST_FMT", Fertilizer: "ST_FMT", Water: "ST_FMT", Light: "ST_FMT", Temperature: "ST_FMT", Humidity: "ST_FMT", Location: "ST_FMT"\n",
                     i+1,    m.seed,         m.soil,         m.fert,               m.water,         m.light,         m.temp,                m.humid,            m.loc);
        // clang-format on
        lowestLocation = __min(lowestLocation, m.loc);
    }

    fflush(stdout);

defer:
    for (size_t i = 0; i < strings.count; ++i)
        free((void *)strings.items[i]);
    nob_da_free(strings);

    nob_da_free(seeds);
    nob_da_free(SoilRange);
    nob_da_free(FertRange);
    nob_da_free(WaterRange);
    nob_da_free(LightRange);
    nob_da_free(TempRange);
    nob_da_free(HumidRange);
    nob_da_free(LocRange);

    nob_da_free(mappings);

    nob_sb_free(sb);

    printf("Final Result ------------------------\n");
    printf("Lowest Location Number: %zu\n", lowestLocation);
    fflush(stdout);

    return result;
}

size_t parseRange(Strings *strings, Ranges *arrRange, size_t index) {
    index++;
    Range range = { 0 };
    while (index < strings->count && strlen(strings->items[index]) != 0) {
        sscanf(strings->items[index], "%zu %zu %zu", &range.dest, &range.src, &range.len);
        nob_da_append(arrRange, range);
        index++;
    }
    return index;
}

void parseRange2(Nob_String_View sv, Ranges *arrRange) {
    Range r = {
        .dest = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        .src = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
        .len = nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')),
    };

    assert(sv.count == 0);

    nob_da_append(arrRange, r);
}

void parseSeeds(Nob_String_View sv, Seeds *seeds) {
    nob_sv_chop_by_delim(&sv, ' ');
    while (sv.count > 0) {
        nob_da_append(seeds, nob_sv_to_u64(nob_sv_chop_by_delim(&sv, ' ')));
    }
}

size_t checkRange(size_t n, Ranges *ranges) {
    for (size_t i = 0; i < ranges->count; ++i) {
        Range r = ranges->items[i];
        if (n >= r.src && n <= (r.src + r.len - 1)) {
            return n - r.src + r.dest;
        }
    }
    return n;
}
