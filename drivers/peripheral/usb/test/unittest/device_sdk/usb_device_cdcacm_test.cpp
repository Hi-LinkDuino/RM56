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

extern "C" {
#include "usb_device_cdcacm_test.h"
}

using namespace std;

static struct UsbInterfaceAssocDescriptor g_acmIadDescriptor = {
    .bLength           = sizeof(g_acmIadDescriptor),
    .bDescriptorType   = USB_DDK_DT_INTERFACE_ASSOCIATION,
#ifdef CDC_ECM
    .bFirstInterface   = 0x02,
#else
    .bFirstInterface   = 0,
#endif
    .bInterfaceCount   = INTF_COUNT,
    .bFunctionClass    = USB_DDK_CLASS_COMM,
    .bFunctionSubClass = USB_DDK_CDC_SUBCLASS_ACM,
    .bFunctionProtocol = USB_DDK_CDC_ACM_PROTO_AT_V25TER,
    .iFunction         = ACM_IAD_IDX,
};

static struct UsbInterfaceDescriptor g_acmControlInterfaceDesc = {
    .bLength            = USB_DDK_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DDK_DT_INTERFACE,
#ifdef CDC_ECM
    .bInterfaceNumber   = 0x02,
#else
    .bInterfaceNumber   = 0,
#endif
    .bAlternateSetting  = 0,
    .bNumEndpoints      = NOTIFY_EP_NUM,
    .bInterfaceClass    = USB_DDK_CLASS_COMM,
    .bInterfaceSubClass = USB_DDK_CDC_SUBCLASS_ACM,
    .bInterfaceProtocol = USB_DDK_CDC_ACM_PROTO_AT_V25TER,
    .iInterface         = ACM_CTRL_IDX,
};

static struct UsbInterfaceDescriptor g_acmDataInterfaceDesc = {
    .bLength            = USB_DDK_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DDK_DT_INTERFACE,
#ifdef CDC_ECM
    .bInterfaceNumber   = 0x03,
#else
    .bInterfaceNumber   = 1,
#endif
    .bAlternateSetting  = 0,
    .bNumEndpoints      = DATA_EP_NUM,
    .bInterfaceClass    = USB_DDK_CLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 2,
    .iInterface         = ACM_DATA_IDX,
};

static struct UsbCdcHeaderDesc g_acmHeaderDesc = {
    .bLength            = sizeof(g_acmHeaderDesc),
    .bDescriptorType    = USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType = USB_DDK_CDC_HEADER_TYPE,
    .bcdCDC             = CpuToLe16(0x0110),
};

static struct UsbCdcCallMgmtDescriptor g_acmCallMgmtDescriptor = {
    .bLength            = sizeof(g_acmCallMgmtDescriptor),
    .bDescriptorType    = USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType = USB_DDK_CDC_CALL_MANAGEMENT_TYPE,
    .bmCapabilities     = 0,
    .bDataInterface     = 1,
};

static struct UsbCdcAcmDescriptor g_acmDescriptor = {
    .bLength            = sizeof(g_acmDescriptor),
    .bDescriptorType    = USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType = USB_DDK_CDC_ACM_TYPE,
    .bmCapabilities     = USB_DDK_CDC_CAP_LINE,
};

static struct UsbCdcUnionDesc g_acmUnionDesc = {
    .bLength            = sizeof(g_acmUnionDesc),
    .bDescriptorType    = USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType = USB_DDK_CDC_UNION_TYPE,
#ifdef CDC_ECM
    .bMasterInterface0  = 0x02,
    .bSlaveInterface0   = 0x03,
#else
    .bMasterInterface0  = 0,
    .bSlaveInterface0   = 1,
#endif
};

static struct UsbEndpointDescriptor g_acmFsNotifyDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_NOTIFY | USB_DDK_DIR_IN,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_INT,
    .wMaxPacketSize   = CpuToLe16(ACM_NOTIFY_MAXPACKET),
    .bInterval        = ACM_NOTIFY_INTERVAL,
};

