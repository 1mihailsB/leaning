#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <pthread.h>

#include "hashmap.cpp"
#include "request.cpp"
#include "queue.cpp"

#define THREADS 5
static pthread_t threads[THREADS];

#define BACKLOG 1000000
#define MAX_CONN 100000
#define BUFS 4096

bool workfinished = false;
Queue sockFdQueue = Queue<int>::init(BACKLOG);
static pthread_cond_t queueCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t queueLock;

HashMap<int, HttpRequest> cons = HashMap<int, HttpRequest>::init(BACKLOG);
static pthread_cond_t consCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t consLock;
int epollfd;
struct epoll_event events[BACKLOG];

// thread routine
void *sock_read(void *arg)
{
    pthread_t t = pthread_self();
    int sockFd;
    HttpRequest *r;
    for(;;) {
        // wait until we can take any sockFd from queue
        pthread_mutex_lock(&queueLock);
        while(sockFdQueue.size == 0) {
            if (workfinished) {
                pthread_mutex_unlock(&queueLock);
                return NULL;
            }

            pthread_cond_wait(&queueCond, &queueLock);
        }

        sockFd = sockFdQueue.take();
        pthread_mutex_unlock(&queueLock);

        // we have some sockFd, get request object corresponding to sockFd
        pthread_mutex_lock(&consLock);
        r = cons.get(sockFd);

        if (r == NULL) {
            HttpRequest newReq = HttpRequest::init(512, sockFd);
            if ( (r = cons.add(sockFd, newReq)) == NULL ) {
                perror("Couldn't add new request to list, even though it wasn't there.\n");
                exit(EXIT_FAILURE);
            }
        }

        pthread_mutex_unlock(&consLock);


        // read data
        int initSize = r->size;
        int read = 0;
        while ((read = recv(sockFd, r->data + r->size, BUFS, 0)) > 0) {
            r->size += read;
            read = 0;
            //                                                                               r->size >-1< is probably wrong, but no errors so far
            printf("Read > 0; %.*s, socket: %d, curSize:%d, thread:%lu, ptr:%p\n", r->size, r->data, r->sockFd, r->size, t, r->data + r->size-1);
        }

        if (r->size >= BUFS) {r-
            printf("Cursize >= BUFS%d\n", r->size);
        }

        if (read == 0) {
            //
            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, r->sockFd, NULL) == -1) {
                perror("epoll_ctl EPOLL_CTL_DEL:");
            }

            close(r->sockFd);
            continue;
        }

        if (read == -1) {
                perror("recv: read == -1");
            if (errno == EWOULDBLOCK) {
                printf("[warning] recv(), [errno %s]\n", "EWOULDBLOCK");
            }

            // rearm EPOLLONESHOT
            struct epoll_event ev;
            ev.data.fd = r->sockFd;
            ev.events = EPOLLIN | EPOLLONESHOT;
            if (epoll_ctl(epollfd, EPOLL_CTL_MOD, r->sockFd, &ev) == -1) {
                perror("Rearm failed");
            }

            continue;
        }

        printf("received %d bytes from client %d: %s\n", r->size, r->sockFd, r->data);
        // memset(&buf[curFd], 0, BUFS);
    }
}

void start_epoll_loop(int listenSockfd)
{
    struct epoll_event ev;
    int conn_sock, nfds;

    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listenSockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenSockfd, &ev) == -1)
    {
        perror("epoll_ctl: listenSockfd");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        do {
            nfds = epoll_wait(epollfd, events, BACKLOG, -1);
            // TODO: remove loop in release build
            // EINTR happens when debugger stops at breakpoint and other cases
            // https://stackoverflow.com/questions/6870158/epoll-wait-fails-due-to-eintr-how-to-remedy-this
        } while (nfds < 0 && errno == EINTR);

        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n)
        {
            int curFd = events[n].data.fd;
            if (curFd == listenSockfd)
            {
                sockaddr con_address;
                socklen_t casize = sizeof con_address;
                conn_sock = accept(listenSockfd, &con_address, &casize);
                if (conn_sock == -1)
                {
                    perror("accept: conn_sock");
                    exit(EXIT_FAILURE);
                };

                int flags = fcntl(conn_sock, F_GETFL);
                if (flags == -1) {
                    perror("fcntl: conn_sock");
                    exit(EXIT_FAILURE);
                }

                fcntl(conn_sock, F_SETFL, flags | O_NONBLOCK);

                ev.data.fd = conn_sock;
                ev.events = EPOLLIN | EPOLLONESHOT;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                              &ev) == -1)
                {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }

                printf("conn_sock: %d\n", conn_sock);
            }
            else
            {
                pthread_mutex_lock(&queueLock);

                sockFdQueue.add(curFd);

                pthread_cond_signal(&queueCond);
                pthread_mutex_unlock(&queueLock);
            }
        }
    }
}

#define PORT "3490"
#define OS_BACKLOG 1000

int create_server_socket() {
    int listenSockfd;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((listenSockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(listenSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(listenSockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(listenSockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(listenSockfd, OS_BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    return listenSockfd;
}

void initThreadStuff()
{
    if (pthread_mutex_init(&queueLock, NULL) != 0) {
        printf("Couldn't create mutex\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&consLock, NULL) != 0) {
        printf("Couldn't create mutex\n");
        exit(EXIT_FAILURE);
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    for (int i = 0; i < THREADS; i++) {
        int num = pthread_create(&threads[i], &attr, sock_read, (void*) i);
    }
}

int main() {
    rlimit r;
    r.rlim_cur = 1048576;
    r.rlim_max = 1048576;
    int set = setrlimit(RLIMIT_NOFILE, &r);
    if(set == -1) {
        printf("setrlimit(RLIMIT_NOFILE, %lu) failed\n", r.rlim_cur);
        perror("err");
        exit(EXIT_FAILURE);
    }

    // HashMapNode<int> x = HashMapNode<int> { true };
    // HashMap hm = HashMap<int>::init(100);

    // hm.add(5);
    // hm.add(7);
    // hm.add(5);
    // hm.add(5);
    // hm.add(100005);
    // hm.add(100005);
    // hm.add(200005);
    // hm.add(300005);
    // hm.add(400005);

    // cons.add(5, HttpRequest::init(20, 5));
    // cons.add(1000005, HttpRequest::init(20, 1000005));
    // cons.add(2000005, HttpRequest::init(20, 2000005));
    // cons.add(3000005, HttpRequest::init(20, 3000005));
    // cons.add(1, HttpRequest::init(20, 1));
    // cons.add(1000001, HttpRequest::init(20, 1000001));
    // cons.add(5, HttpRequest::init(20, 5));
    // cons.add(5, HttpRequest::init(20, 5));

    // HttpRequest *req = cons.get(5);

    int serverSocket = create_server_socket();
    initThreadStuff();
    start_epoll_loop(serverSocket);

    return 0;
}