#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Example of hrtimer usage");
MODULE_VERSION("1.0");

static struct hrtimer my_hrtimer;
static ktime_t kt_period;

/* Timer callback function */
enum hrtimer_restart my_timer_callback(struct hrtimer *timer)
{
    pr_info("hrtimer callback called at jiffies: %lu\n", jiffies);

    // To restart the timer periodically, uncomment the line below:
    // hrtimer_forward_now(timer, kt_period);
    // return HRTIMER_RESTART;

    return HRTIMER_NORESTART;
}

static int __init my_module_init(void)
{
    pr_info("hrtimer module loading\n");

    // Set timer period (1 second)
    kt_period = ktime_set(1, 0); // 1 second, 0 nanoseconds

    // Initialize hrtimer
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = &my_timer_callback;

    // Start the timer
    hrtimer_start(&my_hrtimer, kt_period, HRTIMER_MODE_REL);

    pr_info("hrtimer started\n");
    return 0;
}

static void __exit my_module_exit(void)
{
    int ret;

    // Cancel the timer
    ret = hrtimer_cancel(&my_hrtimer);
    if (ret)
        pr_info("Timer was active and canceled\n");
    else
        pr_info("Timer was not active\n");

    pr_info("hrtimer module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