static struct UsbEndpointDescriptor g_acmFsInDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_DATA_IN | USB_DDK_DIR_IN,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_BULK,
};

static struct UsbEndpointDescriptor g_acmFsOutDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_DATA_OUT | USB_DDK_DIR_OUT,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_BULK,
};

static struct UsbDescriptorHeader *g_acmFsFunction[] = {
    (struct UsbDescriptorHeader *) &g_acmIadDescriptor,
    (struct UsbDescriptorHeader *) &g_acmControlInterfaceDesc,
    (struct UsbDescriptorHeader *) &g_acmHeaderDesc,
    (struct UsbDescriptorHeader *) &g_acmCallMgmtDescriptor,
    (struct UsbDescriptorHeader *) &g_acmDescriptor,
    (struct UsbDescriptorHeader *) &g_acmUnionDesc,
    (struct UsbDescriptorHeader *) &g_acmFsNotifyDesc,
    (struct UsbDescriptorHeader *) &g_acmDataInterfaceDesc,
    (struct UsbDescriptorHeader *) &g_acmFsInDesc,
    (struct UsbDescriptorHeader *) &g_acmFsOutDesc,
    nullptr,
};

static struct UsbEndpointDescriptor g_acmHsNotifyDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_NOTIFY | USB_DDK_DIR_IN,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_INT,
    .wMaxPacketSize   = CpuToLe16(ACM_NOTIFY_MAXPACKET),
    .bInterval        = ACM_HS_NOTIFY_INTERVAL,
};

static struct UsbEndpointDescriptor g_acmHsInDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_DATA_IN | USB_DDK_DIR_IN,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize   = CpuToLe16(MAX_PACKET_SIZE),
};

static struct UsbEndpointDescriptor g_acmHsOutDesc = {
    .bLength           = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType   = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress  = EP_ADD_DATA_OUT | USB_DDK_DIR_OUT,
    .bmAttributes      = USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize    = CpuToLe16(MAX_PACKET_SIZE),
};

static struct UsbDescriptorHeader *g_acmHsFunction[] = {
    (struct UsbDescriptorHeader *) &g_acmIadDescriptor,
    (struct UsbDescriptorHeader *) &g_acmControlInterfaceDesc,
    (struct UsbDescriptorHeader *) &g_acmHeaderDesc,
    (struct UsbDescriptorHeader *) &g_acmCallMgmtDescriptor,
    (struct UsbDescriptorHeader *) &g_acmDescriptor,
    (struct UsbDescriptorHeader *) &g_acmUnionDesc,
    (struct UsbDescriptorHeader *) &g_acmHsNotifyDesc,
    (struct UsbDescriptorHeader *) &g_acmDataInterfaceDesc,
    (struct UsbDescriptorHeader *) &g_acmHsInDesc,
    (struct UsbDescriptorHeader *) &g_acmHsOutDesc,
    nullptr,
};

static struct UsbEndpointDescriptor g_acmSsInDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_DATA_IN | USB_DDK_DIR_IN,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize   = CpuToLe16(SS_MAX_PACKET_SIZE),
};

static struct UsbEndpointDescriptor g_acmSsOutDesc = {
    .bLength          = USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DDK_DT_ENDPOINT,
    .bEndpointAddress = EP_ADD_DATA_OUT | USB_DDK_DIR_OUT,
    .bmAttributes     = USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize   = CpuToLe16(SS_MAX_PACKET_SIZE),
};

static struct UsbSsEpCompDescriptor g_acmSsBulkCompDesc = {
    .bLength         = sizeof(g_acmSsBulkCompDesc),
    .bDescriptorType = USB_DDK_DT_SS_ENDPOINT_COMP,
};

