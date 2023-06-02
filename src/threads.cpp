#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>

#define THREADS 10

static pthread_t threads[THREADS];

void *work(void *arg) {
    pthread_t *t = (pthread_t*) arg;
    pthread_t tnum = *t;

    // int r = rand();
    // int s = 1 + r % THREADS;
    int s = 3;

    printf("Work function #%ld, thread #%ld\n", tnum, threads[tnum]);
    printf("Sleeping: %d sec\n", s);
    sleep(s);

    return NULL;
}

int main() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t *list = (pthread_t*)malloc(sizeof(pthread_t) * THREADS);

    for (int i = 0; i < THREADS; i++) {
        list[i] = i;
        int num = pthread_create(&threads[i], &attr, work, (void*) (list + i));
        // printf("%d created thread %ld\n", i, threads[i]);
    }


    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}