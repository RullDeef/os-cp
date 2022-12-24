#include <linux/input.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include "config.h"
#include "input_dev.h"
#include "usb_joystick_kbd.h"
#include "keycode_map.h"

static int jskbd_input_open(struct input_dev *input_dev);
static void jskbd_input_close(struct input_dev *input_dev);

static void input_timer_callback(struct timer_list *timer);

struct input_dev *allocate_joystick_input_dev(struct usb_device *usb_dev)
{
    int i;
    struct input_dev *input_dev = devm_input_allocate_device(&usb_dev->dev);
    if (input_dev == NULL)
        return input_dev;

    // fill input device struct (minimal needed)
    usb_to_input_id(usb_dev, &input_dev->id);
    input_dev->name = INPUT_DEV_NAME;
    input_dev->open = jskbd_input_open;
    input_dev->close = jskbd_input_close;

    // setup input events that input device can produce
    input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
    input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
    input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y) | BIT_MASK(REL_WHEEL);

    // set keycode bits
    input_dev->keycode = keycode_map;
    input_dev->keycodesize = sizeof(unsigned char);
    input_dev->keycodemax = ARRAY_SIZE(keycode_map);
    for (i = 0; i < ARRAY_SIZE(keycode_map); ++i)
        __set_bit(keycode_map[i], input_dev->keybit);
    __clear_bit(KEY_RESERVED, input_dev->keybit);

    // setup keyboard events
    input_set_capability(input_dev, EV_KEY, KEY_TAB);
    input_set_capability(input_dev, EV_KEY, KEY_LEFTALT);

    return input_dev;
}

unsigned char *get_keycode_map(unsigned int *row, unsigned int *col)
{
    // setup default values for row and col (KEY_F)
    if (row != NULL)
        *row = 3;
    if (col != NULL)
        *col = 4;

    return keycode_map;
}

unsigned char move_keyboard_cursor(unsigned int *row, unsigned int *col, int d_row, int d_col)
{
    unsigned int new_row;
    unsigned int new_col;
    unsigned int index;

    if (d_row == 0 && d_col == 0)
    {
        index = *row == 0 ? *col : keycode_row_len_acc[*row - 1] + *col;
        return keycode_map[index];
    }

    // printk(MOD_PREFIX "cur pos: %u %u delta: %d %d\n", *row, *col, d_row, d_col);

    new_row = ((int)*row + d_row + KEYCODE_ROWS) % KEYCODE_ROWS;
    *row = new_row;
    
    new_col = ((int)*col + d_col + KEYCODE_COLS(*row)) % KEYCODE_COLS(*row);
    *col = new_col;

    // printk(MOD_PREFIX "adjusted pos: %u %u\n", *row, *col);

    index = *row == 0 ? *col : keycode_row_len_acc[*row - 1] + *col;
    if (index > ARRAY_SIZE(keycode_map))
    {
        printk(MOD_PREFIX "invalid keycode index: %u\n", index);
        printk(MOD_PREFIX "given row=%u col=%u\n", *row, *col);
        return keycode_map[0];
    }

    return keycode_map[index];
}

// first time submitting urb on open
int jskbd_input_open(struct input_dev *input_dev)
{
    struct usb_joystick_kbd *usb_joystick_kbd = input_get_drvdata(input_dev);

    // initialize timer
    timer_setup(&usb_joystick_kbd->timer, input_timer_callback, 0);
    mod_timer(&usb_joystick_kbd->timer, jiffies + TIMER_PERIOD);

    // ping first urb
    if (usb_submit_urb(usb_joystick_kbd->urb, GFP_KERNEL) != 0)
        return -EIO;

    return 0;
}

// kills urb
void jskbd_input_close(struct input_dev *input_dev)
{
    struct usb_joystick_kbd *usb_joystick_kbd = input_get_drvdata(input_dev);
    del_timer_sync(&usb_joystick_kbd->timer);
    usb_kill_urb(usb_joystick_kbd->urb);
}

void input_timer_callback(struct timer_list *timer)
{
    struct usb_joystick_kbd *usb_joystick_kbd;
    bool was_update = false;

    usb_joystick_kbd = from_timer(usb_joystick_kbd, timer, timer);

    // check if relative update needed
    if (abs(usb_joystick_kbd->mouse_dx) > 2)
    {
        input_report_rel(usb_joystick_kbd->input_dev, REL_X, usb_joystick_kbd->mouse_dx);
        was_update = true;
    }
    if (abs(usb_joystick_kbd->mouse_dy) > 2)
    {
        input_report_rel(usb_joystick_kbd->input_dev, REL_Y, usb_joystick_kbd->mouse_dy);
        was_update = true;
    }
    if (abs(usb_joystick_kbd->wheel_dy) > 0)
    {
        input_report_rel(usb_joystick_kbd->input_dev, REL_WHEEL, usb_joystick_kbd->wheel_dy);
        was_update = true;
    }

    if (was_update)
    {
        input_sync(usb_joystick_kbd->input_dev);
        mod_timer(timer, jiffies + TIMER_PERIOD);
    }
    else
    {
        usb_joystick_kbd->is_timer_active = false;
    }
}