static struct UsbDescriptorHeader *g_acmSsFunction[] = {
    (struct UsbDescriptorHeader *) &g_acmIadDescriptor,
    (struct UsbDescriptorHeader *) &g_acmControlInterfaceDesc,
    (struct UsbDescriptorHeader *) &g_acmHeaderDesc,
    (struct UsbDescriptorHeader *) &g_acmCallMgmtDescriptor,
    (struct UsbDescriptorHeader *) &g_acmDescriptor,
    (struct UsbDescriptorHeader *) &g_acmUnionDesc,
    (struct UsbDescriptorHeader *) &g_acmHsNotifyDesc,
    (struct UsbDescriptorHeader *) &g_acmSsBulkCompDesc,
    (struct UsbDescriptorHeader *) &g_acmDataInterfaceDesc,
    (struct UsbDescriptorHeader *) &g_acmSsInDesc,
    (struct UsbDescriptorHeader *) &g_acmSsBulkCompDesc,
    (struct UsbDescriptorHeader *) &g_acmSsOutDesc,
    (struct UsbDescriptorHeader *) &g_acmSsBulkCompDesc,
    nullptr,
};

static struct UsbString g_acmStringDefs[] = {
    [0].s = "CDC Abstract Control Model (ACM)",
    [1].s = "CDC ACM Data",
    [2].s = "CDC Serial",
    {  } /* end of list */
};

static struct UsbFnStrings g_acmStringTable = {
    .language = 0x0409, /* en-us */
    .strings  = g_acmStringDefs,
};

static struct UsbFnStrings *g_acmStrings[] = {
    &g_acmStringTable,
    nullptr,
};

static struct UsbFnFunction g_acmFunction = {
    .funcName       = "f_generic.a",
    .strings        = g_acmStrings,
    .fsDescriptors  = g_acmFsFunction,
    .hsDescriptors  = g_acmHsFunction,
    .ssDescriptors  = g_acmSsFunction,
    .sspDescriptors = nullptr,
};

/** device **/
#define BCD_USB                 0x0200
#define DEVICE_VENDOR_ID        0x12D1
#define DEVICE_PRODUCT_ID       0x5000
#define DEVICE_VERSION          0x0223

#define USB_MAX_PACKET_SIZE     0x40
#define POWER                   500

#define USB_FUNC_CONFIG_IDX USB_FUNC_FIRST_AVAIL_IDX
#define DRIVER_DESC "HDC Device"
#define CONFIG_DESC "hdc"

static struct UsbDeviceDescriptor g_cdcMasterDeviceDesc = {
    .bLength            = sizeof(g_cdcMasterDeviceDesc),
    .bDescriptorType    = USB_DDK_DT_DEVICE,
    .bcdUSB             = CpuToLe16(BCD_USB),
    .bDeviceClass       = 0,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = USB_MAX_PACKET_SIZE,
    .idVendor           = CpuToLe16(DEVICE_VENDOR_ID),
    .idProduct          = CpuToLe16(DEVICE_PRODUCT_ID),
    .bcdDevice          = CpuToLe16(DEVICE_VERSION),
    .iManufacturer      = USB_FUNC_MANUFACTURER_IDX,
    .iProduct           = USB_FUNC_PRODUCT_IDX,
    .iSerialNumber      = USB_FUNC_SERIAL_IDX,
    .bNumConfigurations = 1,
};

static struct UsbString g_stringsDev[] = {
    {USB_FUNC_MANUFACTURER_IDX, "HISILICON"},
    {USB_FUNC_PRODUCT_IDX, DRIVER_DESC},
    {USB_FUNC_SERIAL_IDX, "0123456789POPLAR"},
    {USB_FUNC_CONFIG_IDX, CONFIG_DESC},
    {  } /* end of list */
};

static struct UsbFnStrings g_stringTabDev = {
    .language   = 0x0409,       /* en-us */
    .strings    = g_stringsDev,
};

static struct UsbFnStrings *g_devStrings[] = {
    &g_stringTabDev,
    nullptr,
};

struct UsbFnFunction *g_functions[] = {
#ifdef CDC_ECM
    &g_ecmFunction,
#endif
#ifdef CDC_ACM
    &g_acmFunction,
#endif
    nullptr
};

