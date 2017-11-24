#include "Mouse.h"
#include "state.h"
#include "hardware.h"
#include "movement.h"
#include "a_star.h"
#include "queue.h"

//              0    123456789012345
char wc[][16] = {
    " ", "┬", "┤", "┐", //0123
    "┴", "═", "┘", "]", //4567
    "├", "┌", "║", "∩", //8901
    "└", "[", "∪", "□"};//2345
extern Path shortest_path;
void (*state)();
int moves = 4;
//   F  L  R  D
Wall visible[][4] = {
    {U, L, R, D}, //N
    {R, U, D, L}, //E
    {D, R, L, U}, //S
    {L, D, U, R}, //W
};
//   N             E           S             W 
void (*move[][4])() = {
    {move_forward, move_left, move_backward, move_right}, //N
    {move_left, move_forward, move_right, move_backward}, //E
    {move_backward, move_left, move_forward, move_right}, //S
    {move_right, move_backward, move_left, move_forward}, //W
};

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
        uint8_t observed = w & visible[mouse.facing][3];

        if (analogRead(FRONT_SENSOR) > 100) observed |= visible[mouse.facing][0];
        if (analogRead(LEFT_SENSOR)  > 100) observed |= visible[mouse.facing][1];
        if (analogRead(RIGHT_SENSOR) > 100) observed |= visible[mouse.facing][2];
        if ( w != observed ) {
            cell(mouse.maze, mouse.x, mouse.y).walls = observed;
            find_path(mouse.x, mouse.y, MAZE/2, MAZE/2, mouse.maze, mouse.shortest_path);
        }
        Point next = mouse.shortest_path.data[mouse.shortest_path.size];
        Direction d;
             if (mouse.x < next.x) d = E;
        else if (mouse.x > next.x) d = W;
        else if (mouse.y < next.y) d = S;
        else if (mouse.y > next.y) d = N;
        void (*cmd)() = move[mouse.facing][d];
        if (cmd != move_left && cmd != move_right) {
            mouse.x = next.x;
            mouse.y = next.y;
            queue_pop(mouse.shortest_path);
            if (mouse.x == MAZE/2 && mouse.y == MAZE/2) {
                state = VALIDATE_SHORTEST_PATH;
                return;
            }
        }
        if (cmd != move_backward) {
            mouse.facing = d;
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
