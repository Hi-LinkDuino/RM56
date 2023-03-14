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

#ifndef USB_HOST_SDK_API_SPEED_H
#define USB_HOST_SDK_API_SPEED_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "usb_ddk.h"
#include "usb_ddk_interface.h"
#include "usb_session.h"
#include "data_fifo.h"

#define TEST_LENGTH             512
#define TEST_CYCLE              30
#define TEST_TIME               0xffffffff
#define TEST_PRINT_TIME         2
#define TEST_PRINT_TIME_UINT    1000
#define TEST_RECV_COUNT         10000
#define TEST_BYTE_COUNT         1024
#define TEST_FLOAT_COUNT        (1.0)
#define TEST_SLEEP_TIME         10

#define TEST_WRITE              true
#define TEST_READ               false
#define USB_MAX_INTERFACES      32
#define DATARATE                9600
#define DATA_BITS_LENGTH        8
#define ACM_NW                  30
#define ACM_NR                  30
#define READ_BUF_SIZE           8192
#define DIRECTION_MASK          0x1
#define USB_CTRL_SET_TIMEOUT    0
#define USB_PIPE_DIR_OFFSET     7
#define USB_DEVICE_VENDOR_ID 0x12D1
#define USB_DEVICE_PRODUCT_ID 0x5000

typedef enum {
    TEST_ZERO_TYPE = 0,
    TEST_ONE_TYPE,
    TEST_TWO_TYPE,
    TEST_THREE_TYPE,
    TEST_FOUR_TYPE,
    TEST_FIVE_TYPE,
    TEST_SIX_TYPE,
} TestCountType;

enum UsbSerialCmd {
    USB_SERIAL_OPEN = 0,
    USB_SERIAL_CLOSE,
    USB_SERIAL_SPEED,
};
struct UsbSpeedTest {
    int32_t busNum;
    int32_t devAddr;
    int32_t ifaceNum;
    int32_t writeOrRead;
    bool printData;
    int32_t paramNum;
};

struct AcmDevice;
struct AcmDb {
    int32_t dbNum;
    struct UsbRequest *request;
    struct AcmDevice *instance;
    uint8_t *buf;
    int32_t len;
    int32_t use;
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
    struct UsbPipeInfo  *dataPipe;
    struct AcmDb  db[TEST_CYCLE];
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
    struct SerialDevice *port;
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
    uint32_t dataSize;
    struct UsbCdcLineCoding lineCoding;
    bool busy;
};

#endif  /* USB_HOST_SDK_API_SPEED_H */
