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

#ifndef USBD_TYPE_H
#define USBD_TYPE_H

#include <stdint.h>
#include <stdlib.h>

#define USB_MAX_INTERFACES 32

#define HDF_USB_USBD_DESC "hdf.usb.usbd"
#define HDF_USB_USBFN_DESC "hdf.usb.usbfn"

/**
 * Bitmask used for extracting the USBEndpoint direction from it's address
 */
static const int32_t USB_ENDPOINT_DIR_MASK = 0x80;

/**
 * Used to signify direction of data for USBEndpoint is IN, device to host
 */
static const int32_t USB_ENDPOINT_DIR_IN = 0x80;

/**
 * Used to signify direction of data for USBEndpoint is OUT, host to device
 */
static const int32_t USB_ENDPOINT_DIR_OUT = 0;

typedef void (*UsbdRequestCallback)(uint8_t *requestArg);

enum UsbdBulkCbCmd {
    CMD_USBD_BULK_CALLBACK_READ,
    CMD_USBD_BULK_CALLBACK_WRITE,
};

enum UsbdDeviceAction {
    ACT_DEVUP = 0,
    ACT_DEVDOWN,
    ACT_UPDEVICE,
    ACT_DOWNDEVICE,
};

enum UsbdCmd {
    CMD_BIND_USB_SUBSCRIBER = 0,
    CMD_UNBIND_USB_SUBSCRIBER,
    CMD_NOTIFY_DEVICE_DOWN,
    CMD_NOTIFY_DEVICE_FOUND,
    CMD_NOTIFY_PORT_CHANGED,
    CMD_CLASS_CTRL_SYNC,
    CMD_FUN_ADD_FUNCTION,
    CMD_FUN_ADD_INTERFACE,
    CMD_FUN_CLAIM_INTERFACE,
    CMD_FUN_CLOSE_DEVICE,
    CMD_FUN_CONNECT_DEVICE,
    CMD_FUN_DELETE_INTERFACE,
    CMD_FUN_GET_CONFIG,
    CMD_FUN_GET_CURRENT_FUNCTIONS,
    CMD_FUN_GET_DESCRIPTOR,
    CMD_FUN_GET_FILEDESCRIPTOR,
    CMD_FUN_GET_DEVICE_DESCRIPTOR,
    CMD_FUN_GET_CONFIG_DESCRIPTOR,
    CMD_FUN_GET_STRING_DESCRIPTOR,
    CMD_FUN_GET_INTERFACE_DESCRIPTOR,
    CMD_FUN_GET_ENDPOINT_DESCRIPTOR,
    CMD_FUN_HANDLE_REQUEST,
    CMD_FUN_OPEN_DEVICE,
    CMD_FUN_RELEASE_INTERFACE,
    CMD_FUN_REMOVE_FUNCTION,
    CMD_FUN_REMOVE_INTERFACE,
    CMD_FUN_SEND_BULK_READ_ASYNC,
    CMD_FUN_SEND_BULK_READ_SYNC,
    CMD_FUN_SEND_BULK_WRITE_ASYNC,
    CMD_FUN_SEND_BULK_WRITE_SYNC,
    CMD_FUN_SEND_CTRL_READ_ASYNC,
    CMD_FUN_SEND_CTRL_READ_SYNC,
    CMD_FUN_SEND_CTRL_REQUEST_ASYNC,
    CMD_FUN_SEND_CTRL_REQUEST_SYNC,
    CMD_FUN_SEND_CTRL_WRITE_ASYNC,
    CMD_FUN_SEND_CTRL_WRITE_SYNC,
    CMD_FUN_SEND_INTERRUPT_READ_ASYNC,
    CMD_FUN_SEND_INTERRUPT_READ_SYNC,
    CMD_FUN_SEND_INTERRUPT_WRITE_ASYNC,
    CMD_FUN_SEND_INTERRUPT_WRITE_SYNC,
    CMD_FUN_SEND_ISO_READ_ASYNC,
    CMD_FUN_SEND_ISO_READ_SYNC,
    CMD_FUN_SEND_ISO_WRITE_ASYNC,
    CMD_FUN_SEND_ISO_WRITE_SYNC,
    CMD_FUN_SET_CONFIG,
    CMD_FUN_SET_CURRENT_FUNCTIONS,
    CMD_FUN_SET_INTERFACE,
    CMD_ADD_INTERFACE,
    CMD_REMOVE_INTERFACE,
    CMD_GET_CHARGE_STATE,
    CMD_GET_DEVICES,
    CMD_HAS_RIGHT,
    CMD_READ_DATA_SYNC,
    CMD_READ_PARM,
    CMD_SET_BAUDRATE,
    CMD_SET_ROLE,
    CMD_QUERY_PORT,
    CMD_STD_CTRL_GET_CONFIGURATION,
    CMD_STD_CTRL_GET_DESCRIPTOR_ASYNC,
    CMD_STD_CTRL_GET_DESCRIPTOR_CMD,
    CMD_STD_CTRL_GET_INTERFACE,
    CMD_STD_CTRL_GET_STATUS_CMD,
    CMD_NOTIFY_SUBSCRIBER_DEVICE_EVENT,
    CMD_FUN_REQUEST_QUEUE,
    CMD_FUN_REQUEST_WAIT,
    CMD_FUN_REQUEST_CANCEL,
    CMD_FUN_REG_BULK_CALLBACK,
    CMD_FUN_UNREG_BULK_CALLBACK,
    CMD_FUN_BULK_CANCEL,
};

