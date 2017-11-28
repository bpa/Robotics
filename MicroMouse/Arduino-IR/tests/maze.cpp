#include "maze.h"
#include "../src/Mouse.h"
#include <stdio.h>

const char *dir(Direction d) {
    switch (d) {
        case N:  return "N";
        case S:  return "S";
        case E:  return "E";
        case W:  return "W";
        default: return "?";
    }
}

const char *wall(Wall w) {
    switch (w) {
        case U:  return "U";
        case R:  return "R";
        case D:  return "D";
        case L:  return "L";
        case DR: return "DR";
        case DL: return "DL";
        default: return "?";
    }
}

           //        X    Y   XY
char tl[][4] = {" ", "╷", "╶", "┌"};//Top left
char tr[][4] = {" ", "╷", "╴", "┐"};//Top right
char bl[][4] = {" ", "╵", "╶", "└"};//Bottom left
char br[][4] = {" ", "╵", "╴", "┘"};//Bottom right

char wc[][16] = {
    " ", "┬", "┤", "┐", //0123
    "┴", "═", "┘", "]", //4567
    "├", "┌", "║", "∩", //8901
    "└", "[", "∪", "□"};//2345

void print_maze(Maze &maze) {
    int c=0, x, y, w;
    for (y=0; y<MAZE; y++) {
        for (x=0; x<MAZE; x++) {
            w = maze[c].walls;
            printf("%s", tl[(w&L?1:0) + (w&U?2:0)]);
            printf("%s", tr[(w&R?1:0) + (w&U?2:0)]);
            c++;
        }
        printf("\n");
        c-=MAZE;
        for (x=0; x<MAZE; x++) {
            w = maze[c].walls;
            printf("%s", bl[(w&L?1:0) + (w&D?2:0)]);
            printf("%s", br[(w&R?1:0) + (w&D?2:0)]);
            c++;
        }
        printf("\n");
    }
    printf("\n");
}

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
    int c=0, x, y, w;
    for (y=0; y<MAZE; y++) {
        for (x=0; x<MAZE; x++) {
            w = 0;
            if (maze_text[y*17+x*2+1]  == '_') { w |= U; }
            if (maze_text[y*17+x*2+17] == '|') { w |= L; }
            if (maze_text[y*17+x*2+18] == '_') { w |= D; }
            if (maze_text[y*17+x*2+19] == '|') { w |= R; }
            maze[c].walls = w;
            maze[c].maze_ind = c;
            c++;
        }
    }
}

void create_maze(Maze &hidden, Maze &maze, const char *maze_text) {
    int c=0, x, y, w;
    for (y=0; y<MAZE; y++) {
        for (x=0; x<MAZE; x++) {
            w = 0;
            if (maze_text[y*35+x*2+1]  == '_') { w |= U; }
            if (maze_text[y*35+x*2+35] == '|') { w |= L; }
            if (maze_text[y*35+x*2+36] == '_') { w |= D; }
            if (maze_text[y*35+x*2+37] == '|') { w |= R; }
            hidden[c].walls = w;
            hidden[c].maze_ind = c;

            w = 0;
            if (maze_text[y*35+x*2+19] == '_') { w |= U; }
            if (maze_text[y*35+x*2+53] == '|') { w |= L; }
            if (maze_text[y*35+x*2+54] == '_') { w |= D; }
            if (maze_text[y*35+x*2+55] == '|') { w |= R; }
            maze[c].walls = w;
            maze[c].maze_ind = c;
            c++;
        }
    }
}

