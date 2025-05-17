#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "captures.h"
#include "game.h"
#include "move.h"
#include "utils.h"

int execute_move_str(struct game *game, char *move) {
    struct move parsed_move;
    if (!parse_move(move, &parsed_move)) return -1;

    int retval = execute_move(game, &parsed_move);
    return retval;
}

int main(void) {
    struct game game;

    init_game(&game);
    execute_move_str(&game, "9-14");
    // execute_move_str(&game, "21-17");
    execute_move_str(&game, "24-19");
    execute_move_str(&game, "10-15");

    bool last_invalid = false;

    for (;;) {
        // clear_screen();

        if (last_invalid) {
            printf("Invalid move\n");
        }
        last_invalid = false;

        print_game(&game);

        char move[100];
        struct captures *captures = find_captures(&game);
        if (captures != NULL) {
            printf("Captures: ");
            print_captures(captures);
            puts("");

            int i;
            printf("Enter move: ");
            fgets(move, sizeof move, stdin);
            sscanf(move, "%d", &i);

            struct captures *c = captures;
            for (; i > 1 && c != NULL; --i) {
                c = c->next;
            }

            if (c == NULL) {
                last_invalid = true;
            }

            execute_move(&game, &c->move);
        } else {
            printf("Enter move: ");
            fgets(move, sizeof move, stdin);
            int retval = execute_move_str(&game, move);
            if (retval == -1) {
                last_invalid = true;
            }
        }

        cleanup_captures(captures);
    }

    return 0;
}