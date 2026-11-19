/*
 * 01_create_join.c — 线程生命周期管理
 *
 * pthread_create / pthread_exit / pthread_join
 * pthread_self / pthread_equal / pthread_detach
 *
 *   gcc 01_create_join.c -o 01_create_join -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void *worker(void *arg)
{
    long id = (long)arg;
    printf("  [worker %ld] tid=%lu\n", id, (unsigned long)pthread_self());
    /* return 与 pthread_exit 等价：都会走清理流程再退出。
     * 返回值会被 pthread_join 的第二个参数接收。 */
    return (void *)(id * 100);
}

static void *detached_worker(void *arg)
{
    (void)arg;
    printf("  [detached] running; resources auto-reclaimed on exit\n");
    sleep(1);
    return NULL;
}

int main(void)
{
    pthread_t t[3];
    void *ret = NULL;
    int rc;

    printf("main tid=%lu\n", (unsigned long)pthread_self());

    /* ---------- joinable 线程：必须 join，否则栈/TCB 不回收 ---------- */
    for (long i = 0; i < 3; i++) {
        /* 参数 2 = 属性(NULL 用默认)，参数 3 = 入口，参数 4 = 传参 */
        rc = pthread_create(&t[i], NULL, worker, (void *)i);
        if (rc != 0) {                       /* pthread 函数返回错误码，不设 errno */
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 3; i++) {
        rc = pthread_join(t[i], &ret);       /* 阻塞直到 t[i] 结束并回收其资源 */
        if (rc != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
        printf("joined thread %d, ret=%ld\n", i, (long)ret);
    }

    /* pthread_t 是内部类型，不能用 == 比较，必须用 pthread_equal */
    printf("pthread_equal(self, self) = %d\n",
           pthread_equal(pthread_self(), pthread_self()));

    /* ---------- detached 线程：退出即自动回收，不能 join ---------- */
    pthread_t d;
    rc = pthread_create(&d, NULL, detached_worker, NULL);
    if (rc != 0) { fprintf(stderr, "pthread_create: %s\n", strerror(rc)); exit(EXIT_FAILURE); }

    rc = pthread_detach(d);                  /* 也可创建时用 attr 设 PTHREAD_CREATE_DETACHED */
    if (rc != 0) { fprintf(stderr, "pthread_detach: %s\n", strerror(rc)); exit(EXIT_FAILURE); }
    /* pthread_join(d, NULL) 对 detached 线程会返回 EINVAL，别这么写 */

    sleep(2);                                /* 等 detached 线程跑完（main 退出会杀掉所有线程）*/
    printf("main exit\n");
    return 0;
}
