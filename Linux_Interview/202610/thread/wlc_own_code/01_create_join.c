#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static void *worker(void *arg)
{
    long id = (long)arg;
    printf("this is thread[%ld], hello\n", id);
    sleep(2);
    return (void *)(id *10);
}

#define ID_COUNT 20

int main()
{
    pthread_t t[ID_COUNT];
    void *ret = NULL;
    int rc;

    for (long i = 0; i != ID_COUNT; ++i) {
        rc = pthread_create(&t[i], NULL, worker, (void *)(i));
        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    }

    for (long i = 0; i != ID_COUNT; ++i) {
        rc = pthread_join(t[i], &ret);
        if (rc != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    printf("join thread %ld, ret=%ld\n", i, (long)ret);
    }

    return 0;
}



