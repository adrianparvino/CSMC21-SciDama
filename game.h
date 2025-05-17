#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "move.h"

enum turn { WHITE, BLACK };

struct game {
    char board[64];

    enum turn turn;
};

void init_game(struct game *b);
void print_game(struct game *b);

int execute_move(struct game *game, struct move *move);
int execute_move_str(struct game *game, char *move);

#endif /* GAME_H */