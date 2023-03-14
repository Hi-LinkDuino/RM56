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

#ifndef HDF_USB_CDC_ETHER_H
#define HDF_USB_CDC_ETHER_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "usb_session.h"
#include "usb_interface_pool.h"
#include "data_fifo.h"

#define ECM_NW  16
#define ECM_NR  16
#define TARGET_MASK             0x3
#define REQUEST_TYPE_MASK       0x3
#define DIRECTION_MASK          0x1
#define USB_CTRL_SET_TIMEOUT    5000
#define READ_BUF_SIZE           8192
#define USB_MAX_INTERFACES      32
#define ETHER_SLEEP_TIME        100000
#define ETHER_SLEEP_MS_TIME     2

enum EcmCmd {
    CMD_ECM_OPEN = 0,
    CMD_ECM_CLOSE,
    CMD_ECM_READ,
    CMD_ECM_WRITE,
    CMD_ECM_GET_MAC,
    CMD_ECM_ADD_INTERFACE = 14,
    CMD_ECM_REMOVE_INTERFACE,
};

typedef enum {
    HOST_ECM_OPEN = 0,
    HOST_ECM_CLOSE,
    HOST_ECM_READ,
    HOST_ECM_WRITE,
    HOST_ECM_GET_MAC,
    HOST_ECM_ADD_INTERFACE = 16,
    HOST_ECM_REMOVE_INTERFACE,
} EcmModuleTestCmdType;

struct EcmDevice;
struct EcmWb {
    struct UsbRequest *request;
    struct EcmDevice *instance;
    uint8_t *buf;
    int32_t    len;
    int32_t    use;
    struct EcmDevice *ecm;
};

struct EcmRb {
    uint8_t *base;
    int32_t size;
    int32_t index;
    int32_t use;
    struct EcmDevice *instance;
};

struct EcmControlParams {
    uint8_t request;
    uint8_t requestType;
    uint16_t value;
    uint16_t index;
    void *data;
    uint16_t size;
};

struct EcmDevice {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct UsbInterface *ctrIface;
    struct UsbPipeInfo  *ctrPipe;
    struct UsbPipeInfo  *intPipe;
    struct UsbPipeInfo  *dataInPipe;
    struct UsbPipeInfo  *dataOutPipe;
    struct EcmWb wb[ECM_NW];
    struct EcmRb  rb[ECM_NR];
    struct UsbPipeInfo wPipeInfo;
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    struct UsbRequest *notifyReq;
    struct UsbRequest *readReq[ECM_NR];
    struct UsbRequest *writeReq;
    struct UsbRequest *ctrlReq;
    struct UsbInterface *itface;
    UsbInterfaceHandle *devHandle[USB_MAX_INTERFACES];
    UsbInterfaceHandle *ctrDevHandle;
    struct UsbSession *session;
    struct DataFifo readFifo;
    uint32_t nbIndex;
    uint32_t nbSize;
    int32_t transmitting;
    int32_t ctrlReqNum;
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t interfaceCnt;
    uint8_t *notificationBuffer;
    uint8_t interfaceIndex[USB_MAX_INTERFACES];
    struct UsbInterface *iface[USB_MAX_INTERFACES];
    uint32_t ctrlSize;
    uint32_t intSize;
    uint32_t writeSize;
    uint32_t readSize;
    struct UsbCdcLineCoding   lineCoding;
    bool   openFlag;
    uint32_t readReqNum;
    uint32_t writeReqNum;
    bool initFlag;
};

#endif
