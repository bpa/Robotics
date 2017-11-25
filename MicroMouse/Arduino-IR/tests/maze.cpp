#include "maze.h"
#include "../src/Mouse.h"

char wc[][16] = {
    " ", "┬", "┤", "┐", //0123
    "┴", "═", "┘", "]", //4567
    "├", "┌", "║", "∩", //8901
    "└", "[", "∪", "□"};//2345

char empty_maze[] =
//0 1 2 3 4 5 6 7
"_________________"
"| |             |" //0
"|               |" //1
"|               |" //2
"|               |" //3
"|               |" //4
"|               |" //5
"|               |" //6
"|_______________|";//7

void create_maze(Maze &maze, const char *maze_text) {
    int x, y, w;
    maze[0].walls = w;
    for (x=0; x<MAZE; x++) {
        for (y=0; y<MAZE; y++) {
            w = 0;
            if (maze_text[y*17+x*2+1]  == '_') { w |= U; }
            if (maze_text[y*17+x*2+17] == '|') { w |= L; }
            if (maze_text[y*17+x*2+18] == '_') { w |= D; }
            if (maze_text[y*17+x*2+19] == '|') { w |= R; }
            maze[ ind(x, y) ].walls = w;
            maze[ ind(x, y) ].maze_ind = ind(x, y);
        }
    }
}
