/*
 * 04_rwlock.c — 读写锁
 *
 * pthread_rwlock_init / destroy
 *   rdlock / wrlock / tryrdlock / trywrlock / timedrdlock / timedwrlock / unlock
 *
 * 语义：读共享、写独占、写优先（避免写饥饿）
 *
 *   gcc 04_rwlock.c -o 04_rwlock -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define NREAD  4
#define NWRITE 2

static pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
static int shared_data;

static void *reader(void *arg)
{
    long id = (long)arg;
    for (int i = 0; i < 3; i++) {
        pthread_rwlock_rdlock(&rw);          /* 读锁：多个读者可同时持有 */
        printf("  R%ld read  = %d\n", id, shared_data);
        usleep(80000);
        pthread_rwlock_unlock(&rw);
        usleep(40000);
    }
    return NULL;
}

static void *writer(void *arg)
{
    long id = (long)arg;
    for (int i = 0; i < 3; i++) {
        pthread_rwlock_wrlock(&rw);          /* 写锁：独占，读者和写者都进不来 */
        shared_data++;
        printf("W%ld write = %d\n", id, shared_data);
        usleep(80000);
        pthread_rwlock_unlock(&rw);
        usleep(40000);
    }
    return NULL;
}

static void *hold_wrlock(void *arg)
{
    (void)arg;
    pthread_rwlock_wrlock(&rw);              /* 独占持有写锁一段时间 */
    printf("[holder] write lock held for 1.5s\n");
    usleep(1500000);
    pthread_rwlock_unlock(&rw);
    return NULL;
}

int main(void)
{
    pthread_t r[NREAD], w[NWRITE], h;
    int rc;

    for (long i = 0; i < NREAD; i++)  pthread_create(&r[i], NULL, reader, (void *)i);
    for (long i = 0; i < NWRITE; i++) pthread_create(&w[i], NULL, writer, (void *)i);
    for (int i = 0; i < NREAD; i++)   pthread_join(r[i], NULL);
    for (int i = 0; i < NWRITE; i++)  pthread_join(w[i], NULL);

    /* ---------- try / timed 变体：另一个线程占着写锁，主线程拿不到 ---------- */
    pthread_create(&h, NULL, hold_wrlock, NULL);
    usleep(200000);                          /* 等 holder 拿到写锁 */

    rc = pthread_rwlock_tryrdlock(&rw);      /* 非阻塞，立即 EBUSY */
    printf("tryrdlock    rc=%d (%s)\n", rc, rc == EBUSY ? "EBUSY" : "?");
    rc = pthread_rwlock_trywrlock(&rw);
    printf("trywrlock    rc=%d (%s)\n", rc, rc == EBUSY ? "EBUSY" : "?");

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;                          /* 1 秒超时 */
    rc = pthread_rwlock_timedrdlock(&rw, &ts);
    printf("timedrdlock  rc=%d (%s)\n", rc, rc == ETIMEDOUT ? "ETIMEDOUT" : "?");

    pthread_join(h, NULL);
    pthread_rwlock_destroy(&rw);
    return 0;
}