static struct UsbFnConfiguration g_masterConfig = {
    .configurationValue = 1,
    .iConfiguration     = USB_FUNC_CONFIG_IDX,
    .attributes         = USB_CFG_BUS_POWERED,
    .maxPower           = POWER,
    .functions          = g_functions,
};

static struct UsbFnConfiguration *g_configs[] = {
    &g_masterConfig,
    nullptr,
};

struct UsbFnDeviceDesc g_acmFnDevice = {
    .deviceDesc    = &g_cdcMasterDeviceDesc,
    .deviceStrings = g_devStrings,
    .configs       = g_configs,
};

enum DevMasterCmd {
    DEV_MASTER_RELEASE = 1,
};

static struct Serial *SerialAlloc(void)
{
    struct Serial *port = nullptr;
    port = (struct Serial *)OsalMemCalloc(sizeof(*port));
    if (port == nullptr) {
        return nullptr;
    }
    OsalMutexInit(&port->lock);
    DListHeadInit(&port->readPool);
    DListHeadInit(&port->readQueue);
    DListHeadInit(&port->writePool);

    port->lineCoding.dwDTERate = CpuToLe32(PORT_RATE);
    port->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    port->lineCoding.bParityType = USB_CDC_NO_PARITY;
    port->lineCoding.bDataBits = DATA_BIT;
    return port;
}

static int32_t ParseInterfaces(struct AcmDevice *acmDevice)
{
    uint32_t i;
    uint32_t j;
    struct UsbFnInterface *fnIface = nullptr;
    UsbFnInterfaceHandle handle = nullptr;
    int32_t ret;
    for (i = 0; i < acmDevice->fnDev->numInterfaces; i++) {
        fnIface = (struct UsbFnInterface *)UsbFnGetInterface(acmDevice->fnDev, i);
        if (fnIface == nullptr) {
            return -1;
        }
        handle = UsbFnOpenInterface(fnIface);
        if (handle == nullptr) {
            return -1;
        }
        for (j = 0; j < fnIface->info.numPipes; j++) {
            struct UsbFnPipeInfo pipeInfo;
            ret = UsbFnGetInterfacePipeInfo(fnIface, j, &pipeInfo);
            if (ret != HDF_SUCCESS) {
                return -1;
            }
            if (pipeInfo.type == USB_PIPE_TYPE_INTERRUPT) {
                acmDevice->notifyPipe.id = pipeInfo.id;
                acmDevice->notifyPipe.maxPacketSize = pipeInfo.maxPacketSize;
                acmDevice->ctrlIface.fn = fnIface;
                acmDevice->ctrlIface.handle = handle;
            } else if (pipeInfo.type == USB_PIPE_TYPE_BULK) {
                if (pipeInfo.dir == USB_PIPE_DIRECTION_IN) {
                    acmDevice->dataInPipe.id = pipeInfo.id;
                    acmDevice->dataInPipe.maxPacketSize = pipeInfo.maxPacketSize;
                    acmDevice->dataIface.fn = fnIface;
                    acmDevice->dataIface.handle = handle;
                } else {
                    acmDevice->dataOutPipe.id = pipeInfo.id;
                    acmDevice->dataOutPipe.maxPacketSize = pipeInfo.maxPacketSize;
                    acmDevice->dataIface.fn = fnIface;
                    acmDevice->dataIface.handle = handle;
                }
            }
        }
    }
    return 0;
}

static void CtrlComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    if (req == nullptr) {
        HDF_LOGE("%{public}s:%{public}d req is nullptr", __func__, __LINE__);
        return;
    }

    struct CtrlInfo *ctrlInfo = (struct CtrlInfo *)req->context;
    struct AcmDevice *acm = ctrlInfo->acm;

    if (USB_REQUEST_COMPLETED != req->status) {
        goto OUT;
    }

    if (ctrlInfo->request == USB_DDK_CDC_REQ_SET_LINE_CODING) {
        struct UsbCdcLineCoding *value = (struct UsbCdcLineCoding *)req->buf;
        if (req->actual == sizeof(*value)) {
            acm->lineCoding = *value;
        }
    }
