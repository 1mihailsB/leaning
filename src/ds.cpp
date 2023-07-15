#if !defined DATA_STRUCTURES

#include <stdlib.h>

///////////////////////////////////////////////////////////////////// HASHMAP ///////////////////////////////////////////////////////

#define CREATE_NODE(itm) (HashMapNode<T> *) malloc(sizeof(HashMapNode<T>));\
        newNode->taken = true; \
        newNode->item = itm

template <typename T>
struct HashMapNode {
    bool taken;
    T item;
    HashMapNode<T> *next;
};

enum AddResult {
    OK,
    ALREADY_PRESENT = -1
};

template <typename K, typename T>
struct HashMap {
    unsigned int capacity;
    unsigned int count;
    HashMapNode<T> *entries;

    T* add(K, T);
    T* get(K);

    static HashMap<K, T> init(unsigned int);
};

template <typename K, typename T>
HashMap<K, T> HashMap<K, T>::init(unsigned int capacity) {
    HashMap<K, T> hm = { capacity };
    hm.entries = (HashMapNode<T>*) malloc(sizeof(HashMapNode<T>) * hm.capacity);

    if (hm.entries == NULL) {
        printf("Can't allocate hashmap entries\n");
        exit(EXIT_FAILURE);
    }

    return hm;
}

static inline int hash(int key, unsigned int max) {
    return key % max;
}

template <typename K, typename T>
T* HashMap<K, T>::add(K key, T item) {
    int hsh = hash(key, this->capacity);
    HashMapNode<T> *entry = &(this->entries[hsh]);

    if (entry->taken == true) {
        if (entry->next == NULL) {
            if (entry->item == item) return NULL;

            HashMapNode<T> *newNode = CREATE_NODE(item);
            entry->next = newNode;
        } else {
            // for some item types operator== is overriden for this check
            if (entry->next->item == item) return NULL;
            HashMapNode<T> *next = entry->next;

            while (next->next != NULL) {
                next = next->next;
                if (next->item == item) return NULL;
            }

            HashMapNode<T> *newNode =  CREATE_NODE(item);
            next->next = newNode;
        }
    } else {
        entry->item = item;
        entry->taken = true;
    }

    this->count++;

    return &entry->item;
}

template <typename K, typename T>
T* HashMap<K, T>::get(K key) {
    int hsh = hash(key, this->capacity);
    HashMapNode<T> *entry = &(this->entries[hsh]);

    if(!entry->taken) { return NULL; };
    // for some item types operator== is overriden for this check
    if (entry->item == key) { return &entry->item; }

    HashMapNode<T> *next = entry->next;
    if (next == NULL) { return NULL; };

    while (next != NULL) {
        if (next->item == key) { return &next->item; }
        next = next->next;
    }

    return NULL;
}


#undef CREATE_NODE

///////////////////////////////////////////////////////////////////// HASHMAP ///////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////// QUEUE /////////////////////////////////////////////////////////

#define incRotate(inc, compare) inc = (inc < compare - 1) ? inc + 1 : 0
#define incCap(inc, compare) inc = (inc < compare) ? inc + 1 : inc

// TODO: memory cleanup function
// FIFO
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

///////////////////////////////////////////////////////////////////// QUEUE /////////////////////////////////////////////////////////

#define DATA_STRUCTURES
#endif