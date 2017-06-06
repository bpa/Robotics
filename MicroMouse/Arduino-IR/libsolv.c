#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define SIZE 8
#define loc(v, x, y) v[y * SIZE + x]
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

typedef struct {
    uint8_t walls;
    int center_distance;
    int center_score;
    int start_distance;
    int start_score;
    bool closed;
} Cell;

Cell maze[SIZE * SIZE];

void init() {
    Cell *cell;
    int x, y;
    int z = SIZE - 1;
    float c = (SIZE - 1) / 2.0;
    for (y=0; y<SIZE; y++) {
        loc(maze, y, 0).walls = U | KNOWN_U;
        loc(maze, z, y).walls = R | KNOWN_R;
        loc(maze, y, z).walls = D | KNOWN_D;
        loc(maze, 0, y).walls = L | KNOWN_L;
        for (x=0; x<SIZE; x++) {
            cell = &loc(maze, x, y);
            cell->center_distance = dist(c, x) + dist(c, y);
            cell->start_distance = dist(0, x) + dist(0, y);
        }
    }
    loc(maze, 0,0).walls = U | L | KNOWN_U | KNOWN_L;
    loc(maze, z,0).walls = U | R | KNOWN_U | KNOWN_R;
    loc(maze, z,z).walls = D | R | KNOWN_D | KNOWN_R;
    loc(maze, 0,z).walls = D | L | KNOWN_D | KNOWN_L;
}

Cell *cell(int x, int y) {
    return &loc(maze, x, y);
}
