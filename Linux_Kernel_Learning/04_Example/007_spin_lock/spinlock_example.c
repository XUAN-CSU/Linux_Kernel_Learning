#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Spinlock example kernel module");

static spinlock_t my_lock;
static struct task_struct *thread1;
static struct task_struct *thread2;
static int shared_counter = 0;

static int thread_fn(void *data)
{
    int id = (int)(long)data;
    int i;

    for (i = 0; i < 5; ++i) {
        spin_lock(&my_lock);

        pr_info("Thread %d acquired lock, counter = %d\n", id, shared_counter);
        shared_counter++;
        pr_info("Thread %d incremented counter to %d\n", id, shared_counter);

        spin_unlock(&my_lock);

        msleep(500);  // simulate some work
        if (kthread_should_stop())
            break;
    }
    pr_info("Thread %d exiting\n", id);
    return 0;
}

static int __init spinlock_example_init(void)
{
    pr_info("Spinlock example module loaded\n");

    spin_lock_init(&my_lock);

    thread1 = kthread_run(thread_fn, (void *)1, "spinlock_thread1");
    thread2 = kthread_run(thread_fn, (void *)2, "spinlock_thread2");

    return 0;
}

static void __exit spinlock_example_exit(void)
{
    if (thread1) {
        kthread_stop(thread1);
    }
    if (thread1) {
        kthread_stop(thread2);
    }

    pr_info("Spinlock example module unloaded\n");
}

module_init(spinlock_example_init);
module_exit(spinlock_example_exit);
