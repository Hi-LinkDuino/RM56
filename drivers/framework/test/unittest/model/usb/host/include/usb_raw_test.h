/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef USB_RAW_TEST_H
#define USB_RAW_TEST_H

#include "data_fifo.h"
#include "hdf_device_desc.h"
#include "platform_if.h"
#include "implementation/global_implementation.h"
#include "usb_ddk_interface.h"
#include "usb_raw_api.h"

enum UsbRawTestCmd {
    USB_RAW_SDK_IF_START_IO,
    USB_RAW_SDK_IF_INIT_001_TEST,
    USB_RAW_SDK_IF_EXIT_001_TEST,
    USB_RAW_SDK_IF_INIT_002_TEST,
    USB_RAW_SDK_IF_EXIT_002_TEST,
    USB_RAW_SDK_IF_INIT_003_TEST,
    USB_RAW_SDK_IF_INIT_004_TEST,
    USB_RAW_SDK_IF_INIT_005_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_001_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_002_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_003_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_004_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_005_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_006_TEST,
    USB_RAW_SDK_IF_RESET_DEVICE_001_TEST,
    USB_RAW_SDK_IF_RESET_DEVICE_002_TEST,
    USB_RAW_SDK_IF_CLOSE_DEVICE_001_TEST,
    USB_RAW_SDK_IF_CLOSE_DEVICE_002_TEST,
    USB_RAW_SDK_IF_OPEN_DEVICE_007_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_001_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_002_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_003_TEST,
    USB_RAW_SDK_IF_GET_CONFIGURATION_004_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_001_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_002_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_001_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_002_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_003_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_004_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_001_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_002_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_003_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_004_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_005_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_006_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_007_TEST,
    USB_RAW_SDK_IF_SET_CONFIGURATION_008_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_001_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_002_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_003_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_004_TEST,
    USB_RAW_SDK_IF_GET_CONFIG_DESC_005_TEST,
    USB_RAW_SDK_IF_GET_DEVICE_DESC_005_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_001_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_002_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_003_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_004_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_005_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_001_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_002_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_003_TEST,
    USB_RAW_SDK_IF_RELEASE_INTERFACE_004_TEST,
    USB_RAW_SDK_IF_CLAMIN_INTERFACE_006_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_001_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_002_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_003_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_004_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_005_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_006_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_007_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_008_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_010_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_011_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_006_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_005_TEST,
    USB_RAW_SDK_IF_FILL_ISO_REQUEST_006_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FREE_REQUEST_005_TEST,
    USB_RAW_SDK_IF_ALLOC_REQUEST_009_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_001_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_002_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_003_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_004_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_005_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_006_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_007_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_008_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_009_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_010_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_011_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_012_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_013_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_014_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_015_TEST,
    USB_RAW_SDK_IF_GET_DESCRIPTION_016_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_001_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_002_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_005_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_006_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_007_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_REQUEST_008_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_001_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_002_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_003_TEST,
    USB_RAW_SDK_IF_FILL_CONTROL_SETUP_004_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_004_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_005_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_006_TEST,
    USB_RAW_SDK_IF_SEND_CONTROL_REQUEST_007_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_004_TEST,
    USB_RAW_SDK_IF_SEND_BULK_REQUEST_005_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SEND_INT_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_003_TEST,
    USB_RAW_SDK_IF_FILL_BULK_REQUEST_004_TEST,
    USB_RAW_SDK_IF_FILL_INT_REQUEST_005_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_001_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_002_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_003_TEST,
    USB_RAW_SDK_IF_SUBMIT_REQUEST_004_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_001_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_002_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_003_TEST,
    USB_RAW_SDK_IF_CANCEL_REQUEST_004_TEST,
    USB_RAW_SDK_IF_STOP_IO,
};

#define USB_MAX_INTERFACES      32
#define USB_CTRL_REQ_SIZE       256

