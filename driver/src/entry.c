#include <linux/module.h>
#include "config.h"
#include "driver.h"

static int md_init(void)
{
    int status = register_driver();
    if (status != 0)
        printk(KERN_ERR MOD_PREFIX "failed to register usb driver\n");
    else
        printk(KERN_INFO MOD_PREFIX "loaded\n");

    return status;
}

static void md_exit(void)
{
    deregister_driver();
    printk(KERN_INFO MOD_PREFIX "unloaded\n");
}

module_init(md_init);
module_exit(md_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");
MODULE_DESCRIPTION("Курсовая работа по операционным системам");
