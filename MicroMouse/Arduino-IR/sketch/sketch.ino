#include <Mouse.h>
#include <state.h>
#include <movement.h>

void setup() {
  state = INIT;
}

void loop() {
  movement();
  state();
}
