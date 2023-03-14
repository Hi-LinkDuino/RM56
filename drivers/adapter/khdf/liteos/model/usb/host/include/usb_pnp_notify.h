/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LITEOS_USB_PNP_NOTIFY_H
#define LITEOS_USB_PNP_NOTIFY_H

#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_usb_pnp_manage.h"
#include "osal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define USB_PNP_LOADER_SERVICE_NAME         "hdf_usb_pnp_manager_service"

#define USB_PNP_INTERFACE_MSLEEP_TIME       (200)
#define USB_PNP_NOTIFY_MSLEEP_TIME          (10)
#define USB_PNP_NOTIFY_REPORT_STACK_SIZE    (10000)

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif

typedef enum {
    USB_INFO_NORMAL_TYPE,
    USB_INFO_ID_TYPE,
    USB_INFO_DEVICE_ADDRESS_TYPE,
} UsbInfoQueryParaType;

typedef enum {
    USB_PNP_DEVICE_INIT_STATUS,
    USB_PNP_DEVICE_ADD_STATUS,
    USB_PNP_DEVICE_REMOVE_STATUS,
    USB_PNP_DEVICE_INTERFACE_STATUS,
} UsbPnpDeviceStatus;

typedef enum {
    USB_PNP_DEVICE_ADDRESS_TYPE,
    USB_PNP_DEVICE_VENDOR_PRODUCT_TYPE,
} UsbPnpDeviceParaType;

struct UsbPnpDeviceInfo {
    int32_t id;
    struct OsalMutex lock;
    UsbPnpDeviceStatus status;
    struct DListHead list;
    bool interfaceRemoveStatus[USB_PNP_INFO_MAX_INTERFACES];
    struct UsbPnpNotifyMatchInfoTable info;
};

struct UsbInfoQueryPara {
    UsbInfoQueryParaType type;
    union {
        int32_t id;
        uint64_t usbDevAddr;
        struct {
            int32_t devNum;
            int32_t busNum;
        };
    };
};

union UsbPnpDeviceInfoData {
    struct usb_device *usbDev;
    struct UsbPnpAddRemoveInfo *infoData;
};

struct UsbPnpNotifyDeviceList {
    struct usb_device *device;
    struct DListHead deviceList;
};

struct UsbGetDevicePara {
    UsbPnpDeviceParaType type;
    union {
        struct {
            uint8_t busNum;
            uint8_t devNum;
        };
        struct {
            uint16_t vendorId;
            uint16_t productId;
        };
    };
};

void UsbPnpNotifyDevice(const char *type, struct usb_device *udev);
struct usb_device *UsbPnpNotifyGetUsbDevice(struct UsbGetDevicePara paraData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* LITEOS_USB_PNP_NOTIFY_H */
