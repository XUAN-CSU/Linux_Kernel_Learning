/*
 * 10_sched_name.c — 调度参数与线程命名
 *
 * pthread_setschedparam / getschedparam
 * pthread_setschedprio
 * pthread_setname_np / getname_np      (GNU)
 * pthread_getcpuclockid
 * sched_yield
 *
 *   gcc 10_sched_name.c -o 10_sched_name -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <time.h>

static void *worker(void *arg)
{
    (void)arg;

    /* 命名线程：见 /proc/<pid>/task/<tid>/comm、ps -L 的 LWP 名称
     * 注意 POSIX 名称上限 16 字节（含结尾 '\0'，即最多 15 字符）*/
    pthread_setname_np(pthread_self(), "my-worker");

    char name[64] = {0};
    pthread_getname_np(pthread_self(), name, sizeof(name));
    printf("  worker tid=%lu name='%s'\n", (unsigned long)pthread_self(), name);

    /* 查询调度策略与优先级 */
    struct sched_param sp;
    int policy;
    pthread_getschedparam(pthread_self(), &policy, &sp);
    printf("  worker policy=%s prio=%d\n",
           policy == SCHED_FIFO ? "FIFO" :
           policy == SCHED_RR   ? "RR"   : "OTHER", sp.sched_priority);

    /* 查询该线程消耗的 CPU 时间（在 /proc 里就是 utime+stime 的精细表示）*/
    clockid_t cid;
    if (pthread_getcpuclockid(pthread_self(), &cid) == 0) {
        for (volatile long i = 0; i < 20000000; i++) ;      /* 烧点 CPU */
        struct timespec ts;
        clock_gettime(cid, &ts);
        printf("  worker cpu time = %ld.%09ld s\n", (long)ts.tv_sec, ts.tv_nsec);
    }
    return NULL;
}

int main(void)
{
    pthread_t t;
    struct sched_param sp;
    int policy, rc;

    pthread_create(&t, NULL, worker, NULL);
    pthread_join(t, NULL);

    /* 主线程当前的调度参数 */
    pthread_getschedparam(pthread_self(), &policy, &sp);
    printf("main policy=%d prio=%d\n", policy, sp.sched_priority);

    /* 普通线程用 SCHED_OTHER，sched_priority 必须为 0；
     * SCHED_FIFO/RR 需要 CAP_SYS_NICE（root）才能设非 0 优先级。*/
    sp.sched_priority = 0;
    rc = pthread_setschedparam(pthread_self(), SCHED_OTHER, &sp);
    printf("setschedparam rc=%d (%s)\n", rc,
           rc == 0 ? "OK" : "需要权限(CAP_SYS_NICE)");

    /* setschedprio：只改优先级，不改策略 */
    rc = pthread_setschedprio(pthread_self(), 0);
    printf("setschedprio  rc=%d\n", rc);

    /* sched_yield：主动让出 CPU，等价 pthread_yield（已废弃）*/
    sched_yield();
    printf("yielded\n");
    return 0;
}
