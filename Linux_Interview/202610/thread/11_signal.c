/*
 * 11_signal.c — 线程与信号
 *
 * pthread_sigmask  — 设置「本线程」的信号屏蔽字
 * pthread_kill     — 向「指定线程」发送信号
 * sigwait          — 在「指定线程」同步等待信号（多线程下的推荐做法）
 *
 * 关键：信号是进程级资源，但投递有两条路径：
 *   - kill(pid, sig)        -> 进程级 pending，可被任一 sigwait 线程取走
 *   - pthread_kill(tid, sig)-> 线程级 pending，只有「该线程自己」的
 *                              sigwait/sigwaitinfo 能看到
 * 所以要让专职线程收信号，必须 kill() 进程，或 pthread_kill 到「专职线程本身」。
 *
 * 标准模式：
 *   1. 主线程在创建其他线程【之前】屏蔽关心的信号
 *   2. 其余线程继承屏蔽字 -> 都收不到
 *   3. 专门起一个线程 sigwait 同步处理（无需 async-signal-safe）
 *
 *   gcc 11_signal.c -o 11_signal -pthread -Wall
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static void *sig_thread(void *arg)
{
    sigset_t *set = arg;
    int sig;
    for (;;) {
        int rc = sigwait(set, &sig);        /* 同步等待，返回即已从队列取走 */
        if (rc != 0) { fprintf(stderr, "sigwait: %s\n", strerror(rc)); continue; }
        printf("  [sig-thread] caught signal %d (%s)\n", sig, strsignal(sig));
        if (sig == SIGINT || sig == SIGTERM)
            break;                          /* 退出信号 -> 结束线程 */
    }
    return NULL;
}

static void *normal_worker(void *arg)
{
    (void)arg;
    printf("  [worker] running; inherits blocked mask, won't get SIGUSR1\n");
    for (int i = 0; i < 3; i++) { sleep(1); printf("  [worker] tick %d\n", i); }
    return NULL;
}

int main(void)
{
    sigset_t set;
    pthread_t st, wt;

    /* 1) 创建其他线程【之前】屏蔽关心的信号 */
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGUSR1);
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return 1;
    }

    /* 2) 专职信号处理线程 */
    pthread_create(&st, NULL, sig_thread, &set);

    /* 3) 普通工作线程（继承屏蔽字）*/
    pthread_create(&wt, NULL, normal_worker, NULL);

    usleep(500000);

    /* pthread_kill 到「专职线程本身」-> 它自己的 sigwait 能收到 */
    printf("main: pthread_kill(sig-thread, SIGUSR1)\n");
    pthread_kill(st, SIGUSR1);

    usleep(500000);

    /* kill 进程级 -> 同样落到专职线程的 sigwait */
    printf("main: kill(getpid(), SIGTERM) to stop\n");
    kill(getpid(), SIGTERM);

    pthread_join(st, NULL);
    printf("main: sig-thread joined\n");
    pthread_join(wt, NULL);
    printf("main: done\n");
    return 0;
}
