/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef USB_TEST_H
#define USB_TEST_H

#include "data_fifo.h"
#include "hdf_device_desc.h"
#include "platform_if.h"
#include "implementation/global_implementation.h"
#include "usb_ddk.h"
#include "usb_ddk_interface.h"

enum UsbTestCmd {
    USB_WRITE_TEST = 0,
    USB_HOSTSDK_INIT_001_TEST,
    Usb_HOSTSDK_EXIT_001_TEST,
    USB_HOSTSDK_INIT_002_TEST,
    USB_HOSTSDK_EXIT_002_TEST,
    USB_HOSTSDK_INIT_003_TEST,
    USB_HOSTSDK_INIT_004_TEST,
    USB_HOSTSDK_INIT_005_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_001_TEST,
    USB_HOSTSDK_RELEASE_INTERFACE_001_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_002_TEST,
    USB_HOSTSDK_RELEASE_INTERFACE_002_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_003_TEST,
    USB_HOSTSDK_RELEASE_INTERFACE_003_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_004_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_005_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_006_TEST,
    USB_HOSTSDK_OPEN_INTERFACE_001_TEST,
    USB_HOSTSDK_CLOSE_INTERFACE_001_TEST,
    USB_HOSTSDK_OPEN_INTERFACE_002_TEST,
    USB_HOSTSDK_CLOSE_INTERFACE_002_TEST,
    USB_HOSTSDK_OPEN_INTERFACE_003_TEST,
    USB_HOSTSDK_CLOSE_INTERFACE_003_TEST,
    USB_HOSTSDK_OPEN_INTERFACE_004_TEST,
    USB_HOSTSDK_OPEN_INTERFACE_005_TEST,
    USB_HOSTSDK_CLOSE_INTERFACE_005_TEST,
    USB_HOSTSDK_OPEN_INTERFACE_006_TEST,
    USB_HOSTSDK_SELECT_INTERFACE_001_TEST,
    USB_HOSTSDK_SELECT_INTERFACE_002_TEST,
    USB_HOSTSDK_SELECT_INTERFACE_003_TEST,
    USB_HOSTSDK_SELECT_INTERFACE_004_TEST,
    USB_HOSTSDK_SELECT_INTERFACE_005_TEST,
    USB_HOSTSDK_SELECT_INTERFACE_006_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_007_TEST,
    USB_HOSTSDK_CLAIM_INTERFACE_008_TEST,
    USB_HOSTSDK_GET_PIPE_001_TEST,
    USB_HOSTSDK_GET_PIPE_002_TEST,
    USB_HOSTSDK_GET_PIPE_003_TEST,
    USB_HOSTSDK_GET_PIPE_004_TEST,
    USB_HOSTSDK_GET_PIPE_005_TEST,
    USB_HOSTSDK_GET_PIPE_006_TEST,
    USB_HOSTSDK_GET_PIPE_007_TEST,
    USB_HOSTSDK_GET_PIPE_008_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_001_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_002_TEST,
    USB_HOSTSDK_FREE_REQUEST_001_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_003_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_004_TEST,
    USB_HOSTSDK_FREE_REQUEST_002_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_005_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_006_TEST,
    USB_HOSTSDK_FREE_REQUEST_003_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_007_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_008_TEST,

    USB_HOSTSDK_ALLOC_REQUEST_010_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_011_TEST,
    USB_HOSTSDK_FREE_REQUEST_006_TEST,
    USB_HOSTSDK_FILL_ISO_REQUEST_001_TEST,
    USB_HOSTSDK_FILL_ISO_REQUEST_002_TEST,
    USB_HOSTSDK_FILL_ISO_REQUEST_003_TEST,
    USB_HOSTSDK_FILL_ISO_REQUEST_004_TEST,
    USB_HOSTSDK_FILL_ISO_REQUEST_005_TEST,
    USB_HOSTSDK_FILL_ISO_REQUEST_006_TEST,

