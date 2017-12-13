#ifndef TEST_MAZE
#define TEST_MAZE

#include "../src/state.h"
#include "../src/Mouse.h"
#include "../src/a_star.h"
#include "../src/state.h"
#include "../src/hardware.h"
#include "../src/movement.h"

#define place_mouse(X, Y, D, X2, Y2) \
    cx=X; \
    cy=Y; \
    looking = D; \
    mouse.x = X; \
    mouse.y = Y; \
    mouse.facing = D; \
    find_path(mouse.x, mouse.y, X2, Y2, mouse.maze, mouse.path); 

#define ck_assert_step(W, D, X, Y, S) do { \
    update_mouse(X, Y); \
    Wall _f = (W); \
    Direction _d = (D); \
    intmax_t _x = (X); \
    intmax_t _y = (Y); \
    void (*_p)() = (S); \
    ck_assert_msg(_f == moving, "Assertion '%s' failed: %s != %s", "moving == "#W, wall(moving), wall(_f)); \
    ck_assert_msg(_d == mouse.facing, "Assertion '%s' failed: %s != %s", "direction == "#D, dir(mouse.facing), dir(_d)); \
    ck_assert_msg(_x == mouse.x && _y == mouse.y, "Assertion '%s' failed: (%d, %d) != (%d, %d)", "("#X","#Y") == loc", _x, _y, mouse.x, mouse.y); \
    ck_assert_msg(state == _p, "Assertion '%s' failed: %d != %d", "state == "#S, state, _p); \
    looking = D; \
    ck_assert_maze_subset(hidden, mouse.maze); \
} while (0)

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

#define setup_mazes(TEXT) \
    create_maze(TEXT); \
    ck_assert_maze_subset(hidden, mouse.maze);

extern Wall moving;
extern Mouse mouse;
extern Maze hidden;
extern char wc[][16];
extern char empty_maze[];
extern int cx, cy;
extern Direction looking;

const char *wall(Wall w);
const char *dir(Direction d);
void update_mouse(int x, int y);
void print_maze(Maze &maze);
void create_maze(Maze &maze, const char *maze_text);
void create_maze(const char *maze_text);

#endif
