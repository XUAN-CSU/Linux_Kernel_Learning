/*
 * 03_cond.c — 条件变量
 *
 * pthread_cond_init / destroy / wait / timedwait / signal / broadcast
 * 经典模式：生产者-消费者（有界缓冲区）
 *
 *   gcc 03_cond.c -o 03_cond -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define CAP    4
#define ITEMS  12
#define NCONS  2

static int buf[CAP];
static int head, tail, count;
static int done;

static pthread_mutex_t mtx       = PTHREAD_MUTEX_INITIALIZER;   /* 静态初始化 cond/mutex */
static pthread_cond_t  not_full  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;

static void *producer(void *arg)
{
    (void)arg;
    for (int i = 0; i < ITEMS; i++) {
        pthread_mutex_lock(&mtx);
        while (count == CAP)                       /* 必须 while，防虚假唤醒 */
            pthread_cond_wait(&not_full, &mtx);    /* 原子地：解锁 + 睡眠；唤醒时重新加锁 */
        buf[tail] = i;
        tail = (tail + 1) % CAP;
        count++;
        printf("  produce %2d (count=%d)\n", i, count);
        pthread_cond_signal(&not_empty);           /* 唤醒一个消费者（broadcast 唤醒全部）*/
        pthread_mutex_unlock(&mtx);
        usleep(100000);
    }

    /* 生产结束：置标志并唤醒所有消费者，让它们能退出 */
    pthread_mutex_lock(&mtx);
    done = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&mtx);
    return NULL;
}

static void *consumer(void *arg)
{
    long id = (long)arg;
    for (;;) {
        pthread_mutex_lock(&mtx);
        while (count == 0) {
            if (done) {                            /* 无数据且生产结束 -> 收工 */
                pthread_mutex_unlock(&mtx);
                return NULL;
            }
            pthread_cond_wait(&not_empty, &mtx);
        }
        int v = buf[head];
        head = (head + 1) % CAP;
        count--;
        printf("cons%ld <- %2d (count=%d)\n", id, v, count);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mtx);
        usleep(150000);
    }
}

/* ---------- 顺带演示 pthread_cond_timedwait ---------- */
static pthread_cond_t  tcond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t tmtx  = PTHREAD_MUTEX_INITIALIZER;

static void demo_timedwait(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 200 * 1000 * 1000;               /* +200ms */
    if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }

    pthread_mutex_lock(&tmtx);
    int rc = pthread_cond_timedwait(&tcond, &tmtx, &ts);   /* 没人 signal -> 超时 */
    pthread_mutex_unlock(&tmtx);
    printf("timedwait rc=%d (%s)\n", rc, rc == ETIMEDOUT ? "ETIMEDOUT" : "?");
}

int main(void)
{
    pthread_t p, c[NCONS];

    pthread_create(&p, NULL, producer, NULL);
    for (long i = 0; i < NCONS; i++)
        pthread_create(&c[i], NULL, consumer, (void *)i);

    pthread_join(p, NULL);
    for (int i = 0; i < NCONS; i++) pthread_join(c[i], NULL);
    printf("all done: count=%d head=%d tail=%d\n", count, head, tail);

    demo_timedwait();

    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&tcond);
    pthread_mutex_destroy(&tmtx);
    return 0;
}
