#include "mouse.h"
#include "a_star.h"
#include <stdlib.h>

Open open;

void queue_reprioritize(uint8_t value) {
    int i = open.n;
    while (i>=0 && maze[open.data[i]].id != value) {
      i--;
    }

	if (i < 0) {
		return;
	}

    while (i<open.n && maze[value].f < maze[open.data[i+1]].f) {
        open.data[i] = open.data[i+1];
        i++;
    }
    open.data[i] = value;
}

void queue_push_priority(uint8_t value) {
    int i = open.n; 
    while (i>=0 && maze[value].f > maze[open.data[i]].f) {
        open.data[i+1] = open.data[i];
        i--;
    }

    open.data[i+1] = value;
    open.n++;
}

uint8_t distance (int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void look(int i, int d, int diff, int cost) {
    int g;
    int n = i + diff;
    if (maze[i].walls & d || maze[n].closed) {
        return;
    }

    g = maze[i].g + cost;
    if (g > maze[n].g) {
        return;
    }

    maze[n].path_length = maze[i].path_length + 1;
    maze[n].g = g;
    maze[n].f = maze[n].h + g;
    if (maze[n].open) {
        queue_reprioritize(n);
    }
    else {
        queue_push_priority(n);
    }
}

void add_neighbors(int i) {
    look(i, D,  8, 1);
    look(i, R,  1, 1);
    look(i, L, -1, 1);
    look(i, U, -8, 1);
}

void make_path(int i, Path *path) {
    
}

void find_path(int x1, int y1, int x2, int y2, Path *path) {
    int i, j, c, destination;
    destination = ind(x2, y2);

    c=0;
    for (i=0; i<MAZE; i++) {
        for (j=0; j<MAZE; j++) {
            maze[c].open = 0;
            maze[c].closed = 0;
            maze[c].h = distance(j, i, x2, y2);
            maze[c].g = 0xFF;
        }
    }

    i = ind(x1, y1);
    maze[i].path_length = 0;
    maze[i].g = 0;
    maze[i].f = distance(x1, y1, x2, y2);
    maze[i].from = -1;

    queue_init(open);
    queue_push(open, i);
    while (!queue_empty(open)) {
        i = queue_pop(open);
        maze[i].closed = 1;
        if (i == destination) {
            break;
        }
        add_neighbors(i);
    }

    return make_path(i, path);
}
