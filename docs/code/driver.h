struct usb_joystick_kbd
{
    /* device related section */
    struct usb_device *usbdev;
    struct urb *urb;
    unsigned char *transfer_buffer;
    dma_addr_t dma_addr;

    /* mouse input handling stuff */
    struct input_dev *input_dev;
    int mouse_dx;
    int mouse_dy;
    int wheel_dy;
    struct timer_list timer;
    bool is_timer_active;

    /* keyboard input handling */
    unsigned char *keycodes;
    unsigned int keypos_row;
    unsigned int keypos_col;

    // proc entry for events passing between kernel and user space
    struct proc_dir_entry *proc_entry;
    struct wait_queue_head wq;
    bool has_new_event;
    struct joystick_event event;
    struct spinlock event_lock;
    char prev_data[PACKET_LEN];
};