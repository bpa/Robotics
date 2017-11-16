#ifndef _MOUSE_STATE_
#define _MOUSE_STATE_

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <unistd.h>
    //This isn't really correct, I just want it to compile
    #define delay(x) sleep(x/1000)
#endif

extern void (*state)();
extern void INIT();
extern void RANDOM();
extern void EXPLORE_TO_CENTER();
extern void VALIDATE_SHORTEST_PATH();
extern void RACE_TO_CENTER();
extern void DONE();

#endif
