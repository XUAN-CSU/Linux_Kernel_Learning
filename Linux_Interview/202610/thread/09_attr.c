/*
 * 09_attr.c — 线程属性
 *
 * pthread_attr_init / destroy
 *   setdetachstate / getdetachstate
 *   setstacksize  / getstacksize
 *   setguardsize  / getguardsize
 *   setscope      / setinheritsched / setschedpolicy / setschedparam
 *   setstackaddr  / setstack
 *   setaffinity_np / getaffinity_np     (GNU)
 * pthread_getattr_np                    (GNU, 查已存在线程的真实属性)
 *
 *   gcc 09_attr.c -o 09_attr -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

static void *worker(void *arg)
{
    (void)arg;
    printf("  worker running (tid=%lu)\n", (unsigned long)pthread_self());
    return NULL;
}

int main(void)
{
    pthread_attr_t attr, g;
    pthread_t t;
    size_t sz;
    int ds;

    pthread_attr_init(&attr);

    /* 分离状态：DETACHED 线程退出即回收，不能 join */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_getdetachstate(&attr, &ds);
    printf("detachstate = %s\n",
           ds == PTHREAD_CREATE_DETACHED ? "DETACHED" : "JOINABLE");

    /* 栈大小：最小 PTHREAD_STACK_MIN，默认通常 8MiB */
    pthread_attr_setstacksize(&attr, 1 << 20);          /* 1 MiB */
    pthread_attr_getstacksize(&attr, &sz);
    printf("stacksize   = %zu bytes\n", sz);

    /* 保护页：栈末尾的 guard page，越界立即 SIGSEGV */
    pthread_attr_setguardsize(&attr, 4096);
    pthread_attr_getguardsize(&attr, &sz);
    printf("guardsize   = %zu bytes\n", sz);

    /* 竞争范围：SYSTEM(可被内核调度到别的 CPU) 是默认且推荐值 */
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    printf("scope       = PTHREAD_SCOPE_SYSTEM\n");

    /* 是否继承创建者调度属性；EXPLICIT 时以 attr 里的为准 */
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
    printf("inheritsched= EXPLICIT_SCHED, policy = SCHED_OTHER\n");

    /* CPU 亲和性：绑定到 CPU 0（GNU 扩展）*/
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    pthread_attr_setaffinity_np(&attr, sizeof(set), &set);

    if (pthread_create(&t, &attr, worker, NULL) != 0) { perror("pthread_create"); return 1; }
    pthread_attr_destroy(&attr);

    sleep(1);

    /* ---- pthread_getattr_np：查「已存在」线程的真实属性 ---- */
    pthread_attr_init(&g);
    if (pthread_getattr_np(pthread_self(), &g) == 0) {
        void *addr = NULL;
        size_t stack_size = 0, guard = 0;
        pthread_attr_getstack(&g, &addr, &stack_size);
        pthread_attr_getguardsize(&g, &guard);
        printf("main real stack: addr=%p size=%zu guard=%zu\n",
               addr, stack_size, guard);
        pthread_attr_destroy(&g);
    }
    return 0;
}
