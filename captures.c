#include "captures.h"

#include "game.h"
#include "move.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct state {
    struct move move;
    bool promoted;
    int captured;
    int capture_set[12];
};

struct move *new_capture(struct captures **list) {
    struct captures *captures = malloc(sizeof *captures);
    captures->next = *list;
    *list = captures;

    return &captures->move;
}

void pop_capture(struct captures **list) {
    struct captures *captures = *list;

    *list = captures->next;

    free(captures);
}

void cleanup_captures(struct captures *captures) {
    while (captures != NULL) {
        struct captures *next = captures->next;
        free(captures);
        captures = next;
    }
}

bool find_captures_(
    struct game *game, struct captures **list, struct state *state
) {
    char *captures = game->turn == WHITE ? "bB" : "wW";

    bool added = false;

    int captured = state->captured;

    int start = state->move.positions[state->move.length - 1];
    int directions[] = {7, 9, -7, -9};
    for (int i = 0; i < 4; ++i) {
        int direction = directions[i];

        int cursor;
        int steps;
        for (cursor = start + direction, steps = 1; is_valid_position(cursor);
             cursor += direction, ++steps) {
            if (game->board[cursor] != ' ') break;
        }

        if (!is_valid_position(cursor)) continue;
        if (!strchr(captures, game->board[cursor])) continue;
        int capture_idx;
        for (capture_idx = 0; capture_idx < sizeof(state->capture_set) /
                                                sizeof(*state->capture_set);
             ++capture_idx) {
            if (state->capture_set[capture_idx] == cursor) break;
        }
        if (capture_idx !=
            sizeof(state->capture_set) / sizeof(*state->capture_set)) {
            continue;
        }

        int captured_piece = cursor;

        for (cursor += direction, ++steps; is_valid_position(cursor);
             cursor += direction, ++steps) {

            if (!state->promoted && steps != 2) break;
            if (game->board[cursor] != ' ') continue;

            added = true;

            struct state new_state = *state;
            new_state.move = state->move;
            push_move(&new_state.move, cursor);
            new_state.captured = captured + 1;
            new_state.capture_set[captured] = captured_piece;

            if (!find_captures_(game, list, &new_state)) {
                struct move *new_move = new_capture(list);
                *new_move = new_state.move;
            }
        }
    }

    return added;
}

struct captures *find_captures(struct game *game) {
    struct captures *list = NULL;

    char *own = game->turn == WHITE ? "wW" : "bB";

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int start = 8 * i + j;
            char piece = game->board[start];

            if (!strchr(own, piece)) continue;

            struct state state;
            init_move(&state.move, start, CAPTURE);
            state.promoted = strchr("BW", piece) != NULL;
            state.captured = 0;

            find_captures_(game, &list, &state);
        }
    }

    int max_length = 0;
    for (struct captures *cursor = list; cursor != NULL;
         cursor = cursor->next) {
        if (cursor->move.length > max_length) {
            max_length = cursor->move.length;
        }
    }

    struct captures **prev_next = &list;
    struct captures *cursor = list;
    while (cursor != NULL) {
        if (cursor->move.length < max_length) {
            *prev_next = cursor->next;
            free(cursor);
            cursor = *prev_next;
        } else {
            prev_next = &cursor->next;
            cursor = cursor->next;
        }
    }

    return list;
}

void print_captures(struct captures *captures) {
    for (int i = 1; captures != NULL; ++i) {
        printf("%d. ", i);
        print_move(&captures->move);
        printf(" ");
        captures = captures->next;
    }
}
