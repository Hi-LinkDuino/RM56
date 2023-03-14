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

#ifndef USBFN_DEV_MGR_H
#define USBFN_DEV_MGR_H

#include "usb_object.h"
#include "device_resource_if.h"
#include "usbfn_device.h"
#include "usbfn_request.h"
#include "usbfn_interface.h"
#include "usbfn_io_mgr.h"
#include "adapter_if.h"

struct UsbFnDeviceMgr {
    struct UsbFnDevice         fnDev;
    uint8_t                    numFunc;
    struct UsbFnFuncMgr        *funcMgr;
    char                       name[MAX_NAMELEN];
    char                       udcName[MAX_NAMELEN];
    UsbFnDeviceState           devState;
    struct UsbFnInterfaceMgr   *interfaceMgr;
    const struct DeviceResourceNode  *node;
    struct UsbFnDeviceDesc     *des;
    struct OsalThread          thread;
    bool                       running;
};

const struct UsbFnDeviceMgr *UsbFnMgrDeviceCreate(const char *udcName,
    struct UsbFnDeviceDesc *des, const struct DeviceResourceNode *node);
int32_t UsbFnMgrDeviceRemove(struct UsbFnDevice *fnDevice);
const struct UsbFnDeviceMgr *UsbFnMgrDeviceGet(const char *udcName);
int32_t UsbFnMgrDeviceGetState(struct UsbFnDevice *fnDevice, UsbFnDeviceState *devState);
const struct UsbFnInterfaceMgr *UsbFnMgrDeviceGetInterface(struct UsbFnDevice *fnDevice, uint8_t interfaceIndex);
int32_t UsbFnMgrStartRecvEvent(struct UsbFnInterface *interface, uint32_t eventMask,
    UsbFnEventCallback callback, void *context);
int32_t UsbFnStopRecvEvent(struct UsbFnInterface *interface);

#endif /* USBFN_DEV_MGR_H */
