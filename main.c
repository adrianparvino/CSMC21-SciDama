#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "move.h"
#include "utils.h"

int execute_move_str(struct game *game, char *move) {
    struct move *parsed_move = parse_move(move);
    if (parsed_move == NULL)
        return -1;

    int retval = execute_move(game, parsed_move);
    free(parsed_move);
    return retval;
}

int main(void) {
    struct game game;

    init_game(&game);

    for (;;) {
        char move[100];

        print_game(&game);
        struct moves *captures = find_captures(&game);

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
        if (retval == -1) {
            printf("Invalid move\n");
        }
    }

    return 0;
}