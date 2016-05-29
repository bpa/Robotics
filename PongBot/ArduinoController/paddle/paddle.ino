#include <AFMotor.h>

AF_Stepper left(4096, 1);
AF_Stepper right(4096, 2);

void setup() {
  Serial.begin(9600);
  Serial.println("Stepper test!");

  left.setSpeed(10);
}

void loop() {
  Serial.println("Double coil steps");
  left.step(100, FORWARD, DOUBLE); 
}