OUT:
    DListInsertTail(&req->list, &acm->ctrlPool);
}

static int32_t AllocCtrlRequests(struct AcmDevice *acmDevice)
{
    struct DListHead *head = &acmDevice->ctrlPool;
    struct UsbFnRequest *req = nullptr;
    struct CtrlInfo *ctrlInfo = nullptr;
    int32_t i;

    DListHeadInit(&acmDevice->ctrlPool);
    acmDevice->ctrlReqNum = 0;

    for (i = 0; i < 2; i++) {
        ctrlInfo = (struct CtrlInfo *)OsalMemCalloc(sizeof(*ctrlInfo));
        if (nullptr == ctrlInfo) {
            return -1;
        }
        ctrlInfo->acm = acmDevice;
        req = UsbFnAllocCtrlRequest(acmDevice->ctrlIface.handle,
            sizeof(struct UsbCdcLineCoding) + sizeof(struct UsbCdcLineCoding));
        if (nullptr == req) {
            return -1;
        }
        req->complete = CtrlComplete;
        req->context  = ctrlInfo;
        DListInsertTail(&req->list, head);
        acmDevice->ctrlReqNum++;
    }

    return 0;
}

static int32_t SendNotifyRequest(struct AcmDevice *acm, uint8_t type,
    uint16_t value, void *data, uint32_t length)
{
    if (acm == nullptr) {
        HDF_LOGE("%{public}s:%{public}d req is nullptr", __func__, __LINE__);
        return -1;
    }

    struct UsbFnRequest *req = nullptr;
    struct UsbCdcNotification *notify = nullptr;
    int32_t ret;

    req = acm->notifyReq;
    if ((req == nullptr) || ((req != nullptr) && (req->buf == nullptr))) {
        return -1;
    }
    acm->notifyReq = nullptr;
    acm->pending   = false;
    req->length    = sizeof(*notify) + length;

    notify = (struct UsbCdcNotification *)req->buf;
    notify->bmRequestType = USB_DDK_DIR_IN | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    notify->bNotificationType = type;
    notify->wValue = CpuToLe16(value);
    notify->wIndex = CpuToLe16(acm->ctrlIface.fn->info.index);
    notify->wLength = CpuToLe16(length);
    memcpy_s((void *)(notify + 1), length, data, length);

    ret = UsbFnSubmitRequestAsync(req);
    return ret;
}

static int32_t NotifySerialState(struct AcmDevice *acm)
{
    int32_t ret = 0;
    uint16_t serialState;

    OsalMutexLock(&acm->lock);
    if (acm->notifyReq) {
        serialState = CpuToLe16(acm->serialState);
        ret = SendNotifyRequest(acm, USB_DDK_CDC_NOTIFY_SERIAL_STATE,
            0, &serialState, sizeof(acm->serialState));
    } else {
        acm->pending = true;
    }
    OsalMutexUnlock(&acm->lock);

    return ret;
}

static void NotifyComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct AcmDevice *acm = (struct AcmDevice *)req->context;
    bool pending = false;

    if (acm == nullptr) {
        return;
    }
    OsalMutexLock(&acm->lock);
    if (req->status != USB_REQUEST_NO_DEVICE) {
        pending = acm->pending;
    }
    acm->notifyReq = req;
    OsalMutexUnlock(&acm->lock);
    if (pending) {
        NotifySerialState(acm);
    }
}

static int32_t AllocNotifyRequest(struct AcmDevice *acmDevice)
{
    /* allocate notification request */
    acmDevice->notifyReq = UsbFnAllocRequest(acmDevice->ctrlIface.handle, acmDevice->notifyPipe.id,
        sizeof(struct UsbCdcNotification));
    if (acmDevice->notifyReq == nullptr) {
        return -1;
    }
    acmDevice->notifyReq->complete = NotifyComplete;
    acmDevice->notifyReq->context = acmDevice;

    return HDF_SUCCESS;
}

