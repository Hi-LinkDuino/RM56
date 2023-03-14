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

#ifndef HDF_USB_SERIAL_H
#define HDF_USB_SERIAL_H

#include "data_fifo.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "usb_ddk.h"
#include "usb_ddk_interface.h"

#define USB_MAX_INTERFACES      32
#define DATARATE                9600
#define DATA_BITS_LENGTH        8
#define ACM_NW                  16
#define ACM_NR                  16
#define READ_BUF_SIZE           8192
#define DIRECTION_MASK          0x1
#define USB_CTRL_SET_TIMEOUT    5000
#define USB_PIPE_DIR_OFFSET     7

typedef enum {
    CMD_OPEN_PARM = 0,
    CMD_CLOSE_PARM,
    CMD_WRITE_PARM,
    CMD_READ_PARM,
    CMD_GET_BAUDRATE,
    CMD_SET_BAUDRATE,
    CMD_WRITE_DATA_SYNC,
    CMD_READ_DATA_SYNC,
    CMD_CLASS_CTRL_SYNC,
    CMD_STD_CTRL_GET_DESCRIPTOR_CMD,
    CMD_STD_CTRL_GET_STATUS_CMD,
    CMD_STD_CTRL_GET_CONFIGURATION,
    CMD_STD_CTRL_GET_INTERFACE,
    CMD_STD_CTRL_GET_DESCRIPTOR_ASYNC,
    CMD_ADD_INTERFACE,
    CMD_REMOVE_INTERFACE,
} SerialOPCmd;

typedef enum {
    HOST_ACM_SYNC_READ = 1,
    HOST_ACM_SYNC_WRITE,
    HOST_ACM_ASYNC_READ,
    HOST_ACM_ASYNC_WRITE,
    HOST_ACM_CTRL_READ,
    HOST_ACM_CTRL_WRITE,
    HOST_ACM_CTRL_CLASS_SYNC,
    HOST_ACM_CTRL_GET_STATUS,
    HOST_ACM_CTRL_SYNC_DESCRIPTOR,
    HOST_ACM_CTRL_ASYNC_DESCRIPTOR,
    HOST_ACM_CTRL_GET_CONFIGURATION,
    HOST_ACM_CTRL_GET_INTERFACE,
    HOST_ACM_SPEED_TEST,
    HOST_ACM_SET_BAUDRATE,
    HOST_ACM_GET_BAUDRATE,
    HOST_ACM_ADD_INTERFACE,
    HOST_ACM_REMOVE_INTERFACE,
} AcmModuleTestCmdType;

struct AcmDevice;

struct AcmWb {
    struct UsbRequest *request;
    struct AcmDevice *instance;
    uint8_t *buf;
    uint32_t len;
    int32_t use;
};

struct AcmRb {
    uint8_t *base;
    int32_t size;
    int32_t index;
    int32_t use;
    struct AcmDevice *instance;
};

struct SerialDevice {
    struct AcmDevice *acm;
    struct UsbCdcLineCoding lineCoding;
    struct OsalMutex lock;
    struct DataFifo readFifo;
};

struct AcmDevice {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    struct UsbInterface *ctrIface;
    struct UsbPipeInfo  *ctrPipe;
    struct UsbPipeInfo  *intPipe;
    struct UsbPipeInfo  *dataInPipe;
    struct UsbPipeInfo  *dataOutPipe;
    struct AcmWb wb[ACM_NW];
    struct AcmRb rb[ACM_NR];
    struct UsbPipeInfo wPipeInfo;
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    struct UsbRequest *notifyReq;
    struct UsbRequest *readReq[ACM_NR];
    struct UsbRequest *writeReq;
    struct UsbRequest *ctrlReq;
    struct OsalMutex lock;
    struct UsbInterface *itface;
    UsbInterfaceHandle *devHandle[USB_MAX_INTERFACES];
    UsbInterfaceHandle *ctrDevHandle;
    struct UsbSession *session;
    struct SerialDevice  *port;
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
    struct UsbCdcLineCoding lineCoding;
    bool initFlag;
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
#endif
