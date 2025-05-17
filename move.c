#include "move.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

struct move *push_move(struct move *move, unsigned int position) {
    move->positions[move->length++] = position;
    return move;
}

struct move *init_move(struct move *move, int start, enum move_type move_type) {
    move->type = move_type;
    move->length = 0;
    push_move(move, start);
    return move;
}

/** Parse a PDN move into a data structure
 *
 */
struct move *parse_move(char *move, struct move *parsed_move) {
    unsigned int cursor = 0;
    unsigned int start, next;
    int bytes_read;
    char move_type_c;
    enum move_type move_type = UNKNOWN;

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

    init_move(parsed_move, start_index, move_type);
    push_move(parsed_move, next_index);

    if (move_type_c == '-') {
        return parsed_move;
    }

    if (move_type == MOVE) return parsed_move;

    while (sscanf(move + cursor, "x%u%n", &next, &bytes_read) == 1) {
        next_index = pdn_to_index(next);

        push_move(parsed_move, next_index);

        cursor += bytes_read;
    }

    char eos = move[cursor + bytes_read];
    if (eos != '\0' && !isspace(eos)) return NULL;

    return parsed_move;
}

void print_move(struct move *move) {
    printf("%d", index_to_pdn(move->positions[0]));

    for (int i = 1; i < move->length; ++i) {
        printf(
            "%c%d", move->type == CAPTURE ? 'x' : '-',
            index_to_pdn(move->positions[i])
        );
    }
}
