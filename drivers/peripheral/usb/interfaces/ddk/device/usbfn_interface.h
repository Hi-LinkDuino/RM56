/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file usbfn_interface.h
 *
 * @brief Declares USB interface-specific definitions.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef USBFN_INTERFACE_H
#define USBFN_INTERFACE_H

#include "usb_object.h"
#include "usbfn_request.h"

/**
 * @brief Defines USB interfaces of the unspecified class.
 */
#define USB_INTERFACE_CLASS_UNSPEC              0x00

/**
 * @brief Defines USB interfaces of the Audio class.
 */
#define USB_INTERFACE_CLASS_AUDIO               0x01

/**
 * @brief Defines USB interfaces of the Communication Device Class (CDC).
 */
#define USB_INTERFACE_CLASS_CDC                 0x02

/**
 * @brief Defines USB interfaces of the Human Interface Device (HID) class.
 */
#define USB_INTERFACE_CLASS_HID                 0x03

/**
 * @brief Defines USB interfaces of the Physical class.
 */
#define USB_INTERFACE_CLASS_PHYSICAL            0x05

/**
 * @brief Defines USB interfaces of the Image class.
 */
#define USB_INTERFACE_CLASS_IMAGE               0x06

/**
 * @brief Defines USB interfaces of the Printer class.
 */
#define USB_INTERFACE_CLASS_PRINTER             0x07

/**
 * @brief Defines USB interfaces of the Mass Storage (MSD) class.
 */
#define USB_INTERFACE_CLASS_MASS_STORAGE        0x08

/**
 * @brief Defines USB interfaces of the Hub class.
 */
#define USB_INTERFACE_CLASS_HUB                 0x09

/**
 * @brief Defines USB interfaces of the CDC-Data class.
 */
#define USB_INTERFACE_CLASS_CDC_DATA            0x0a

/**
 * @brief Defines USB interfaces of the Smart Card class.
 */
#define USB_INTERFACE_CLASS_SMARTCARD           0x0b

/**
 * @brief Defines USB interfaces of the Firmware Upgrade class.
 */
#define USB_INTERFACE_CLASS_FIRM_UPD            0x0c

/**
 * @brief Defines USB interfaces of the Content Security class.
 */
#define USB_INTERFACE_CLASS_SECURITY            0x0d

/**
 * @brief Defines USB interfaces of the Video class.
 */
#define USB_INTERFACE_CLASS_VIDEO               0x0e

/**
 * @brief Defines USB interfaces of the Diagnostic Device class.
 */
#define USB_INTERFACE_CLASS_DIAGNOSTIC          0xdc

/**
 * @brief Defines USB interfaces of the Wireless Controller class.
 */
#define USB_INTERFACE_CLASS_WIRELESS            0xe0

/**
 * @brief Defines USB interfaces of the Miscellaneous class.
 */
#define USB_INTERFACE_CLASS_IAD                 0xef

/**
 * @brief Defines USB interfaces of the Application Specific class.
 */
#define USB_INTERFACE_CLASS_APP_SPEC            0xfe

/**
 * @brief Defines USB interfaces of the Vendor Specific class.
 */
#define USB_INTERFACE_CLASS_VENDOR              0xff

/**
 * @brief Defines the USB device event status.
 */
typedef enum {
    /** USB binding event */
    USBFN_STATE_BIND,
    /** USB unbinding event */
    USBFN_STATE_UNBIND,
    /** USB enabling event */
    USBFN_STATE_ENABLE,
    /** USB disabling event */
    USBFN_STATE_DISABLE,
    /** USB connection setup event */
    USBFN_STATE_SETUP,
    /** USB suspending event */
    USBFN_STATE_SUSPEND,
    /** USB resuming event */
    USBFN_STATE_RESUME,
} UsbFnDeviceState;

/**
 * @brief Defines structure parameters for the USB interface request.
 */
struct UsbFnCtrlRequest {
    /** Request type */
    uint8_t reqType;
    /** Specific request */
    uint8_t request;
    /** Request data value */
    uint16_t value;
    /** Request index value */
    uint16_t index;
    /**
    * Number of bytes in the data communication phase, which must be less than the
    * value of <b>bMaxPacketSize0</b> defined in the device descriptor
    */
    uint16_t length;
} __attribute__((packed));

/**
 * @brief Defines the structure of the events received by the USB interface.
 */
struct UsbFnEvent {
    /** Pointer to the received parameters of the control request */
    struct UsbFnCtrlRequest *setup;
    UsbFnDeviceState        type;
    void                    *context;
};

/**
 * @brief Defines the USB interface information.
 */
struct UsbFnInterfaceInfo {
    /** Interface index */
    uint8_t index;
    /** Number of pipes */
    uint8_t numPipes;
    /** Interface class */
    uint8_t interfaceClass;
    /** Interface subclass */
    uint8_t subclass;
    /** Interface protocol */
    uint8_t protocol;
    /** Interface configuration index */
    uint8_t configIndex;
};

/**
 * @brief Defines USB interface objects.
 */
struct UsbFnInterface {
    /** Pointer to the USB object */
    const struct UsbObject    *object;
    /** USB interface information */
    struct UsbFnInterfaceInfo info;
};

/**
 * @brief Defines the USB pipe information.
 */
struct UsbFnPipeInfo {
    /** Pipe ID */
    uint8_t           id;
    /** Pipe type */
    UsbPipeType       type;
    /** Pipe direction */
    UsbPipeDirection  dir;
    /** Maximum size of data packets that can be sent or received over the pipe */
    uint16_t          maxPacketSize;
    /** Data transmission interval of the pipe */
    uint8_t           interval;
};

