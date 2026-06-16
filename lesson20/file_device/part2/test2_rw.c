#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mutex.h>  
#include <linux/uaccess.h>    

static int major = 0;
static DEFINE_MUTEX(lock);    // замена rwlock_t на mutex
static char test_string[15] = "Hello!\n\0";

ssize_t test_read(struct file *fd, char __user *buff, size_t size, loff_t *off)
{
    size_t rc;

    if (mutex_lock_interruptible(&lock))
        return -ERESTARTSYS;

    rc = simple_read_from_buffer(buff, size, off, test_string, 15);

    mutex_unlock(&lock);
    return rc;
}

ssize_t test_write(struct file *fd, const char __user *buff, size_t size, loff_t *off)
{
    size_t rc = 0;

    if (size > 15)
        return -EINVAL;

    if (mutex_lock_interruptible(&lock))
        return -ERESTARTSYS;

    rc = simple_write_to_buffer(test_string, 15, off, buff, size);

    mutex_unlock(&lock);
    return rc;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = test_read,
    .write = test_write,
};

int init_module(void)
{
    pr_info("My module 2 loaded!\n");

    major = register_chrdev(0, "test2_rw", &fops);
    if (major < 0)
        return major;

    pr_info("Major number is %d\n", major);
    return 0;
}

void cleanup_module(void)
{
    unregister_chrdev(major, "test2_rw");
    pr_info("My module 2 unloaded!\n");
}

MODULE_LICENSE("GPL");