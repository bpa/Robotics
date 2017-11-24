#include "a_star.h"
#include "queue.h"

#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include <cmath>
  #define abs std::abs
#endif

PriorityQueue open;

uint8_t distance (int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void look(Maze &maze, Cell *from, int d, int diff, int cost) {
    int g;

    if (from->walls & d) return;

    Cell *to = &maze[from->maze_ind + diff];
    if (to->closed) return;

    g = from->g + cost;
    if (g > to->g) {
        return;
    }

    to->g = g;
    to->f = to->h + g;
    to->from = from->maze_ind;
    if (to->open) {
        queue_reprioritize(open, to);
    }
    else {
        queue_push_priority(open, to);
    }
}

void add_neighbors(Cell *cell, Maze &maze) {
    look(maze, cell, D,  8, 1);
    look(maze, cell, R,  1, 1);
    look(maze, cell, L, -1, 1);
    look(maze, cell, U, -8, 1);
}

void make_path(Cell *cell, Maze &maze, Path &path) {
    Point *p;
    path.size = -1;
    while (cell->maze_ind != cell->from) {
        p = &path.data[++path.size];
        p->x = cell->maze_ind % MAZE;
        p->y = cell->maze_ind / MAZE;
        cell = &maze[cell->from];
    }
}

void find_path(int x1, int y1, int x2, int y2, Maze &maze, Path &path) {
    int i, j, c, destination;
    Cell *cell;
    destination = ind(x2, y2);

    c=0;
    for (i=0; i<MAZE; i++) {
        for (j=0; j<MAZE; j++) {
            cell = &maze[c];
            cell->open = 0;
            cell->closed = 0;
            cell->h = distance(j, i, x2, y2);
            cell->g = 0xFF;
            c++;
        }
    }

    i = ind(x1, y1);
    cell = &maze[i];
    cell->g = 0;
    cell->f = maze[i].h;
    cell->from = i;

    queue_init(open);

    queue_push(open, &maze[i]);

    while (!queue_empty(open)) {
        cell = queue_pop(open);
        cell->closed = 1;
        if (cell->maze_ind == destination) {
            break;
        }
        add_neighbors(cell, maze);
    }

    make_path(cell, maze, path);
}
