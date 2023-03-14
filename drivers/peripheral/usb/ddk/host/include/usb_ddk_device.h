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

#ifndef USB_DDK_DEVICE_H
#define USB_DDK_DEVICE_H

#include "usb_ddk.h"
#include "usb_session.h"

#define USB_MAXENDPOINTS    32
#define USB_MAXALTSETTING   128

struct UsbDeviceConfigDescriptor {
    struct UsbConfigDescriptor *desc;
    size_t actualLen;
};

struct UsbDevice {
    struct HdfSListNode list;
    OsalAtomic refcnt;
    struct UsbSession *session;
    struct UsbDevice *parentDev;
    struct UsbDeviceHandle *devHandle;
    uint8_t busNum;
    uint8_t portNum;
    uint8_t devAddr;
    enum UsbDeviceSpeed speed;
    struct UsbDeviceDescriptor deviceDescriptor;
    void *descriptors;
    size_t descriptorsLength;
    uint8_t activeConfig;
    struct UsbDeviceConfigDescriptor *configDescriptors;
    struct OsalMutex requestLock;
    struct HdfSList requestList;
    void *privateObject;
    void *privateData;
};

struct UsbDeviceHandle {
    struct OsalMutex lock;
    uint64_t claimedInterfaces;
    struct UsbDevice *dev;
    struct UsbSession *session;
    uint32_t caps;
    int32_t fd;
};

#endif /* USB_DDK_DEVICE_H */
