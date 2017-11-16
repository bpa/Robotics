#ifndef _MOUSE_MOVEMENT_
#define _MOUSE_MOVEMENT_

#ifdef ARDUINO
    #include <Arduino.h>
#endif

extern bool near_target;
extern bool stopped;

extern void movement();
extern void move_right();
extern void move_left();
extern void move_forward();
extern void move_backward();
extern void stop();

#endif
