#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "captures.h"
#include "game.h"
#include "move.h"
#include "utils.h"
#include "leaderboard.h"

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
            if (!fgets(move, sizeof move, stdin) || sscanf(move, "%d", &i) != 1) {
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

    init_game(&game);

    /* for (int i = 0; i < 64; ++i) {
        game.board[i] = ' ';
    }

    game.board[28] = 'w';
    // game.board[37] = 'b';
    game.board[35] = 'b';
    game.board[33] = 'b';

    // execute_move_str(&game, "9-14");
    // execute_move_str(&game, "21-17");
    // execute_move_str(&game, "24-19");
    // execute_move_str(&game, "10-15");
    */
    bool last_invalid = false;

    FILE *leaderboard_file = fopen("leaderboard.txt", "r+");
    if (leaderboard_file == NULL) {
        leaderboard_file = fopen("leaderboard.txt", "w+");
    }
    struct leaderboard *leaderboard = load_leaderboard(leaderboard_file);

    print_leaderboard(leaderboard);

    while (!feof(stdin)) {
        int choice;
        printf("----------------------------------------------------------------------------\n");
        printf("*******       *       ******  *****           *       **      **       *\n");
        printf("*******      ***      ******  *******        ***      ***    ***      ***\n");
        printf("**          ** **       **    ***   **      ** **     ****  ****     ** **\n");
        printf("*******    *** ***      **    ***   ***    *** ***    **********    *** ***\n");
        printf("     **   *********     **    ***   ***   *********   **  *** **   *********\n");
        printf("*******  ****   ****  ******  ********   ****   ****  **   *  **  ****   ****\n");
        printf("*******  ***     *** ******  *******     ***     ***  **      **  ***     ***\n");
        printf("------------------------------------------------------------------------------\n");
        
        printf("Welcome to SAI-DAMA!\n");
        printf("What would you like to do?\n");
        printf("1.Play game\n2.View Leaderboards\n3.Exit\n");
        printf("---------------------------------------------\n");
        printf("Enter Choice: ");
        scanf("%d" ,&choice);
        switch (choice){
            case 1:
            enum turn winner = run_game_loop(&game);

            if (winner != NONE) {
                printf("Congratulations %s! You won!\n", winner == WHITE ? "White" : "Black");

                printf("Enter your name: ");
                char name[4];
                fgets(name, sizeof name, stdin);
                add_leaderboard(leaderboard, name, 1);
                dump_leaderboard(leaderboard, leaderboard_file);
                break;
            case 2:
                print_leaderboard(leaderboard);
                int lead;
                printf("---------------------\n");
                printf("Enter 1 to go back, any other to exit");
                scanf("%d" , &lead);
                if(lead != 1) exit(1);
            case 3:
            exit(1);
            break;
        }
    }
    }

    dump_leaderboard(leaderboard, leaderboard_file);
    cleanup_leaderboard(leaderboard);
    fclose(leaderboard_file);

    return 0;
}