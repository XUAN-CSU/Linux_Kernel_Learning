#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("deepseek");
MODULE_DESCRIPTION("Timer interrupt with tasklet example");
MODULE_VERSION("1.0");

// Module parameters
static unsigned long timer_interval = 1000;     // milliseconds
module_param(timer_interval, ulong, 0644);
MODULE_PARM_DESC(timer_interval, "Timer interval in milliseconds (default: 1000)");

// Global variables
static struct tasklet_struct my_tasklet;
static struct timer_list my_timer;
static unsigned long tasklet_count = 0;
static unsigned long timer_count = 0;

void tasklet_handler(unsigned long data)
{
    unsigned long flags;
    tasklet_count++;

    // Get current time for display
    struct timespec64 now;;
    ktime_get_real_ts64(&now);

    printk(KERN_INFO "TASKLET[%lu]: Executing at %ll.%09ld seconds\n", tasklet_count, now.tv_sec, now.tv_nsec);
    printk(KERN_INFO "TASKLET[%lu]: Running on CPU %d\n", tasklet_count, smp_processor_id());

    // Show context information
    printk(KERN_INFO "TASKLET[%lu]: in_atomic=%d, in_softirq=%d, in_irq=%d\n", tasklet_count, in_atomic(), in_softirq(), in_irq());

    // Simulate some quick work (atomic_safe only!)
    int i;
    for (int i = 0; i < 5; i++) {
        // Use memory barries for compiler optimization prevention
        barrier();
    }

    printk(KERN_INFO "TASKLET[%lu]: Work completed\n\n", tasklet_count);
}

void timer_callback(struct timer_list *t)
{
    unsigned long flags;
    timer_count++;

    //  Get current time
    struct timespec64 now;
    ktime_get_real_ts64(&now);

    printk(KERN_INFO "TIMER[%lu]: Fired at %ll.%09ld seconds\n", timer_count, now.tv_sec, now.tv_nsec);
    printk(KERN_INFO "TIMER[%lu]: Running on CPU %d\n", timer_count, smp_processor_id());

    // Show we're in interrupt context
    printk(KERN_INFO "TIMER[%lu]: in_atomic=%d, in_softirq=%d, in_irq=%d\n“, in_softirq=%d, in_irq=%d\n", timer_count, in_atomic(), in_softirq(), in_irq());


    // Schedule the tasklet for deferred work
    printk(KERN_INFO "TIMER[%lu]: Scheduling tasklet...\n", timer_count);
    tasklet_schedule(&my_tasklet);

    // Reschedule the timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));

    printk(KERN_INFO "TIMER[%lu]: Rescheduled for %lu ms later\n\n", timer_count, timer_interval);
}

// Module initialization function
static int __init tasklet_timer_interrupt_example_init(void)
{
    int ret = 0;

    printk(KERN_INFO "============================================");
    printk(KERN_INFO "TIMER_TASKLET: Module loading...\n");
    printk(KERN_INFO "TIMER_TASKLET: Timer interval：%lu ms\n", timer_interval);

    // Initialize the tasklet
    tasklet_init(&my_tasklet, tasklet_handler, 0);
    printk(KERN_INFO "TIMER_TASKLET: Tasklet initialized\n");

    // Initialize and start the timer 
    timer_setup(&my_timer, timer_callback, 0);
    ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));

    if (ret) {
        printk(KERN_INFO "TIMER_TASKLET: Failed to start timer\n");
        return ret;
    }

    printk(KERN_INFO "TIMER_TASKLET: Timer started successfully\n");
    printk(KERN_INFO "TIMER_TASKLET: Module loaded successfully\n");
    printk(KERN_INFO "============================================");

    return 0;
}

// Module cleanup function
static void __exit tasklet_timer_interrupt_example_exit(void)
{
    printk(KERN_INFO "============================================");
    printk(KERN_INFO "TIMER_TASKLET: Module unloading...\n");

    // Delete the timer (synchronously)
    del_timer_sync(&my_timer);
    printk(KERN_INFO "TIMER_TASKLET: Timer deleted\n");

    // Kill the tasklet (wait fo completion if running
    tasklet_kill(&my_tasklet);
    printk(KERN_INFO "TIMER_TASKLET: Tasklet killed\n");

    // Print statistics
    printk(KERN_INFO "TIMER_TASKLET: Statistics:\n");
    printk(KERN_INFO "TIMER_TASKLET:    Timer callback: %lu\n", timer_count);
    printk(KERN_INFO "TIMER_TASKLET:    Tasklet execution: %lu\n", tasklet_count);

    printk(KERN_INFO "TIMER_TASKLET: Module unloaded successfully\n");
    printk(KERN_INFO "============================================");
}

module_init(tasklet_timer_interrupt_example_init);
module_exit(tasklet_timer_interrupt_example_exit);
