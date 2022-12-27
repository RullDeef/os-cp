int probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    int status;
    struct usb_joystick_kbd *jst;
    struct usb_endpoint_descriptor *int_in, *int_out;
    int pipe;

    // allocating memory for usb_joystick_kbd struct
    jst = kzalloc(sizeof(struct usb_joystick_kbd), GFP_KERNEL);
    if (jst == NULL)
    {
        printk(KERN_ERR MOD_PREFIX "failed to allocate memory for struct
            usb_joystick_kbd\n");
        return -ENOMEM;
    }

    /* set up the endpoint. Use only the first int-in endpoint */
    status = usb_find_common_endpoints(intf->cur_altsetting, NULL, NULL,
        &int_in, &int_out);
    if (status != 0)
    {
        printk(KERN_ERR MOD_PREFIX "failed to find common endpoints: %d\n",
            status);
        goto ret1;
    }

    // allocating buffer data for data transfer
    jst->usbdev = interface_to_usbdev(intf);
    jst->transfer_buffer = usb_alloc_coherent(jst->usbdev, PACKET_LEN,
        GFP_KERNEL, &jst->dma_addr);
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

    input_set_drvdata(jst->input_dev, jst);

    // link keycodes
    jst->keycodes = get_keycode_map(&jst->keypos_row, &jst->keypos_col);

    // register created input device
    status = input_register_device(jst->input_dev);
    if (status != 0)
    {
        printk(KERN_ERR MOD_PREFIX "failed to register input device.
            status: %d\n", status);
        goto ret4;
    }

    pipe = usb_rcvintpipe(jst->usbdev, int_in->bEndpointAddress);

    // fill urb for interrupt type
    usb_fill_int_urb(jst->urb, jst->usbdev, pipe, jst->transfer_buffer,
        PACKET_LEN, jskbd_complete, jst, int_in->bInterval);
    jst->urb->transfer_dma = jst->dma_addr;
    jst->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

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
    usb_free_coherent(jst->usbdev, PACKET_LEN, jst->transfer_buffer,
        jst->dma_addr);
ret1:
    kfree(jst);
    return status;
}