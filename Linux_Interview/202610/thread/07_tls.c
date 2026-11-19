/*
 * 07_tls.c — 线程私有数据 (TSD / TLS)
 *
 * pthread_key_create / pthread_key_delete
 * pthread_setspecific / pthread_getspecific
 * 对比 GCC 的 __thread 关键字
 *
 *   gcc 07_tls.c -o 07_tls -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NTHREAD 3

static pthread_key_t key;
static __thread int tls_counter;      /* GCC 扩展：每线程一份，纯 POD 用它更轻更快 */

/* 析构函数：线程退出时，对这个 key 自动调用一次（main 退出不触发） */
static void free_buf(void *p)
{
    printf("  [dtor tid=%lu] free %p\n", (unsigned long)pthread_self(), p);
    free(p);
}

static void *worker(void *arg)
{
    long id = (long)arg;

    /* pthread_key：动态申请，可存任意类型、自动析构 */
    char *buf = malloc(64);
    snprintf(buf, 64, "thread-%ld private buffer", id);
    pthread_setspecific(key, buf);
    printf("thread %ld: key -> %p (%s)\n", id, (void *)buf,
           (char *)pthread_getspecific(key));

    /* __thread：每个线程初始为 0，各自独立累加 */
    for (int i = 0; i < 3; i++) tls_counter += (int)id + 1;
    printf("thread %ld: __thread tls_counter = %d\n", id, tls_counter);

    usleep(300000);                    /* 让三个线程并存，否则 malloc 会复用同一地址 */
    return NULL;                       /* 返回后触发 key 的析构函数 */
}

int main(void)
{
    pthread_t t[NTHREAD];

    /* 参数 2 = 析构函数，可为 NULL */
    if (pthread_key_create(&key, free_buf) != 0) { perror("pthread_key_create"); return 1; }

    for (long i = 0; i < NTHREAD; i++) pthread_create(&t[i], NULL, worker, (void *)i);
    for (int i = 0; i < NTHREAD; i++)  pthread_join(t[i], NULL);

    printf("main getspecific = %p (main 没 set，应为 NULL)\n",
           pthread_getspecific(key));

    pthread_key_delete(key);           /* 注意：传 key 本身（值），不是 &key；只删 key，不触发析构 */
    printf("key deleted; tls_counter in main = %d\n", tls_counter);
    return 0;
}
