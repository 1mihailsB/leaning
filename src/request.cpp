#if !defined REQUEST

#include <stdlib.h>
#include <stdint.h>

enum HttpRequestAssemblyStatus {
    New,
    Header,
    Body,
};

struct HttpRequest {
    uint32_t capacity;
    uint32_t size;
    // int socket is linux specific
    int sockFd;
    // write r->capacity - r->size bytes into this buffer at location r->data + r->size.
    // call r->resize() before writing into this buffer.
    int8_t *data;
    HttpRequestAssemblyStatus status;

    inline static HttpRequest init(uint32_t, int);
    int8_t resize();
    bool operator==(HttpRequest& r) { return sockFd == r.sockFd; }
    bool operator==(int& compSockFd) { return sockFd == compSockFd; }
};

inline HttpRequest HttpRequest::init(uint32_t capacity, int sockFd) {
    HttpRequest r = { capacity, 0, sockFd };
    r.data = (int8_t *) malloc(sizeof(int8_t) * capacity);

    return r;
}

int8_t HttpRequest::resize() {
    if (capacity - size < 256U) {
        if (capacity < 1024U) {
            capacity = 2048U;
            data = (int8_t *) realloc(data, capacity);
        } else if (capacity < 16384U) {
            capacity = capacity + capacity;
            data = (int8_t *) realloc(data, capacity);
        } else {
            capacity = capacity + 8192U;
            data = (int8_t *) realloc(data, capacity );
        }

        return 1;
    }

    return 0;
}

#define REQUEST
#endif