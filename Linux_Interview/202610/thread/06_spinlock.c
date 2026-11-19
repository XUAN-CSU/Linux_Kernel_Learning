/*
 * 06_spinlock.c — 自旋锁
 *
 * pthread_spin_init / destroy / lock / trylock / unlock
 *
 * 适用：临界区极短（几十条指令）且不希望线程睡眠/上下文切换的场景。
 * 注意：拿不到锁时是「忙等」烧 CPU；临界区一长，性能反而比 mutex 差。
 *
 *   gcc 06_spinlock.c -o 06_spinlock -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define NTHREAD 4
#define LOOPS   2000000

static pthread_spinlock_t spin;
static long counter;

static void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < LOOPS; i++) {
        pthread_spin_lock(&spin);        /* 拿不到就自旋（busy-wait），不睡眠 */
        counter++;                       /* 极短临界区 */
        pthread_spin_unlock(&spin);
    }
    return NULL;
}

int main(void)
{
    pthread_t t[NTHREAD];
    int rc;

    /* 参数 2: PTHREAD_PROCESS_PRIVATE(线程间) / PTHREAD_PROCESS_SHARED(跨进程) */
    if (pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE) != 0) {
        perror("pthread_spin_init");
        return 1;
    }

    for (int i = 0; i < NTHREAD; i++) pthread_create(&t[i], NULL, worker, NULL);
    for (int i = 0; i < NTHREAD; i++) pthread_join(t[i], NULL);
    printf("counter = %ld (expect %d)\n", counter, NTHREAD * LOOPS);

    /* trylock：拿不到立即返回 EBUSY，不自旋 */
    pthread_spin_lock(&spin);
    rc = pthread_spin_trylock(&spin);
    printf("trylock rc=%d (%s)\n", rc, rc == EBUSY ? "EBUSY (未自旋)" : "?");
    pthread_spin_unlock(&spin);

    pthread_spin_destroy(&spin);
    return 0;
}
