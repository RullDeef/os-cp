#ifndef __JOYSTICK_EVENT__
#define __JOYSTICK_EVENT__

struct joystick_event
{
    int left_stick_dx : 8;
    int left_stick_dy : 8;
    int right_stick_dx : 8;
    int right_stick_dy : 8;

    unsigned int a_pressed : 1;
    unsigned int a_released : 1;
    unsigned int b_pressed : 1;
    unsigned int b_released : 1;
    unsigned int y_pressed : 1;
    unsigned int y_released : 1;
    unsigned int x_pressed : 1;
    unsigned int x_released : 1;

    unsigned int lb_pressed : 1;
    unsigned int lb_released : 1;
    unsigned int start_pressed : 1;
    unsigned int start_released : 1;
    unsigned int back_pressed : 1;
    unsigned int back_released : 1;
    unsigned int mode_pressed : 1;
    unsigned int mode_released : 1;

    unsigned int keyboard_cursor_row;
    unsigned int keyboard_cursor_col;
};

#endif
