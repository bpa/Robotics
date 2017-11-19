#ifndef MM_MOUSE_H
#define MM_MOUSE_H 

#include "maze.h"
#include "queue.h"

typedef struct {
    Maze maze;
    Path shortest_path;
    Path current_path;
    Direction facing;
    int x;
    int y;
} Mouse;

void mouse_init();
extern Mouse mouse;
    
#endif
