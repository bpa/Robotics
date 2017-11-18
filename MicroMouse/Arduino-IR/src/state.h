#ifndef _MOUSE_STATE_
#define _MOUSE_STATE_

extern void (*state)();
extern void INIT();
extern void RANDOM();
extern void EXPLORE_TO_CENTER();
extern void VALIDATE_SHORTEST_PATH();
extern void RACE_TO_CENTER();
extern void DONE();
extern void SENSE();

#endif