/**
 * @brief Defines callback called when endpoint 0 receives a control event.
 *
 * This function needs to be defined by the function driver.
 *
 * @param event Indicates the pointer to the event information, including the event type and request.
 */
typedef void (*UsbFnEventCallback)(struct UsbFnEvent *event);

/**
 * @brief Defines the callback called when the function interface obtains or sets custom attributes.
 *
 * This function needs to be defined by the function driver.
 *
 * @param interface Indicates the pointer to the USB interface object.
 * @param name Indicates the pointer to the attribute name of the string type.
 * @param value Indicates the pointer to the attribute value of the string type.
 *
 * @return Returns <b>0</b> if the operation is successful;
 * returns a negative value defined in {@link UsbErrorType} otherwise.
 */
typedef int32_t (*UsbFnPropCallback)(const struct UsbFnInterface *intf, const char *name, const char *value);

/**
 * @brief Defines custom attribute information.
 */
struct UsbFnRegistInfo {
    /** Pointer to the attribute name */
    const char *name;
    /** Pointer to the attribute value */
    const char *value;
    /** Pointer to the callback for the request of obtaining custom attributes */
    UsbFnPropCallback getProp;
    /** Pointer to the callback for the request of setting custom attributes */
    UsbFnPropCallback setProp;
};

/**
 * @brief Defines the callback called when the USB interface starts a thread to receive EP0 events,
 * such as the setup event.
 *
 * @param interface Indicates the pointer to the USB interface object.
 * @param eventMask Indicates the type of the event to be processed.
 * The event is defined by <b>UsbFnDeviceState</b>.
 * Each bit of <b>eventMask</b> indicates an event.
 * For example, if the value of <b>eventMask</b> is <b>0xff</b>, all events are received;
 * if the value of <b>eventMask</b> is <b>0x01</b>, only the <b>USBFN_STATE_BIND</b> event is received.
 * @param callback Indicates the callback function called after a request is processed.
 * It needs to be defined by the application.
 * @param context Indicates the pointer to the private context data.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value
 * defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnStartRecvInterfaceEvent(struct UsbFnInterface *interface, uint32_t eventMask,
    UsbFnEventCallback callback, void *context);

/**
 * @brief Defines the callback called when the USB interface stops receiving EP0 events.
 *
 * @param interface Indicates the pointer to the USB interface object.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative
 * value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnStopRecvInterfaceEvent(struct UsbFnInterface *interface);

/**
 * @brief Enables a USB interface.
 *
 * This function is mainly used to initialize a request queue and open a pipe.
 *
 * @param interface Indicates the pointer to the USB interface object.
 *
 * @return Returns a <b>#UsbFnInterfaceHandle</b> containing the USB interface information
 * if the operation is successful; returns <b>NULL</b> otherwise.
 */
UsbFnInterfaceHandle UsbFnOpenInterface(struct UsbFnInterface *interface);

/**
 * @brief Disables a USB interface.
 *
 * This function is mainly used to close a pipe and delete a request queue.
 *
 * @param UsbFnInterfaceHandle Indicates the handle of the USB interface object.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value
 * defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnCloseInterface(UsbFnInterfaceHandle handle);

/**
 * @brief Obtains USB pipe information based on the specified pipe ID.
 *
 * @param interface Indicates the pointer to the USB interface object.
 * @param pipeId Indicates the pipe ID. The value ranges from 0 to the total number of pipes on the USB interface.
 * @param info Indicates the pointer to the obtained pipe information.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative
 * value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnGetInterfacePipeInfo(struct UsbFnInterface *interface, uint8_t pipeId, struct UsbFnPipeInfo *info);

/**
 * @brief Registers custom attributes for a USB interface.
 *
 * In addition to custom attributes, you can use this function to register the callback function
 * for the request of obtaining and setting custom attributes.
 *
 * @param interface Indicates the pointer to the USB interface object.
 * @param registInfo Indicates the pointer to the attribute information.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative
 * value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnRegistInterfaceProp(const struct UsbFnInterface *interface,
    const struct UsbFnRegistInfo *registInfo);

/**
 * @brief Obtains custom attributes of a USB interface.
 *
 * Custom attributes can be any of the following: attributes of device descriptors, such as VID and PID,
 * attributes of the USB interface defined in the HCS file, and attributes registered
 * in the code at the application layer.
 * For example, if <b>name</b> is <b>idVendor</b> when obtaining the VID, the obtained value will be
 * stored in <b>value</b> as a hex string.
 *
 * @param interface Indicates the pointer to the USB interface object.
 * @param name Indicates the pointer to the attribute name of the string type.
 * @param value Indicates the pointer to the attribute value of the string type. Except integer data,
 * which is a string converted from hex data, all the other types of data are strings by default.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative
 * value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnGetInterfaceProp(const struct UsbFnInterface *interface,
    const char *name, char *value);

/**
 * @brief Sets custom attributes of a USB interface.
 *
 * Custom attributes can be any of the following: attributes of device descriptors, such as VID and PID,
 * attributes of the USB interface defined in the HCS file, and attributes registered
 * in the code at the application layer.
 * For example, if <b>name</b> is <b>idVendor</b> when setting the VID, the set value will be
 * stored in <b>value</b> as a hex string.
 *
 * @param interface Indicates the pointer to the USB interface object.
 * @param name Indicates the pointer to the attribute name of the string type.
 * @param value Indicates the pointer to the attribute value of the string type. Except integer data,
 * which is a string converted from hex data, all the other types of data are strings by default.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative
 * value defined in {@link UsbErrorType} otherwise.
 */
int32_t UsbFnSetInterfaceProp(const struct UsbFnInterface *interface,
    const char *name, const char *value);

#endif /* USBFN_INTERFACE_H */
