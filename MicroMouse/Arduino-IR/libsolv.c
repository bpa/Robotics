#include <stdio.h>
#include <stdint.h>

#define MAZE 8
#define MAZE_END (MAZE-1)
#define DIST (MAZE*MAZE)
#define DIST_END (DIST-1)
#define loc(v, x, y) v[y * MAZE + x]
#define dist(a, b) (a < b ? b - a : a - b)

typedef enum {
    U = 1,
    R = 2,
    D = 4,
    L = 8,
    KNOWN_U = 16,
    KNOWN_R = 32,
    KNOWN_D = 64,
    KNOWN_L = 128
} Wall;

extern uint8_t maze[MAZE*MAZE];
extern int distance[DIST*DIST];

uint8_t maze[MAZE*MAZE];
int distance[DIST*DIST];

void init() {
    int i;
    for (i=0; i<DIST*DIST; i++) {
        distance[i] = DIST*DIST;
    }
    for (i=0; i<MAZE*MAZE; i++) {
        loc(distance, i, i) = 0;
        maze[i] = 0;
    }
    for (i=1; i<MAZE; i++) {
        loc(maze, i, 0) = U | KNOWN_U;
        loc(maze, 0, i) = L | KNOWN_L;
        loc(maze, MAZE_END, i) = R | KNOWN_R;
        loc(maze, i, MAZE_END) = D | KNOWN_D;
    }
    loc(maze, 0,       0) = U | L | KNOWN_U | KNOWN_L;
    loc(maze, MAZE_END,0) = U | R | KNOWN_U | KNOWN_R;
    loc(maze, MAZE_END,MAZE_END) = D | R | KNOWN_D | KNOWN_R;
    loc(maze, 0,       MAZE_END) = D | L | KNOWN_D | KNOWN_L;
}
