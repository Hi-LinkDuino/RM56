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

#ifndef USB_HOST_SDK_RAW_SPEED_H
#define USB_HOST_SDK_RAW_SPEED_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "usb_raw_api.h"
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
#define USB_CTRL_SET_TIMEOUT    0
#define USB_PIPE_DIR_OFFSET     7

typedef enum {
    TEST_ZERO_TYPE = 0,
    TEST_ONE_TYPE,
    TEST_TWO_TYPE,
    TEST_THREE_TYPE,
    TEST_FOUR_TYPE,
    TEST_FIVE_TYPE,
    TEST_SIX_TYPE,
} TestCountType;

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

struct AcmDevice;
struct AcmDb {
    struct UsbRawRequest *request;
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

struct UsbEndpoint {
    uint8_t addr;
    uint8_t interval;
    uint16_t maxPacketSize;
};

struct AcmDevice {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    uint8_t ctrlIface;
    uint8_t dataIface;
    struct UsbEndpoint *notifyEp;
    struct UsbEndpoint *dataInEp;
    struct UsbEndpoint *dataOutEp;
    struct UsbEndpoint *dataEp;
    struct UsbRawConfigDescriptor *config;
    struct AcmDb db[TEST_CYCLE];
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    struct UsbRawRequest *notifyReq;
    struct UsbRawRequest *readReq[ACM_NR];
    struct UsbRawRequest *writeReq;
    struct UsbRawRequest *ctrlReq;
    uint32_t dataSize;
    struct OsalMutex lock;
    UsbRawHandle *devHandle;
    struct UsbSession *session;
    struct SerialDevice *port;
    uint32_t nbIndex;
    uint32_t nbSize;
    int32_t transmitting;
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t interfaceCnt;
    uint8_t *notificationBuffer;
    uint8_t interfaceIndex[USB_MAX_INTERFACES];
    struct UsbCdcLineCoding lineCoding;
    struct OsalThread ioThread;
    struct OsalThread ioSendThread;
};

#endif /* USB_HOST_SDK_RAW_SPEED_H */
