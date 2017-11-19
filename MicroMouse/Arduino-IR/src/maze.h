#ifndef MM_MAZE_H
#define MM_MAZE_H

#include <stdint.h>
#define MAZE 8
#define MAZE_END (MAZE-1)
#define CELLS (MAZE*MAZE)
#define CELLS_END (CELLS-1)
#define ind(x, y) (y * MAZE + x)
#define cell(v, x, y) v[ ind(x,y) ]
#define dist(a, b) (a < b ? b - a : a - b)

typedef enum {
    U = 1,
    R = 2,
    D = 4,
    L = 8,
} Wall;

typedef enum {
    N = 0,
    E,
    S,
    W,
} Direction;

typedef struct {
    uint8_t walls;        //Mask of directions available
    uint8_t maze_ind;
    uint8_t open;
    uint8_t closed;
    uint8_t f;
    uint8_t g;
    uint8_t h;
    uint8_t from;
    uint8_t path_length;
} Cell;

typedef Cell Maze[CELLS];

#endif
