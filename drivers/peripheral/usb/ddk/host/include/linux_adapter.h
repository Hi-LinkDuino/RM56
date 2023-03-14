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

#ifndef USB_HOST_LINUX_ADAPTER_H
#define USB_HOST_LINUX_ADAPTER_H

#include "usb_raw_api_library.h"

#if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
#include <sys/syscall.h>
static inline pid_t gettid()
{
    return syscall(SYS_gettid);
}
#endif

#define USBDEV_PATH     "/dev"
#define USB_DEV_FS_PATH "/dev/bus/usb"

#define BUS_OFFSET      8

#define MAX_BULK_DATA_BUFFER_LENGTH     4096
#define MAX_BULK_URBS_PER_REQUEST       1

#define MAX_ISO_PACKETS_PER_URB         128
#define MAX_ISO_DATA_BUFFER_LEN         (8 * 1024)
#define MAX_ISO_URBS_PER_REQUEST        (((MAX_ISO_DATA_BUFFER_LEN - 1) / MAX_ISO_PACKETS_PER_URB) + 1)

#define URBS_PER_REQUEST                MAX(MAX_BULK_URBS_PER_REQUEST, MAX_ISO_URBS_PER_REQUEST)

#define MAX_ISO_SHARE_MEMERY_SIZE       sizeof(struct UsbHostRequest) + \
    (URBS_PER_REQUEST * sizeof(struct UsbAdapterUrb)) + MAX_ISO_DATA_BUFFER_LEN
#define MAX_BULK_SHARE_MEMERY_SIZE      sizeof(struct UsbHostRequest) + \
    (URBS_PER_REQUEST * sizeof(struct UsbAdapterUrb)) + MAX_BULK_DATA_BUFFER_LENGTH
#define MAX_CTRL_BUFFER_LENGTH  4096

#define USB_ADAPTER_URB_TYPE_ISO                0
#define USB_ADAPTER_URB_TYPE_INTERRUPT          1
#define USB_ADAPTER_URB_TYPE_CONTROL            2
#define USB_ADAPTER_URB_TYPE_BULK               3
#define USB_ADAPTER_URB_SHORT_NOT_OK            0x01
#define USB_ADAPTER_URB_ISO_ASAP                0x02
#define USB_ADAPTER_URB_BULK_CONTINUATION       0x04
#define USB_ADAPTER_URB_QUEUE_BULK              0x10
#define USB_ADAPTER_URB_ZERO_PACKET             0x40
#define USB_ADAPTER_CAP_ZERO_PACKET             0x01
#define USB_ADAPTER_CAP_BULK_CONTINUATION       0x02
#define USB_ADAPTER_CAP_NO_PACKET_SIZE_LIM      0x04
#define USB_ADAPTER_CAP_BULK_SCATTER_GATHER     0x08
#define USB_ADAPTER_CAP_REAP_AFTER_DISCONNECT   0x10

#define USBDEVFS_CONTROL            _IOWR('U', 0, struct UsbControlRequestData)
#define USBDEVFS_SETINTERFACE       _IOR('U', 4, struct UsbAdapterSetInterface)
#define USBDEVFS_SETCONFIGURATION   _IOR('U', 5, unsigned int)
#define USBDEVFS_SUBMITURB          _IOR('U', 10, struct UsbAdapterUrb)
#define USBDEVFS_DISCARDURB         _IO('U', 11)
#define USBDEVFS_REAPURB            _IOW('U', 12, void *)
#define USBDEVFS_CLAIMINTERFACE     _IOR('U', 15, unsigned int)
#define USBDEVFS_RELEASEINTERFACE   _IOR('U', 16, unsigned int)
#define USBDEVFS_RESET              _IO('U', 20)
#define USBDEVFS_CLEAR_HALT         _IOR('U', 21, unsigned int)
#define USBDEVFS_GET_CAPABILITIES   _IOR('U', 26, unsigned int)
#define USBDEVFS_ALLOC_STREAMS      _IOR('U', 28, struct UsbAdapterStreams)
#define USBDEVFS_FREE_STREAMS       _IOR('U', 29, struct UsbAdapterStreams)

struct UsbAdapterSetInterface {
    unsigned int interface;
    unsigned int altSetting;
};

struct UsbAdapterStreams {
    unsigned int numStreams;
    unsigned int numEps;
    unsigned char eps[0];
};

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
    int32_t (*cancelRequest)(struct UsbHostRequest *request);
    int32_t (*urbCompleteHandle)(const struct UsbDeviceHandle *devHandle);
};

struct UsbOsAdapterOps *UsbAdapterGetOps(void);
UsbRawTidType UsbAdapterGetTid(void);
int32_t UsbAdapterRegisterSignal(void);
int32_t UsbAdapterKillSignal(struct UsbDeviceHandle *devHandle, UsbRawTidType tid);
int32_t AdapterAtomicInc(OsalAtomic *v);
int32_t AdapterAtomicDec(OsalAtomic *v);

#endif /* USB_HOST_LINUX_ADAPTER_H */
