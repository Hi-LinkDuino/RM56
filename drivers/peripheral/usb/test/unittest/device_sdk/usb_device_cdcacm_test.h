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

#ifndef USB_CDCACM_TEST_H
#define USB_CDCACM_TEST_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "osal_atomic.h"
#include "osal_mutex.h"
#include "osal_time.h"

#include "usbfn_device.h"
#include "usbfn_request.h"
#include "usbfn_interface.h"
#define DEV_MASTER_SERVICE_NAME "usbfn_master"
#define TEST_TIMES  10
#define BUFFER_LEN 64
#define WAIT_10MS 10
#define SYNC_5000MS 500
#define CDC_ACM
#define QUEUE_SIZE              8
#define PORT_RATE       9600
#define DATA_BIT        8

#define SS_MAX_PACKET_SIZE 1024
#define MAX_PACKET_SIZE 512
#define EP_ADD_NOTIFY   1
#define EP_ADD_DATA_IN  2
#define EP_ADD_DATA_OUT 3
#define DATA_EP_NUM     2
#define NOTIFY_EP_NUM   1
#define INTF_COUNT      2

#define ACM_NOTIFY_INTERVAL     32 /* ms */
#define ACM_HS_NOTIFY_INTERVAL  9  /* ms */
#define ACM_NOTIFY_MAXPACKET    10 /* notification + 2 bytes */

#define ACM_CTRL_IDX            1
#define ACM_DATA_IDX            2
#define ACM_IAD_IDX             3


struct Serial {
    struct AcmDevice         *acm;
    struct UsbCdcLineCoding     lineCoding;
    struct OsalMutex            lock;
    struct DListHead            readPool;
    struct DListHead            readQueue;
    int32_t                     readStarted;
    int32_t                     readAllocated;
    struct DListHead            writePool;
    int32_t                     writeStarted;
    int32_t                     writeAllocated;
    bool                        writeBusy;

    bool                        suspended;
    bool                        startDelayed;
    int32_t                         refCount;
};

struct AcmNotifyMethod {
    void (*Connect)(struct AcmDevice *acm);
    void (*Disconnect)(struct AcmDevice *acm);
    int32_t (*SendBreak)(struct AcmDevice *acm, int32_t duration);
};

struct AcmPipe {
    uint8_t                     id;
    uint16_t                    maxPacketSize;
    struct UsbFnInterface       *ctrlIface;
};

struct AcmInterface {
    struct UsbFnInterface       *fn;
    UsbFnInterfaceHandle                   handle;
};

struct AcmDevice {
    struct UsbFnDevice          *fnDev;
    struct AcmInterface      ctrlIface;
    struct AcmInterface      dataIface;
    struct AcmPipe           notifyPipe;
    struct AcmPipe           dataInPipe;
    struct AcmPipe           dataOutPipe;
    struct DListHead            ctrlPool;
    int32_t                     ctrlReqNum;
    struct UsbFnRequest         *notifyReq;
    struct OsalMutex            lock;
    bool                        pending;
    uint32_t                    enableEvtCnt;
    char                        *udcName;
    char                        submit;
    char                        submitExit;
    struct Serial               *port;
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
    uint8_t      request;
    struct AcmDevice         *acm;
};

extern struct UsbFnDeviceDesc g_acmFnDevice;
struct AcmDevice * SetUpAcmDevice(void);
void ReleaseAcmDevice(struct AcmDevice *acm);
void AcmEventCallback(struct UsbFnEvent *event);
void AcmDeviceRelease(struct AcmDevice *acmDevice);

#endif
