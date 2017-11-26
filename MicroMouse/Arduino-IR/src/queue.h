#ifndef MM_QUEUE_H
#define MM_QUEUE_H

#include "maze.h"

typedef struct {
    int size;
    Cell *data[CELLS];
} PriorityQueue;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int size;
    Point data[CELLS];
} Path;

/*
 * These are purposely #defines because I want to allow
 * a path to use them as well.  Call it my poor man's templates
 */
#define queue_init(q) q.size = -1
#define queue_peek(q) q.data[q.size]
#define queue_pop(q) q.data[q.size--]
#define queue_push(q, v) (q.data[++q.size] = v)
#define queue_empty(q) (q.size < 0)

void queue_print(Path &);
void queue_push_priority(PriorityQueue &, Cell *);
void queue_reprioritize(PriorityQueue &, Cell *);

#endif
