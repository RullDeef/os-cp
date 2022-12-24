struct input_dev *allocate_joystick_input_dev(struct usb_device *usb_dev)
{
    struct input_dev *input_dev = devm_input_allocate_device(&usb_dev->dev);
    if (input_dev != NULL)
    {
        usb_to_input_id(usb_dev, &input_dev->id);
        input_dev->name = INPUT_DEV_NAME;
        input_dev->open = input_open;
        input_dev->close = input_close;
    }
    return input_dev;
}

int input_open(struct input_dev *input_dev)
{
    struct usb_joystick_kbd *jst = input_get_drvdata(input_dev);
    timer_setup(&jst->timer, input_timer_callback, 0);
    mod_timer(&jst->timer, jiffies + TIMER_PERIOD);
    if (usb_submit_urb(jst->urb, GFP_KERNEL) != 0)
        return -EIO;
    return 0;
}

void input_close(struct input_dev *input_dev)
{
    struct usb_joystick_kbd *jst = input_get_drvdata(input_dev);
    del_timer_sync(&jst->timer);
    usb_kill_urb(jst->urb);
}

void input_timer_callback(struct timer_list *timer)
{
    bool was_update = false;
    struct usb_joystick_kbd *jst = from_timer(usb_joystick_kbd,
        timer, timer);

    if (abs(jst->mouse_dx) > 2)
    {
        input_report_rel(jst->input_dev, REL_X, jst->mouse_dx);
        was_update = true;
    }
    if (abs(jst->mouse_dy) > 2)
    {
        input_report_rel(jst->input_dev, REL_Y, jst->mouse_dy);
        was_update = true;
    }
    if (abs(jst->wheel_dy) > 0)
    {
        input_report_rel(jst->input_dev, REL_WHEEL, jst->wheel_dy);
        was_update = true;
    }

    if (was_update)
    {
        input_sync(jst->input_dev);
        mod_timer(timer, jiffies + TIMER_PERIOD);
    }
    else
        jst->is_timer_active = false;
}
