.PHONY: clean

obj-m += joystick_kbd.o
# joystick_kbd-objs := $(subst .c,.o,$(shell find src -name '*.c'))
joystick_kbd-objs := src/entry.o src/proc_event.o src/input_dev.o src/driver.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
