#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

static struct timer_list my_timer;

void my_timer_callback(struct timer_list *t)
{
    printk("%s called (%ld).\n", __FUNCTION__, jiffies);
}

static int __init my_init(void)
{
    int retval;
    printk("Timer module loaded\n");

    timer_setup(&my_timer, my_timer_callback, 0);

    printk("Setup timer to fire in 300ms (%ld)\n", jiffies);

    retval = mod_timer(&my_timer, jiffies + msecs_to_jiffies(300));

    if (retval)
        printk("Timer firing failed\n");
    return 0;
}

static void __exit my_exit(void)
{
    int retval;
    retval = del_timer(&my_timer);

    if (retval) 
        printk("The timer is still in use ...\n");
    pr_info("Timer module unloaded\n");
    printk(KERN_INFO, "Timer module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
