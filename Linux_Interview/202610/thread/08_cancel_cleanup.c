/*
 * 08_cancel_cleanup.c — 线程取消与资源清理
 *
 * pthread_cancel / pthread_setcancelstate / pthread_setcanceltype
 * pthread_testcancel / pthread_cleanup_push / pthread_cleanup_pop
 *
 * 关键点：
 *   - 默认 deferred：取消只在「取消点」（read/write/sleep/cond_wait 等）生效
 *   - 取消点之外不会被打断，保证临界区一致性
 *   - 清理函数 push/pop 必须成对，且在同一作用域（它们是宏）
 *
 *   gcc 08_cancel_cleanup.c -o 08_cancel_cleanup -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void cleanup(void *arg)
{
    printf("  [cleanup] releasing: %s\n", (char *)arg);
    free(arg);
}

static void *deferred_worker(void *arg)
{
    (void)arg;

    char *res = strdup("heap-buffer");
    pthread_cleanup_push(cleanup, res);      /* 注册清理函数 */

    printf("  worker: enter loop (sleep = cancel point)\n");
    for (;;) {
        sleep(1);                            /* 取消点在 sleep 内生效 */
        printf("  worker: still alive\n");
    }

    pthread_cleanup_pop(1);                  /* 1 = 正常退出也执行清理 */
    return NULL;
}

static void *guarded_worker(void *arg)
{
    (void)arg;

    int old;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);   /* 屏蔽取消：进入临界区 */
    printf("  guarded: cancel DISABLED, doing critical work\n");
    sleep(2);
    pthread_setcancelstate(old, NULL);                      /* 恢复：挂起的取消此时仍不生效 */
    printf("  guarded: cancel re-ENABLED\n");

    pthread_testcancel();                    /* 手动插入取消点，挂起的取消在这里响应 */
    printf("  guarded: survived (not cancelled)\n");
    return NULL;
}

int main(void)
{
    pthread_t t, g;
    void *ret = NULL;

    /* 1) 默认 deferred 取消：清理函数会被调用 */
    pthread_create(&t, NULL, deferred_worker, NULL);
    sleep(2);
    pthread_cancel(t);
    pthread_join(t, &ret);
    printf("deferred joined, ret=%s\n",
           ret == PTHREAD_CANCELED ? "PTHREAD_CANCELED" : "(other)");

    /* 2) DISABLE 期间收到取消 -> 挂起，重新使能到 testcancel 才生效 */
    pthread_create(&g, NULL, guarded_worker, NULL);
    usleep(300000);                          /* 此刻它正处于 DISABLE */
    pthread_cancel(g);
    pthread_join(g, &ret);
    printf("guarded joined, ret=%s\n",
           ret == PTHREAD_CANCELED ? "PTHREAD_CANCELED" : "(other)");

    /* 说明：PTHREAD_CANCEL_ASYNCHRONOUS 可在任意指令处被取消，极易破坏
     * 数据一致性，除非没有可用的取消点，否则不要用。*/
    return 0;
}
