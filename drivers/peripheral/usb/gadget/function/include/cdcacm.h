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

#ifndef HDF_USB_CDCACM_H
#define HDF_USB_CDCACM_H

#include "data_fifo.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "osal_mutex.h"
#include "usbfn_request.h"
#include "usb_ddk.h"
#include "usb_object.h"

#define USB_MAX_PACKET_SIZE     0x40

enum UsbSerialCmd {
    USB_SERIAL_OPEN = 0,
    USB_SERIAL_CLOSE,
    USB_SERIAL_READ,
    USB_SERIAL_WRITE,
    USB_SERIAL_GET_BAUDRATE,
    USB_SERIAL_SET_BAUDRATE,
    USB_SERIAL_SET_PROP,
    USB_SERIAL_GET_PROP,
    USB_SERIAL_REGIST_PROP,
    USB_SERIAL_WRITE_SPEED,
    USB_SERIAL_WRITE_GET_TEMP_SPEED,
    USB_SERIAL_WRITE_SPEED_DONE,
    USB_SERIAL_WRITE_GET_TEMP_SPEED_UINT32,
    USB_SERIAL_READ_SPEED,
    USB_SERIAL_READ_GET_TEMP_SPEED,
    USB_SERIAL_READ_SPEED_DONE,
    USB_SERIAL_READ_GET_TEMP_SPEED_UINT32,
    USB_SERIAL_INIT = 100,
    USB_SERIAL_RELEASE = 101,
};

struct UsbSerial {
    struct UsbAcmDevice         *acm;
    struct UsbCdcLineCoding     lineCoding;
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

    bool                        suspended;
    bool                        startDelayed;
    int32_t                         refCount;
};

struct AcmNotifyMethod {
    void (*Connect)(struct UsbAcmDevice *acm);
    void (*Disconnect)(struct UsbAcmDevice *acm);
    int32_t (*SendBreak)(struct UsbAcmDevice *acm, int32_t duration);
};

struct UsbAcmPipe {
    uint8_t                     id;
    uint16_t                    maxPacketSize;
    struct UsbFnInterface       *ctrlIface;
};

struct UsbAcmInterface {
    struct UsbFnInterface       *fn;
    UsbFnInterfaceHandle        handle;
};

struct UsbAcmDevice {
    struct IDeviceIoService     service;
    struct HdfDeviceObject      *device;
    struct UsbFnDevice          *fnDev;
    struct UsbAcmInterface      ctrlIface;
    struct UsbAcmInterface      dataIface;
    struct UsbAcmPipe           notifyPipe;
    struct UsbAcmPipe           dataInPipe;
    struct UsbAcmPipe           dataOutPipe;

    struct DListHead            ctrlPool;
    int32_t                     ctrlReqNum;
    struct UsbFnRequest         *notifyReq;
    struct OsalMutex            lock;
    bool                        pending;
    bool                        initFlag;
    uint32_t                    enableEvtCnt;
    char                        *udcName;
    struct UsbSerial            *port;
    struct UsbCdcLineCoding     lineCoding;
    uint16_t                    serialState;
#define SERIAL_STATE_DCD        (1 << 0)
#define SERIAL_STATE_DSR        (1 << 1)
#define SERIAL_STATE_BREAK      (1 << 2)
#define SERIAL_STATE_RING       (1 << 3)
#define SERIAL_STATE_FRAMING    (1 << 4)
#define SERIAL_STATE_PARITY     (1 << 5)
#define SERIAL_STATE_OVERRUN    (1 << 6)

    uint16_t                    handshakeBits;
    /* notification callbacks */
    struct AcmNotifyMethod      *notify;
};

struct CtrlInfo {
    uint8_t                     request;
    struct UsbAcmDevice         *acm;
};

#endif /* HDF_USB_CDCACM_H */