#define DATARATE                9600
#define DATA_BITS_LENGTH        8
#define ACM_NW                  16
#define ACM_NR                  16
#define READ_BUF_SIZE           8192
#define USB_CTRL_SET_TIMEOUT    5000
#define USB_PIPE_DIR_OFFSET     7
#define USB_DEVICE_VENDOR_ID    0x12D1
#define USB_DEVICE_PRODUCT_ID   0x5000
#define USB_MAX_BYTE            255
#define BITS_PER_WORD   10
#define MAX_SPEED_HZ    10000000
#define TEST_SLEEP_TIME 500
#define USB_LOOP_NUM    100
#define USB_DESC_TYPE   2
#define USB_ISO_PACKAT_CNT 16

#define USB_RAW_REQUEST_TIME_ZERO_MS    0
#define USB_RAW_IO_SLEEP_MS_TIME        500
#define USB_IO_THREAD_STACK_SIZE        8192

#define USB_BUFFER_MAX_SIZE     256

struct AcmRawDevice;
struct RawWb {
    struct UsbRawRequest *request;
    struct AcmRawDevice *instance;
    uint8_t *buf;
    uint32_t len;
    int32_t use;
};

struct RawRb {
    uint8_t *base;
    int32_t size;
    int32_t index;
    int32_t use;
    struct AcmRawDevice *instance;
};

struct SerialRawDevice {
    struct AcmRawDevice *acm;
    struct UsbCdcLineCoding lineCoding;
    struct OsalMutex lock;
    struct DataFifo readFifo;
};

struct UsbEndpoint {
    uint8_t addr;
    uint8_t interval;
    uint16_t maxPacketSize;
};

