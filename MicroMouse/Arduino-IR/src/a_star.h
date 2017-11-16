#ifndef __MM_A_STAR_H__
#define __MM_A_STAR_H__

#include "Mouse.h"
#include "queue.h"

typedef struct {
    int x;
    int y;
} Pair;

Queue(uint8_t, Open, DIST);
Queue(Pair, Path, DIST);

void find_path(int x1, int y1, int x2, int y2, Path *dest);

#endif
