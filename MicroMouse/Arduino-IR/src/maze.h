#ifndef MM_MAZE_H
#define MM_MAZE_H

#include <stdint.h>

#define UNIT_SQUARE 178

#define MAZE 8
#define MAZE_END (MAZE-1)
#define CELLS (MAZE*MAZE)
#define CELLS_END (CELLS-1)
#define ind(x, y) (y * MAZE + x)
#define cell(v, x, y) v[ ind(x,y) ]
#define dist(a, b) (a < b ? b - a : a - b)
#define cardinal(a, b) (Direction)((a + b) % 8)
#define cardinal3(a, b, c) (Direction)((a + b + c) % 8)

typedef enum {
#ifndef ANDROID
    X  =  0,  //Testing for no movement
    UR =  3,
    DR =  6,
    UL =  9,
    DL = 12,
    H  = 15, //Testing for backup to home
#endif
    U  =  1,
    R  =  2,
    D  =  4,
    L  =  8,
} Wall;

typedef enum {
    N = 0,
    NE,
     E,
    SE,
    S,
    SW,
     W,
    NW,
} Direction;

typedef enum {
    OPEN = 1,
    CLOSED = 2,
    VISITED = 4,
} Flag;

typedef struct {
    uint8_t walls;        //Mask of directions available
    uint8_t maze_ind;
    uint8_t flags;
    uint8_t f;
    uint8_t g;
    uint8_t h;
    uint8_t from;
    uint8_t empty;
} Cell;

typedef Cell Maze[CELLS];

#endif
