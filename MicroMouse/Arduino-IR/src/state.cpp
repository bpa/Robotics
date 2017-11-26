#include "Mouse.h"
#include "state.h"
#include "hardware.h"
#include "movement.h"
#include "a_star.h"
#include "queue.h"

extern Path shortest_path;
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
//   N             E           S             W 
void (*move[][4])() = {
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
    find_path(0, 0, MAZE/2, MAZE/2, mouse.maze, mouse.shortest_path);
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

void EXPLORE_TO_CENTER() {
    if (near_target) {
        uint8_t w = cell(mouse.maze, mouse.x, mouse.y).walls;
        uint8_t observed = w & visible[mouse.facing][S];

        if (analogRead(FRONT_SENSOR) > 100) observed |= visible[mouse.facing][N];
        if (analogRead(RIGHT_SENSOR) > 100) observed |= visible[mouse.facing][E];
        if (analogRead(LEFT_SENSOR)  > 100) observed |= visible[mouse.facing][W];
        if ( w != observed ) {
            cell(mouse.maze, mouse.x, mouse.y).walls = observed;
            find_path(mouse.x, mouse.y, MAZE/2, MAZE/2, mouse.maze, mouse.shortest_path);
        }
        Point next = queue_pop(mouse.shortest_path);
        Direction d = direction(next);
        void (*cmd)() = move[mouse.facing][d];
        mouse.x = next.x;
        mouse.y = next.y;
        if (cmd == move_backward) {
            int behind = ind(mouse.x, mouse.y) + offset[mouse.facing][S];
            uint8_t cell = mouse.maze[behind].walls;
            next = queue_peek(mouse.shortest_path);
            Direction db = direction(next);
            if (cell & visible[mouse.facing][S]) {
                //There is a wall behind the mouse, need to turn
                if (cell & visible[mouse.facing][db]) {
                }
                else {
                }
            }
        }
        else {
            mouse.facing = d;
        }
        if (mouse.x == MAZE/2 && mouse.y == MAZE/2) {
            state = VALIDATE_SHORTEST_PATH;
        }
        cmd();
    }
}

void VALIDATE_SHORTEST_PATH() {
    state = DONE;
}

void RACE_TO_CENTER() {
    state = DONE;
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
