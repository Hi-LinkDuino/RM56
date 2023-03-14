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

#ifndef U_ECM_H
#define U_ECM_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "osal_mutex.h"
#include "usb_ddk.h"
#include "usb_object.h"
#include "usbfn_request.h"
#include "data_fifo.h"

enum EcmNotifyState {
    ECM_NOTIFY_NONE,        /* don't notify */
    ECM_NOTIFY_CONNECT,     /* issue CONNECT next */
    ECM_NOTIFY_SPEED,       /* issue SPEED_CHANGE next */
};

enum UsbEcmCmd {
    USB_ECM_OPEN = 0,
    USB_ECM_CLOSE,
    USB_ECM_READ,
    USB_ECM_WRITE,
    USB_ECM_INIT = 100,
    USB_ECM_RELEASE = 101,
};

struct UsbEcm {
    struct UsbEcmDevice         *ecm;
    struct OsalMutex            lock;

    struct DListHead            readPool;
    struct DListHead            readQueue;
    int32_t                     readStarted;
    int32_t                     readAllocated;
    struct DataFifo             readFifo;
    struct DListHead            writePool;
    int32_t                     writeStarted;
    int32_t                     writeAllocated;
    struct DataFifo             writeFifo;
    bool                        writeBusy;
    struct OsalMutex            lockRW;
    struct OsalMutex            lockReadFifo;
    struct OsalMutex            lockWriteFifo;
    int32_t                         refCount;
};

struct UsbEcmPipe {
    uint8_t                     id;
    uint16_t                    maxPacketSize;
    struct UsbFnInterface       *ctrlIface;
};

struct UsbEcmInterface {
    struct UsbFnInterface       *fn;
    UsbFnInterfaceHandle        handle;
};

struct UsbEcmDevice {
    struct IDeviceIoService     service;
    struct HdfDeviceObject      *device;
    struct UsbFnDevice          *fnDev;
    struct UsbEcmInterface      ctrlIface;
    struct UsbEcmInterface      dataIface;
    struct UsbEcmPipe           notifyPipe;
    struct UsbEcmPipe           dataInPipe;
    struct UsbEcmPipe           dataOutPipe;
    uint8_t ctrlId;
    uint8_t dataId;
    struct OsalMutex            lock;
    char                        *udcName;
    struct UsbEcm            *port;

    struct UsbFnRequest         *notifyReq;
    struct UsbFnRequest         *ep0Req;
    uint8_t              notifyState;
    bool                isOpen;
    bool                initFlag;
};

#endif /* U_ECM_H */
