#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("A simple tasklet example");

static struct tasklet_struct my_tasklet;

void my_tasklet_handler(unsigned long data)
{

    printk(KERN_INFO, "Tasklet is running. jiffies = %lu, data = %lu\n", jiffies, data);
}

static int __init tasklet_example_init(void)
{
    unsigned long data = 1234;

    pr_info("Loading tasklet module\n");

    // Initialize the tasklet 
    tasklet_init(&my_tasklet, my_tasklet_handler, data);

    // Schedule the tasklet for execution
    tasklet_schedule(&my_tasklet);

    return 0;
}

static void __exit tasklet_example_exit(void)
{
    pr_info("Unloading tasklet module\n");

    tasklet_kill(&my_tasklet);
}

module_init(tasklet_example_init);
module_exit(tasklet_example_exit);
