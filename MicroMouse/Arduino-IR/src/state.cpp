#include "Mouse.h"
#include "state.h"
#include "hardware.h"
#include "movement.h"
#include "a_star.h"
#include "queue.h"

const char *dir(Direction);
const char *wall(Wall);
void (*state)();
int moves = 4;

//   N  E  S  W
int offset[][4] = {
    {-MAZE, 1, MAZE, -1}, //N
    {1, MAZE, -MAZE, -1}, //E
    {MAZE, -1, 1, -MAZE}, //S
    {-1, -MAZE, MAZE, 1}, //W
};
//   N  E  S  W
Wall visible[][4] = {
    {U, R, D, L}, //N
    {R, D, L, U}, //E
    {D, L, U, R}, //S
    {L, U, R, D}, //W
};
//   N  E  S  W
Direction turn[][4] = {
    {N, E, S, W}, //N
    {E, S, W, N}, //E
    {S, W, N, E}, //S
    {W, N, E, S}, //W
};

void (*back_up[][4])() = {
//   N             E           S             W
    {move_forward, move_back_right, move_backward, move_back_left}, //N
    {move_back_left, move_forward, move_back_right, move_backward}, //E
    {move_backward, move_back_left, move_forward, move_back_right}, //S
    {move_back_right, move_backward, move_back_left, move_forward}, //W
};

void (*move[][4])() = {
//   N             E           S             W
    {move_forward, move_right, move_backward, move_left}, //N
    {move_left, move_forward, move_right, move_backward}, //E
    {move_backward, move_left, move_forward, move_right}, //S
    {move_right, move_backward, move_left, move_forward}, //W
};

Direction direction(Point &next) {
         if (mouse.x < next.x) return E;
    else if (mouse.x > next.x) return W;
    else if (mouse.y < next.y) return S;
    else if (mouse.y > next.y) return N;
}

void FWD() {
    if (near_target) {
        if ( moves == 0 ) {
            Serial.println("Done.");
            stop();
            state = DONE;
            return;
        }
        moves--;
        if (analogRead(FRONT_SENSOR) > 100) {
            Serial.println("Left.");
            move_left();
        }
        else {
            Serial.println("Forward.");
            move_forward();
        }
    }
}

void INIT() {
    delay(500);
    if (analogRead(FRONT_SENSOR) > 100) {
        state = SENSE;
        return;
    }

    mouse_init();
    find_path(0, 0, MAZE/2, MAZE/2, mouse.maze, mouse.current_path);
    randomSeed(analogRead(0));
    delay(1500);
    near_target = true;
    state = EXPLORE_TO_CENTER;
}

void RANDOM() {
    if (near_target) {
        if (moves == 0) {
            state = DONE;
            return;
        }
        moves--;
        int available = 0;
        void (*dir[4])();
        if (analogRead(FRONT_SENSOR) < 100) {
            dir[available++] = move_forward;
            Serial.print("F ");
        }
        if (analogRead(LEFT_SENSOR) < 100) {
            dir[available++] = move_left;
            Serial.print("L ");
        }
        if (analogRead(RIGHT_SENSOR) < 100) {
            dir[available++] = move_right;
            Serial.print("R ");
        }
        if (available == 0) {
            dir[available++] = move_backward;
            Serial.print("B ");
        }
        int d = random(available);
        Serial.print(d);
        Serial.println();
        dir[d]();
    }
}

bool found_new_walls() {
    Cell *c = &cell(mouse.maze, mouse.x, mouse.y);
    c->flags |= VISITED;
    uint8_t w = c->walls;
    uint8_t observed = w & visible[mouse.facing][S];

    if (analogRead(FRONT_SENSOR) > 100) observed |= visible[mouse.facing][N];
    if (analogRead(RIGHT_SENSOR) > 100) observed |= visible[mouse.facing][E];
    if (analogRead(LEFT_SENSOR)  > 100) observed |= visible[mouse.facing][W];
    if (w != observed) {
        cell(mouse.maze, mouse.x, mouse.y).walls = observed;
        return true;
    }
    return false;
}

void do_current_move() {
    Point next = queue_pop(mouse.current_path);
    Direction d = direction(next);
    void (*cmd)() = move[mouse.facing][d];
    mouse.x = next.x;
    mouse.y = next.y;
    if (cmd == move_backward) {
        uint8_t w = cell(mouse.maze, mouse.x, mouse.y).walls;
        next = queue_peek(mouse.current_path);
        Direction db = direction(next);
        // backing up again || wall behind mouse
        if (db == d || w & visible[mouse.facing][S]) {
            if (db == d) {
                //Backing up again, just need to turn around, E or W would work
                db = turn[mouse.facing][E];
            }
            if ((w & visible[db][S]) == 0) {
                //Opposite direction is open
                cmd = back_up[mouse.facing][turn[db][S]];
                mouse.facing = db;
            }
            else if ((w & visible[db][N]) == 0) {
                //Direction we are going is open
                cmd = back_up[mouse.facing][db];
                mouse.facing = turn[db][S];
            }
        }
    }
    else {
        mouse.facing = d;
    }
    cmd();
}

void EXPLORE_TO_CENTER() {
    if (near_target) {
        if ( found_new_walls() ) {
            find_path(mouse.x, mouse.y, MAZE/2, MAZE/2, mouse.maze, mouse.current_path);
        }
        do_current_move();
        if (queue_empty(mouse.current_path)) {
            state = VALIDATE_SHORTEST_PATH;
        }
    }
}

void VALIDATE_SHORTEST_PATH() {
    if (near_target) {
        if ( found_new_walls() || queue_empty(mouse.current_path) ) {
            Point next;
            find_path(0, 0, MAZE/2, MAZE/2, mouse.maze, mouse.shortest_path);
            for (int i=0; i<= mouse.shortest_path.size; i++) {
                next = mouse.shortest_path.data[i];
                if ((cell(mouse.maze, next.x, next.y).flags & VISITED) == 0) {
                    find_path(mouse.x, mouse.y, next.x, next.y, mouse.maze, mouse.current_path);
                    break;
                }
            }
        }
        if (queue_empty(mouse.current_path)) {
            state = BACK_INTO_START;
        }
        else {
            do_current_move();
        }
    }
}

void BACK_INTO_START() {
    if (near_target) {
        state = RACE_TO_CENTER;
        find_path(0, 0, MAZE/2, MAZE/2, mouse.maze, mouse.current_path);
    }
}

void RACE_TO_CENTER() {
    if (near_target) {
        if (queue_empty(mouse.current_path)) {
            state = DONE;
        }
        else {
            do_current_move();
        }
    }
}

void SENSE() {
    delay(500);
    Serial.print("F: ");
    Serial.print(analogRead(FRONT_SENSOR));
    Serial.print(" L: ");
    Serial.print(analogRead(LEFT_SENSOR));
    Serial.print(" R: ");
    Serial.print(analogRead(RIGHT_SENSOR));
    Serial.println();
}

void DONE() {
    stop();
    delay(10000);
}
