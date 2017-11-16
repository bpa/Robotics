#include "Mouse.h"
#include "state.h"
#include "hardware.h"
#include "movement.h"

void (*state)();
int moves = 7;

void INIT() {
    Serial.begin(9600);
    right_motor.run(RELEASE);
    mouse_init();
    randomSeed(analogRead(0));
    delay(2000);
    near_target = true;
    state = RANDOM;
}

void RANDOM() {
    if (near_target) {
        if (moves == 0) {
            state = EXPLORE_TO_CENTER;
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
    stop();
    delay(100);
    Serial.print("L: ");
    Serial.print(leftOdometer.read());
    Serial.print(" R: ");
    Serial.print(rightOdometer.read());
    Serial.print(" F: ");
    Serial.print(analogRead(FRONT_SENSOR));
    Serial.print(" L: ");
    Serial.print(analogRead(LEFT_SENSOR));
    Serial.print(" R: ");
    Serial.print(analogRead(RIGHT_SENSOR));
    Serial.println();
    state = DONE;
}

void VALIDATE_SHORTEST_PATH() {
}

void RACE_TO_CENTER() {
    delay(500);
    Serial.print("L: ");
    Serial.print(leftOdometer.read());
    Serial.print(" R: ");
    Serial.print(rightOdometer.read());
    Serial.print(" F: ");
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
