/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef USB_DEVICE_LITE_CDCACM_TEST_H
#define USB_DEVICE_LITE_CDCACM_TEST_H

#include <unistd.h>
#include <stdlib.h>
#include "usbfn_device.h"
#include "usbfn_request.h"
#include "usbfn_interface.h"
#include "osal_atomic.h"
#include "osal_mutex.h"
#include "osal_time.h"

#define TEST_TIMES  10
#define WAIT_TIMES  60
#define BUFFER_LEN 64
#define WAIT_100MS 100
#define SYNC_5000MS 5000
#define CDC_ACM
#define QUEUE_SIZE      8
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

#define REQUEST_ALLOC_PIPE      ((uint8_t)(0x20))
#define REQUEST_ALLOC_LENGTH    ((uint32_t)(0x0800))

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
    UsbFnInterfaceHandle        handle;
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
    bool                        connect;
    bool                        havedSubmit;
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

struct AcmDevice *SetUpAcmDevice(void);
void ReleaseAcmDevice(struct AcmDevice *acm);
void AcmEventCallback(struct UsbFnEvent *event);
void AcmDeviceRelease(struct AcmDevice *acmDevice);
int32_t remove_usb_device(void);

struct AcmDevice *UsbGetAcmDevice(void);
int32_t UsbFnDviceTestCreate(void);
int32_t UsbFnDviceTestCreate002(void);
int32_t UsbFnDviceTestCreate003(void);
int32_t UsbFnDviceTestCreate004(void);
int32_t UsbFnDviceTestCreate005(void);
int32_t UsbFnDviceTestCreate006(void);
int32_t UsbFnDviceTestStatus(void);
int32_t UsbFnDviceTestStatus002(void);
int32_t UsbFnDviceTestStatus003(void);
int32_t UsbFnDviceTestStatus004(void);
int32_t UsbFnDviceTestStatus005(void);
int32_t UsbFnDviceTestGetDevice(void);
int32_t UsbFnDviceTestGetDevice002(void);
int32_t UsbFnDviceTestGetDevice003(void);
int32_t UsbFnDviceTestGetDevice004(void);
int32_t UsbFnDviceTestGetDevice005(void);
int32_t UsbFnDviceTestGetDevice006(void);
int32_t UsbFnDviceTestGetInterface(void);
int32_t UsbFnDviceTestGetInterface002(void);
int32_t UsbFnDviceTestGetInterface003(void);
int32_t UsbFnDviceTestGetInterface004(void);
int32_t UsbFnDviceTestGetInterface005(void);
int32_t UsbFnDviceTestGetInterface006(void);
int32_t UsbFnDviceTestGetPipeInfo(void);
int32_t UsbFnDviceTestGetPipeInfo002(void);
int32_t UsbFnDviceTestGetPipeInfo003(void);
int32_t UsbFnDviceTestGetPipeInfo004(void);
int32_t UsbFnDviceTestGetPipeInfo005(void);
int32_t UsbFnDviceTestGetPipeInfo006(void);
int32_t UsbFnDviceTestRequestAsync(void);
int32_t UsbFnDviceTestRequestAsync002(void);
int32_t UsbFnDviceTestRequestAsync003(void);
int32_t UsbFnDviceTestRequestAsync004(void);
int32_t UsbFnDviceTestRequestAsync005(void);
int32_t UsbFnDviceTestRequestAsync006(void);
int32_t UsbFnDviceTestRequestSync(void);
int32_t UsbFnDviceTestRequestSync002(void);
int32_t UsbFnDviceTestRequestSync003(void);
int32_t UsbFnDviceTestRequestSync004(void);
int32_t UsbFnDviceTestRequestSync005(void);
int32_t UsbFnDviceTestRequestSync006(void);
int32_t UsbFnDviceTestRequestSync007(void);
int32_t UsbFnDviceTestRegistProp(void);
int32_t UsbFnDviceTestRegistProp002(void);
int32_t UsbFnDviceTestRegistProp003(void);
int32_t UsbFnDviceTestRegistProp004(void);
int32_t UsbFnDviceTestRegistProp005(void);
int32_t UsbFnDviceTestRegistProp006(void);
int32_t UsbFnDviceTestRegistProp007(void);
int32_t UsbFnDviceTestGetProp(void);
int32_t UsbFnDviceTestGetProp002(void);
int32_t UsbFnDviceTestGetProp003(void);
int32_t UsbFnDviceTestGetProp004(void);
int32_t UsbFnDviceTestGetProp005(void);
int32_t UsbFnDviceTestGetProp006(void);
int32_t UsbFnDviceTestGetProp007(void);
int32_t UsbFnDviceTestGetProp008(void);
int32_t UsbFnDviceTestSetProp(void);
int32_t UsbFnDviceTestSetProp002(void);
int32_t UsbFnDviceTestSetProp003(void);
int32_t UsbFnDviceTestSetProp004(void);
int32_t UsbFnDviceTestSetProp005(void);
int32_t UsbFnDviceTestSetProp006(void);
int32_t UsbFnDviceTestSetProp007(void);
int32_t UsbFnDviceTestAllocCtrlRequest(void);
int32_t UsbFnDviceTestAllocCtrlRequest002(void);
int32_t UsbFnDviceTestAllocCtrlRequest003(void);
int32_t UsbFnDviceTestAllocCtrlRequest004(void);
int32_t UsbFnDviceTestAllocCtrlRequest005(void);
int32_t UsbFnDviceTestAllocCtrlRequest006(void);
int32_t UsbFnDviceTestAllocCtrlRequest007(void);
int32_t UsbFnDviceTestAllocCtrlRequest008(void);
int32_t UsbFnDviceTestAllocRequest(void);
int32_t UsbFnDviceTestAllocRequest002(void);
int32_t UsbFnDviceTestAllocRequest003(void);
int32_t UsbFnDviceTestAllocRequest004(void);
int32_t UsbFnDviceTestAllocRequest005(void);
int32_t UsbFnDviceTestAllocRequest006(void);
int32_t UsbFnDviceTestAllocRequest007(void);
int32_t UsbFnDviceTestAllocRequest008(void);
int32_t UsbFnDviceTestAllocRequest009(void);
int32_t UsbFnDviceTestFreeRequest(void);
int32_t UsbFnDviceTestFreeRequest002(void);
int32_t UsbFnDviceTestFreeRequest003(void);
int32_t UsbFnDviceTestFreeRequest004(void);
int32_t UsbFnDviceTestFreeRequest005(void);
int32_t UsbFnDviceTestFreeRequest006(void);
int32_t UsbFnDviceTestGetRequestStatus(void);
int32_t UsbFnDviceTestGetRequestStatus002(void);
int32_t UsbFnDviceTestGetRequestStatus003(void);
int32_t UsbFnDviceTestGetRequestStatus004(void);
int32_t UsbFnDviceTestGetRequestStatus005(void);
int32_t UsbFnDviceTestGetRequestStatus006(void);
int32_t UsbFnDviceTestCancelRequest(void);
int32_t UsbFnDviceTestCancelRequest002(void);
int32_t UsbFnDviceTestCancelRequest003(void);
int32_t UsbFnDviceTestCancelRequest004(void);
int32_t UsbFnDviceTestCancelRequest005(void);
int32_t UsbFnDviceTestCancelRequest006(void);
int32_t UsbFnDviceTestStopReceEvent(void);
int32_t UsbFnDviceTestStopReceEvent002(void);
int32_t UsbFnDviceTestStopReceEvent003(void);
int32_t UsbFnDviceTestStopReceEvent004(void);
int32_t UsbFnDviceTestStartReceEvent(void);
int32_t UsbFnDviceTestStartReceEvent002(void);
int32_t UsbFnDviceTestStartReceEvent003(void);
int32_t UsbFnDviceTestStartReceEvent004(void);
int32_t UsbFnDviceTestStartReceEvent005(void);
int32_t UsbFnDviceTestStartReceEvent006(void);
int32_t UsbFnDviceTestStartReceEvent007(void);
int32_t UsbFnDviceTestCloseInterface(void);
int32_t UsbFnDviceTestCloseInterface002(void);
int32_t UsbFnDviceTestCloseInterface003(void);
int32_t UsbFnDviceTestCloseInterface004(void);
int32_t UsbFnDviceTestOpenInterface(void);
int32_t UsbFnDviceTestOpenInterface002(void);
int32_t UsbFnDviceTestOpenInterface003(void);
int32_t UsbFnDviceTestOpenInterface004(void);
int32_t UsbFnDviceTestOpenInterface005(void);
int32_t UsbFnDviceTestRemove(void);
int32_t UsbFnDviceTestRemove002(void);

#endif
