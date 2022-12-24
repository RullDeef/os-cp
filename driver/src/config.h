#ifndef __JOYSTICK_DRIVER_CONFIG__
#define __JOYSTICK_DRIVER_CONFIG__

#define MOD_PREFIX "joystick_kbd: "

#define PACKET_LEN 64
#define USB_JSKBD_VENDOR_ID 0x046d
#define USB_JSKBD_PRODUCT_ID 0xc21d

#define INPUT_DEV_NAME "Joystick to Mouse Input"
#define TIMER_PERIOD (HZ / 100) // 10 ms

#endif
