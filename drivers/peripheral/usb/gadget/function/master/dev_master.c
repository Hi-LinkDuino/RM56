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

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_device_object.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usbfn_device.h"
#include "usbfn_interface.h"
#include "usbfn_request.h"
#include "default_config.h"

#define HDF_LOG_TAG dev_master

#define CDC_ACM
#define CDC_ECM

#define SS_MAX_PACKET_SIZE 1024
#define MAX_PACKET_SIZE 512
#define EP_ADD_NOTIFY   1
#define EP_ADD_DATA_IN  2
#define EP_ADD_DATA_OUT 3
#define DATA_EP_NUM     2
#define NOTIFY_EP_NUM   1
#define INTF_COUNT      2

#ifdef CDC_ECM
#define ECM_HS_INTERVAL             9
#define ECM_STATUS_INTERVAL_MS      32
#define ECM_STATUS_BYTECOUNT        16  /* 8 byte header + data */

#define ECM_CTRL_IDX            1
#define ECM_DATA_IDX            2
#define ECM_IAD_IDX             3

enum DevMasterCmd {
    DEV_MASTER_INIT = 0x1,
    DEV_MASTER_RELEASE,
};

struct DevMasterMgr {
    struct IDeviceIoService     service;
    struct UsbFnDescriptorData descData;
    struct HdfDeviceObject      *device;
    const char                  *udcName;
};

/* interface descriptor: */
static struct UsbInterfaceAssocDescriptor g_ecmIadDescriptor = {
    .bLength =      sizeof(g_ecmIadDescriptor),
    .bDescriptorType =  USB_DDK_DT_INTERFACE_ASSOCIATION,
    .bFirstInterface   = 0,
    .bInterfaceCount =  INTF_COUNT,
    .bFunctionClass =   USB_DDK_CLASS_COMM,
    .bFunctionSubClass =    USB_DDK_CDC_SUBCLASS_ETHERNET,
    .bFunctionProtocol =    USB_DDK_CDC_PROTO_NONE,
    .iFunction         = ECM_IAD_IDX,
};

static struct UsbInterfaceDescriptor g_ecmControlIntf = {
    .bLength =      sizeof(g_ecmControlIntf),
    .bDescriptorType =  USB_DDK_DT_INTERFACE,
    .bInterfaceNumber   = 0,
    .bNumEndpoints =    NOTIFY_EP_NUM,
    .bInterfaceClass =  USB_DDK_CLASS_COMM,
    .bInterfaceSubClass =   USB_DDK_CDC_SUBCLASS_ETHERNET,
    .bInterfaceProtocol =   USB_DDK_CDC_PROTO_NONE,
    .iInterface         = ECM_CTRL_IDX,
};

static struct UsbCdcHeaderDesc g_ecmHeaderDesc = {
    .bLength =      sizeof(g_ecmHeaderDesc),
    .bDescriptorType =  USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType =   USB_DDK_CDC_HEADER_TYPE,

    .bcdCDC =       CpuToLe16(0x0110),
};

static struct UsbCdcUnionDesc g_ecmUnionDesc = {
    .bLength =      sizeof(g_ecmUnionDesc),
    .bDescriptorType =  USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType =   USB_DDK_CDC_UNION_TYPE,
    .bMasterInterface0 = 0,
    .bSlaveInterface0 =  1,
};

#define MAC_ADDR 6
static struct UsbCdcEtherDesc g_ecmDesc = {
    .bLength =      sizeof(g_ecmDesc),
    .bDescriptorType =  USB_DDK_DT_CS_INTERFACE,
    .bDescriptorSubType =   USB_DDK_CDC_ETHERNET_TYPE,
    .iMACAddress = MAC_ADDR,
    .bmEthernetStatistics = CpuToLe32(0),
    .wMaxSegmentSize =  CpuToLe16(0x05ea),
    .wNumberMCFilters = CpuToLe16(0),
    .bNumberPowerFilters =  0,
};

