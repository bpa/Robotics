#include "Mouse.h"
#include "a_star.h"
#include "state.h"

Cell maze[MAZE*MAZE];
Direction facing;
Path shortest_path;
int x, y;

void mouse_init() {
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
    cell(maze, 0,       0).walls = U | L | R; //Start location
    cell(maze, MAZE_END,0).walls = U | R;
    cell(maze, MAZE_END,MAZE_END).walls = D | R;
    cell(maze, 0,       MAZE_END).walls = D | L;
    find_path(0, 0, MAZE/2, MAZE/2, &shortest_path);
    facing = S;
    x = y = 0;
}
