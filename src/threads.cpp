#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include "ds.cpp"

#define THREADS 10
#define WORKSIZE 1000000

static pthread_t threads[THREADS];
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t lock;
static int count = 0;
static bool workfinished = false;

Queue jq = Queue<float>::init(WORKSIZE);

void *work(void *arg) {
    pthread_t *t = (pthread_t*) arg;
    pthread_t tnum = *t;

    for(;;) {
        pthread_mutex_lock(&lock);
        while(jq.size == 0) {
            if (workfinished) {
                printf("Thread %lu returning\n", tnum);
                pthread_mutex_unlock(&lock);
                return NULL;
            }

            printf("Thread %lu waiting for condition\n", tnum);
            pthread_cond_wait(&cond, &lock);
        }

        float item = jq.take();

        printf("Work function #%lu, thread #%lu, count: %d\n", tnum, threads[tnum], count);
        printf("Queue size: %d, value: %.2f\n", jq.size, item);

        count++;
        printf("Incremented count: %d\n", count);
        if (count >= WORKSIZE) {
            workfinished = true;
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void initQueue() {
    for (int i = 0; i < WORKSIZE; i++) {
        pthread_mutex_lock(&lock);

        jq.add(55.32);
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&lock);

        // sleep(1);
    }
}

int main() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t *list = (pthread_t*)malloc(sizeof(pthread_t) * THREADS);

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Couldn't create mutex\n");
        return 1;
    }

    initQueue();
    for (int i = 0; i < THREADS; i++) {
        list[i] = i;
        int num = pthread_create(&threads[i], &attr, work, (void*) (list + i));
    }

    // initQueue();

    for (int i = 0; i < THREADS; i++) {
        printf("thread join %d\n", i);
        pthread_join(threads[i], NULL);
    }

    printf("Count %d, \n", count);

    pthread_mutex_destroy(&lock);

    return 0;
}