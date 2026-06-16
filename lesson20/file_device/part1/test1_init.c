#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
    pr_info("My module loaded!\n");
    
    return 0;
}

void cleanup_module(void)
{
    pr_info("My module unloaded!\n");
}

MODULE_LICENSE("GPL");