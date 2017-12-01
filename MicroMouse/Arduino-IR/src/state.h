#ifndef MM_STATE_H
#define MM_STATE_H

extern void (*state)();

void INIT();
void RANDOM();
void EXPLORE_TO_CENTER();
void VALIDATE_SHORTEST_PATH();
void RETURN_TO_START();
void BACK_INTO_START();
void RACE_TO_CENTER();
void DONE();
void SENSE();

#endif
