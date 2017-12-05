#include "Mouse.h"
#include "state.h"
#include "hardware.h"
#include "movement.h"
#include "a_star.h"
#include "queue.h"

void (*state)();
int moves = 4;

               //   N    NE E  SE   S  SW  W  NW
int offset[]   = {-MAZE, 0, 1, 0, MAZE, 0, -1, 0};
Wall visible[] = {    U, X, R, X,    D, X,  L, X};

//                 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
int wallCount[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

Direction direction(Point &next) {
    Direction realdir;
         if (mouse.x < next.x) realdir = E;
    else if (mouse.x > next.x) realdir = W;
    else if (mouse.y < next.y) realdir = S;
    else if (mouse.y > next.y) realdir = N;
    return (Direction)((8 + realdir - mouse.facing) % 8);
}

void FWD() {
    if (near_target) {
        if ( moves == 0 ) {
            Serial.println("Done.");
            state = DONE;
            return;
        }
        moves--;
        if (analogRead(FRONT_SENSOR) > 100) {
            Serial.println("Left.");
            move(W);
        }
        else {
            Serial.println("Forward.");
            move(N);
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
        Direction dir[4];
        if (analogRead(FRONT_SENSOR) < 100) {
            dir[available++] = N;
            Serial.print("F ");
        }
        if (analogRead(LEFT_SENSOR) < 100) {
            dir[available++] = W;
            Serial.print("L ");
        }
        if (analogRead(RIGHT_SENSOR) < 100) {
            dir[available++] = E;
            Serial.print("R ");
        }
        if (available == 0) {
            dir[available++] = S;
            Serial.print("B ");
        }
        int d = random(available);
        Serial.print(d);
        Serial.println();
        move(dir[d]);
    }
}

bool found_new_walls() {
    int i;
    Cell *c = &cell(mouse.maze, mouse.x, mouse.y);
    c->flags |= VISITED;
    uint8_t w = c->walls;
    uint8_t observed = w & visible[cardinal(mouse.facing, S)];

    if (analogRead(FRONT_SENSOR) > 100) observed |= visible[cardinal(mouse.facing, N)];
    if (analogRead(RIGHT_SENSOR) > 100) observed |= visible[cardinal(mouse.facing, E)];
    if (analogRead(LEFT_SENSOR)  > 100) observed |= visible[cardinal(mouse.facing, W)];
    if (w != observed) {
        c->walls = observed;
        if (observed & visible[cardinal(mouse.facing, N)]) {
            i = c->maze_ind + offset[cardinal(mouse.facing, N)];
            if (i < CELLS && i >= 0) {
                mouse.maze[i].walls |= visible[cardinal(mouse.facing,S)];
            }
        }
        if (observed & visible[cardinal(mouse.facing,E)]) {
            i = c->maze_ind + offset[cardinal(mouse.facing,E)];
            if (i < CELLS && i >= 0) {
                mouse.maze[i].walls |= visible[cardinal(mouse.facing,W)];
            }
        }
        if (observed & visible[cardinal(mouse.facing,W)]) {
            i = c->maze_ind + offset[cardinal(mouse.facing,W)];
            if (i < CELLS && i >= 0) {
                mouse.maze[i].walls |= visible[cardinal(mouse.facing,E)];
            }
        }
        return true;
    }
    return false;
}

void do_current_move() {
    if (queue_empty(mouse.current_path)) {
        return;
    }

    Point next = queue_pop(mouse.current_path);
    Direction d = direction(next);
    mouse.x = next.x;
    mouse.y = next.y;
    if (d == S) {
        uint8_t w = cell(mouse.maze, mouse.x, mouse.y).walls;
        next = queue_peek(mouse.current_path);
        Direction next_dir = direction(next);
        // backing up again || wall behind mouse
        if (next_dir == S || w & visible[cardinal(mouse.facing,S)]) {
            if (next_dir == S) {
                //Backing up again, just need to turn around, E or W will both work
                next_dir = E;
            }
            Direction opposite = cardinal(next_dir, S);
            if ((w & visible[cardinal(mouse.facing, opposite)]) == 0) {
                //E(2)/2 or W(6)/2 + E(2) = SE(3) or SW(5)
                d = (Direction)(E + opposite / 2);
                mouse.facing = cardinal(mouse.facing, next_dir);
            }
            else if ((w & visible[cardinal(mouse.facing, next_dir)]) == 0) {
                d = (Direction)(E + next_dir / 2);
                mouse.facing = cardinal(mouse.facing, opposite);
            }
        }
    }
    else {
        mouse.facing = cardinal(mouse.facing, d);
    }
    move(d);
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
            state = RETURN_TO_START;
            find_path(mouse.x, mouse.y, 0, 0, mouse.maze, mouse.current_path);
            for (int i=0; i<= mouse.current_path.size; i++) {
                Point next = mouse.current_path.data[i];
                uint8_t w = cell(mouse.maze, next.x, next.y).walls;
                if (wallCount[w] < 2) {
                    find_path(mouse.x, mouse.y, next.x, next.y, mouse.maze, mouse.current_path);
                    break;
                }
            }
        }
        else {
            do_current_move();
        }
    }
}

void RETURN_TO_START() {
    do_current_move();
    if (queue_empty(mouse.current_path)) {
        state = BACK_INTO_START;
        find_path(mouse.x, mouse.y, 0, 0, mouse.maze, mouse.current_path);
    }
}

Direction fwd[] = { W, N, E };

void BACK_INTO_START() {
    if (near_target) {
        Point next = queue_peek(mouse.current_path);
        Direction d = direction(next);

        if (d == S) {
            mouse.x = next.x;
            mouse.y = next.y;
            if (mouse.current_path.size == 0) {
                move_to_start();
                state = RACE_TO_CENTER;
                find_path(0, 0, MAZE/2, MAZE/2, mouse.maze, mouse.current_path);
                return;
            }
            queue_pop(mouse.current_path);
            next = queue_peek(mouse.current_path);
            Direction next_dir = direction(next);
            switch (next_dir) {
                case S: move( S);  break;
                case E: move(_SE); break; //Thank Arduino for defining SE
                case W: move( SW); break;
            }
            mouse.facing = (Direction)((mouse.facing + next_dir + S) % 8);
        }
        else {
            Cell c = cell(mouse.maze, mouse.x, mouse.y);
            for (int i=0; i<3; i++) {
                Direction alt = cardinal(mouse.facing, fwd[i]);
                if (!(d == fwd[i] || (c.walls & visible[alt]))) {
                    move(fwd[i]);
                    mouse.facing = alt;
                    next.x = mouse.x;
                    next.y = mouse.y;
                    queue_push(mouse.current_path, next);
                    int maze_ind = c.maze_ind + offset[mouse.facing];
                    mouse.x = maze_ind % MAZE;
                    mouse.y = maze_ind / MAZE;
                    break;
                }
            }
        }
    }
}

void RACE_TO_CENTER() {
    if (near_target) {
        do_current_move();
        if (queue_empty(mouse.current_path)) {
            state = DONE;
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
    delay(10000);
}
