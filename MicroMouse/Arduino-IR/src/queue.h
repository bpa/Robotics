#ifndef __MM_QUEUE_H__
#define __MM_QUEUE_H__

#define Queue(type, name, size) \
typedef struct { \
    int n; \
    type data[size]; \
} name

#define queue_init(q) q.n = -1
#define queue_pop(q) q.data[q.n--]
#define queue_push(q, v) (q.data[++q.n] = v)
#define queue_empty(q) (q.n < 0)

#endif
