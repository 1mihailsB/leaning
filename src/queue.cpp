#if !defined (QUEUE)
#include <stdlib.h>

#define incRotate(inc, compare) inc = (inc < compare - 1) ? inc + 1 : 0
#define incCap(inc, compare) inc = (inc < compare) ? inc + 1 : inc

// TODO: memory cleanup function
template <typename T>
struct Queue {
    int maxLen;
    int size;
    int nextTakeAt;
    int nextAddAt;
    T *queue;

    static Queue init(int);
    inline void add(T);
    inline T take();
};

template <typename T>
Queue<T> Queue<T>::init(int len) {
    Queue jq = { len };
    jq.queue = (T*) malloc(sizeof(T) * jq.maxLen);
    return jq;
}

// Currently this function will override existing 'nextTakeAt' value if it's called when
// nextAddAt == nextTakeAt. It does not resize when size reaches maxLen
template <typename T>
void Queue<T>::add(T x) {
    queue[nextAddAt] = x;
    incRotate(nextAddAt, maxLen);
    incCap(size, maxLen);
}

template <typename T>
T Queue<T>::take() {
    T ret = queue[nextTakeAt];
    size = size == 0 ? 0 : size - 1;
    incRotate(nextTakeAt, maxLen);

    return ret;
}

#undef incRotate
#undef incCap

#define QUEUE
#endif