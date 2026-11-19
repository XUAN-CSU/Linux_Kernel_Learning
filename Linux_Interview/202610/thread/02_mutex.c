/*
 * 02_mutex.c — 互斥量
 *
 * pthread_mutex_init / destroy / lock / trylock / timedlock / unlock
 * pthread_mutexattr_init / destroy / settype / setpshared / setrobust
 * 静态初始化宏 PTHREAD_MUTEX_INITIALIZER
 *
 *   gcc 02_mutex.c -o 02_mutex -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define NTHREAD 4
#define LOOPS   1000000

static long counter_unsafe = 0;
static long counter_safe   = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;   /* 静态初始化 */

static void *unsafe_add(void *arg)
{
    (void)arg;
    for (int i = 0; i < LOOPS; i++)
        counter_unsafe++;                /* 非原子：读-改-写会被打断 */
    return NULL;
}

static void *safe_add(void *arg)
{
    (void)arg;
    for (int i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&mtx);
        counter_safe++;                  /* 临界区：同一时刻只有一个线程执行 */
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(void)
{
    pthread_t t[NTHREAD];
    int rc;

    /* 1) 不加锁 —— 结果几乎一定小于 NTHREAD*LOOPS，演示数据竞争 */
    for (int i = 0; i < NTHREAD; i++) pthread_create(&t[i], NULL, unsafe_add, NULL);
    for (int i = 0; i < NTHREAD; i++) pthread_join(t[i], NULL);
    printf("unsafe counter = %ld (expect %d)  <-- 丢更新\n",
           counter_unsafe, NTHREAD * LOOPS);

    /* 2) 加锁 —— 结果精确 */
    for (int i = 0; i < NTHREAD; i++) pthread_create(&t[i], NULL, safe_add, NULL);
    for (int i = 0; i < NTHREAD; i++) pthread_join(t[i], NULL);
    printf("safe   counter = %ld (expect %d)\n", counter_safe, NTHREAD * LOOPS);

    /* ---------- 动态初始化 + 属性 ---------- */
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    /* 递归锁：同一线程可重复加锁，但解锁次数必须匹配 */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_t rec;
    pthread_mutex_init(&rec, &attr);
    pthread_mutex_lock(&rec);
    rc = pthread_mutex_lock(&rec);       /* 递归锁：同线程可再次加锁，返回 0 */
    printf("recursive: 2nd lock rc=%d (%s)\n", rc, rc == 0 ? "OK" : "?");
    pthread_mutex_unlock(&rec);
    pthread_mutex_unlock(&rec);
    pthread_mutex_destroy(&rec);

    /* 错误检查锁：同线程重复加锁返回 EDEADLK 而不是死锁 */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_t err;
    pthread_mutex_init(&err, &attr);
    pthread_mutex_lock(&err);
    rc = pthread_mutex_lock(&err);
    printf("errorcheck: re-lock rc=%d (%s)\n", rc,
           rc == EDEADLK ? "EDEADLK" : "?");
    pthread_mutex_unlock(&err);
    pthread_mutex_destroy(&err);

    /* 进程共享锁：放在共享内存里可跨进程使用（CLONE 语义对齐 Q1） */
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    printf("pshared set to PTHREAD_PROCESS_SHARED\n");

    pthread_mutexattr_destroy(&attr);

    /* ---------- trylock / timedlock ---------- */
    pthread_mutex_t busy;
    pthread_mutex_init(&busy, NULL);
    pthread_mutex_lock(&busy);                    /* 先占住，不释放 */

    rc = pthread_mutex_trylock(&busy);            /* 非阻塞 */
    printf("trylock   rc=%d (%s)\n", rc, rc == EBUSY ? "EBUSY" : "?");

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;                               /* 1 秒超时 */
    rc = pthread_mutex_timedlock(&busy, &ts);
    printf("timedlock rc=%d (%s)\n", rc, rc == ETIMEDOUT ? "ETIMEDOUT" : "?");

    pthread_mutex_unlock(&busy);
    pthread_mutex_destroy(&busy);
    pthread_mutex_destroy(&mtx);
    return 0;
}
