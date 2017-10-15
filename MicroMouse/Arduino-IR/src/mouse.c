#include <stdio.h>
#include <stdint.h>
#include "mouse.h"
#include "a_star.h"

Cell maze[MAZE*MAZE];
Path shortest_path;

void (*state)();

void setup() {
    int i;
    for (i=0; i<DIST; i++) {
        maze[i].id = i;
    }
    for (i=1; i<MAZE; i++) {
        cell(maze, i, 0).walls = U;
        cell(maze, 0, i).walls = L;
        cell(maze, MAZE_END, i).walls = R;
        cell(maze, i, MAZE_END).walls = D;
    }
    cell(maze, 0,       0).walls = U | L;
    cell(maze, MAZE_END,0).walls = U | R;
    cell(maze, MAZE_END,MAZE_END).walls = D | R | L; //Start location
    cell(maze, 0,       MAZE_END).walls = D | L;
    find_path(MAZE_END, MAZE_END, MAZE/2, MAZE/2, &shortest_path);
    state = reach_center;
}

void loop() {
    state();
}

void reach_center() {
}

void discover() {
}

void done() {
    //We reached the center and can power down
}