/* the default data interface has no endpoints ... */
static struct UsbInterfaceDescriptor g_ecmDataNopIntf = {
    .bLength =      sizeof(g_ecmDataNopIntf),
    .bDescriptorType =  USB_DDK_DT_INTERFACE,

    .bInterfaceNumber = 1,
    .bAlternateSetting =    0,
    .bNumEndpoints =    0,
    .bInterfaceClass =  USB_DDK_CLASS_CDC_DATA,
    .bInterfaceSubClass =   0,
    .bInterfaceProtocol =   6,
};

/* ... but the "real" data interface has two bulk endpoints */
static struct UsbInterfaceDescriptor g_ecmDataIntf = {
    .bLength =      sizeof(g_ecmDataIntf),
    .bDescriptorType =  USB_DDK_DT_INTERFACE,

    .bInterfaceNumber = 1,
    .bAlternateSetting =    1,
    .bNumEndpoints =    DATA_EP_NUM,
    .bInterfaceClass =  USB_DDK_CLASS_CDC_DATA,
    .bInterfaceSubClass =   0,
    .bInterfaceProtocol =   6,
    .iInterface         = ECM_DATA_IDX,
};

/* full speed support: */
static struct UsbEndpointDescriptor g_fsEcmNotifyDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_NOTIFY | USB_DDK_DIR_IN,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_INT,
    .wMaxPacketSize =   CpuToLe16(ECM_STATUS_BYTECOUNT),
    .bInterval =        ECM_STATUS_INTERVAL_MS,
};

static struct UsbEndpointDescriptor g_fsEcmInDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_DATA_IN | USB_DDK_DIR_IN,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_BULK,
};

static struct UsbEndpointDescriptor g_fsEcmOutDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_DATA_OUT | USB_DDK_DIR_OUT,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_BULK,
};

static struct UsbDescriptorHeader *g_ecmFsFunction[] = {
    /* CDC ECM control descriptors */
    (struct UsbDescriptorHeader *) &g_ecmIadDescriptor,
    (struct UsbDescriptorHeader *) &g_ecmControlIntf,
    (struct UsbDescriptorHeader *) &g_ecmHeaderDesc,
    (struct UsbDescriptorHeader *) &g_ecmUnionDesc,
    (struct UsbDescriptorHeader *) &g_ecmDesc,

    /* NOTE: status endpoint might need to be removed */
    (struct UsbDescriptorHeader *) &g_fsEcmNotifyDesc,

    /* data interface, altsettings 0 and 1 */
    (struct UsbDescriptorHeader *) &g_ecmDataNopIntf,
    (struct UsbDescriptorHeader *) &g_ecmDataIntf,
    (struct UsbDescriptorHeader *) &g_fsEcmInDesc,
    (struct UsbDescriptorHeader *) &g_fsEcmOutDesc,
    NULL,
};

/* high speed support: */
static struct UsbEndpointDescriptor g_hsEcmNotifyDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_NOTIFY | USB_DDK_DIR_IN,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_INT,
    .wMaxPacketSize =   CpuToLe16(ECM_STATUS_BYTECOUNT),
    .bInterval =        ECM_HS_INTERVAL,
};

static struct UsbEndpointDescriptor g_hsEcmInDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_DATA_IN | USB_DDK_DIR_IN,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   CpuToLe16(MAX_PACKET_SIZE),
};

static struct UsbEndpointDescriptor g_hsEcmOutDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_DATA_OUT | USB_DDK_DIR_OUT,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   CpuToLe16(MAX_PACKET_SIZE),
};

static struct UsbDescriptorHeader *g_ecmHsFunction[] = {
    /* CDC ECM control descriptors */
    (struct UsbDescriptorHeader *) &g_ecmIadDescriptor,
    (struct UsbDescriptorHeader *) &g_ecmControlIntf,
    (struct UsbDescriptorHeader *) &g_ecmHeaderDesc,
    (struct UsbDescriptorHeader *) &g_ecmUnionDesc,
    (struct UsbDescriptorHeader *) &g_ecmDesc,

