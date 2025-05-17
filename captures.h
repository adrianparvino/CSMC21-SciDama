#ifndef CAPTURES_H
#define CAPTURES_H

#include "move.h"
#include "game.h"

struct captures
{
    struct move move;
    struct captures *next;
};

struct move *new_capture(struct captures **list);
void pop_capture(struct captures **list);
void cleanup_captures(struct captures *captures);

struct captures *find_captures(struct game *game);
void print_captures(struct captures *captures);

#endif /* CAPTURES_H */