struct AcmRawDevice {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    uint8_t ctrlIface;
    uint8_t dataIface;
    struct UsbEndpoint notifyEp;
    struct UsbEndpoint dataInEp;
    struct UsbEndpoint dataOutEp;
    struct UsbEndpoint isoEp;
    struct UsbRawConfigDescriptor *config;
    struct RawWb wb[ACM_NW];
    struct RawRb rb[ACM_NR];
    struct OsalMutex writeLock;
    struct OsalMutex readLock;
    struct UsbRawRequest *notifyReq;
    struct UsbRawRequest *readReq[ACM_NR];
    struct UsbRawRequest *writeReq;
    struct UsbRawRequest *ctrlReq;
    struct UsbRawRequest *isoReq;
    struct OsalMutex lock;
    UsbRawHandle *devHandle;
    struct UsbSession *session;
    struct SerialRawDevice *port;
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
    /* get the device descriptor from a specific USB device/interface/endpoint  */
    USB_REQUEST_GET_DESCRIPTOR,
    /* set descriptor on a device/interface/endpoint  */
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

struct UsbRawTestFunc {
    enum UsbRawTestCmd type;
    int32_t (*Func)(void);
};

struct AcmRawDevice *UsbGetIoAcm(void);
int32_t UsbParseConfigDescriptor(struct AcmRawDevice *acm, struct UsbRawConfigDescriptor *config);
int32_t AcmWriteBufAlloc(struct AcmRawDevice *acm);
void AcmNotifyReqCallback(const void *requestArg);
void AcmReadBulkCallback(const void *requestArg);
void AcmWriteIsoCallback(const void *requestArg);
void AcmWriteBulkCallback(const void *requestArg);
int32_t UsbStopIo(void);
int32_t UsbStartIo(void);
int32_t UsbIoThread(void *data);
void AcmWriteBufFree(struct AcmRawDevice *acm);
void AcmCtrlReqCallback(const void *requestArg);
int32_t CheckRawSdkIfInit001(void);
int32_t CheckRawSdkIfExit001(void);
int32_t CheckRawSdkIfInit002(void);
int32_t CheckRawSdkIfExit002(void);
int32_t CheckRawSdkIfInit003(void);
int32_t CheckRawSdkIfInit004(void);
int32_t CheckRawSdkIfInit005(void);
int32_t CheckRawSdkIfOpenDevice001(void);
int32_t CheckRawSdkIfOpenDevice002(void);
int32_t CheckRawSdkIfOpenDevice003(void);
int32_t CheckRawSdkIfOpenDevice004(void);
int32_t CheckRawSdkIfOpenDevice005(void);
int32_t CheckRawSdkIfOpenDevice006(void);
int32_t CheckRawSdkIfResetDevice001(void);
int32_t CheckRawSdkIfResetDevice002(void);
int32_t CheckRawSdkIfCloseDevice001(void);
int32_t CheckRawSdkIfCloseDevice002(void);
int32_t CheckRawSdkIfOpenDevice007(void);
int32_t CheckRawSdkIfGetConfiguration001(void);
int32_t CheckRawSdkIfGetConfiguration002(void);
int32_t CheckRawSdkIfGetConfiguration003(void);
int32_t CheckRawSdkIfGetConfiguration004(void);
int32_t CheckRawSdkIfGetDevice001(void);
int32_t CheckRawSdkIfGetDevice002(void);
int32_t CheckRawSdkIfGetConfigDescriptor001(void);
int32_t CheckRawSdkIfGetConfigDescriptor002(void);
int32_t CheckRawSdkIfGetConfigDescriptor003(void);
int32_t CheckRawSdkIfGetConfigDescriptor004(void);
int32_t CheckRawSdkIfSetConfiguration001(void);
int32_t CheckRawSdkIfSetConfiguration002(void);
int32_t CheckRawSdkIfSetConfiguration003(void);
int32_t CheckRawSdkIfSetConfiguration004(void);
int32_t CheckRawSdkIfSetConfiguration005(void);
int32_t CheckRawSdkIfSetConfiguration006(void);
int32_t CheckRawSdkIfSetConfiguration007(void);
int32_t CheckRawSdkIfSetConfiguration008(void);
int32_t CheckRawSdkIfGetDeviceDescriptor001(void);
int32_t CheckRawSdkIfGetDeviceDescriptor002(void);
int32_t CheckRawSdkIfGetDeviceDescriptor003(void);
int32_t CheckRawSdkIfGetDeviceDescriptor004(void);
int32_t CheckRawSdkIfGetConfigDescriptor005(void);
int32_t CheckRawSdkIfGetDeviceDescriptor005(void);
int32_t CheckRawSdkIfClaimInterface001(void);
int32_t CheckRawSdkIfClaimInterface002(void);
int32_t CheckRawSdkIfClaimInterface003(void);
int32_t CheckRawSdkIfClaimInterface004(void);
int32_t CheckRawSdkIfClaimInterface005(void);
int32_t CheckRawSdkIfReleaseInterface001(void);
int32_t CheckRawSdkIfReleaseInterface002(void);
int32_t CheckRawSdkIfReleaseInterface003(void);
int32_t CheckRawSdkIfReleaseInterface004(void);
int32_t CheckRawSdkIfClaimInterface006(void);
int32_t CheckRawSdkIfAllocRequest001(void);
int32_t CheckRawSdkIfAllocRequest002(void);
int32_t CheckRawSdkIfAllocRequest003(void);
int32_t CheckRawSdkIfAllocRequest004(void);
int32_t CheckRawSdkIfAllocRequest005(void);
int32_t CheckRawSdkIfAllocRequest006(void);
int32_t CheckRawSdkIfAllocRequest007(void);
int32_t CheckRawSdkIfAllocRequest008(void);
int32_t CheckRawSdkIfAllocRequest010(void);
int32_t CheckRawSdkIfAllocRequest011(void);
int32_t CheckRawSdkIfFreeRequest006(void);
int32_t CheckRawSdkIfFillIsoRequest001(void);
int32_t CheckRawSdkIfFillIsoRequest002(void);
int32_t CheckRawSdkIfFillIsoRequest003(void);
int32_t CheckRawSdkIfFillIsoRequest004(void);
int32_t CheckRawSdkIfFillIsoRequest005(void);
int32_t CheckRawSdkIfFillIsoRequest006(void);
int32_t CheckRawSdkIfFreeRequest001(void);
int32_t CheckRawSdkIfFreeRequest002(void);
int32_t CheckRawSdkIfFreeRequest003(void);
int32_t CheckRawSdkIfFreeRequest004(void);
int32_t CheckRawSdkIfFreeRequest005(void);
int32_t CheckRawSdkIfAllocRequest009(void);
int32_t CheckRawSdkIfGetDescriptor001(void);
int32_t CheckRawSdkIfGetDescriptor002(void);
int32_t CheckRawSdkIfGetDescriptor003(void);
int32_t CheckRawSdkIfGetDescriptor004(void);
int32_t CheckRawSdkIfGetDescriptor005(void);
int32_t CheckRawSdkIfGetDescriptor006(void);
int32_t CheckRawSdkIfGetDescriptor007(void);
int32_t CheckRawSdkIfGetDescriptor008(void);
int32_t CheckRawSdkIfGetDescriptor009(void);
int32_t CheckRawSdkIfGetDescriptor010(void);
int32_t CheckRawSdkIfGetDescriptor011(void);
int32_t CheckRawSdkIfGetDescriptor012(void);
int32_t CheckRawSdkIfGetDescriptor013(void);
int32_t CheckRawSdkIfGetDescriptor014(void);
int32_t CheckRawSdkIfGetDescriptor015(void);
int32_t CheckRawSdkIfGetDescriptor016(void);
int32_t CheckRawSdkIfFillBulkRequest001(void);
int32_t CheckRawSdkIfFillBulkRequest002(void);
int32_t CheckRawSdkIfFillInterruptRequest001(void);
int32_t CheckRawSdkIfFillInterruptRequest002(void);
int32_t CheckRawSdkIfFillInterruptRequest003(void);
int32_t CheckRawSdkIfFillInterruptRequest004(void);
int32_t CheckRawSdkIfFillControlRequest001(void);
int32_t CheckRawSdkIfFillControlRequest002(void);
int32_t CheckRawSdkIfFillControlRequest003(void);
int32_t CheckRawSdkIfFillControlRequest004(void);
int32_t CheckRawSdkIfFillControlRequest005(void);
int32_t CheckRawSdkIfFillControlRequest006(void);
int32_t CheckRawSdkIfFillControlRequest007(void);
int32_t CheckRawSdkIfFillControlRequest008(void);
int32_t CheckRawSdkIfFillControlSetup001(void);
int32_t CheckRawSdkIfFillControlSetup002(void);
int32_t CheckRawSdkIfFillControlSetup003(void);
int32_t CheckRawSdkIfFillControlSetup004(void);
int32_t CheckRawSdkIfSendControlRequest001(void);
int32_t CheckRawSdkIfSendControlRequest002(void);
int32_t CheckRawSdkIfSendControlRequest003(void);
int32_t CheckRawSdkIfSendControlRequest004(void);
int32_t CheckRawSdkIfSendControlRequest005(void);
int32_t CheckRawSdkIfSendControlRequest006(void);
int32_t CheckRawSdkIfSendControlRequest007(void);
int32_t CheckRawSdkIfSendBulkRequest001(void);
int32_t CheckRawSdkIfSendBulkRequest002(void);
int32_t CheckRawSdkIfSendBulkRequest003(void);
int32_t CheckRawSdkIfSendBulkRequest004(void);
int32_t CheckRawSdkIfSendBulkRequest005(void);
int32_t CheckRawSdkIfSendInterruptRequest001(void);
int32_t CheckRawSdkIfSendInterruptRequest002(void);
int32_t CheckRawSdkIfSendInterruptRequest003(void);
int32_t CheckRawSdkIfSendInterruptRequest004(void);
int32_t CheckRawSdkIfFillBulkRequest003(void);
int32_t CheckRawSdkIfFillBulkRequest004(void);
int32_t CheckRawSdkIfFillInterruptRequest005(void);
int32_t CheckRawSdkIfSubmitRequest001(void);
int32_t CheckRawSdkIfSubmitRequest002(void);
int32_t CheckRawSdkIfSubmitRequest003(void);
int32_t CheckRawSdkIfSubmitRequest004(void);
int32_t CheckRawSdkIfCancelRequest001(void);
int32_t CheckRawSdkIfCancelRequest002(void);
int32_t CheckRawSdkIfCancelRequest003(void);
int32_t CheckRawSdkIfCancelRequest004(void);

int32_t UsbRawTestEntry(int32_t cmd);

#endif /* USB_TEST_H */
