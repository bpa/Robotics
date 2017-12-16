#ifndef MM_MOUSE_H
#define MM_MOUSE_H 

#include "maze.h"
#include "queue.h"

typedef struct {
    Maze maze;
    Path path;
    Direction facing;
    int x;
    int y;
} Mouse;

extern Mouse mouse;
    
#endif
