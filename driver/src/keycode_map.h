#ifndef __JOYSTICK_KEYCODE_MAP__
#define __JOYSTICK_KEYCODE_MAP__

#include <linux/input.h>

static unsigned char keycode_map[] = {
    KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_DELETE,
    KEY_GRAVE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE,
    KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_BACKSLASH,
    KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_ENTER,
    KEY_LEFTSHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_RIGHTSHIFT,
    KEY_LEFTCTRL, 0, 0, KEY_LEFTALT, KEY_SPACE, KEY_RIGHTALT, KEY_RIGHTCTRL, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_RIGHT,
};

static int keycode_row_len[] = { 14, 14, 14, 13, 12, 11 };
static int keycode_row_len_acc[] = { 14, 28, 42, 55, 67, 78 };

#define KEYCODE_ROWS (sizeof(keycode_row_len) / sizeof(keycode_row_len[0]))
#define KEYCODE_COLS(row) (keycode_row_len[row])

#endif
