#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0)
#   include <linux/module.h>
#   include <linux/slab.h>
#endif
#include <linux/usb.h>
#include <linux/input.h>
#include <linux/proc_fs.h>
#include "config.h"
#include "usb_joystick_kbd.h"
#include "proc_event.h"
#include "input_dev.h"
#include "driver.h"

static const struct usb_device_id joystick_dev_table[] = {
    {USB_DEVICE(USB_JSKBD_VENDOR_ID, USB_JSKBD_PRODUCT_ID)},
    {}};
MODULE_DEVICE_TABLE(usb, joystick_dev_table);

static int probe(struct usb_interface *intf, const struct usb_device_id *id);
static void disconnect(struct usb_interface *intf);

static void dispatch_joystick_input(struct usb_joystick_kbd *usb_joystick_kbd);

static struct usb_driver joystick_kbd_driver = {
    .name = "joystick_kbd",
    .id_table = joystick_dev_table,
    .probe = probe,
    .disconnect = disconnect,
};

int register_driver()
{
    return usb_register(&joystick_kbd_driver);
}

void deregister_driver()
{
    usb_deregister(&joystick_kbd_driver);
}

void dispatch_joystick_input(struct usb_joystick_kbd *usb_joystick_kbd)
{
    // struct joystick_event old_event;
    struct joystick_event new_event;

    unsigned char *data;
    unsigned char *prev_data;

    // virtual keyboard cursor offset
    int keypos_d_row;
    int keypos_d_col;
    unsigned char keycode;

    data = usb_joystick_kbd->transfer_buffer;
    prev_data = usb_joystick_kbd->prev_data;

    // valid pad data
    if (data[0] != 0x00)
        return;

    // left stick
    usb_joystick_kbd->mouse_dx = (__s16)le16_to_cpup((__le16 *)(data + 6));
    usb_joystick_kbd->mouse_dy = -(__s16)le16_to_cpup((__le16 *)(data + 8));

    // right stick
    usb_joystick_kbd->wheel_dy = (__s16)le16_to_cpup((__le16 *)(data + 12));

    do_div(usb_joystick_kbd->mouse_dx, 256 * 16);
    do_div(usb_joystick_kbd->mouse_dy, 256 * 16);
    do_div(usb_joystick_kbd->wheel_dy, 256 * 64);

    if (usb_joystick_kbd->mouse_speed_mode == MOUSE_SPEED_MODE_FAST)
    {
        usb_joystick_kbd->mouse_dx *= 2;
        usb_joystick_kbd->mouse_dy *= 2;
    }

    if (((abs(usb_joystick_kbd->mouse_dx) > 2) ||
         (abs(usb_joystick_kbd->mouse_dy) > 2) ||
         (abs(usb_joystick_kbd->wheel_dy) > 0)) &&
        !usb_joystick_kbd->is_timer_active)
    {
        input_report_rel(usb_joystick_kbd->input_dev, REL_X, usb_joystick_kbd->mouse_dx);
        input_report_rel(usb_joystick_kbd->input_dev, REL_Y, usb_joystick_kbd->mouse_dy);
        input_report_rel(usb_joystick_kbd->input_dev, REL_WHEEL, usb_joystick_kbd->wheel_dy);

        mod_timer(&usb_joystick_kbd->timer, jiffies + TIMER_PERIOD);
        usb_joystick_kbd->is_timer_active = true;
    }

    input_report_key(usb_joystick_kbd->input_dev, BTN_LEFT, data[3] & 0x10);
    input_report_key(usb_joystick_kbd->input_dev, BTN_RIGHT, data[3] & 0x20);
    input_report_key(usb_joystick_kbd->input_dev, BTN_MIDDLE, data[3] & 0x40);

    // task switch with left/right triggers
    // printk(KERN_INFO MOD_PREFIX "BUTTONS:  %d %d", data[3] & 0x01, data[3] & 0x02);
    // printk(KERN_INFO MOD_PREFIX "TRIGGERS: %d %d", data[4], data[5]);

    // TAB for right trigger
    if (data[5] >= 200)
    {
        if (!usb_joystick_kbd->tab_pressed)
        {
            input_report_key(usb_joystick_kbd->input_dev, KEY_TAB, 1);
            usb_joystick_kbd->tab_pressed = true;
        }
    }
    else
    {
        input_report_key(usb_joystick_kbd->input_dev, KEY_TAB, 0);
        usb_joystick_kbd->tab_pressed = false;
    }

    // alt for left trigger
    if (data[4] >= 200)
    {
        if (!usb_joystick_kbd->alt_pressed)
        {
            input_report_key(usb_joystick_kbd->input_dev, KEY_LEFTALT, 1);
            usb_joystick_kbd->alt_pressed = true;
        }
    }
    else
    {
        input_report_key(usb_joystick_kbd->input_dev, KEY_LEFTALT, 0);
        usb_joystick_kbd->alt_pressed = false;
    }

    // read D-pad input for virtual keyboard cursor movement
    keypos_d_row = ((int)(data[2] >> 1) & 1) - ((int)(data[2] >> 0) & 1);
    keypos_d_col = ((int)(data[2] >> 3) & 1) - ((int)(data[2] >> 2) & 1);

    // release old key
    keycode = move_keyboard_cursor(&usb_joystick_kbd->keypos_row, &usb_joystick_kbd->keypos_col, 0, 0);
    input_report_key(usb_joystick_kbd->input_dev, keycode, 0);

    // get new key
    keycode = move_keyboard_cursor(
        &usb_joystick_kbd->keypos_row,
        &usb_joystick_kbd->keypos_col,
        keypos_d_row,
        keypos_d_col);

    // read input from keyboard (LB)
    input_report_key(usb_joystick_kbd->input_dev, keycode, data[3] & BIT(0));
    
    // backspace on RB
    input_report_key(usb_joystick_kbd->input_dev, KEY_BACKSPACE, data[3] & BIT(1));

    input_sync(usb_joystick_kbd->input_dev);

    // fill in event structure (maybe use spinlock here to annihilate races)
    // old_event = usb_joystick_kbd->event;

    new_event.left_stick_dx = (__s16)le16_to_cpup((__le16 *)(data + 6)) + 128;
    new_event.left_stick_dy = (__s16)le16_to_cpup((__le16 *)(data + 8)) + 128;
    new_event.right_stick_dx = (__s16)le16_to_cpup((__le16 *)(data + 10)) + 128;
    new_event.right_stick_dy = (__s16)le16_to_cpup((__le16 *)(data + 12)) + 128;
    new_event.a_pressed = ~((prev_data[3] >> 4) & 1) & (data[3] >> 4) & 1;
    new_event.a_released = ((prev_data[3] >> 4) & 1) & ~((data[3] >> 4) & 1);
    new_event.b_pressed = ~((prev_data[3] >> 5) & 1) & (data[3] >> 5) & 1;
    new_event.b_released = ((prev_data[3] >> 5) & 1) & ~((data[3] >> 5) & 1);
    new_event.x_pressed = ~((prev_data[3] >> 6) & 1) & (data[3] >> 6) & 1;
    new_event.x_released = ((prev_data[3] >> 6) & 1) & ~((data[3] >> 6) & 1);
    new_event.y_pressed = ~((prev_data[3] >> 7) & 1) & (data[3] >> 7) & 1;
    new_event.y_released = ((prev_data[3] >> 7) & 1) & ~((data[3] >> 7) & 1);
    new_event.keyboard_cursor_row = usb_joystick_kbd->keypos_row;
    new_event.keyboard_cursor_col = usb_joystick_kbd->keypos_col;
    /// TODO: fill rest...

    memcpy(prev_data, data, PACKET_LEN);

    spin_lock(&usb_joystick_kbd->event_lock);
    usb_joystick_kbd->event = new_event;
    usb_joystick_kbd->has_new_event = true;
    spin_unlock(&usb_joystick_kbd->event_lock);

    wake_up_all(&usb_joystick_kbd->wq);
}

