void disconnect(struct usb_interface *intf)
{
    struct usb_joystick_kbd *jst = usb_get_intfdata(intf);

    del_timer_sync(&jst->timer);

    proc_remove(jst->proc_entry);

    // removing custom input device
    input_unregister_device(jst->input_dev);
    input_free_device(jst->input_dev);

    // no need to kill. Killed in input close already
    usb_free_urb(jst->urb);
    usb_free_coherent(jst->usbdev, PACKET_LEN, jst->transfer_buffer,
        jst->dma_addr);
    kfree(jst);

    module_put(THIS_MODULE);
    printk(KERN_INFO MOD_PREFIX "disconnect\n");
}