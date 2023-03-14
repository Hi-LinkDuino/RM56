/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_USB_PNP_MANAGE_H
#define HDF_USB_PNP_MANAGE_H

#include "hdf_base.h"

#define USB_PNP_NOTIFY_TEST_MODE        false
#define USB_PNP_INFO_MAX_INTERFACES     32

enum UsbPnpNotifyServiceCmd {
    USB_PNP_NOTIFY_ADD_INTERFACE,
    USB_PNP_NOTIFY_REMOVE_INTERFACE,
    USB_PNP_NOTIFY_REPORT_INTERFACE,
    USB_PNP_NOTIFY_ADD_DEVICE,
    USB_PNP_NOTIFY_REMOVE_DEVICE,
    #if USB_PNP_NOTIFY_TEST_MODE == true
    USB_PNP_NOTIFY_ADD_TEST,
    USB_PNP_NOTIFY_REMOVE_TEST,
    #endif
    USB_PNP_DRIVER_REGISTER_DEVICE,
    USB_PNP_DRIVER_UNREGISTER_DEVICE,
    USB_PNP_DRIVER_GETDEVICES = 20,
    USB_PNP_DRIVER_GADGET_ADD = 30,
    USB_PNP_DRIVER_GADGET_REMOVE,
};

enum UsbPnpNotifyRemoveType {
    USB_PNP_NOTIFY_REMOVE_BUS_DEV_NUM,
    USB_PNP_NOTIFY_REMOVE_INTERFACE_NUM,
};

enum {
    USB_PNP_NOTIFY_MATCH_VENDOR = 0x0001,
    USB_PNP_NOTIFY_MATCH_PRODUCT = 0x0002,
    USB_PNP_NOTIFY_MATCH_DEV_LOW = 0x0004,
    USB_PNP_NOTIFY_MATCH_DEV_HIGH = 0x0008,
    USB_PNP_NOTIFY_MATCH_DEV_CLASS = 0x0010,
    USB_PNP_NOTIFY_MATCH_DEV_SUBCLASS = 0x0020,
    USB_PNP_NOTIFY_MATCH_DEV_PROTOCOL = 0x0040,
    USB_PNP_NOTIFY_MATCH_INT_CLASS = 0x0080,
    USB_PNP_NOTIFY_MATCH_INT_SUBCLASS = 0x0100,
    USB_PNP_NOTIFY_MATCH_INT_PROTOCOL = 0x0200,
    USB_PNP_NOTIFY_MATCH_INT_NUMBER = 0x0400,
};

struct UsbPnpNotifyServiceInfo {
    uint32_t length;

    int32_t devNum;
    int32_t busNum;

    int32_t interfaceLength;
    uint8_t interfaceNumber[USB_PNP_INFO_MAX_INTERFACES];
} __attribute__ ((packed));

struct UsbPnpNotifyInterfaceInfo {
    uint8_t interfaceClass;
    uint8_t interfaceSubClass;
    uint8_t interfaceProtocol;

    uint8_t interfaceNumber;
};

struct UsbPnpNotifyDeviceInfo {
    uint16_t vendorId;
    uint16_t productId;

    uint16_t bcdDeviceLow;
    uint16_t bcdDeviceHigh;

    uint8_t deviceClass;
    uint8_t deviceSubClass;
    uint8_t deviceProtocol;
};

struct UsbPnpNotifyMatchInfoTable {
    uint64_t usbDevAddr;
    int32_t devNum;
    int32_t busNum;

    struct UsbPnpNotifyDeviceInfo deviceInfo;

    uint8_t removeType;
    uint8_t numInfos;

    struct UsbPnpNotifyInterfaceInfo interfaceInfo[USB_PNP_INFO_MAX_INTERFACES];
};

struct UsbPnpAddRemoveInfo {
    int32_t devNum;
    int32_t busNum;
    uint8_t interfaceNumber;
    uint8_t interfaceClass;
    uint8_t interfaceSubClass;
    uint8_t interfaceProtocol;
};

#endif /* HDF_USB_PNP_MANAGE_H */
