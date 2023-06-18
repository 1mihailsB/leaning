#if !defined HASHMAP

#include <stdlib.h>

template <typename T>
struct Node {
    T item;
    Node<T> *next;
};

template <typename T>
struct HashMap {
    int bucketsLen;
    Node<T> *buckets;

    static int hash(T);

    static HashMap<T> init(int);
};

template <typename T>
HashMap<T> HashMap<T>::init(int len) {
    HashMap hm = { len };
    hm.buckets = (Node<T>*) malloc(sizeof(Node<T>) * hm.bucketsLen);

    if (hm.buckets == NULL) {
        printf("Can't allocate hashmap buckets\n");
        exit(EXIT_FAILURE);
    }

    return hm;
}

// specialzation for int T
template<>
int HashMap<int>::hash(int) {
    printf("integer hash\n");
    exit(EXIT_FAILURE);
    return 0;
}

#define HASHMAP
#endif