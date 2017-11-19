#include <Mouse.h>
#include <state.h>
#include <movement.h>

void setup() {
  Serial.begin(9600);
  state = INIT;
}

void loop() {
  movement();
  state();
}
