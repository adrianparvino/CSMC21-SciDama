#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#define OS_Windows
#endif

#define MAX_MOVES 64

enum checker { WHITE, BLACK, EMPTY };

enum move_type { MOVE, CAPTURE, ERROR };

struct game {
    char board[64];

    enum checker turn;
};

struct move {
    enum move_type type;
    unsigned int length;
    unsigned int positions[MAX_MOVES];
};

void debug_move(struct move *move) {
    printf("Move: ");
    for (int i = 0; i < move->length; ++i) {
        printf("%d ", move->positions[i]);
    }
    printf("\n");
}

struct moves {
    struct move move;
    struct moves *next;
};

struct move *add_move(struct moves **list) {
    struct moves *moves = malloc(sizeof *moves);
    moves->next = *list;
    *list = moves;

    return &moves->move;
}

void cleanup_moves(struct moves **moves) {
    struct moves *move = *moves;

    while (move != NULL) {
        struct moves *next = move->next;
        free(move);
        move = next;
    }

    *moves = NULL;
}

int pdn_to_index(int pdn) {
    --pdn;
    int i = pdn / 4;
    int j = 2 * (pdn % 4) + (i + 1) % 2;

    return 8 * i + j;
}

int index_to_pdn(int index) {
    int i = index / 8;
    int j = index % 8;

    j /= 2;

    return 4 * i + j + 1;
}

// https://stackoverflow.com/a/47406222
#ifdef OS_Windows
void clear_screen(void) {
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};
    DWORD dwCount, dwSize;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOutput, &csbi);
    dwSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hOutput, 0x20, dwSize, topLeft, &dwCount);
    FillConsoleOutputAttribute(hOutput, 0x07, dwSize, topLeft, &dwCount);
    SetConsoleCursorPosition(hOutput, topLeft);
}
#else
void clear_screen(void) { printf("\x1B[2J"); }
#endif /* __unix__ */

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

bool is_valid_position(int position) {
    if (position < 0 || position >= 64)
        return false;

    int i = position / 8;
    int j = position % 8;

    return (i % 2) != (j % 2);
}

void print_game(struct game *b) {
    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            printf("%c", b->board[8 * i + j]);
        }
        printf("\n");
    }
}

struct move *push_move(struct move *move, unsigned int position) {
    move->positions[move->length++] = position;
    return move;
}

struct move *
init_move(struct move *move, int start, int next, enum move_type move_type) {
    move->type = move_type;
    move->length = 0;
    push_move(move, start);
    push_move(move, next);
    return move;
}

/** Parse a PDN move into a data structure
 *
 */
struct move *parse_move(char *move) {
    unsigned int cursor = 0;
    unsigned int start, next;
    int bytes_read;
    char move_type_c;
    enum move_type move_type = ERROR;

    if (sscanf(
            move + cursor, "%u%c%u%n", &start, &move_type_c, &next, &bytes_read
        ) < 3) {
        return NULL;
    }
    cursor += bytes_read;
    switch (move_type_c) {
    case '-':
        move_type = MOVE;
        break;
    case 'x':
        move_type = CAPTURE;
        break;
    }

    // Convert a PDN position into an index to an 8x8 2D array
    unsigned int start_index = pdn_to_index(start);
    unsigned int next_index = pdn_to_index(next);

    struct move *parsed_move = malloc(sizeof *parse_move);
    init_move(parsed_move, start_index, next_index, move_type);

    if (move_type_c == '-') {
        return parsed_move;
    }

    if (move_type == MOVE)
        return parsed_move;

    while (sscanf(move + cursor, "x%u%n", &next, &bytes_read) == 1) {
        next_index = pdn_to_index(next);

        push_move(parsed_move, next_index);

        cursor += bytes_read;
    }

    char eos = move[cursor + bytes_read];
    if (eos != '\0' && !isspace(eos))
        goto cleanup;

    return parsed_move;
cleanup:
    free(parsed_move);

    return NULL;
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

int execute_move_str(struct game *game, char *move) {
    struct move *parsed_move = parse_move(move);
    if (parsed_move == NULL)
        return -1;

    int retval = execute_move(game, parsed_move);
    free(parsed_move);
    return retval;
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

int main(void) {
    struct game game;
    struct moves *captures;

    init_game(&game);

    execute_move_str(&game, "9-14");
    // execute_move_str(&game, "21-17");
    print_game(&game);
    captures = find_captures(&game);

    for (;;) {
        char move[100];
        fgets(move, 100, stdin);

        int retval;

        if (captures) {
            int choice;
            sscanf(move, "%d", &choice);

            struct moves *move = captures;
            while (choice > 1) {
                --choice;
                move = move->next;
            }

            retval = execute_move(&game, &move->move);
        } else {
            retval = execute_move_str(&game, move);
        }
        cleanup_moves(&captures);
        
        clear_screen();
        print_game(&game);
        captures = find_captures(&game);

        int i = 1;
        for (struct moves *move = captures; move != NULL; move = move->next) {
            printf(
                "%d. %dx%d", i++, index_to_pdn(move->move.positions[0]),
                index_to_pdn(move->move.positions[1])
            );

            for (int j = 2; j < move->move.length; ++j) {
                printf("x%d", index_to_pdn(move->move.positions[j]));
            }
        }
        printf("\n");

        if (retval == -1) {
            printf("Invalid move\n");
        }
    }

    return 0;
}