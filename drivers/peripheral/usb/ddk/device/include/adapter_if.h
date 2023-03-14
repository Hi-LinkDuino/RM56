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

#ifndef USBFN_ADAPTER_H
#define USBFN_ADAPTER_H

#include <stdbool.h>
#include <sys/mman.h>
#include "hdf_slist.h"
#include "hdf_dlist.h"
#include "osal_mutex.h"
#include "osal_file.h"
#include "osal_mem.h"
#include "securec.h"
#include "hdf_log.h"
#include "usbfn_interface.h"
#include "usbfn_device.h"
#include "usb_ddk.h"

#define FCONFIG_IOC_MAGIC                'c'
#define FCONFIG_CMD_MAKE_GADGET          _IO(FCONFIG_IOC_MAGIC, 1)
#define FCONFIG_CMD_DROP_GADGET          _IO(FCONFIG_IOC_MAGIC, 2)
#define FCONFIG_CMD_WRITE_DEV_DESC       _IO(FCONFIG_IOC_MAGIC, 3)
#define FCONFIG_CMD_ADD_CONFIG           _IO(FCONFIG_IOC_MAGIC, 4)
#define FCONFIG_CMD_REMOVE_CONFIG        _IO(FCONFIG_IOC_MAGIC, 5)
#define FCONFIG_CMD_WRITE_STRINGS        _IO(FCONFIG_IOC_MAGIC, 6)
#define FCONFIG_CMD_MAKE_FUNCTION        _IO(FCONFIG_IOC_MAGIC, 7)
#define FCONFIG_CMD_DROP_FUNCTION        _IO(FCONFIG_IOC_MAGIC, 8)
#define FCONFIG_CMD_ENABLE_UDC           _IO(FCONFIG_IOC_MAGIC, 9)
#define FCONFIG_CMD_DISABLE_UDC          _IO(FCONFIG_IOC_MAGIC, 10)
#define FCONFIG_CMD_CHAGE_DEVINFO        _IO(FCONFIG_IOC_MAGIC, 11)
#define FCONFIG_CMD_CHAGE_DEVSTRING      _IO(FCONFIG_IOC_MAGIC, 12)

#define GENERIC_IOC_MAGIC               'g'
#define GENERIC_CMD_FREE_MEM            _IO(GENERIC_IOC_MAGIC, 1)
#define GENERIC_CMD_CANCEL_REQUEST      _IO(GENERIC_IOC_MAGIC, 2)
#define GENERIC_CMD_GET_PIPE_INFO       _IO(GENERIC_IOC_MAGIC, 3)

#define GENERIC_CMD_GET_EP0_EVENT       _IO(GENERIC_IOC_MAGIC, 4)
#define GENERIC_CMD_ENDPOINT_IO         _IO(GENERIC_IOC_MAGIC, 5)
#define GENERIC_CMD_GET_REQ_STATUS      _IO(GENERIC_IOC_MAGIC, 6)

struct FconfigString {
    uint32_t                      len;
    char                          *s;
};

struct FconfigUsbString {
    uint8_t                       id;
    struct FconfigString         str;
};

struct FconfigDevStrings {
    struct FconfigString    gadgetName;
    uint16_t                language;
    uint32_t                strCount;
    struct FconfigUsbString *strings;
} __attribute__((packed));

struct FconfigDevDesc {
    struct FconfigString        gadgetName;
    struct UsbDeviceDescriptor  devDesc;
} __attribute__((packed));

struct FconfigCfgDesc {
    struct FconfigString        gadgetName;
    struct FconfigString        configName;
    struct UsbConfigDescriptor  cfgDesc;
} __attribute__((packed));

struct FconfigFuncInfo {
    struct FconfigString        gadgetName;
    struct FconfigString        configName;
    struct FconfigString        funcName;
};

struct FconfigUdcInfo {
    struct FconfigString      gadgetName;
    struct FconfigString      udcName;
};

struct FconfigProp {
    const char  *propName;
    uint16_t    propValue;
};

struct FconfigPropSting {
    uint16_t    lang;
    const char  *propName;
    const char  *propValue;
};

struct FconfigDevdescInfo {
    struct FconfigString      gadgetName;
    struct FconfigProp        Prop;
};

struct FconfigDevDescString {
    struct FconfigString      gadgetName;
    struct FconfigPropSting   Prop;
};

struct FconfigPollFd {
    int32_t fd;
    uint32_t revents;
    int32_t events;
};

#define FUNCTIONFS_ENDPOINT_DESC             _IOR('g', 130, struct UsbEndpointDescriptor)

