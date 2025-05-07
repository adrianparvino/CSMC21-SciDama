#include "game.h"
#include <string.h>
#include <stdio.h>

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

struct moves *find_captures(struct game *game) {
    struct moves *list = NULL;

    char *captures = game->turn == WHITE ? "bB" : "wW";

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            char piece = game->board[8 * i + j];

            if (piece == ' ' || strchr(captures, piece))
                continue;

            bool promoted = strchr("BW", piece) != NULL;

            int directions[] = {7, 9, -7, -9};

            for (int k = 0; k < 4; ++k) {
                int direction = directions[k];

                int start = 8 * i + j;
                int cursor;
                for (cursor = start + direction; is_valid_position(cursor);
                     cursor += direction) {
                    if (game->board[cursor] != ' ') {
                        break;
                    }
                }
                if (!is_valid_position(cursor)) {
                    continue;
                }

                if (!strchr(captures, game->board[cursor])) {
                    continue;
                }

                for (cursor += direction; is_valid_position(cursor);
                     cursor += direction) {
                    int steps = (cursor - start) / direction;

                    if (!promoted && steps != 2) {
                        break;
                    }

                    if (game->board[cursor] != ' ') {
                        break;
                    }

                    struct move *move = add_move(&list);
                    init_move(move, start, cursor, CAPTURE);
                }
            }
        }
    }

    return list;
}

int execute_move(struct game *game, struct move *move) {
    int cur_pos = move->positions[0];
    char piece = game->board[move->positions[0]];

    if (piece == ' ') {
        return -1;
    }

    if (game->turn == WHITE && (piece != 'w' && piece != 'W')) {
        return -1;
    }

    if (game->turn == BLACK && (piece != 'b' && piece != 'B')) {
        return -1;
    }

    int min_steps = 1;
    int max_steps = 1;

    if (move->type == CAPTURE) {
        min_steps = 2;
        max_steps = 2;
    }

    if (piece == 'W' || piece == 'B') {
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

        if (steps < min_steps || steps > max_steps)
            return -1;

        game->board[cur_pos] = ' ';
        for (cur_pos += direction; cur_pos != next_pos; cur_pos += direction) {
            if (move->type != CAPTURE && game->board[cur_pos] != ' ')
                break;
            game->board[cur_pos] = ' ';
        }

        if (cur_pos != next_pos)
            return -1;

        cur_pos = next_pos;
    }

    game->turn = game->turn == WHITE ? BLACK : WHITE;
    game->board[cur_pos] = piece;

    return 0;
}

