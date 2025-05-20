#include "leaderboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct leaderboard *load_leaderboard(FILE *file) {
    struct leaderboard *leaderboard =
        malloc(sizeof *leaderboard + 10 * sizeof *leaderboard->entries);

    leaderboard->size = 0;
    leaderboard->capacity = 10;

    if (leaderboard == NULL) {
        return NULL;
    }

    unsigned int score;
    char name[4];

    while (fscanf(file, "%u %3s", &score, name) == 2) {
        leaderboard = add_leaderboard(leaderboard, name, score);
    }

    return leaderboard;
}

void dump_leaderboard(struct leaderboard *leaderboard, FILE *file) {
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < leaderboard->size; ++i) {
        fprintf(
            file, "%d %s\n", leaderboard->entries[i].score,
            leaderboard->entries[i].name
        );
    }
}

struct leaderboard *add_leaderboard(
    struct leaderboard *leaderboard, char *name, unsigned int score
) {
    size_t entry_idx;
    for (entry_idx = 0; entry_idx < leaderboard->size; ++entry_idx) {
        if (strcmp(leaderboard->entries[entry_idx].name, name) == 0) {
            break;
        }
    }

    if (entry_idx == leaderboard->size) {
        if (leaderboard->size == leaderboard->capacity) {
            leaderboard->capacity *= 2;
            leaderboard = realloc(
                leaderboard,
                sizeof *leaderboard +
                    leaderboard->capacity * sizeof *leaderboard->entries
            );
        }

        strncpy(
            leaderboard->entries[entry_idx].name, name,
            sizeof leaderboard->entries[0].name
        );
        leaderboard->entries[entry_idx].score = 0;

        ++leaderboard->size;
    }

    leaderboard->entries[entry_idx].score += score;

    // Bubble up
    for (int i = entry_idx; i > 0; --i) {
        if (leaderboard->entries[i - 1].score < leaderboard->entries[i].score) {
            struct entry tmp = leaderboard->entries[i];
            leaderboard->entries[i] = leaderboard->entries[i - 1];
            leaderboard->entries[i - 1] = tmp;
        }
    }

    return leaderboard;
}

void print_leaderboard(struct leaderboard *leaderboard) {
    for (int i = 0; i < leaderboard->size; ++i) {
        printf(
            "%s %d\n", leaderboard->entries[i].name,
            leaderboard->entries[i].score
        );
    }
}

void cleanup_leaderboard(struct leaderboard *leaderboard) { free(leaderboard); }