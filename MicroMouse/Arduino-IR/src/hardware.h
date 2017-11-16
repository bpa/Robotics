#ifndef MOUSE_BODY
#define MOUSE_BODY

#ifdef ARDUINO
  #include <AFMotor.h>
  #include <Encoder.h>

#else
  #include "mocks.h"
#endif

#define NEAR 5

class Wheel {
public:
    Wheel(AF_DCMotor *motor, Encoder *encoder);
    bool near_target(long current);
    void forward(int target, int speed);
    void backwards(int target, int speed);
    void reset(long current);
    void stop();

private:
    int near = 0;
    int target = 0;
    int speed = 0;
    bool _forward = true;
    bool targeting = false;
    AF_DCMotor *motor;
    Encoder *encoder;
};
    
extern AF_DCMotor right_motor;
extern AF_DCMotor left_motor;
extern Encoder rightOdometer;
extern Encoder leftOdometer;
extern Wheel left;
extern Wheel right;

#define FRONT_SENSOR 1
#define LEFT_SENSOR  2
#define RIGHT_SENSOR 3

#endif