    USB_HOSTSDK_FREE_REQUEST_004_TEST,
    USB_HOSTSDK_ALLOC_REQUEST_009_TEST,
    USB_HOSTSDK_FILL_REQUEST_001_TEST,
    USB_HOSTSDK_FILL_REQUEST_002_TEST,
    USB_HOSTSDK_FILL_REQUEST_003_TEST,
    USB_HOSTSDK_FILL_REQUEST_004_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_SYNC_001_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_SYNC_002_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_SYNC_003_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_SYNC_004_TEST,
    USB_HOSTSDK_FILL_REQUEST_005_TEST,
    USB_HOSTSDK_FILL_REQUEST_006_TEST,
    USB_HOSTSDK_FILL_REQUEST_007_TEST,
    USB_HOSTSDK_FILL_REQUEST_008_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_001_TEST,
    USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_001_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_002_TEST,
    USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_002_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_003_TEST,
    USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_003_TEST,
    USB_HOSTSDK_SUBMIT_REQUEST_ASYNC_004_TEST,
    USB_HOSTSDK_SUBMIT_CANCEL_REQUEST_004_TEST,
    USB_HOSTSDK_CLEAR_INTERFACE_HALT_002_TEST,
    USB_HOSTSDK_CLEAR_INTERFACE_HALT_003_TEST,
    USB_HOSTSDK_CLEAR_INTERFACE_HALT_004_TEST,
    USB_HOSTSDK_REMOVE_INTERFACE_001_TEST,
    USB_HOSTSDK_ADD_INTERFACE_001_TEST,
    USB_HOSTSDK_REMOVE_INTERFACE_002_TEST,
    USB_HOSTSDK_ADD_INTERFACE_002_TEST,
    USB_HOSTSDK_REMOVE_INTERFACE_003_TEST,
    USB_HOSTSDK_ADD_INTERFACE_003_TEST,
    USB_HOSTSDK_CLOSE_INTERFACE_006_TEST,
};

#define TARGET_MASK             0x3
#define REQUEST_TYPE_MASK       0x3

#define DATARATE                9600
#define DATA_BITS_LENGTH        8
#define ACM_NW                  16
#define ACM_NR                  16
#define READ_BUF_SIZE           8192
#define DIRECTION_MASK          0x1
#define USB_CTRL_SET_TIMEOUT    5000
#define USB_PIPE_DIR_OFFSET     7
#define USB_DEVICE_VENDOR_ID    0x12D1
#define USB_DEVICE_PRODUCT_ID   0x5000
#define USB_MAX_BYTE            255

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
    struct UsbInterface *intIface;
    struct UsbInterface *dataIface;
    struct UsbPipeInfo *ctrPipe;
    struct UsbPipeInfo *intPipe;
    struct UsbPipeInfo *dataInPipe;
    struct UsbPipeInfo *dataOutPipe;
    struct UsbPipeInfo *isoPipe;
    struct AcmWb wb[ACM_NW];
    struct AcmRb rb[ACM_NR];
    struct UsbPipeInfo wPipeInfo;
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    uint8_t *notificationBuffer;
    struct UsbRequest *notifyReq;
    struct UsbRequest *readReq[ACM_NR];
    struct UsbRequest *writeReq;
    struct UsbRequest *ctrlReq;
    struct UsbRequest *isoReq;
    struct OsalMutex lock;
    struct UsbInterface *itface;
    UsbInterfaceHandle *devHandle;
    UsbInterfaceHandle *data_devHandle;
    UsbInterfaceHandle *ctrl_devHandle;
    UsbInterfaceHandle *int_devHandle;
    UsbInterfaceHandle *iso_devHandle;
    struct UsbSession *session;
    struct SerialDevice *port;
    uint32_t nbIndex;
    uint32_t nbSize;
    int32_t transmitting;
    int32_t ctrlReqNum;
    uint8_t busNum;
    uint8_t devAddr;
    uint8_t interfaceIndex;
    uint32_t ctrlSize;
    uint32_t intSize;
    uint32_t writeSize;
    uint32_t readSize;
    struct UsbCdcLineCoding lineCoding;
};

struct UsbTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct UsbTest *test, int32_t cmd);
    uint32_t port;
    uint32_t len;
    uint8_t *wbuf;
    uint8_t *rbuf;
    DevHandle handle;
};

struct TestControlMsgData {
    uint8_t request;
    uint8_t requestType;
    uint16_t value;
    uint16_t index;
    void *data;
    uint16_t size;
};


struct UsbTestFunc {
    enum UsbTestCmd type;
    int32_t (*Func)(void);
};

#define BITS_PER_WORD   10
#define MAX_SPEED_HZ    10000000
#define USB_LOOP_NUM    100
#define USB_ISO_PACKAT_CNT 16

#define USB_RAW_REQUEST_TIME_ZERO_MS 0

int32_t UsbTestEntry(int32_t cmd);
#endif /* USB_TEST_H */
