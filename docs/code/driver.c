void dispatch_joystick_input(struct usb_joystick_kbd *jst)
{
    struct joystick_event new_event;
    unsigned char *data;
    unsigned char *prev_data;
    unsigned char keycode;
    int keypos_d_row; // virtual keyboard cursor offset
    int keypos_d_col; //

    data = jst->transfer_buffer;
    prev_data = jst->prev_data;
    if (data[0] != 0x00)
        return;

    jst->mouse_dx = data[6]; // left stick
    jst->mouse_dy = data[8];
    jst->wheel_dy = data[12]; // right stick
    if (((abs(jst->mouse_dx) > 2) || (abs(jst->mouse_dy) > 2) ||
         (abs(jst->wheel_dy) > 0)) && !jst->is_timer_active)
    {
        input_report_rel(jst->input_dev, REL_X, jst->mouse_dx);
        input_report_rel(jst->input_dev, REL_Y, jst->mouse_dy);
        input_report_rel(jst->input_dev, REL_WHEEL, jst->wheel_dy);

        mod_timer(&jst->timer, jiffies + TIMER_PERIOD);
        jst->is_timer_active = true;
    }

    input_report_key(jst->input_dev, BTN_LEFT, data[3] & 0x10);
    input_report_key(jst->input_dev, BTN_RIGHT, data[3] & 0x20);
    input_report_key(jst->input_dev, BTN_MIDDLE, data[3] & 0x40);

    // read D-pad input for virtual keyboard cursor movement
    keypos_d_row = ((int)(data[2] >> 1) & 1) - ((int)(data[2] >> 0) & 1);
    keypos_d_col = ((int)(data[2] >> 3) & 1) - ((int)(data[2] >> 2) & 1);

    keycode = move_keyboard_cursor(&jst->keypos_row, &jst->keypos_col,0,0);
    input_report_key(jst->input_dev, keycode, 0); // release old key

    // get new key
    keycode = move_keyboard_cursor(&jst->keypos_row, &jst->keypos_col,
        keypos_d_row, keypos_d_col);

    input_report_key(jst->input_dev, keycode, data[3] & BIT(0));
    input_sync(jst->input_dev);

    new_event.keyboard_cursor_row = jst->keypos_row;
    new_event.keyboard_cursor_col = jst->keypos_col;

    memcpy(prev_data, data, PACKET_LEN);

    spin_lock(&jst->event_lock);
    jst->event = new_event;
    jst->has_new_event = true;
    spin_unlock(&jst->event_lock);
    wake_up_all(&jst->wq);
}
