#include "game.h"
#include <stdio.h>
#include <string.h>

#include "move.h"
#include "utils.h"

void init_game(struct game *b) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            b->board[8 * i + j] = ' ';
        }
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = (i + 1) % 2; j < 8; j += 2) {
            b->board[8 * i + j] = 'w';
        }
    }

    for (int i = 5; i < 8; ++i) {
        for (int j = (i + 1) % 2; j < 8; j += 2) {
            b->board[8 * i + j] = 'b';
        }
    }

    b->turn = WHITE;
}

void print_game(struct game *b) {
    printf("[%s's turn]\n", b->turn == WHITE ? "White" : "Black");

    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            printf("%c", b->board[8 * i + j]);
        }
        printf("\n");
    }
}

int execute_move(struct game *game, struct move *move) {
    int cur_pos = move->positions[0];
    char piece = game->board[move->positions[0]];

    char *own = game->turn == WHITE ? "wW" : "bB";
    if (strchr(own, piece) == NULL) return -1;

    int min_steps = 1;
    int max_steps = 1;

    if (move->type == CAPTURE) {
        min_steps = 2;
        max_steps = 2;
    }

    if (strchr("BW", piece) != NULL) {
        max_steps = 8;
    }

    for (int i = 1; i < move->length; ++i) {
        int next_pos = move->positions[i];

        if (game->board[next_pos] != ' ') {
            return -1;
        }

        int direction;
        if ((next_pos - cur_pos) % 7 == 0) {
            // northeast direction
            direction = 7;
        } else if ((next_pos - cur_pos) % 9 == 0) {
            // northwest direction
            direction = 9;
        } else {
            return -1;
        }

        int steps = (next_pos - cur_pos) / direction;
        if (steps < 0) {
            steps = -steps;
            direction = -direction;
        }

        if (steps < min_steps || steps > max_steps) return -1;

        game->board[cur_pos] = ' ';
        for (cur_pos += direction; cur_pos != next_pos; cur_pos += direction) {
            if (move->type != CAPTURE && game->board[cur_pos] != ' ') break;
            game->board[cur_pos] = ' ';
        }

        if (cur_pos != next_pos) return -1;

        cur_pos = next_pos;
    }

    game->turn = game->turn == WHITE ? BLACK : WHITE;
    game->board[cur_pos] = piece;

    return 0;
}
