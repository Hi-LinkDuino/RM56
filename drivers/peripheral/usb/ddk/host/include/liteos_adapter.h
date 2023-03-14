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

#ifndef USB_DDK_LITEOS_ADAPTER_H
#define USB_DDK_LITEOS_ADAPTER_H

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "liteos_ddk_usb.h"

#define MAX_BULK_DATA_BUFFER_LENGTH     4096
#define MAX_BULK_URBS_PER_REQUEST       1
#define MAX_ISO_PACKETS_PER_URB         128
#define MAX_ISO_DATA_BUFFER_LEN         (8 * 1024)
#define MAX_ISO_URBS_PER_REQUEST        (((MAX_ISO_DATA_BUFFER_LEN - 1) / MAX_ISO_PACKETS_PER_URB) + 1)

#define URBS_PER_REQUEST                MAX(MAX_BULK_URBS_PER_REQUEST, MAX_ISO_URBS_PER_REQUEST)

struct UsbCtrltransfer {
    uint8_t requestType;
    uint8_t request;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint32_t timeout;
    void *data;
};

struct UsbSetInterface {
    unsigned int interface;
    unsigned int altSetting;
};

enum UsbRequestFlags {
    USB_REQUEST_SHORT_NOT_OK = (1U << 0),
    USB_REQUEST_FREE_BUFFER = (1U << 1),
    USB_REQUEST_FREE_TRANSFER = (1U << 2),
    USB_REQUEST_ADD_ZERO_PACKET = (1U << 3)
};

struct UsbAdapterStreams {
    unsigned int numStreams;
    unsigned int numEps;
    unsigned char eps[0];
};

struct ConfigDescriptor {
    struct UsbConfigDescriptor *desc;
    size_t actualLen;
};

struct UsbDevicePriv {
    void *descriptors;
    size_t descriptorsLen;
    uint8_t activeConfig;
    struct ConfigDescriptor *configDescriptors;
};

#define BUS_OFFSET      8
#define USB_ADAPTER_URB_TYPE_ISO        0
#define USB_ADAPTER_URB_TYPE_INTERRUPT  1
#define USB_ADAPTER_URB_TYPE_CONTROL        2
#define USB_ADAPTER_URB_TYPE_BULK       3
#define USB_ADAPTER_URB_SHORT_NOT_OK        0x01
#define USB_ADAPTER_URB_ISO_ASAP        0x02
#define USB_ADAPTER_URB_BULK_CONTINUATION   0x04
#define USB_ADAPTER_URB_QUEUE_BULK      0x10
#define USB_ADAPTER_URB_ZERO_PACKET     0x40
#define USB_ADAPTER_CAP_ZERO_PACKET         0x01
#define USB_ADAPTER_CAP_BULK_CONTINUATION       0x02
#define USB_ADAPTER_CAP_NO_PACKET_SIZE_LIM      0x04
#define USB_ADAPTER_CAP_BULK_SCATTER_GATHER     0x08
#define USB_ADAPTER_CAP_REAP_AFTER_DISCONNECT       0x10

struct UsbOsAdapterOps {
    int32_t (*init)(const struct UsbSession *session);
    void (*exit)(const struct UsbSession *session);
    struct UsbDeviceHandle *(*openDevice)(struct UsbSession *session, uint8_t busNum, uint8_t usbAddr);
    void (*closeDevice)(struct UsbDeviceHandle *devHandle);
    int32_t (*getConfigDescriptor)(const struct UsbDevice *device, uint8_t configIndex, void *buffer, size_t len);
    int32_t (*getConfiguration)(const struct UsbDeviceHandle *devHandle, uint8_t *activeConfig);
    int32_t (*setConfiguration)(struct UsbDeviceHandle *devHandle, int32_t activeConfig);
    int32_t (*claimInterface)(const struct UsbDeviceHandle *devHandle, unsigned int interfaceNumber);
    int32_t (*releaseInterface)(const struct UsbDeviceHandle *devHandle, unsigned int interfaceNumber);
    int32_t (*setInterfaceAltsetting)(const struct UsbDeviceHandle *devHandle, uint8_t interfaceNumber,
        uint8_t altsetting);
    int32_t (*clearHalt)(const struct UsbDeviceHandle *devHandle, unsigned int endpoint);
    int32_t (*resetDevice)(const struct UsbDeviceHandle *devHandle);
    struct UsbHostRequest *(*allocRequest)(const struct UsbDeviceHandle *handle, int32_t isoPackets, size_t len);
    int32_t (*freeRequest)(struct UsbHostRequest *request);
    int32_t (*submitRequest)(struct UsbHostRequest *request);
    int32_t (*cancelRequest)(const struct UsbHostRequest *request);
    int32_t (*urbCompleteHandle)(const struct UsbDeviceHandle *devHandle);
};

struct UsbOsAdapterOps *UsbAdapterGetOps(void);
UsbRawTidType UsbAdapterGetTid(void);
int32_t UsbAdapterRegisterSignal(void);
int32_t UsbAdapterKillSignal(struct UsbDeviceHandle *devHandle, UsbRawTidType tid);
int32_t AdapterAtomicInc(OsalAtomic *v);
int32_t AdapterAtomicDec(OsalAtomic *v);

#endif  /* USB_DDK_LITEOS_ADAPTER_H */
