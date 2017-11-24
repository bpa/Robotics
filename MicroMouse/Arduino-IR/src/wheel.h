#ifndef MM_WHEEL_H
#define MM_WHEEL_H

#ifdef ARDUINO
  #include <Encoder.h>
  #include <AFMotor.h>
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
    int near;
    int target;
    int speed;
    bool _forward;
    bool targeting;
    AF_DCMotor *motor;
    Encoder *encoder;
};
    
#endif
