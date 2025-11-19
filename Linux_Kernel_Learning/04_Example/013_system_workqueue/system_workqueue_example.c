#include <linux/module.h>
#include <linux/init.h> 
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deepseek");
MODULE_DESCRIPTION("USB insert/remove monitor (notifier example)");

// static DECLARE_WAIT_QUEUE_HEAD(my_wq);
static int sleep = 0;

struct work_data {
    struct work_struct my_work;
    wait_queue_head_t my_wq;
    int the_data;
    atomic_t completed;     /* Use atomic for syncchronization */
};

static void work_handler(struct work_struct *work) 
{
    pr_info("Work handler start\n");

    struct work_data *my_data = container_of(work, struct work_data, my_work);
    pr_info("Work queue module handler: %s, data is %d\n", __FUNCTION__, my_data->the_data);
    msleep(2000);

    /* Mark work as completed and wake up waiting process */
    atomic_set(&my_data->completed, 1);
    wake_up_interruptible(&my_data->my_wq);
    kfree(my_data);
    pr_info("Work handler end\n");
}

static int __init system_workqueue_example_init(void)
{
    struct work_data *my_data;
    my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
    if (!my_data) {
        pr_err("Failed to allocate memory\n");
        return -ENOMEM;
    }

    my_data->the_data = 829;
    atomic_set(&my_data->completed, 0);

    init_waitqueue_head(&my_data->my_wq);

    INIT_WORK(&my_data->my_work, work_handler);

    schedule_work(&my_data->my_work);

    pr_info("Module initialization completed\n");
    return 0;
}

static void __exit system_workqueue_example_exit(void)
{
    pr_info("Work queue module exit: %s %d\n", __FUNCTION__, __LINE__);
}

module_init(system_workqueue_example_init);
module_exit(system_workqueue_example_exit);
