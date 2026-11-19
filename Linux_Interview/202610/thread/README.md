# pthread 接口代码示例集

每个 `.c` 文件都是**独立可编译可运行**的最小示例，覆盖一类 pthread 接口。

```bash
make            # 编译全部
make run        # 编译并依次运行全部
make 03_cond    # 只编译某一个
./03_cond       # 运行
# 或手动: gcc 03_cond.c -o 03_cond -pthread -Wall
```

> 约定：错误码用 `strerror(rc)` 打印，因为 pthread 函数**不设 errno，直接返回错误码**。

## 文件索引

| 文件 | 覆盖接口 |
|---|---|
| `01_create_join.c` | `pthread_create` `pthread_exit` `pthread_join` `pthread_self` `pthread_equal` `pthread_detach` |
| `02_mutex.c` | `pthread_mutex_init/destroy/lock/trylock/timedlock/unlock` + `mutexattr`(type / pshared / robust) |
| `03_cond.c` | `pthread_cond_init/destroy/wait/timedwait/signal/broadcast`（生产者-消费者）|
| `04_rwlock.c` | `pthread_rwlock_init/destroy/rdlock/wrlock/tryrdlock/trywrlock/timedrdlock/timedwrlock/unlock` |
| `05_barrier.c` | `pthread_barrier_init/destroy/wait` |
| `06_spinlock.c` | `pthread_spin_init/destroy/lock/trylock/unlock` |
| `07_tls.c` | `pthread_key_create/delete` `pthread_setspecific/getspecific` + `__thread` 对比 |
| `08_cancel_cleanup.c` | `pthread_cancel` `setcancelstate` `setcanceltype` `testcancel` `cleanup_push/pop` |
| `09_attr.c` | `pthread_attr_*`（detachstate / stacksize / guardsize / scope / inheritsched / schedpolicy / affinity_np）+ `pthread_getattr_np` |
| `10_sched_name.c` | `pthread_setschedparam/getschedparam` `setschedprio` `setname_np/getname_np` `getcpuclockid` |
| `11_signal.c` | `pthread_sigmask` `pthread_kill` `sigwait`（信号专职线程模式）|
| `12_once_atfork.c` | `pthread_once` `pthread_atfork` |

## 接口全景速查

- **线程管理** create exit join detach self equal
- **取消与清理** cancel setcancelstate setcanceltype testcancel cleanup_push cleanup_pop
- **互斥量** mutex_(init destroy lock trylock timedlock unlock) + mutexattr_(init destroy settype gettype setpshared setrobust …)
- **条件变量** cond_(init destroy wait timedwait signal broadcast) + condattr_*
- **读写锁** rwlock_(init destroy rdlock wrlock tryrdlock trywrlock timedrdlock timedwrlock unlock) + rwlockattr_*
- **屏障** barrier_(init destroy wait) + barrierattr_*
- **自旋锁** spin_(init destroy lock trylock unlock)
- **线程私有数据** key_(create delete) setspecific getspecific
- **属性** attr_(init destroy setdetachstate getdetachstate setstacksize getstacksize setstackaddr setstack setguardsize setscope setinheritsched setschedpolicy setschedparam setaffinity_np getaffinity_np …) + getattr_np
- **调度/命名** setschedparam getschedparam setschedprio setname_np getname_np getcpuclockid yield
- **信号** sigmask kill sigwait
- **一次性/派生** once atfork

## 面试高频陷阱

1. `pthread_t` 不可直接用 `==` 比较 → 用 `pthread_equal`。
2. `pthread_create` 系列**返回错误码而不设置 errno**。
3. `pthread_cond_wait` 必须在持锁时调用，且**条件判断用 `while` 不用 `if`**（防虚假唤醒）。
4. `pthread_join` / `pthread_detach` 二选一；detached 线程 join 返回 `EINVAL`。
5. `pthread_cancel` 默认是 **deferred**：只在**取消点**生效（`read`/`write`/`sleep`/`cond_wait` 等）。
6. 线程退出不回收栈，join 或 detach 才回收 → 否则内存泄漏。
7. `fork` 后子进程只保留调用线程，可能持有已死的锁 → 用 `pthread_atfork` 处理。
8. 信号是**进程级**资源，`pthread_kill` 发给指定线程，但进程级信号可能被任一未屏蔽线程处理。
