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

#ifndef USB_INTERFACE_POOL_H
#define USB_INTERFACE_POOL_H

#include "usb_session.h"
#include "usb_ddk_device.h"
#include "usb_ddk_interface.h"
#include "usb_ddk_request.h"
#include "usb_raw_api_library.h"

#define INTERFACE_POOL_ID_MAX   (128)
#define INTERFACE_REFCOUNT_UNFORCE  (1)

typedef enum {
    USB_PIPE_INDEX_TYPE,
    USB_PIPE_DIRECTION_TYPE
} UsbPipeQueryParaType;

typedef enum {
    USB_INTERFACE_INTERFACE_INDEX_TYPE,
    USB_INTERFACE_ALT_SETTINGS_TYPE
} UsbInterfaceQueryParaType;

typedef enum {
    USB_POOL_NORMAL_TYPE,
    USB_POOL_OBJECT_ID_TYPE
} UsbPoolQueryParaType;

typedef enum {
    USB_POOL_PROCESS_RUNNING,
    USB_POOL_PROCESS_STOP,
    USB_POOL_PROCESS_STOPED
} UsbPoolProcessStatusType;

struct UsbInterfaceHandleEntity {
    struct UsbDeviceHandle *devHandle;
    uint8_t interfaceIndex;
};
struct UsbSdkInterface {
    struct UsbInterface interface;
    int32_t parentObjectId;
    struct DListHead pipeList;
    struct OsalMutex listLock;
    UsbInterfaceStatus status;
    uint8_t altSettingId;
    struct UsbSession *session;
    OsalAtomic refCount;
};

struct UsbInterfacePool {
    struct UsbObject object;
    struct UsbSession *session;
    struct OsalMutex mutex;
    struct DListHead interfaceList;
    struct OsalMutex interfaceLock;
    OsalAtomic refCount;
    uint8_t busNum;
    uint8_t devAddr;
    OsalAtomic ioRefCount;
    struct OsalThread ioSendProcess;
    struct OsalThread ioAsyncReceiveProcess;
    struct UsbMessageQueue submitRequestQueue;
    UsbRawTidType ioProcessTid;
    UsbPoolProcessStatusType ioProcessStopStatus;
    struct OsalMutex ioStopLock;
    struct UsbDevice *device;
};

struct UsbPipeQueryPara {
    UsbPipeQueryParaType type;
    union {
        uint8_t pipeId;
        UsbPipeDirection pipeDirection;
    };
};

struct UsbInterfaceQueryPara {
    UsbInterfaceQueryParaType type;
    uint8_t interfaceIndex;
    uint8_t altSettingId;
};

struct UsbPoolQueryPara {
    UsbPoolQueryParaType type;
    union {
        struct {
            uint8_t busNum;
            uint8_t usbAddr;
        };
        int32_t objectId;
    };
};

struct UsbIfRequest {
    struct UsbRequest request;
    struct UsbHostRequest *hostRequest;
    bool isSyncReq;
}__attribute__((aligned(4)));

int32_t UsbIfCreatPipeObj(const struct UsbSdkInterface *interfaceObj, struct UsbPipe **pipeObj);
int32_t UsbIfCreatInterfaceObj(const struct UsbInterfacePool *interfacePool, struct UsbSdkInterface **interfaceObj);
HDF_STATUS UsbIfDestroyInterfaceObj(
    const struct UsbInterfacePool *interfacePool, const struct UsbSdkInterface *interfaceObj);
int32_t UsbIfCreatInterfacePool(const struct UsbSession *session, uint8_t busNum, uint8_t devAddr,
    struct UsbInterfacePool **interfacePool);

#endif /* USB_INTERFACE_POOL_H */
