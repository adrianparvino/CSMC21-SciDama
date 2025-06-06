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
        printf("    ");
        for (int j = 7; j >= 0; --j) {
            char piece = b->board[8 * i + j];
            if (piece == ' ' && !is_valid_position(8 * i + j)) {
                printf("\033[7m       \033[0m");
            } else {
                printf("       ");
            }
        }
        printf("\n");
        printf("%2d  ", 4 * i + 4);
        for (int j = 7; j >= 0; --j) {
            char piece = b->board[8 * i + j];
            if (piece == ' ' && !is_valid_position(8 * i + j)) {
                printf("\033[7m       \033[0m");
            } else {
                printf("   %c   ", piece);
            }
        }
        printf("  %2d\n", 4 * i + 1);
        printf("    ");
        for (int j = 7; j >= 0; --j) {
            char piece = b->board[8 * i + j];
            if (piece == ' ' && !is_valid_position(8 * i + j)) {
                printf("\033[7m       \033[0m");
            } else {
                printf("       ");
            }
        }
        printf("\n");
    }
}

int execute_move_(
    struct game *game, struct move *move, int min_steps, int max_steps,
    char *own, char piece, bool promoted, int index
) {
    if (index >= move->length) return 0;

    int cur_pos = move->positions[index - 1];
    int next_pos = move->positions[index];

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

    if (!promoted && game->turn == WHITE && direction < 0 &&
        move->type != CAPTURE)
        return -1;
    if (!promoted && game->turn == BLACK && direction > 0 &&
        move->type != CAPTURE)
        return -1;

    bool can_capture = move->type == CAPTURE;
    for (int test_pos = cur_pos + direction; test_pos != next_pos;
         test_pos += direction) {
        if (game->board[test_pos] == ' ') continue;
        if (strchr(own, game->board[test_pos]) != NULL) return -1;
        if (!can_capture) return -1;

        can_capture = false;
    }
    if (game->board[next_pos] != ' ') return -1;

    if (execute_move_(
            game, move, min_steps, max_steps, own, piece, promoted, index + 1
        ) < 0)
        return -1;

    game->board[cur_pos] = ' ';
    for (cur_pos += direction; cur_pos != next_pos; cur_pos += direction) {
        game->board[cur_pos] = ' ';
    }

    if (index < move->length - 1) return 0;

    game->board[cur_pos] = piece;
    if (piece == 'w' && (cur_pos >= 56 && cur_pos < 64)) {
        game->board[cur_pos] = 'W';
    } else if (piece == 'b' && (cur_pos >= 0 && cur_pos < 8)) {
        game->board[cur_pos] = 'B';
    }

    game->turn = game->turn == WHITE ? BLACK : WHITE;

    return 0;
}

int execute_move(struct game *game, struct move *move) {
    char piece = game->board[move->positions[0]];

    char *own = game->turn == WHITE ? "wW" : "bB";

    if (strchr(own, piece) == NULL) return -1;

    bool promoted = strchr("WB", piece) != NULL;

    int min_steps = 1;
    int max_steps = 1;

    if (move->type == CAPTURE) {
        min_steps = 2;
        max_steps = 2;
    }

    if (promoted) {
        max_steps = 8;
    }

    return execute_move_(
        game, move, min_steps, max_steps, own, piece, promoted, 1
    );
}

int execute_move_str(struct game *game, char *move) {
    struct move parsed_move;
    if (!parse_move(move, &parsed_move)) return -1;

    int retval = execute_move(game, &parsed_move);
    return retval;
}

enum turn winner(struct game *game) {
    if (strpbrk(game->board, "wW") == NULL) {
        return WHITE;
    } else if (strpbrk(game->board, "bB") == NULL) {
        return BLACK;
    }

    return NONE;
}