#define FUNCTIONFS_NEWFN                     _IOW('g', 60, struct FuncNew)
#define FUNCTIONFS_DELFN                     _IOW('g', 61, struct FuncNew)
#define FUNCTIONFS_ENDPOINT_GET_REQ_STATUS   _IOW('g', 48, struct IoData)
#define FUNCTIONFS_ENDPOINT_WRITE            _IOW('g', 49, struct IoData)
#define FUNCTIONFS_ENDPOINT_READ             _IOW('g', 50, struct IoData)
#define FUNCTIONFS_ENDPOINT_RW_CANCEL        _IOW('g', 51, struct IoData)
#define FUNCTIONFS_ENDPOINT_QUEUE_INIT       _IO('g', 52)
#define FUNCTIONFS_ENDPOINT_QUEUE_DEL        _IO('g', 53)
#define FUNCTIONFS_ENDPOINT_RELEASE_BUF      _IOR('g', 54, struct GenericMemory)
#define FUNCTIONFS_ENDPOINT_GET_EP0_EVENT    _IOR('g', 56, struct UsbFnReqEvent)

#define FUNCTION_GENERIC "f_generic"
#define FUNCTION_GENERIC_ECM "f_generic.e"
#define FUNCTION_GENERIC_ACM "f_generic.a"
#define FUNCTION_ACM_MASK 0x1
#define FUNCTION_ECM_MASK 0x2
#define CONFIGFS_DIR "/config/usb_gadget"
#define USBFN_DEV "/dev/usbfn"
#define MAX_REQUEST         64
#define MAX_NAMELEN         64
#define MAX_PATHLEN         128

#define USB_EVENT_COUNT 10
#define MAX_EP          16
#define MAX_BUFLEN      2048

struct UsbFnCtrlEvent {
    union {
        struct UsbFnCtrlRequest  setup;
    } __attribute__((packed))    u;
    uint8_t                      type;
    uint8_t                      pad[3];
} __attribute__((packed));

typedef enum {
    USB_EP0_INVALID,
    USB_EP0_CTRL_EVENT,
    USB_EP0_IO_COMPLETED,
} UsbEp0EventType;

struct FuncNew {
    uint32_t nameLen;
    char     name[MAX_NAMELEN];
};

struct IoData {
    uint32_t aio;       /* 0 for sync ,1 for async */
    uint32_t read;      /* 0 for write ,1 for read */
    uint32_t len;       /* the len of this io request */
    uint32_t timeout;   /* sync timeout */
    uintptr_t buf;       /* the address of map buf */
};

struct GenericMemory {
    size_t size;
    uintptr_t buf;
    char storage[];
};

struct UsbFnReqEvent {
    uintptr_t buf;
    uint32_t actual;
    int32_t      status;
};

struct UsbEp0Event {
    union {
        struct UsbFnCtrlEvent ctrlEvent;
        struct UsbFnReqEvent reqEvent;
    };
    UsbEp0EventType type;
};
#define MAX_EP0_NUM 5
struct UsbFnEventAll {
    int32_t ep0[MAX_EP0_NUM];
    uint8_t ep0Num;
    struct UsbEp0Event ep0Event[MAX_EP0_NUM];
    int32_t epx[MAX_EP];
    uint8_t epNum;
    struct UsbFnReqEvent *reqEvent[MAX_EP];
    uint8_t numEvent[MAX_EP];
};

struct UsbFnAdapterOps {
    int32_t (*createDevice)(const char *udcName, const char *devName, struct UsbFnDeviceDesc *descriptor);
    int32_t (*delDevice)(const char *devName, const char *udcName, struct UsbFnDeviceDesc *descriptor);

    int32_t (*openPipe)(const char *interfaceName, int32_t epIndex);
    int32_t (*closePipe)(int32_t ep);
    int32_t (*getPipeInfo)(int32_t ep, struct UsbFnPipeInfo *pipeInfo);

    int32_t (*queueInit)(int32_t ep);
    int32_t (*queueDel)(int32_t ep);
    int32_t (*releaseBuf)(int32_t ep, const struct GenericMemory *mem);
    int32_t (*pipeIo)(int32_t ep, struct IoData *ioData);
    int32_t (*cancelIo)(int32_t ep, const struct IoData *ioData);
    int32_t (*getReqStatus)(int32_t ep, const struct IoData *ioData);
    uint8_t *(*mapAddr)(int32_t ep, uint32_t len);
    int32_t (*unmapAddr)(uint8_t *mapAddr, uint32_t len);
    int32_t (*pollEvent)(struct UsbFnEventAll *event, int32_t timeout);

    int32_t (*writeUDC)(const char *deviceName, const char *udcName, int32_t enable);
    int32_t (*writeProp)(const char *deviceName, const char *propName, uint32_t propValue);
    int32_t (*writeDesString)(const char *deviceName,
        uint16_t lang, const char *stringName, const char *stringValue);
};

struct RawUsbRamTestList {
    uintptr_t address;
    uint32_t size;
    struct DListHead list;
    struct OsalMutex lock;
};

struct UsbFnAdapterOps *UsbFnAdapterGetOps(void);
void *UsbFnMemAlloc(size_t size);
void *UsbFnMemCalloc(size_t size);
void UsbFnMemFree(const void *mem);
int32_t UsbFnAdpMemTestTrigger(bool enable);

#endif /* USBFN_ADAPTER_H */
