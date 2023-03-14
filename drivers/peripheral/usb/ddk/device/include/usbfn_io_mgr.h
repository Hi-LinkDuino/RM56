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

#ifndef USBFN_IO_MGR_H
#define USBFN_IO_MGR_H

#include "usb_object.h"
#include "device_resource_if.h"
#include "usbfn_device.h"
#include "usbfn_request.h"
#include "usbfn_interface.h"
#include "adapter_if.h"
#include "osal_thread.h"

#define HDF_PROCESS_STACK_SIZE 10000

struct ReqList {
    struct UsbFnRequest req;
    struct DListHead    entry;
    uint8_t             pipe;
    int32_t                 fd;
    uintptr_t           buf;
    uint32_t            bufLen;
    struct UsbHandleMgr *handle;
};

struct UsbHandleMgr {
    uint32_t                 numFd;
    int32_t                      fds[MAX_EP];
    struct UsbFnInterfaceMgr *intfMgr;
    struct DListHead         reqEntry;
    struct UsbFnReqEvent     *reqEvent[MAX_EP];
};

struct UsbFnFuncMgr {
    const struct UsbObject *object;
    int32_t                    fd;
    char                   name[MAX_NAMELEN];
    UsbFnEventCallback     callback;
    uint32_t               eventMask;
    void                   *context;
    struct DListHead        reqEntry;
};

struct UsbFnInterfaceMgr {
    struct UsbFnInterface  interface;
    struct UsbFnFuncMgr    *funcMgr;
    struct UsbHandleMgr    *handle;
    uint8_t                startEpId;
    bool                   isOpen;
};

struct UsbFnRequest *UsbFnIoMgrRequestAlloc(struct UsbHandleMgr *handle, uint8_t pipe, uint32_t len);
int32_t UsbFnIoMgrRequestFree(struct UsbFnRequest *req);
int32_t UsbFnIoMgrRequestGetStatus(struct UsbFnRequest *req, UsbRequestStatus *status);
int32_t UsbFnIoMgrRequestSubmitAsync(struct UsbFnRequest *req);
int32_t UsbFnIoMgrRequestSubmitSync(struct UsbFnRequest *req, uint32_t timeout);
int32_t UsbFnIoMgrRequestCancel(struct UsbFnRequest *req);
struct UsbHandleMgr *UsbFnIoMgrInterfaceOpen(struct UsbFnInterface *interface);
int32_t UsbFnIoMgrInterfaceClose(struct UsbHandleMgr *handle);
int32_t UsbFnIoMgrInterfaceGetPipeInfo(struct UsbFnInterface *interface, uint8_t pipeId, struct UsbFnPipeInfo *info);
int32_t OpenEp0AndMapAddr(struct UsbFnFuncMgr *funcMgr);
#endif /* USBFN_IO_MGR_H */

