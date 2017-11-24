#include "queue.h"

#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include "mocks.h"
#endif

void queue_reprioritize(PriorityQueue &q, Cell *cell) {
    int i = q.size;
    while (i>=0 && q.data[i]->maze_ind != cell->maze_ind) {
      i--;
    }

	if (i < 0) {
		return;
	}

    while (i < q.size && cell->f < q.data[i+1]->f) {
        q.data[i] = q.data[i+1];
        i++;
    }
    q.data[i] = cell;
}

void queue_push_priority(PriorityQueue &q, Cell *cell) {
    int i = q.size; 
    while (i>=0 && cell->f > q.data[i]->f) {
        q.data[i+1] = q.data[i];
        i--;
    }

    q.data[i+1] = cell;
    q.size++;
}

void queue_print(Path &p) {
    for (int i=p.size; i>=0; i--) {
        Serial.print("(");
        Serial.print(p.data[i].x);
        Serial.print(",");
        Serial.print(p.data[i].y);
        Serial.print(") ");
    }
    Serial.println();
}
