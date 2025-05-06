#include<stdio.h>
#include<stdlib.h>

#if defined(_WIN32) || defined(WIN32) 
#include<windows.h>
#define OS_Windows
#endif

#define MAX_MOVES 64

enum checker {
    WHITE,
    BLACK,
    EMPTY
};

enum move_type {
    MOVE,
    CAPTURE
};

struct game {
    char board[64];

    enum checker turn;
};

struct move {
    unsigned int length;
    unsigned int positions[MAX_MOVES];
};

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
void clear_screen(void) {
    printf("\x1B[2J");
}
#endif /* __unix__ */

void init_game(struct game *b) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            b->board[8*i + j] = ' ';
        }
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = (i + 1) % 2; j < 8; j += 2) {
            b->board[8*i + j] = 'w';
        }
    }

    for (int i = 5; i < 8; ++i) {
        for (int j = (i + 1) % 2; j < 8; j += 2) {
            b->board[8*i + j] = 'b';
        }
    }

    b->turn = WHITE;
}

void print_game(struct game *b) {
    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            printf("%c", b->board[8*i + j]);
        }
        printf("\n");
    }
}

struct move *new_move(void) {
    struct move *move = malloc(sizeof *move);
    move->length = 0;
    return move;
}

struct move *push_move(struct move *move, unsigned int position) {
    move->positions[move->length++] = position;
    return move;
}

struct move *parse_move(char *move) {
    unsigned int cursor = 0;
    unsigned int start, next;
    int bytes_read;
    char move_type_c;
    enum move_type move_type;
    struct move *parsed_move = new_move();

    if (sscanf(move + cursor, "%u%c%u%n", &start, &move_type_c, &next, &bytes_read) < 3) {
        return NULL;
    }
    cursor += bytes_read;
    switch(move_type_c) {
        case '-':
            move_type = MOVE;
            break;
        case 'x':
            move_type = CAPTURE;
            break;
    }

    unsigned int start_i = start / 4;
    unsigned int start_j = 2*(start % 4) + (start_i + 1)%2;
    unsigned int start_index = 8 * start_i + start_j;

    unsigned int next_i = next / 4;
    unsigned int next_j = 2*(next % 4) + (next_i + 1)%2;
    unsigned int next_index = 8 * next_i + next_j;

    push_move(parsed_move, start_index);
    push_move(parsed_move, next_index);

    if (move_type == MOVE) return parsed_move; 

    while (sscanf(move + cursor, "x%u%n", &next, &bytes_read) == 1) {
        cursor += bytes_read;

        next_i = next / 4;
        next_j = 2*(next % 4) + (next_i + 1)%2;
        next_index = 8 * next_i + next_j;

        push_move(parsed_move, next_index);
    }

    return parsed_move;
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

    for (int i = 1; i < move->length; ++i) {
        int next_pos = move->positions[i];

        int min_pos = next_pos < cur_pos ? next_pos : cur_pos;
        int max_pos = next_pos > cur_pos ? next_pos : cur_pos;

        int direction;
        if ((max_pos - min_pos)%7 == 0) {
            // northeast direction
            direction = 7;
        } else if ((max_pos - min_pos)%9 == 0) {
            // northwest direction
            direction = 9;
        } else {
            return -1;
        }

        int steps = (max_pos - min_pos) / direction;

        for (int step = 0; step <= steps; ++step) {
            game->board[min_pos + step*direction] = ' ';
        }

        cur_pos = next_pos;
    }

    game->turn = game->turn == WHITE ? BLACK : WHITE;
    game->board[cur_pos] = piece;
}

int execute_move_str(struct game *game, char *move) {
    struct move *parsed_move = parse_move(move);
    if (parsed_move == NULL) return -1;

    int retval = execute_move(game, parsed_move);
    free(parsed_move);
    return retval;
}


int main(void) {
    struct game game;

    init_game(&game);

    execute_move_str(&game, "9-14");
    execute_move_str(&game, "21-17");
    print_game(&game);

    for(;;) {
        char move[100];
        fgets(move, 100, stdin);

        int retval = execute_move_str(&game, move);
        clear_screen();
        print_game(&game);

        if (retval == -1) {
            printf("Invalid move\n");
        }
    }

    return 0;
}