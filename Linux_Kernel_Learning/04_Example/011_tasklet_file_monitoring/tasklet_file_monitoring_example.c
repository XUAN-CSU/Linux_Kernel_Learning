#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/path.h>
#include <linux/delay.h>     // for msleep()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("deepseek");
MODULE_DESCRIPTION("Fsnotify-like example with tasklet for directory monitoring");

#define WATCH_DIR "/tmp/watch"
#define MAX_EVENTS 10

struct fsnotify_trigger {
    struct tasklet_struct file_tasklet;
    char filename[256];
    char dirname[256];
    int event_count;
};

static struct fsnotify_trigger *fsnotify_data;

/* ------------------ Tasklet Handler ------------------ */
static void file_tasklet_handler(unsigned long data)
{
    struct fsnotify_trigger *ft = (struct fsnotify_trigger *)data;

    ft->event_count++;

    pr_info("FSNOTIFY_TASKLET[%d]: Processing file event\n", ft->event_count);
    pr_info("FSNOTIFY_TASKLET[%d]: File: %s\n", ft->event_count, ft->filename);
    pr_info("FSNOTIFY_TASKLET[%d]: Running on CPU %u\n",
            ft->event_count, smp_processor_id());
    pr_info("FSNOTIFY_TASKLET[%d]: in_atomic=%d, in_softirq=%d, in_irq=%d\n",
            ft->event_count, in_atomic(), in_softirq(), in_irq());

    /* Simulated lightweight work */
    int i;
    for (i = 0; i < 1000; ++i)
        barrier();

    pr_info("FSNOTIFY_TASKLET[%d]: File processing completed\n\n", ft->event_count);
}

/* ------------------ Fsnotify Callback (Simulated) ------------------ */
static int fsnotify_event_callback(struct fsnotify_group *group,
                                   struct inode *inode,
                                   u32 mask,
                                   const void *data,
                                   int data_type,
                                   const struct qstr *file_name,
                                   u32 cookie)
{
    struct fsnotify_trigger *ft = fsnotify_data;

    if (file_name && file_name->name) {
        pr_info("FSNOTIFY: Event: mask=0x%x, file=%s\n", mask, file_name->name);
        snprintf(ft->filename, sizeof(ft->filename), "%s/%s", ft->dirname, file_name->name);
        tasklet_schedule(&ft->file_tasklet);
    }

    return 0;
}

static const struct fsnotify_ops fsnotify_ops = {
    .handle_event = fsnotify_event_callback,
};

/* ------------------ Module Init ------------------ */
static int __init tasklet_file_monitoring_example_init(void)
{
    int ret;
    struct path path;

    pr_info("FSNOTIFY_TASKLET: Module loading...\n");

    ret = kern_path(WATCH_DIR, LOOKUP_DIRECTORY, &path);
    if (ret) {
        pr_err("FSNOTIFY_TASKLET: Directory %s not found\n", WATCH_DIR);
        pr_info("FSNOTIFY_TASKLET: Please create it: mkdir -p %s\n", WATCH_DIR);
        return -ENOENT;
    }

    fsnotify_data = kzalloc(sizeof(*fsnotify_data), GFP_KERNEL);
    if (!fsnotify_data) {
        path_put(&path);
        return -ENOMEM;
    }

    tasklet_init(&fsnotify_data->file_tasklet, file_tasklet_handler, (unsigned long)fsnotify_data);
    fsnotify_data->event_count = 0;
    strscpy(fsnotify_data->dirname, WATCH_DIR, sizeof(fsnotify_data->dirname));
    strscpy(fsnotify_data->filename, "unknown", sizeof(fsnotify_data->filename));

    pr_info("FSNOTIFY_TASKLET: Monitoring directory: %s\n", WATCH_DIR);

    /* In a real kernel watcher, you'd register a fsnotify_mark here.
     * For this simplified example, we simulate 3 events manually.
     */
    path_put(&path);

    int i;
    for (i = 0; i < 3; ++i) {
        snprintf(fsnotify_data->filename, sizeof(fsnotify_data->filename),
                 "%s/demo_file_%d.txt", WATCH_DIR, i);
        pr_info("FSNOTIFY: Simulating file event: %s\n", fsnotify_data->filename);
        tasklet_schedule(&fsnotify_data->file_tasklet);
        msleep(2000);
    }

    pr_info("FSNOTIFY_TASKLET: Module loaded successfully.\n");
    return 0;
}

/* ------------------ Module Exit ------------------ */
static void __exit tasklet_file_monitoring_example_exit(void)
{
    pr_info("FSNOTIFY_TASKLET: Module unloading...\n");

    tasklet_kill(&fsnotify_data->file_tasklet);
    pr_info("FSNOTIFY_TASKLET: Total events processed: %d\n", fsnotify_data->event_count);

    kfree(fsnotify_data);
    pr_info("FSNOTIFY_TASKLET: Module unloaded.\n");
}

module_init(tasklet_file_monitoring_example_init);
module_exit(tasklet_file_monitoring_example_exit);

