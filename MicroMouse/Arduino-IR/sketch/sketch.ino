//#include <mouse.h>
#include <AFMotor.h>
#include <Encoder.h>

AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);
Encoder rightOdometer(1, 0);
Encoder leftOdometer(2, 3);

void setup() {
  Serial.begin(9600);
  left_motor.setSpeed(200);
  right_motor.setSpeed(250);
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
//  init();
}

void loop() {
//  left_motor.run(FORWARD);
//  right_motor.run(FORWARD);
//  left_motor.setSpeed(250);
//  left_motor.setSpeed(250);
  delay(500);
  Serial.print("L: ");
  Serial.print(leftOdometer.read());
  Serial.print(" R: ");
  Serial.print(rightOdometer.read());
  Serial.print(" 1: ");
  Serial.print(analogRead(1));
  Serial.print(" 2: ");
  Serial.print(analogRead(2));
  Serial.print(" 3: ");
  Serial.print(analogRead(3));
  Serial.println();
}
