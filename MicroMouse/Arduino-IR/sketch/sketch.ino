#include <Mouse.h>
#include <state.h>
#include <movement.h>

void setup() {
  Serial.begin(9600);
  INIT();
}

void loop() {
  update_position();
  if (near_target()) {
    state();
  }
}