static void Connect(struct AcmDevice *acm)
{
    if (acm == nullptr) {
        return;
    }
    acm->serialState |= SERIAL_STATE_DSR | SERIAL_STATE_DCD;
    NotifySerialState(acm);
}

static void Disconnect(struct AcmDevice *acm)
{
    if (acm == nullptr) {
        return;
    }
    acm->serialState &= ~(SERIAL_STATE_DSR | SERIAL_STATE_DCD);
    NotifySerialState(acm);
}

static int32_t SendBreak(struct AcmDevice *acm, int32_t duration)
{
    uint16_t state;

    if (acm == nullptr) {
        return -1;
    }
    state = acm->serialState;
    state &= ~SERIAL_STATE_BREAK;
    if (duration)
        state |= SERIAL_STATE_BREAK;

    acm->serialState = state;
    return NotifySerialState(acm);
}

static struct AcmNotifyMethod g_notifyMethod = {
    .Connect    = Connect,
    .Disconnect = Disconnect,
    .SendBreak  = SendBreak,
};

static uint32_t Enable(struct AcmDevice *acm)
{
    struct Serial *port = acm->port;
    OsalMutexLock(&port->lock);
    port->acm = acm;
    acm->lineCoding = port->lineCoding;
    if (port->refCount) {
        if (acm->notify && acm->notify->Connect) {
            acm->notify->Connect(acm);
        }
    } else {
        if (acm->notify && acm->notify->Disconnect) {
            acm->notify->Disconnect(acm);
        }
    }
    OsalMutexUnlock(&port->lock);

    return HDF_SUCCESS;
}

static uint32_t Disable(struct AcmDevice *acm)
{
    return HDF_SUCCESS;
}

static struct UsbFnRequest *GetCtrlReq(struct AcmDevice *acm)
{
    struct UsbFnRequest *req = nullptr;
    struct DListHead *pool = &acm->ctrlPool;
    if (!DListIsEmpty(pool)) {
        req = DLIST_FIRST_ENTRY(pool, struct UsbFnRequest, list);
        DListRemove(&req->list);
    }
    return req;
}

static void Setup(struct AcmDevice *acm, struct UsbFnCtrlRequest *setup)
{
    struct UsbFnRequest *req = nullptr;
    struct CtrlInfo *ctrlInfo = nullptr;
    uint16_t value  = Le16ToCpu(setup->value);
    uint16_t length = Le16ToCpu(setup->length);
    int32_t ret = 0;
    req = GetCtrlReq(acm);
    if (req == nullptr) {
        return;
    }
    switch (setup->request) {
        case USB_DDK_CDC_REQ_SET_LINE_CODING:
            if (length != sizeof(struct UsbCdcLineCoding)) {
                goto OUT;
            }
            ret = length;
            break;
        case USB_DDK_CDC_REQ_GET_LINE_CODING:
            ret = (length > sizeof(struct UsbCdcLineCoding)) ? \
                sizeof(struct UsbCdcLineCoding) : length;
            if (acm->lineCoding.dwDTERate == 0) {
                acm->lineCoding = acm->port->lineCoding;
            }
            memcpy_s(req->buf, req->length, &acm->lineCoding, ret);
            break;
        case USB_DDK_CDC_REQ_SET_CONTROL_LINE_STATE:
            ret = 0;
            acm->handshakeBits = value;
            break;
        default:
            break;
    }

OUT:
    ctrlInfo = (struct CtrlInfo *)req->context;
    ctrlInfo->request = setup->request;
    req->length = ret;
    ret = UsbFnSubmitRequestAsync(req);
}

static void Suspend(struct AcmDevice *acm)
{
    struct Serial *port = acm->port;

    OsalMutexLock(&port->lock);
    port->suspended = true;
    OsalMutexUnlock(&port->lock);
}

