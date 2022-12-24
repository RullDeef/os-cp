#ifndef __USB_JOYSTICK_KBD__
#define __USB_JOYSTICK_KBD__

#include <linux/types.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include "config.h"
#include "joystick_event.h"

enum mouse_speed_mode
{
    MOUSE_SPEED_MODE_PRECISE = 0,
    MOUSE_SPEED_MODE_FAST = 1,
};

struct usb_joystick_kbd
{
    /* device related section */
    struct usb_device *usbdev;
    struct urb *urb;
    unsigned char *transfer_buffer;
    dma_addr_t dma_addr;

    /* mouse input handling stuff */
    struct input_dev *input_dev;
    struct timer_list timer;

    int mouse_dx;
    int mouse_dy;
    int wheel_dy;
    bool is_timer_active;
    enum mouse_speed_mode mouse_speed_mode;

    /* keyboard input handling */
    unsigned char *keycodes;
    unsigned int keypos_row;
    unsigned int keypos_col;

    // for triggers
    bool alt_pressed;
    bool tab_pressed;

    // proc entry for events passing between kernel and user space
    struct proc_dir_entry *proc_entry;

    // wait queue for events in proc_entry
    struct wait_queue_head wq;
    bool has_new_event;
    struct joystick_event event;
    struct spinlock event_lock;

    char prev_data[PACKET_LEN];
};

#endif