    /* NOTE: status endpoint might need to be removed */
    (struct UsbDescriptorHeader *) &g_hsEcmNotifyDesc,

    /* data interface, altsettings 0 and 1 */
    (struct UsbDescriptorHeader *) &g_ecmDataNopIntf,
    (struct UsbDescriptorHeader *) &g_ecmDataIntf,
    (struct UsbDescriptorHeader *) &g_hsEcmInDesc,
    (struct UsbDescriptorHeader *) &g_hsEcmOutDesc,
    NULL,
};

/* super speed support: */
static struct UsbEndpointDescriptor g_ssEcmNotifyDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_NOTIFY | USB_DDK_DIR_IN,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_INT,
    .wMaxPacketSize =   CpuToLe16(ECM_STATUS_BYTECOUNT),
    .bInterval =        ECM_HS_INTERVAL,
};

static struct UsbSsEpCompDescriptor g_ssEcmIntrCompDesc = {
    .bLength =      sizeof(g_ssEcmIntrCompDesc),
    .bDescriptorType =  USB_DDK_DT_SS_ENDPOINT_COMP,

    /* the following 3 values can be tweaked if necessary */
    /* .bMaxBurst =     0, */
    /* .bmAttributes =  0, */
    .wBytesPerInterval =    CpuToLe16(ECM_STATUS_BYTECOUNT),
};

static struct UsbEndpointDescriptor g_ssEcmInDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_DATA_IN | USB_DDK_DIR_IN,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   CpuToLe16(SS_MAX_PACKET_SIZE),
};

static struct UsbEndpointDescriptor g_ssEcmOutDesc = {
    .bLength =      USB_DDK_DT_ENDPOINT_SIZE,
    .bDescriptorType =  USB_DDK_DT_ENDPOINT,

    .bEndpointAddress = EP_ADD_DATA_OUT | USB_DDK_DIR_OUT,
    .bmAttributes =     USB_DDK_ENDPOINT_XFER_BULK,
    .wMaxPacketSize =   CpuToLe16(SS_MAX_PACKET_SIZE),
};

static struct UsbSsEpCompDescriptor g_ssEcmBulkCompDesc = {
    .bLength =      sizeof(g_ssEcmBulkCompDesc),
    .bDescriptorType =  USB_DDK_DT_SS_ENDPOINT_COMP,

    /* the following 2 values can be tweaked if necessary */
    /* .bMaxBurst =     0, */
    /* .bmAttributes =  0, */
};

static struct UsbDescriptorHeader *g_ecmSsFunction[] = {
    /* CDC ECM control descriptors */
    (struct UsbDescriptorHeader *) &g_ecmIadDescriptor,
    (struct UsbDescriptorHeader *) &g_ecmControlIntf,
    (struct UsbDescriptorHeader *) &g_ecmHeaderDesc,
    (struct UsbDescriptorHeader *) &g_ecmUnionDesc,
    (struct UsbDescriptorHeader *) &g_ecmDesc,

    /* NOTE: status endpoint might need to be removed */
    (struct UsbDescriptorHeader *) &g_ssEcmNotifyDesc,
    (struct UsbDescriptorHeader *) &g_ssEcmIntrCompDesc,

    /* data interface, altsettings 0 and 1 */
    (struct UsbDescriptorHeader *) &g_ecmDataNopIntf,
    (struct UsbDescriptorHeader *) &g_ecmDataIntf,
    (struct UsbDescriptorHeader *) &g_ssEcmInDesc,
    (struct UsbDescriptorHeader *) &g_ssEcmBulkCompDesc,
    (struct UsbDescriptorHeader *) &g_ssEcmOutDesc,
    (struct UsbDescriptorHeader *) &g_ssEcmBulkCompDesc,
    NULL,
};

