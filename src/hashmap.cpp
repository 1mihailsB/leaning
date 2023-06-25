#if !defined HASHMAP

#include <stdlib.h>

#define CREATE_NODE(itm) (HashMapNode<T> *) malloc(sizeof(HashMapNode<T>));\
        newNode->taken = true; \
        newNode->item = itm

#define RETURN_IF_TAKEN(itm, comp) if (itm == comp) {\
            return AddResult::ALREADY_PRESENT;\
        }

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

template <typename T>
struct HashMap {
    int capacity;
    int count;
    HashMapNode<T> *entries;

    AddResult add(T);

    static int hash(T, int);
    static HashMap<T> init(int);
};

template <typename T>
HashMap<T> HashMap<T>::init(int len) {
    HashMap hm = { len };
    hm.entries = (HashMapNode<T>*) malloc(sizeof(HashMapNode<T>) * hm.capacity);

    if (hm.entries == NULL) {
        printf("Can't allocate hashmap entries\n");
        exit(EXIT_FAILURE);
    }

    return hm;
}

template <typename T>
AddResult HashMap<T>::add(T item) {
    int hash = HashMap<T>::hash(item, this->capacity);
    HashMapNode<T> *entry = &(this->entries[hash]);

    if (entry->taken == true) {
        if (entry->next == NULL) {
            RETURN_IF_TAKEN(entry->item, item)

            HashMapNode<T> *newNode = CREATE_NODE(item);
            entry->next = newNode;
        } else {
            RETURN_IF_TAKEN(entry->next->item, item)
            HashMapNode<T> *next = entry->next;

            while (next->next != NULL) {
                next = next->next;
                RETURN_IF_TAKEN(next->item, item)
            }

            HashMapNode<T> *newNode =  CREATE_NODE(item);
            next->next = newNode;
        }
    } else {
        entry->item = item;
        entry->taken = true;
    }

    this->count++;

    return AddResult::OK;
}

// specialzation for int T
template<>
int HashMap<int>::hash(int val, int max) {
    return val % max;
}

#undef CREATE_NODE
#undef RETURN_IF_TAKEN

#define HASHMAP
#endif