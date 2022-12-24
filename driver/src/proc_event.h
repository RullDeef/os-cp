#ifndef __JOYSTICK_KBD_PROC_EVENT__
#define __JOYSTICK_KBD_PROC_EVENT__

struct proc_dir_entry;
struct usb_joystick_kbd;

struct proc_dir_entry *create_joystick_event_entry(struct usb_joystick_kbd *usb_joystick_kbd);

#endif
