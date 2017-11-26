#ifndef TEST_MAZE
#define TEST_MAZE

#include "../src/state.h"
#include "../src/Mouse.h"

#define ck_assert_maze_subset(M1, M2) do { \
    int _i = 0; \
    int __x, __y; \
    for (; _i<CELLS; _i++) { \
        if (~M1[_i].walls & M2[_i].walls) { \
            print_maze(M1); \
            print_maze(M2); \
            __x = _i % MAZE; \
            __y = _i / MAZE; \
            ck_assert_msg(0, "Assertion failed: Extra wall found at (%d,%d) '%s'", __x, __y, wc[M2[_i].walls]); \
        } \
    } \
} while (0)

#define ck_assert_path_coord(X, Y, P) do { \
    intmax_t _x = (X); \
    intmax_t _y = (Y); \
    Point _p = (P); \
    ck_assert_msg(_x == _p.x && _y == _p.y, "Assertion '%s' failed: (%d, %d) != (%d, %d)", "("#X","#Y") == "#P, _x, _y, _p.x, _p.y); \
} while (0)

#define ck_assert_state(S) do { \
    void (*_p)() = (S); \
    ck_assert_msg(state == _p, "Assertion '%s' failed: (%d) != (%d)", "state == "#S, state, _p); \
} while (0)

#define setup_mazes(M1, M2, TEXT) \
    create_maze(M1, M2, TEXT); \
    ck_assert_maze_subset(M1, M2);

extern char wc[][16];
extern char empty_maze[];
void print_maze(Maze &maze);
void create_maze(Maze &maze, const char *maze_text);
void create_maze(Maze &hidden, Maze &maze, const char *maze_text);

#endif
