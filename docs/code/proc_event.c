static struct proc_ops proc_ops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
};

struct proc_dir_entry *create_joystick_event_entry
    (struct usb_joystick_kbd *usb_joystick_kbd)
{
    return proc_create_data("joystick_kbd", S_IRUGO, NULL, &proc_ops,
        usb_joystick_kbd);
}

int proc_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
    file->private_data = PDE_DATA(inode);
#else
    file->private_data = pde_data(inode);
#endif
    return 0;
}

ssize_t proc_read(struct file *file, char __user *buff, size_t size,
    loff_t *offset)
{
    struct usb_joystick_kbd *joystick_kbd = file->private_data;
    struct joystick_event event;

    wait_event_interruptible(joystick_kbd->wq,joystick_kbd->has_new_event);

    spin_lock(&joystick_kbd->event_lock);
    event = joystick_kbd->event;
    joystick_kbd->has_new_event = false;
    spin_unlock(&joystick_kbd->event_lock);

    size = min(size, sizeof(struct joystick_event));
    if (copy_to_user(buff, &event, size))
    {
        printk(KERN_ERR MOD_PREFIX "failed to copy to user in read");
        return -1;
    }

    return size;
}
