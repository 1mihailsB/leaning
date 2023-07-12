#if !defined REQUEST

#include <stdlib.h>

enum HttpRequestAssemblyStatus {
    New,
    Header,
    Body,
};

struct HttpRequest {
    int capacity;
    int sockFd;
    int size;
    char *data;
    HttpRequestAssemblyStatus status;

    inline static HttpRequest init(int, int);
    bool operator==(HttpRequest& r) { return sockFd == r.sockFd; }
    bool operator==(int& compSockFd) { return sockFd == compSockFd; }
};

inline HttpRequest HttpRequest::init(int capacity, int sockFd) {
    HttpRequest r = { capacity, sockFd};
    r.data = (char *) malloc(sizeof(char) * capacity);

    return r;
}

#define REQUEST
#endif