void jskbd_complete(struct urb *urb)
{
    int status = urb->status;
    struct usb_joystick_kbd *usb_joystick_kbd = (struct usb_joystick_kbd *)urb->context;

    switch (status)
    {
    case 0:
        // printk(KERN_INFO MOD_PREFIX "urb completed!\n");
        dispatch_joystick_input(usb_joystick_kbd);
        break;

    case -ECONNRESET:
    case -ENOENT:
    case -ESHUTDOWN:
        spin_lock(&usb_joystick_kbd->event_lock);
        memset(&usb_joystick_kbd->event, 0xFF, sizeof(struct joystick_event));
        usb_joystick_kbd->has_new_event = true;
        spin_unlock(&usb_joystick_kbd->event_lock);
        wake_up_all(&usb_joystick_kbd->wq);

        printk(KERN_WARNING MOD_PREFIX "bad urb status: %d\n", status);
        return;

    default:
        printk(KERN_INFO MOD_PREFIX "non-zero urb status: %d\n", status);
        break;
    }

    // printk(KERN_INFO MOD_PREFIX "submitting new urb...\n");
    memset(usb_joystick_kbd->transfer_buffer, 0, PACKET_LEN);
    status = usb_submit_urb(urb, GFP_ATOMIC);
    if (status != 0)
    {
        printk(KERN_ERR MOD_PREFIX "failed to resubmit urb. status=%d\n", status);
    }
}

int probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    int status;
    struct usb_joystick_kbd *jst;
    struct usb_endpoint_descriptor *int_in, *int_out;
    int pipe;

    printk(KERN_INFO MOD_PREFIX "probe\n");

    // allocating memory for usb_joystick_kbd struct
    jst = kzalloc(sizeof(struct usb_joystick_kbd), GFP_KERNEL);
    if (jst == NULL)
    {
        printk(KERN_ERR MOD_PREFIX "failed to allocate memory for struct usb_joystick_kbd\n");
        return -ENOMEM;
    }

    /* set up the endpoint information. Use only the first int-in endpoint */
    status = usb_find_common_endpoints(intf->cur_altsetting, NULL, NULL, &int_in, &int_out);
    if (status != 0)
    {
        printk(KERN_ERR MOD_PREFIX "failed to find common endpoints: %d\n", status);
        goto ret1;
    }

    // allocating buffer data for data transfer
    jst->usbdev = interface_to_usbdev(intf);
    jst->transfer_buffer = usb_alloc_coherent(jst->usbdev, PACKET_LEN, GFP_KERNEL, &jst->dma_addr);
    if (jst->transfer_buffer == NULL)
    {
        printk(KERN_ERR MOD_PREFIX "failed to allocate transfer buffer\n");
        status = -ENOMEM;
        goto ret1;
    }

    // allocating USB request block
    jst->urb = usb_alloc_urb(0, GFP_KERNEL);
    if (jst->urb == NULL)
    {
        printk(KERN_ERR MOD_PREFIX "failed to allocate urb\n");
        status = -ENOMEM;
        goto ret2;
    }

    // allocate input device
    jst->input_dev = allocate_joystick_input_dev(jst->usbdev);
    if (jst->input_dev == NULL)
    {
        printk(KERN_ERR MOD_PREFIX "failed to allocate input device\n");
        status = -ENOMEM;
        goto ret3;
    }

    // to access jst from input_dev
    input_set_drvdata(jst->input_dev, jst);

    // link keycodes
    jst->keycodes = get_keycode_map(&jst->keypos_row, &jst->keypos_col);

    // register created input device
    status = input_register_device(jst->input_dev);
    if (status != 0)
    {
        printk(KERN_ERR MOD_PREFIX "failed to register input device. status: %d\n", status);
        goto ret4;
    }

    pipe = usb_rcvintpipe(jst->usbdev, int_in->bEndpointAddress);

    // fill urb for interrupt type
    usb_fill_int_urb(jst->urb, jst->usbdev, pipe, jst->transfer_buffer, PACKET_LEN, jskbd_complete, jst, int_in->bInterval);
    jst->urb->transfer_dma = jst->dma_addr;
    jst->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    // custom joystick related stuff
    jst->mouse_speed_mode = MOUSE_SPEED_MODE_FAST;

    usb_set_intfdata(intf, jst);

    // creating entry in proc vfs
    jst->proc_entry = create_joystick_event_entry(jst);
    if (jst->proc_entry == NULL)
    {
        printk(KERN_ERR MOD_PREFIX "could not create proc entry");
        status = -EINVAL;
        goto ret5;
    }

    init_waitqueue_head(&jst->wq);
    __module_get(THIS_MODULE);
    return 0;

ret5:
    input_unregister_device(jst->input_dev);
ret4:
    input_free_device(jst->input_dev);
ret3:
    usb_free_urb(jst->urb);
ret2:
    usb_free_coherent(jst->usbdev, PACKET_LEN, jst->transfer_buffer, jst->dma_addr);
ret1:
    kfree(jst);
    return status;
}

void disconnect(struct usb_interface *intf)
{
    struct usb_joystick_kbd *usb_joystick_kbd = usb_get_intfdata(intf);

    del_timer_sync(&usb_joystick_kbd->timer);

    proc_remove(usb_joystick_kbd->proc_entry);

    // removing custom input device
    input_unregister_device(usb_joystick_kbd->input_dev);
    input_free_device(usb_joystick_kbd->input_dev);

    // no need to kill. Killed in input close already
    usb_free_urb(usb_joystick_kbd->urb);
    usb_free_coherent(usb_joystick_kbd->usbdev, PACKET_LEN, usb_joystick_kbd->transfer_buffer, usb_joystick_kbd->dma_addr);
    kfree(usb_joystick_kbd);

    module_put(THIS_MODULE);
    printk(KERN_INFO MOD_PREFIX "disconnect\n");
}
