#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shirley");
MODULE_DESCRIPTION("Mutex Demo for Linux Kernel");
MODULE_VERSION("1.0");

/* shared resource protected by mutex */
struct shared_data {
    struct mutex lock;
    int counter;
    char *buffer;
};

static struct shared_data *data;
static struct task_struct *thread1;
static struct task_struct *thread2;
static int stop_threads = 0;

static int worker_thread(void *arg)
{
    int thread_num = *(int *)arg;
    int i;
    
    while (!stop_threads) {
        mutex_lock(&data->lock);
        data->counter++;
        snprintf(data->buffer, 128, "Thread %d updated counter to %d", thread_num, data->counter);
        pr_info("[Thread %d] %s\n", thread_num, data->buffer);

        /* Simulate some work */
        msleep(100);
        
        mutex_unlock(&data->lock);

        /* Sleep outside critical section */
        msleep(100);
    }

    return 0;
}


/* Trylock example function */
static void trylock_demo(void)
{
    pr_info("=== Mutex Trylock Demo ===\n");
    
    if (mutex_trylock(&data->lock)) {
        pr_info("Successfully acquired mutex with trylock\n");
        /* Do something */
        mutex_unlock(&data->lock);
    } else {
        pr_info("Failed to acquire mutex with trylock - already locked\n");
    }
}

static int __init mutex_demo_init(void)
{
    int ret;
    int *thread1_arg;
    int *thread2_arg;

    pr_info("Mutex Demo Module loading...\n");

    data = kmalloc(sizeof(struct shared_data), GFP_KERNEL);
    if (!data) {
        pr_err("Failed to allocate shared data\n");
        return  -ENOMEM;
    }

    mutex_init(&data->lock);

    data->counter = 0;
    data->buffer = kmalloc(128, GFP_KERNEL);
    if (!data->buffer) {
        pr_err("Failed to allocate shared data\n");
        kfree(data);
        return  -ENOMEM;
    }

    memset(data->buffer, 0, 128);

    pr_info("Testing basic mutex operation...\n");

    mutex_lock(&data->lock);
    pr_info("Mutex acquired\n");

    if (mutex_is_locked(&data->lock)) {
        pr_info("Mutex is locked (by us) \n");
    }

    mutex_unlock(&data->lock);
    pr_info("Mutex released\n");

    trylock_demo();

    thread1_arg = kmalloc(sizeof(int), GFP_KERNEL);
    thread2_arg = kmalloc(sizeof(int), GFP_KERNEL);
    
    if (!thread1_arg || !thread2_arg) {
        pr_err("Failed to allocate thread arguments\n");
        kfree(data->buffer);
        kfree(data);
        kfree(thread1_arg);
        kfree(thread2_arg);
        return -ENOMEM;
    }

    *thread1_arg = 1;
    *thread2_arg = 2;

    thread1 = kthread_run(worker_thread, thread1_arg, "mutex_worker_1");
    thread2 = kthread_run(worker_thread, thread2_arg, "mutex_worker_1");

    if (IS_ERR(thread1) || IS_ERR(thread2)) {
        pr_err("Failed to create threads\n");
        kfree(data->buffer);
        kfree(data);
        kfree(thread1_arg);
        kfree(thread2_arg);
        return -EINVAL;
    }

    pr_info("Mutex demo threads started successfully\n");
    pr_info("Check 'dmesg' to see output\n");

    return 0;
}

/* Module cleanup */
static void __exit mutex_demo_exit(void)
{
    pr_info("Mutex Demo Module Unloading...\n");

    /* Stop threads */
    stop_threads = 1;

    /* Wait for threads to finish */
    if (thread1)
        kthread_stop(thread1);
    if (thread2)
        kthread_stop(thread2);

    /* Final counter value */
    mutex_lock(&data->lock);
    pr_info("Final counter value: %d\n", data->counter);
    pr_info("Final buffer content: %s\n", data->buffer);
    mutex_unlock(&data->lock);

    /* Cleanup */
    kfree(data->buffer);
    mutex_destroy(&data->lock);
    kfree(data);

    pr_info("Mutex demo module unloaded\n");
}

module_init(mutex_demo_init);
module_exit(mutex_demo_exit);

