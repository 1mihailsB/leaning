#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include "queue.cpp"

#define THREADS 1000

static pthread_t threads[THREADS];
static pthread_mutex_t lock;
static int count = 0;

void *work(void *arg) {
    pthread_t *t = (pthread_t*) arg;
    pthread_t tnum = *t;

    int r = rand();
    int s = 1 + r % 10;
    // int s = 3;

    pthread_mutex_lock(&lock);
    int c = count;

    printf("Work function #%lu, thread #%lu, count: %d\n", tnum, threads[tnum], count);
    printf("Sleeping: %d * 100000 microsec\n", s);

    count = c + 1;
    pthread_mutex_unlock(&lock);

    usleep(s * 100000);
    return NULL;
}

int main() {
    Queue jq = Queue<float>::init(1000);
    for (int i = 0; i < 1000; i++) {
        jq.add(55.32);
    }


    for (int i = 0; jq.size > 0; i++) {
        float take = jq.take();
        printf("Taken job:%.2f\n", take);
    }

    printf("Size of queue: %lu\n", sizeof(jq));

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t *list = (pthread_t*)malloc(sizeof(pthread_t) * THREADS);

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Couldn't create mutex\n");
        return 1;
    }

    for (int i = 0; i < THREADS; i++) {
        list[i] = i;
        int num = pthread_create(&threads[i], &attr, work, (void*) (list + i));
        // printf("%d created thread %ld\n", i, threads[i]);
    }


    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Sleep %d\n", i);
    }

    printf("Count %d, \n", count);

    pthread_mutex_destroy(&lock);

    return 0;
}