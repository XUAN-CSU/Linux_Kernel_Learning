/*
 * 05_barrier.c — 屏障
 *
 * pthread_barrier_init / destroy / wait   (+ pthread_barrierattr_*)
 *
 * 用途：分阶段并行计算，每阶段结束必须所有线程对齐后再进入下一阶段。
 *
 *   gcc 05_barrier.c -o 05_barrier -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 4
static pthread_barrier_t barrier;

static void *phase_worker(void *arg)
{
    long id = (long)arg;

    printf("thread %ld: phase-1 running\n", id);
    usleep((id + 1) * 100000);                   /* 模拟各线程耗时不同 */
    printf("thread %ld: reach barrier\n", id);

    int rc = pthread_barrier_wait(&barrier);      /* 全部到齐才一起放行 */

    /* 恰好有一个线程拿到 PTHREAD_BARRIER_SERIAL_THREAD，可让它做汇总工作 */
    if (rc == PTHREAD_BARRIER_SERIAL_THREAD)
        printf(">>> thread %ld is the SERIAL thread (do the global work)\n", id);

    printf("thread %ld: phase-2 start (all synced)\n", id);
    return NULL;
}

int main(void)
{
    pthread_t t[N];
    pthread_barrierattr_t attr;

    pthread_barrierattr_init(&attr);
    pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);  /* 默认，仅本进程 */
    if (pthread_barrier_init(&barrier, &attr, N) != 0) {             /* 计数 = N */
        perror("pthread_barrier_init");
        return 1;
    }
    pthread_barrierattr_destroy(&attr);

    for (long i = 0; i < N; i++) pthread_create(&t[i], NULL, phase_worker, (void *)i);
    for (int i = 0; i < N; i++)  pthread_join(t[i], NULL);

    pthread_barrier_destroy(&barrier);
    printf("main: all phases done\n");
    return 0;
}
