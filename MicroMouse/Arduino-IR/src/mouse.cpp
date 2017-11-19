#include "Mouse.h"
#include "a_star.h"

Mouse mouse;

void mouse_init() {
    int i;
    for (i=1; i<CELLS; i++) {
        mouse.maze[i].maze_ind = i;
    }
    for (i=1; i<MAZE; i++) {
        cell(mouse.maze, i, 0).walls = U;
        cell(mouse.maze, 0, i).walls = L;
        cell(mouse.maze, MAZE_END, i).walls = R;
        cell(mouse.maze, i, MAZE_END).walls = D;
    }
    cell(mouse.maze, 0,       0).walls = U | L | R; //Start location
    cell(mouse.maze, MAZE_END,0).walls = U | R;
    cell(mouse.maze, MAZE_END,MAZE_END).walls = D | R;
    cell(mouse.maze, 0,       MAZE_END).walls = D | L;
    mouse.facing = S;
    mouse.x = mouse.y = 0;
}