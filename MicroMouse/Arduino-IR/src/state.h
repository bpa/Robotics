#ifndef MM_STATE_H
#define MM_STATE_H

extern void (*state)();
extern void INIT();
extern void RANDOM();
extern void EXPLORE_TO_CENTER();
extern void BACK_INTO_START();
extern void VALIDATE_SHORTEST_PATH();
extern void RACE_TO_CENTER();
extern void DONE();
extern void SENSE();

#endif
