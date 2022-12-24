#ifndef __INPUT_DEV__
#define __INPUT_DEV__

struct input_dev;
struct usb_device;

struct input_dev *allocate_joystick_input_dev(struct usb_device *usb_dev);

unsigned char *get_keycode_map(unsigned int *row, unsigned int *col);

unsigned char move_keyboard_cursor(unsigned int *row, unsigned int *col, int d_row, int d_col);

#endif