/* string descriptors: */
static struct UsbString g_ecmStringDefs[] = {
    [0].s = "CDC Ethernet Control Model (ECM)",
    [0x01].s = "0ac75ae91c79",
    [0x02].s = "CDC Ethernet Data",
    [0x03].s = "CDC ECM",
    {  } /* end of list */
};

static struct UsbFnStrings g_ecmStringTable = {
    .language =     0x0409, /* en-us */
    .strings =      g_ecmStringDefs,
};

static struct UsbFnStrings *g_ecmStrings[] = {
    &g_ecmStringTable,
    NULL,
};

static struct UsbFnFunction g_ecmFunction = {
    .enable         = true,
    .funcName       = "f_generic.e",
    .strings        = g_ecmStrings,
    .fsDescriptors  = g_ecmFsFunction,
    .hsDescriptors  = g_ecmHsFunction,
    .ssDescriptors  = g_ecmSsFunction,
    .sspDescriptors = NULL,
};
#endif

#ifdef CDC_ACM
#define ACM_NOTIFY_INTERVAL     32 /* ms */
#define ACM_HS_NOTIFY_INTERVAL  9  /* ms */
#define ACM_NOTIFY_MAXPACKET    10 /* notification + 2 bytes */

#define ACM_CTRL_IDX            1
#define ACM_DATA_IDX            2
#define ACM_IAD_IDX             3

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
    NULL,
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
    NULL,
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
    NULL,
};

static struct UsbString g_acmStringDefs[] = {
    [0].s = "CDC Abstract Control Model (ACM)",
    [0x01].s = "CDC ACM Data",
    [0x02].s = "CDC Serial",
    {  } /* end of list */
};

static struct UsbFnStrings g_acmStringTable = {
    .language = 0x0409, /* en-us */
    .strings  = g_acmStringDefs,
};

static struct UsbFnStrings *g_acmStrings[] = {
    &g_acmStringTable,
    NULL,
};

static struct UsbFnFunction g_acmFunction = {
    .enable         = true,
    .funcName       = "f_generic.a",
    .strings        = g_acmStrings,
    .fsDescriptors  = g_acmFsFunction,
    .hsDescriptors  = g_acmHsFunction,
    .ssDescriptors  = g_acmSsFunction,
    .sspDescriptors = NULL,
};
#endif
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
    NULL,
};

struct UsbFnFunction *g_functions[] = {
#ifdef CDC_ECM
    &g_ecmFunction,
#endif
#ifdef CDC_ACM
    &g_acmFunction,
#endif
    NULL
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
    NULL,
};

static struct UsbFnDeviceDesc g_masterFuncDevice = {
    .deviceDesc    = &g_cdcMasterDeviceDesc,
    .deviceStrings = g_devStrings,
    .configs       = g_configs,
};

