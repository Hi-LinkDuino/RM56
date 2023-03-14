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

#ifndef USBFN_CFG_MGR_H
#define USBFN_CFG_MGR_H

#include <device_resource_if.h>
#include "usbfn_device.h"
#include "usbfn_request.h"
#include "usbfn_interface.h"

#define DESC_LENGTH "bLength"
#define DESC_TYPE "bDescriptorType"

/* USB_DT_DEVICE: Device descriptor */
#define USBDEV_CLASS "bDeviceClass"
#define USBDEV_SUBCLASS "bDeviceSubClass"
#define USBDEV_PROTOCOL "bDeviceProtocol"
#define USBDEV_MAXSIZE "bMaxPacketSize0"
#define USBDEV_MANUFACTURER "manufacturer"
#define USBDEV_PRODUCT "product"
#define USBDEV_SERIALNUM "serialnumber"
#define USBDEV_NUMCFG "numConfigurations"
#define USBDEV_BCD "bcdUSB"
#define USBDEV_VENDOR "idVendor"
#define USBDEV_IDPRODUCT "idProduct"
#define USBDEV_BCDDEVICE "bcdDevice"

/* USB_DT_INTERFACE */
#define INTERFACE_NUMBER "bInterfaceNumber"
#define INTERFACE_ALTERNATE "bAlternateSetting"
#define INTERFACE_NENDPOINT "bNumEndpoints"
#define INTERFACE_CLASS "bInterfaceClass"
#define INTERFACE_SUBCLASS "bInterfaceSubClass"
#define INTERFACE_PROTOCOL "bInterfaceProtocol"
#define INTERFACE_INTERFACE "iInterface"

/* USB_DT_INTERFACE_ASSOCIATION */
#define INTERFACE_FIRST "bFirstInterface"
#define INTERFACE_COUNT "bInterfaceCount"
#define FUNCTION_CLASS "bFunctionClass"
#define FUNCTION_SUBCLASS "bFunctionSubClass"
#define FUNCTION_PROTOCOL "bFunctionProtocol"
#define FUNCTION_INDEX "iFunction"

/* USB_DT_ENDPOINT */
#define ENDPOINT_ADDRESS "bEndpointAddress"
#define ENDPOINT_MATTR "bmAttributes"
#define ENDPOINT_MAXPACKSIZE_W "wMaxPacketSize"
#define ENDPOINT_INTERVAL "bInterval"
#define ENDPOINT_REFRESH "bRefresh"
#define ENDPOINT_SYNCADDR "bSynchAddress"

/* USB_DT_STRING: String descriptor */
#define STRING_DATA "wData"

/* USB_DT_SSP_ISOC_ENDPOINT_COMP */
#define SSP_ISOC_EPCOMP_WReseved "wReseved"
#define SSP_ISOC_EPCOMP_DWPERINTERVAL "dwBytesPerInterval"

/* USB_DT_SS_ENDPOINT_COMP */
#define SS_EP_COMP_MAXBURST "bMaxBurst"
#define SS_EP_COMP_MTTRIBUTE "bmAttributes"
#define SS_EP_COMP_WPERINTERVAL "wBytesPerInterval"

/* USB_DT_DEVICE_QUALIFIER */
#define QUALIFIER_BCD "bcdUSB"
#define QUALIFIER_CLASS "bDeviceClass"
#define QUALIFIER_SUBCLASS "bDeviceSubClass"
#define QUALIFIER_PROTOCOL "bDeviceProtocol"
#define QUALIFIER_MAXSIZE "bMaxPacketSize0"
#define QUALIFIER_NUMCFG "bNumConfigurations"
#define QUALIFIER_RESERVED "bRESERVED"

/* USB_DT_OTG (from OTG 2.0 supplement) */
#define OTG_MTTRIBUTE "bmAttributes"
#define OTG_BCDOTG "bcdOTG"

/* USB_DT_DEBUG */
#define DEBUG_IN "bDebugInEndpoint"
#define DEBUG_OUT "bDebugOutEndpoint"

/* USB_DT_SECURITY */
#define SECURITY_TOTALLENGTH "wTotalLength"
#define SECURITY_ENCRYTYPE "bNumEncryptionTypes"

#define MAX_LEN 64

typedef struct  {
    uint8_t size;
    uint8_t cmd;
    uint8_t type;
    uint8_t nameLen;
    uint8_t dataLen;
}SelfProp;

typedef enum  {
    PROP_TYPE_UINT,
    PROP_TYPE_STRING
}FuncDescType;

struct UsbFnCfgPropMgr {
    const struct UsbFnInterface *intf;
    char name[MAX_LEN];
    char value[MAX_LEN];
    uint8_t isDevProp;
    UsbFnPropCallback getPropCallback;
    UsbFnPropCallback setPropCallback;
    struct DListHead entry;
};

struct UsbFnDeviceDesc *UsbFnCfgMgrGetInstanceFromHCS(const struct DeviceResourceNode *node);
void UsbFnCfgMgrFreeUsbFnDeviceDesc(struct UsbFnDeviceDesc *fnDevDesc);
int32_t UsbFnCfgMgrSetProp(const struct UsbFnInterface *intf, const char *name, const char *value);
int32_t UsbFnCfgMgrGetProp(const struct UsbFnInterface *intf, const char *name, char *value);
int32_t UsbFnCfgMgrRegisterProp(const struct UsbFnInterface *intf,
    const struct UsbFnRegistInfo *registInfo);
void UsbFnCfgMgrUnRegisterAllProp(void);

#endif /* USBFN_CFG_MGR_H */
