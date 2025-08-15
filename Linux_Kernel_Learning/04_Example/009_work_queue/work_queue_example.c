#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h> /* 睡眠 */
#include <linux/wait.h> /* 等待列队 */
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/workqueue.h>

//static DECLARE_WAIT_QUEUE_HEAD(my_wq);

struct work_data {
    struct work_struct my_work;
    wait_queue_head_t my_wq;
    int the_data;
    int done;       /* flag to indicate completion */
};

static void work_handler(struct work_struct *work)
{
    struct work_data *my_data = container_of(work, struct work_data, my_work);
    printk("Work queue module handler: %s, data is %d\n", __FUNCTION__, my_data->the_data);
    msleep(2000);
    my_data->done = 1;
    wake_up_interruptible(&my_data->my_wq);
}

static int __init my_init(void)
{
    struct work_data * my_data;
    my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
    if (!my_data) 
        return -ENOMEM;

    my_data->the_data = 34;
    my_data->done = 0;

    INIT_WORK(&my_data->my_work, work_handler);
    init_waitqueue_head(&my_data->my_wq);

    printk("Queuing work ...\n");
    schedule_work(&my_data->my_work);

    printk("I'm goint to sleep ...\n");
    wait_event_interruptible(my_data->my_wq, my_data->done != 0);
    printk("I am Waked up...\n");

    kfree(my_data);
    return 0;
}

static void __exit my_exit(void)
{
    printk("Work queue module exit: %s %d\n", __FUNCTION__, __LINE__);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WLC");
MODULE_DESCRIPTION("Shared workqueue");
