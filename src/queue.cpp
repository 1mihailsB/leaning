#if !defined (QUEUE)
#include <stdlib.h>

// #define incRotate(inc, compare) if (inc >= compare - 1) {\
//         inc = 0;\
//     } else {\
//         inc++;\
//     }

#define incRotate(inc, compare) inc = (inc < compare - 1) ? inc + 1 : 0;

// #define incCap(inc, compare) if (inc < compare) {\
//     inc++;\
// }

#define incCap(inc, compare) inc = (inc < compare) ? inc + 1 : inc;

struct Queue {
    int maxLen;
    int size;
    int nextTakeAt;
    int nextAddAt;
    int *queue;

    static Queue init(int);
    inline void add(int);
    inline int take();
};

Queue Queue::init(int len) {
    Queue jq = { len };
    jq.queue = (int*) malloc(sizeof(int) * jq.maxLen);
    return jq;
}

// Currently this function will override existing 'nextTakeAt' value if it's called when
// last == nextTakeAt. It does not resize when size reaches maxLen
void Queue::add(int x) {
    queue[nextAddAt] = x;
    incRotate(nextAddAt, maxLen)
    incCap(size, maxLen)
}

int Queue::take() {
    int ret = queue[nextTakeAt];
    size = size == 0 ? 0 : size - 1;
    incRotate(nextTakeAt, maxLen)

    return ret;
}

#define QUEUE
#endif