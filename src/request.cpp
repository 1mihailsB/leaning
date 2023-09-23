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
    // int type socket is linux specific
    int sockFd;
    uint32_t size;
    uint32_t processed_size;
    // write r->capacity - r->size bytes into this buffer at location r->data + r->size.
    // call r->resize() before writing into this buffer.
    char *data;
    HttpRequestAssemblyStatus status;

    inline static HttpRequest init(uint32_t capacity, int sockFd);
    int8_t resize();
    bool operator==(HttpRequest& r) { return sockFd == r.sockFd; }
    bool operator==(int& compSockFd) { return sockFd == compSockFd; }
};

inline HttpRequest HttpRequest::init(uint32_t capacity, int sockFd) {
    HttpRequest r = { capacity, sockFd };
    r.data = (char *) malloc(sizeof(char) * capacity);

    return r;
}

int8_t HttpRequest::resize() {
    if (capacity - size < 256U) {
        if (capacity < 1024U) {
            capacity = 2048U;
            data = (char *) realloc(data, capacity);
        } else if (capacity < 16384U) {
            capacity = capacity + capacity;
            data = (char *) realloc(data, capacity);
        } else {
            capacity = capacity + 8192U;
            data = (char *) realloc(data, capacity );
        }

        return 1;
    }

    return 0;
}

#define REQUEST
#endif