struct UsbFnDevice *g_fnDev = NULL;
static int32_t MasterReleaseFuncDevice()
{
    int32_t ret;
    if (g_fnDev == NULL) {
        HDF_LOGE("%s: fnDev is null", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnRemoveDevice(g_fnDev);
    if (ret == HDF_SUCCESS) {
        g_fnDev = NULL;
    } else {
        HDF_LOGE("%{public}s: remove usb function device failed", __func__);
    }

    return ret;
}

static int32_t UsbFnRegistUsbfnDevice(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdfDeviceObject *device = client->device;
    struct DevMasterMgr *devMgr = NULL;
    struct UsbFnDevice *fnDev = NULL;
    uint8_t value;
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint32 == NULL || device == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }
    devMgr = (struct DevMasterMgr *)device->service;
    if (HdfSbufReadUint8(data, &value) != true) {
        HDF_LOGE("%s: read sbuf failed", __func__);
        return HDF_FAILURE;
    }
    devMgr->descData.functionMask = value;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice(devMgr->udcName, &devMgr->descData);
    if (fnDev == NULL) {
        HDF_LOGE("%s: create usb function device failed", __func__);
        if (!HdfSbufWriteInt8(reply, 0)) {
            HDF_LOGE("UsbEcmRead HdfSbufWriteInt8 error");
        }
        return HDF_FAILURE;
    }
    g_fnDev = fnDev;
    if (!HdfSbufWriteInt8(reply, value)) {
        HDF_LOGE("UsbEcmRead HdfSbufWriteInt8 error");
        return HDF_FAILURE;
    }
    return 0;
}

int32_t MasterDispatch(struct HdfDeviceIoClient *client, int32_t cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    if (client == NULL) {
        HDF_LOGE("%s: client is NULL", __func__);
        return HDF_FAILURE;
    }

    if (HdfDeviceObjectCheckInterfaceDesc(client->device, data) == false) {
        HDF_LOGE("%{public}s:%{public}d check interface desc fail", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmdId) {
        case DEV_MASTER_INIT:
            ret = UsbFnRegistUsbfnDevice(client, data, reply);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s: create usbfn device failed", __func__);
            }
            break;
        case DEV_MASTER_RELEASE:
            ret = MasterReleaseFuncDevice();
            break;
        default:
            ret = HDF_ERR_INVALID_OBJECT;
            HDF_LOGE("%s: unknown cmd id %d", __func__, cmdId);
            break;
    }
    return ret;
}

/* HdfDriverEntry implementations */
static int32_t MasterDriverBind(struct HdfDeviceObject *device)
{
    struct DevMasterMgr *devMgr = NULL;
    if (device == NULL) {
        HDF_LOGE("%s:%d device is null\n", __func__, __LINE__);
        return HDF_FAILURE;
    }
    devMgr = (struct DevMasterMgr *)OsalMemCalloc(sizeof(*devMgr));
    if (devMgr == NULL) {
        HDF_LOGE("%s: usbfn Alloc usb devMgr failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfDeviceObjectSetInterfaceDesc(device, "hdf.usb.usbfn") != HDF_SUCCESS) {
        HDF_LOGE(" Set Desc fail!");
        OsalMemFree(devMgr);
        return HDF_FAILURE;
    }

    devMgr->device  = device;
    device->service = &(devMgr->service);
    devMgr->device->service->Dispatch = MasterDispatch;
    return HDF_SUCCESS;
}

static int32_t MasterDriverInit(struct HdfDeviceObject *device)
{
    struct DevMasterMgr *devMgr = NULL;
    uint8_t useHcs;

    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL || device == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }
    devMgr = (struct DevMasterMgr *)device->service;
    if (iface->GetString(device->property, "udc_name",
        &devMgr->udcName, UDC_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: read udc_name failed, use default", __func__);
    }
    if (iface->GetUint8(device->property, "use_hcs", &useHcs, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read use_hcs fail, use default", __func__);
    }
    if (useHcs == 0) {
        devMgr->descData.type = USBFN_DESC_DATA_TYPE_DESC;
        devMgr->descData.descriptor = &g_masterFuncDevice;
    } else {
        devMgr->descData.type = USBFN_DESC_DATA_TYPE_PROP;
        devMgr->descData.property = device->property;
    }
    return HDF_SUCCESS;
}

static void MasterDriverRelease(struct HdfDeviceObject *device)
{
    struct DevMasterMgr *devMgr = NULL;
    devMgr = (struct DevMasterMgr *)device->service;
    if (devMgr == NULL) {
        HDF_LOGE("%s: descriptor is NULL", __func__);
        return;
    }
    OsalMemFree(devMgr);
}

struct HdfDriverEntry g_masterDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usbfn_master",
    .Bind          = MasterDriverBind,
    .Init          = MasterDriverInit,
    .Release       = MasterDriverRelease,
};

HDF_INIT(g_masterDriverEntry);
