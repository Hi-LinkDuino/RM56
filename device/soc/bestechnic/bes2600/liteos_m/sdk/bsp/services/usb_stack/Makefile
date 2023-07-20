ifeq ($(USB_STACK), 1)
obj-y += usbhost/ adapter/
subdir-ccflags-y += -Iplatform/hal -Iplatform/cmsis/inc -Iplatform/drivers/ana \
					-Iservices/usb_stack/include -Iservices/usb_stack/adapter \
					-Iservices/usb_stack/usbdevice/class -Iservices/usb_stack/usbhost/class \
					-I$(LWIP_ROOT)/src/include/\
					-I$(LWIP_ROOT)/src/include/lwip/\
					-I$(LWIP_ROOT)/src/include/lwip/arch \
					-Inet/utils
ifeq ($(USB_HIGH_SPEED),1)
subdir-ccflags-y += -DUSB_HIGH_SPEED
endif
endif
