#ifndef LIBSOLV_H
#define LIBSOLV_H

#include <stdint.h>
#define MAZE 8
#define MAZE_END (MAZE-1)
#define DIST (MAZE*MAZE)
#define DIST_END (DIST-1)
#define ind(x, y) (y * MAZE + x)
#define cell(v, x, y) v[ ind(x,y) ]
#define set_coord(i, x, y) x=i%MAZE; y=i/MAZE
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
    uint8_t id;
    uint8_t visited;
    uint8_t open;
    uint8_t closed;
    uint8_t f;
    uint8_t g;
    uint8_t h;
    uint8_t from;
    uint8_t walls;
    uint8_t path_length;
} Cell;

extern Cell maze[MAZE*MAZE];
extern Direction facing;
extern int x, y;

void mouse_init();
void reach_center();
void discover();
void done();
    
#endif
