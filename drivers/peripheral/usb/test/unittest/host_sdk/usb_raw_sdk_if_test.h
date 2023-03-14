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

#ifndef USB_RAW_SDK_IF_TEST_H
#define USB_RAW_SDK_IF_TEST_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"
#include "usb_raw_api.h"
#include "data_fifo.h"

#define USB_MAX_INTERFACES     32
#define USB_CTRL_REQ_SIZE          64

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
} SerialOPCmd;

#define DATARATE                     9600
#define DATA_BITS_LENGTH             8
#define ACM_NW                       16
#define ACM_NR                       16
#define READ_BUF_SIZE                8192
#define USB_CTRL_SET_TIMEOUT         0
#define USB_PIPE_DIR_OFFSET          7

struct AcmDevice;
struct AcmWb {
    struct UsbRawRequest *request;
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
    struct UsbRawConfigDescriptor *config;
    struct AcmWb wb[ACM_NW];
    struct AcmRb rb[ACM_NR];
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    struct UsbRawRequest *notifyReq;
    struct UsbRawRequest *readReq[ACM_NR];
    struct UsbRawRequest *writeReq;
    struct UsbRawRequest *ctrlReq;
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
};

typedef void (*UsbRequestCallback)(struct UsbRequest *request);

typedef enum {
    /* select a configuration for the device */
    USB_REQUEST_SELECT_CONFIGURATION,
    /* select an alternate setting for an interface */
    USB_REQUEST_SELECT_INTERFACE,
    /* get the device descriptor from a specific USB device/interface/endpoint */
    USB_REQUEST_GET_DESCRIPTOR,
    /* set descriptor on a device/interface/endpoint */
    USB_REQUEST_SET_DESCRIPTOR,
    /* set a feature on a device/interface/endpoint/other */
    USB_REQUEST_SET_FEATURE,
    /* clear a USB-defined feature on a device/interface/endpoint/other */
    USB_REQUEST_CLEAR_FEATURE,
    /* get status form a USB device/interface/endpoint/other */
    USB_REQUEST_GET_STATUS,
    /* get the current configuration on a USB device */
    USB_REQUEST_GET_CONFIGURATION = 0x08,
    /* get the current setting for an interface */
    USB_REQUEST_GET_INTERFACE,
} UsbStandardRequest;

typedef enum {
    USB_REQUEST_TYPE_CONTROL = 0U,
    USB_REQUEST_TYPE_ISOCHRONOUS = 1U,
    USB_REQUEST_TYPE_BULK = 2U,
    USB_REQUEST_TYPE_INTERRUPT = 3U,
} UsbRequestType;

struct UsbIsoPacketDesc {
    unsigned int length;
    unsigned int actualLength;
    unsigned int status;
};

struct UsbAdapterUrb {
    unsigned char type;
    unsigned char endPoint;
    int32_t status;
    unsigned int flags;
    void *buffer;
    int32_t bufferLength;
    int32_t actualLength;
    int32_t startFrame;
    int32_t numberOfPackets;
    int32_t errorCount;
    unsigned int signr;
    void *userContext;
    struct UsbIsoPacketDesc isoFrameDesc[0];
};

/* Note:The first five fields of the definition must be consistent with the UsbRawRequest structure definition. */
struct UsbHostRequest {
    /* the address of data buffer */
    unsigned char *buffer;
    /* the length of data buffer */
    uint32_t length;
    /* the actual length of the requested data */
    int32_t actualLength;
    /* the status of the request */
    UsbRequestStatus status;
    /* private user data */
    void *userData;

    /* the following fields are not visible to user */
    struct UsbDeviceHandle *devHandle;
    UsbRawRequestCallback callback;
    UsbRequestCallback userCallback;
    /* the length of data buffer */
    int32_t bufLen;
    struct OsalSem sem;
    unsigned char endPoint;
    unsigned int timeout;
    unsigned char requestType;
    union {
        void *urbs;
        void **isoUrbs;
    };
    int32_t numUrbs;
    int32_t numRetired;
    UsbRequestStatus reqStatus;
    int32_t isoPacketOffset;
    struct OsalMutex lock;
    void *privateObj;
    int32_t numIsoPackets;
    /* isopacketdesc ptr */
    struct UsbIsoPacketDesc isoPacketDesc[0];
}__attribute__((aligned(4)));

/* Note:The first seven fields of this definition must be consistent with the UsbRawFillRequestData
  * structure definition.
  */
struct UsbFillRequestData {
    unsigned char endPoint;
    unsigned char *buffer;
    uint32_t length;
    int32_t numIsoPackets;
    UsbRawRequestCallback callback;
    void *userData;
    unsigned int timeout;
    UsbRequestCallback userCallback;
};

#endif /* USB_DDK_RAW_SDK_IF_TEST_H */
