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

#ifndef USBD_H
#define USBD_H

#include "data_fifo.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "osal_sem.h"
#include "usb_ddk.h"
#include "usb_ddk_interface.h"
#include "usb_session.h"
#include "usbd_publisher.h"
#include "usbd_type.h"

#define USB_MAX_INTERFACES 32
#define DIRECTION_MASK 0x1
#define USB_CTRL_SET_TIMEOUT 5000
#define USB_PIPE_DIR_OFFSET 7

struct UsbdService;

#define USBD_BULKASYNCREQ_NUM_MAX 64

struct UsbdBulkASyncReqList;
struct UsbdBulkASyncList;

struct UsbdBufferHandle {
    int32_t fd;   /**< buffer fd, -1 if not supported */
    int32_t size; /* < size of memory */
    uint8_t *starAddr;
    int32_t cur;
    int32_t rcur;
    uint8_t cbflg;
    struct OsalMutex lock;
};

struct UsbdBulkASyncReqNode {
    struct DListHead node;
    struct UsbRequest *request;
    struct UsbdBulkASyncReqList *list;
    int32_t use;
    int32_t id;
};

struct UsbdBulkASyncReqList {
    struct UsbdBulkASyncReqNode node[USBD_BULKASYNCREQ_NUM_MAX];
    struct UsbdBulkASyncList *pList;
    struct DListHead eList;
    struct DListHead uList;
    struct OsalMutex elock;
    struct OsalMutex ulock;
};

struct UsbdBulkASyncList {
    struct HostDevice *instance;
    struct UsbdBulkASyncList *next;
    UsbInterfaceHandle *ifHandle;
    struct HdfRemoteService *cb;
    struct UsbdBulkASyncReqList rList;
    struct UsbPipeInfo pipe;
    struct UsbRequestParams params;
    struct UsbdBufferHandle asmHandle;
    uint8_t ifId;
    uint8_t epId;
};

struct HostDevice {
    struct HdfSListNode node;
    struct UsbdService *service;
    struct DataFifo readFifo;
    struct HdfSList requestQueue;
    struct OsalMutex requestLock;
    uint8_t interfaceIndex[USB_MAX_INTERFACES];
    uint8_t interfaceCnt;
    struct UsbInterface *iface[USB_MAX_INTERFACES];
    UsbInterfaceHandle *ctrDevHandle;
    UsbInterfaceHandle *devHandle[USB_MAX_INTERFACES];
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    struct OsalMutex lock;
    struct UsbRequest *ctrlReq;
    struct UsbInterface *ctrIface;
    struct UsbPipeInfo *ctrPipe;
    uint8_t busNum;
    uint8_t devAddr;
    bool initFlag;
    struct HdfSList reqSyncList;
    struct OsalMutex reqSyncLock;
    struct HdfSList reqASyncList;
    struct OsalMutex reqASyncLock;
    struct UsbdBulkASyncList *bulkASyncList;
};

struct RequestMsg {
    struct UsbRequest *request;
    void *clientData;
    uint32_t clientLength;
    void *buffer;
    uint32_t length;
};

struct UsbControlParams {
    uint8_t request;
    UsbRequestTargetType target;
    UsbControlRequestType reqType;
    UsbRequestDirection directon;
    uint16_t value;
    uint16_t index;
    void *data;
    uint16_t size;
};

struct UsbDescriptorParams {
    UsbInterfaceHandle *devHandle;
    struct UsbRequest *request;
    uint8_t type;
    uint8_t index;
    void *buf;
    uint16_t size;
};

struct UsbdService {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct UsbdSubscriber *subscriber;
    struct UsbSession *session;
    struct HdfSList devList;
    struct OsalMutex lock;
};

struct UsbdRequestSync {
    struct HdfSListNode node;
    struct UsbRequest *request;
    UsbInterfaceHandle *ifHandle;
    struct OsalMutex lock;
    struct UsbPipeInfo pipe;
    struct UsbRequestParams params;
    uint8_t endPointAddr;
};

struct UsbdRequestASync {
    struct HdfSListNode node;
    struct HdfSListNode qNode;
    UsbInterfaceHandle *ifHandle;
    struct RequestMsg reqMsg;
    struct OsalMutex lock;
    struct UsbPipeInfo pipe;
    struct UsbRequestParams params;
    uint8_t endPointAddr;
    uint8_t status;
};

struct UsbdSubscriber;
int32_t BindUsbSubscriber(struct UsbdService *service, struct UsbdSubscriber *subscriber);
int32_t UnbindUsbSubscriber(struct UsbdService *service);

#endif // USBD_H