static void Resume(struct AcmDevice *acm)
{
    struct Serial *port = acm->port;

    OsalMutexLock(&port->lock);
    port->suspended = false;
    if (acm->notify && acm->notify->Connect) {
        acm->notify->Connect(acm);
    }
    port->startDelayed = false;
    OsalMutexUnlock(&port->lock);
}

void AcmEventCallback(struct UsbFnEvent *event)
{
    struct AcmDevice *acm = nullptr;

    if (event == nullptr || event->context == nullptr)
        return;
    acm = (struct AcmDevice *)event->context;
    switch (event->type) {
        case USBFN_STATE_BIND:
            break;
        case USBFN_STATE_UNBIND:
            break;
        case USBFN_STATE_ENABLE:
            Enable(acm);
            break;
        case USBFN_STATE_DISABLE:
            Disable(acm);
            acm->enableEvtCnt = 0;
            break;
        case USBFN_STATE_SETUP:
            Setup(acm, event->setup);
            break;
        case USBFN_STATE_SUSPEND:
            Suspend(acm);
            break;
        case USBFN_STATE_RESUME:
            Resume(acm);
            break;
        default:
            break;
    }
}

struct AcmDevice * SetUpAcmDevice(void)
{
    int32_t ret;

    struct AcmDevice *acmDevice = nullptr;
    struct UsbFnDescriptorData descData;
    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    acmDevice = (struct AcmDevice *)OsalMemCalloc(sizeof(*acmDevice));
    if (acmDevice == nullptr) {
        return nullptr;
    }
    acmDevice->fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    if (acmDevice->fnDev == nullptr) {
        return nullptr;
    }
    acmDevice->port = (struct Serial *)SerialAlloc();
    if (acmDevice->port == nullptr) {
        return nullptr;
    }
    ret = ParseInterfaces(acmDevice);
    if (ret != 0) {
        return nullptr;
    }
    ret = AllocCtrlRequests(acmDevice);
    if (ret != 0) {
        return nullptr;
    }
    ret = AllocNotifyRequest(acmDevice);
    if (ret != 0) {
        return nullptr;
    }
    ret = UsbFnStartRecvInterfaceEvent(acmDevice->ctrlIface.fn, 0xff, AcmEventCallback, acmDevice);
    if (ret != 0) {
        return nullptr;
    }
    acmDevice->notify = &g_notifyMethod;
    return acmDevice;
}

static int32_t FreeNotifyRequest(struct AcmDevice *acmDevice)
{
    int32_t ret;

    /* allocate notification request */
    if (acmDevice->notifyReq == nullptr) {
        HDF_LOGE("%{public}s: notifyReq is nullptr", __func__);
        return -1;
    }
    acmDevice->notifyReq->complete = nullptr;
    acmDevice->notifyReq->context = nullptr;
    ret = UsbFnFreeRequest(acmDevice->notifyReq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: free notifyReq failed", __func__);
        return -1;
    }
    return HDF_SUCCESS;
}

static int32_t FreeCtrlRequests(struct AcmDevice *acmDevice)
{
    struct DListHead *head = &acmDevice->ctrlPool;
    struct UsbFnRequest *req = nullptr;

    while (!DListIsEmpty(head)) {
        req = DLIST_FIRST_ENTRY(head, struct UsbFnRequest, list);
        DListRemove(&req->list);
        OsalMemFree(req->context);
        (void)UsbFnFreeRequest(req);
        acmDevice->ctrlReqNum--;
    }
    return 0;
}

void ReleaseAcmDevice(struct AcmDevice *acm)
{
    if (acm == nullptr) {
        HDF_LOGE("%{public}s: acm is nullptr", __func__);
        return;
    }
    FreeNotifyRequest(acm);
    FreeCtrlRequests(acm);
    (void)UsbFnCloseInterface(acm->ctrlIface.handle);
    (void)UsbFnCloseInterface(acm->dataIface.handle);
    UsbFnStopRecvInterfaceEvent(acm->ctrlIface.fn);
    OsalMemFree(acm->port);
}

