/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef USB_DDK_PNP_LOADER_H
#define USB_DDK_PNP_LOADER_H

#include "devmgr_service_if.h"
#include "hdf_usb_pnp_manage.h"

#define USB_PNP_NOTIFY_SERVICE_NAME "hdf_usb_pnp_notify_service"

typedef enum {
    USB_PNP_NORMAL_STATUS,
    USB_PNP_ADD_STATUS,
    USB_PNP_REMOVE_STATUS
} UsbPnpDriverStatus;

struct UsbPnpMatchIdTable {
    const char *moduleName;
    const char *serviceName;
    const char *deviceMatchAttr;

    int32_t interfaceClassLength;
    uint32_t interfaceClassMask;
    int32_t interfaceSubClassLength;
    uint32_t interfaceSubClassMask;
    int32_t interfaceProtocolLength;
    uint32_t interfaceProtocolMask;
    int32_t interfaceLength;
    uint32_t interfaceMask;

    uint8_t pnpMatchFlag;

    uint8_t length;

    uint16_t matchFlag;

    uint16_t vendorId;
    uint16_t productId;

    uint16_t bcdDeviceLow;
    uint16_t bcdDeviceHigh;

    uint8_t deviceClass;
    uint8_t deviceSubClass;
    uint8_t deviceProtocol;

    uint8_t interfaceClass[USB_PNP_INFO_MAX_INTERFACES];
    uint8_t interfaceSubClass[USB_PNP_INFO_MAX_INTERFACES];
    uint8_t interfaceProtocol[USB_PNP_INFO_MAX_INTERFACES];

    uint8_t interfaceNumber[USB_PNP_INFO_MAX_INTERFACES];
};

struct UsbPnpDeviceListTable {
    struct DListHead list;
    const char *moduleName;
    const char *serviceName;
    const char *deviceMatchAttr;
    UsbPnpDriverStatus status;
    uint64_t usbDevAddr;
    int32_t devNum;
    int32_t busNum;
    int32_t interfaceLength;
    uint8_t interfaceNumber[USB_PNP_INFO_MAX_INTERFACES];
};

struct UsbPnpRemoveInfo {
    uint8_t removeType;
    uint64_t usbDevAddr;
    int32_t devNum;
    int32_t busNum;
    uint8_t interfaceNum;
};

int32_t UsbDdkPnpLoaderEventReceived(void *usbPnpManagerPtr, uint32_t id, struct HdfSBuf *data);
int32_t UsbDdkPnpLoaderEventHandle(void);

#endif /* USB_DDK_PNP_LOADER_H */
