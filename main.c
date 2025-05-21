#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "captures.h"
#include "game.h"
#include "leaderboard.h"
#include "move.h"
#include "utils.h"

struct leaderboard *leaderboard = NULL;
FILE *leaderboard_file = NULL;

void dump_on_exit(void) {
    if (leaderboard && leaderboard_file) {
        dump_leaderboard(leaderboard, leaderboard_file);
    }

    if (leaderboard_file) {
        fclose(leaderboard_file);
    }

    if (leaderboard) {
        cleanup_leaderboard(leaderboard);
    }
}

enum turn run_game_loop(struct game *game) {
    bool last_invalid = false;

    while (!feof(stdin)) {
        clear_screen();

        if (last_invalid) {
            printf("Invalid move\n");
        }
        last_invalid = false;

        print_game(game);

        char move[100];
        struct captures *captures = find_captures(game);
        if (captures != NULL) {
            printf("Captures: ");
            print_captures(captures);
            puts("");

            int i;
            printf("Enter move: ");
            if (!fgets(move, sizeof move, stdin) ||
                sscanf(move, "%d", &i) != 1) {
                last_invalid = true;
                continue;
            }

            struct captures *c = captures;
            for (; i > 1 && c != NULL; --i) {
                c = c->next;
            }

            if (c == NULL) {
                last_invalid = true;
            }

            execute_move(game, &c->move);
        } else {
            printf("Enter move: ");
            fgets(move, sizeof move, stdin);
            int retval = execute_move_str(game, move);
            if (retval == -1) {
                last_invalid = true;
            }
        }

        cleanup_captures(captures);

        enum turn winner_ = winner(game);
        if (winner_ != NONE) {
            return winner_;
        }
    }
}

int main(void) {
    struct game game;

    atexit(dump_on_exit);

    init_game(&game);

    // for (int i = 0; i < 64; ++i) {
    //     game.board[i] = ' ';
    // }
    // game.board[49] = 'w';
    // game.board[8] = 'b';

    // game.board[28] = 'w';
    // // game.board[37] = 'b';
    // game.board[35] = 'b';
    // game.board[33] = 'b';

    // execute_move_str(&game, "9-14");
    // execute_move_str(&game, "21-17");
    // execute_move_str(&game, "24-19");
    // execute_move_str(&game, "10-15");

    bool last_invalid = false;

    leaderboard_file = fopen("leaderboard.txt", "r+");
    if (leaderboard_file == NULL) {
        leaderboard_file = fopen("leaderboard.txt", "w+");
    }
    leaderboard = load_leaderboard(leaderboard_file);

    // print_leaderboard(leaderboard);

    while (!feof(stdin)) {
        int choice;
        char buffer[100];
        printf(
            "----------------------------------------------------------------------------\n"
            "*******       *       ******  *****           *       **      **       *\n"
            "*******      ***      ******  *******        ***      ***    ***      ***\n"
            "**          ** **       **    ***   **      ** **     ****  ****     ** **\n"
            "*******    *** ***      **    ***   ***    *** ***    **********    *** ***\n"
            "     **   *********     **    ***   ***   *********   **  *** **   *********\n"
            "*******  ****   ****  ******  ********   ****   ****  **   *  **  ****   ****\n"
            "*******  ***     ***  ******  *******    ***     ***  **      **  ***     ***\n"
            "------------------------------------------------------------------------------\n"
        );

        do {
            printf("Welcome to SAI-DAMA!\n");
            printf("What would you like to do?\n");
            printf("1. Play game\n"
                   "2. View leaderboards\n"
                   "3. Exit\n");
            printf("---------------------------------------------\n");
            printf("Enter Choice: ");

            fgets(buffer, sizeof buffer, stdin);
        }
        while (sscanf(buffer, "%d", &choice) < 1);
        switch (choice) {
        case 1:
            enum turn winner = run_game_loop(&game);

            if (winner != NONE) {
                clear_screen();
                print_game(&game);

                printf(
                    "Congratulations %s! You won!\n",
                    winner == WHITE ? "White" : "Black"
                );

                char name[100];
                char *name_;
                do {
                    printf("Enter your name: ");
                    fgets(name, sizeof name, stdin);
                } while ((name_ = strtok(name, "\n")) == NULL);
                add_leaderboard(leaderboard, name_, 1);
                dump_leaderboard(leaderboard, leaderboard_file);

                init_game(&game);
            }
            break;
        case 2:
            print_leaderboard(leaderboard);
            int lead;
            printf("---------------------\n");
            printf("Enter 1 to go back, any other to exit");
            fgets(buffer, sizeof buffer, stdin);
            sscanf(buffer, "%d", &lead);
            if (lead != 1) exit(1);
            continue;
        case 3:
            exit(1);
            break;
        }
    }

    return 0;
}