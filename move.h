#ifndef MOVE_H
#define MOVE_H

#define MAX_MOVES 64

enum move_type
{
    MOVE,
    CAPTURE,
    UNKNOWN
};
struct move
{
    enum move_type type;
    unsigned int length;
    unsigned int positions[MAX_MOVES];
};
struct moves
{
    struct move move;
    struct moves *next;
};


struct move *add_move(struct moves **list);
void cleanup_moves(struct moves **moves);

struct move *push_move(struct move *move, unsigned int position);
struct move *init_move(struct move *move, int start, int next, enum move_type move_type);
struct move *parse_move(char *move);

#endif
