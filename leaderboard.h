#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdio.h>

struct entry {
    unsigned int score;

    char name[4];
};

struct leaderboard {
    size_t capacity, size;

    struct entry entries[];
};

struct leaderboard *load_leaderboard(FILE *file);
void dump_leaderboard(struct leaderboard *leaderboard, FILE *file);

struct leaderboard *add_leaderboard(
    struct leaderboard *leaderboard, char *name, unsigned int score
);

void print_leaderboard(struct leaderboard *leaderboard);

void cleanup_leaderboard(struct leaderboard *leaderboard);

#endif