#include <AFMotor.h>

#define SYNC_ERROR    0b00000001
#define LEFT_MOTOR    0b00000010
#define RIGHT_MOTOR   0b00000100
#define MOVE_FORWARD  0b00001000
#define MOVE_BACKWARD 0b00010000
#define STOP          0b00100000

#define TIMEOUT 1000

AF_DCMotor left_motor(2);
AF_DCMotor right_motor(3);

uint8_t cmd[2];
boolean syncing = false;
boolean recent = false;
boolean connection_lost = true;
unsigned long last_read_time;

void handle_cmd();
void check_for_timeout();

void setup() {
  Serial.begin(115200);
  left_motor.setSpeed(0);
  right_motor.setSpeed(0);
}
 
void loop() {
  if (syncing) {
    if (Serial.available()) {
      Serial.read();
      recent = true;
      syncing = false;
    }
    else {
      check_for_timeout();
    }
    return;
  }
  
  if (Serial.available() > 1) {
    Serial.readBytes((char*)&cmd, 2);
    recent = true;
    if (cmd[0] & SYNC_ERROR) {
      syncing = true;
    }
    else {
      handle_cmd();
    }
    return;
  }
  
  check_for_timeout();
}

inline void handle_cmd() {
  uint8_t action;
  if (cmd[0] & STOP) {
    action = RELEASE;
  }
  else if (cmd[0] & MOVE_FORWARD) {
    action = FORWARD;
  }
  else if (cmd[0] & MOVE_BACKWARD) {
    action = BACKWARD;
  }
  else {
    return;
  }
   
  if (cmd[0] & LEFT_MOTOR) {
    left_motor.setSpeed(cmd[1]);
    left_motor.run(action);
  }
  if (cmd[0] & RIGHT_MOTOR) {
    right_motor.setSpeed(cmd[1]);
    right_motor.run(action);
  }
}

void check_for_timeout() {
  if (connection_lost) {
    return;
  }
  
  if (recent) {
    recent = false;
    connection_lost = false;
    last_read_time = millis();
  }
  else {
    if (millis() - last_read_time > TIMEOUT) {
      connection_lost = true;
      left_motor.run(RELEASE);
      right_motor.run(RELEASE);
    }
  }
}
