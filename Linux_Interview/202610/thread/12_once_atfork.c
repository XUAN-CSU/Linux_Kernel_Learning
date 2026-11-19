/*
 * 12_once_atfork.c — pthread_once 与 pthread_atfork
 *
 * pthread_once    — 保证初始化代码在多线程下「只执行一次」
 * pthread_atfork  — fork 前后挂钩子，避免子进程继承「已被锁死的锁」
 *
 *   gcc 12_once_atfork.c -o 12_once_atfork -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static pthread_once_t  once = PTHREAD_ONCE_INIT;
static pthread_mutex_t mtx  = PTHREAD_MUTEX_INITIALIZER;

static void init_once(void)
{
    printf("  [once] initializing (只执行一次，无论多少线程并发调用)\n");
}

/* ---- atfork 三个钩子 ---- */
static void atfork_prepare(void)   /* fork 之前，在【父进程】里运行 */
{
    pthread_mutex_lock(&mtx);
    printf("    atfork: prepare -> parent holds lock\n");
}
static void atfork_parent(void)    /* fork 返回【父进程】后运行 */
{
    pthread_mutex_unlock(&mtx);
    printf("    atfork: parent  -> released lock\n");
}
static void atfork_child(void)     /* fork 返回【子进程】后运行（子进程只剩本线程）*/
{
    pthread_mutex_unlock(&mtx);
    printf("    atfork: child   -> released lock\n");
}

static void *worker(void *arg)
{
    (void)arg;
    pthread_once(&once, init_once);    /* 4 个线程抢，但 init_once 只跑一次 */
    return NULL;
}

int main(void)
{
    pthread_t t[4];

    for (int i = 0; i < 4; i++) pthread_create(&t[i], NULL, worker, NULL);
    for (int i = 0; i < 4; i++) pthread_join(t[i], NULL);

    pthread_atfork(atfork_prepare, atfork_parent, atfork_child);

    printf("main: forking...\n");
    /* fork 会连 stdio 缓冲区一起复制；先 flush，否则子进程 flush 时
     * 会把父进程残留的缓冲输出「重放」一遍（非终端运行时会看到重复行）。*/
    fflush(stdout);
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* 子进程：若没有 atfork，mtx 可能停在「父进程某线程持锁」的状态 ->
         * 子进程里再 lock 就永久死锁。atfork 的 prepare/child 把锁复位。*/
        printf("child: pid=%d, mutex usable\n", getpid());
        fflush(stdout);        /* _exit 不刷新 stdio 缓冲，非终端运行时会丢输出 */
        _exit(0);
    }

    waitpid(pid, NULL, 0);
    printf("main: child reaped\n");
    return 0;
}