struct UsbdDevice {
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t interface;
    uint8_t endPoint;
};
struct UsbdPeripheral {
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t iSerialNumber;
};
struct UsbdPort {
    uint8_t modes;
    uint8_t roles;
};
struct UsbdRequestDataSync {
    uint8_t endPoint;
    int32_t *requested;
    uint8_t *data;
    int32_t length;
    uint32_t timeout;
};
struct UsbdRequestDataAsync {
    unsigned char endPoint;
    unsigned char *buffer;
    int32_t length;
    int32_t numIsoPackets;
    UsbdRequestCallback callback;
    uint8_t *userData;
    uint32_t timeout;
};
struct UsbdControlRequestSetupData {
    uint8_t requestType;
    uint8_t requestCmd;
    uint16_t value;
    uint16_t index;
    uint8_t *data;
    uint16_t length;
    uint32_t timeout;
};
struct UsbdDescriptorParam {
    uint8_t descType;
    uint8_t descIndex;
    int32_t length;
};
struct UsbdDescriptorHeader {
    uint8_t bLength;
    uint8_t bDescriptorType;
} __attribute__((packed));

struct UsbdDeviceDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __attribute__((packed));

struct UsbdConfigDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} __attribute__((packed));

struct UsbdInterfaceDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} __attribute__((packed));

struct UsbdEndpointDescriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} __attribute__((packed));

struct UsbNotifyServiceInfo {
    uint32_t length;

    int32_t devNum;
    int32_t busNum;

    int32_t interfaceLength;
    uint8_t interfaceNumber[USB_MAX_INTERFACES];
} __attribute__((packed));

enum PortRoleType { DATA_ROLE, POWER_ROLE, MODE };

enum PortPowerRole { SOURCE, SINK };

enum PortMode { HOST, DEVICE, OTG };

struct PortInfo {
    int32_t portId;
    int32_t powerRole;
    int32_t dataRole;
    int32_t mode;
};

// Keep it same as the inner kit usb_info.h
struct UsbdInfo {
    int32_t capacity_;
    int32_t voltage_;
    int32_t temperature_;
    int32_t healthState_;
    int32_t pluggedType_;
    int32_t pluggedMaxCurrent_;
    int32_t pluggedMaxVoltage_;
    int32_t chargeState_;
    int32_t chargeCounter_;
    int8_t present_;
    const char *technology_;
};

struct UsbdRequest {
};

#endif // USBD_TYPE_H
