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

int prompt_title_screen() {
    bool last_invalid = false;
    int bytes_read;

    for (;;) {
        int choice;
        char buffer[100];
        clear_screen();

        printf(
            "-----------------------------------------------------------------------------\n"
            "*******       *       ******  *****           *       **      **       *     \n"
            "*******      ***      ******  *******        ***      ***    ***      ***    \n"
            "**          ** **       **    ***   **      ** **     ****  ****     ** **   \n"
            "*******    *** ***      **    ***   ***    *** ***    **********    *** ***  \n"
            "     **   *********     **    ***   ***   *********   **  *** **   ********* \n"
            "*******  ****   ****  ******  ********   ****   ****  **   *  **  ****   ****\n"
            "*******  ***     *** ******  *******     ***     ***  **      **  ***     ***\n"
            "-----------------------------------------------------------------------------\n"
        );

        if (last_invalid) printf("Invalid choice\n");

        printf("Welcome to SAI-DAMA!\n");
        printf("What would you like to do?\n");
        printf("1. Play game\n"
               "2. View leaderboards\n"
               "3. Exit\n");
        printf("---------------------------------------------\n");
        printf("Enter Choice: ");

        fgets(buffer, sizeof buffer, stdin);

        if (sscanf(buffer, "%d %n", &choice, &bytes_read) == 1 && buffer[bytes_read] == '\0' && choice >= 1 && choice <= 3)
            return choice;

        last_invalid = true;
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
                continue;
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

    return NONE;
}

int main(void) {
    struct game game;

    atexit(dump_on_exit);

    init_game(&game);

    leaderboard_file = fopen("leaderboard.txt", "r+");
    if (leaderboard_file == NULL) {
        leaderboard_file = fopen("leaderboard.txt", "w+");
    }
    leaderboard = load_leaderboard(leaderboard_file);

    while (!feof(stdin)) {
        char buffer[100];
        int choice = prompt_title_screen();

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
            printf("Enter 1 to go back, any other to exit ");
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