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

#include "usbfn_cfg_mgr.h"
#include <securec.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include <osal_thread.h>
#include <device_resource_if.h>
#include <hcs_tree_if.h>
#include "usbfn_dev_mgr.h"
#include "adapter_if.h"

#define HDF_LOG_TAG usbfn_cfg
static struct DListHead g_cfgEntry = {0};

static uint32_t UsbFnCfgMgrParseDevDesc(const struct DeviceResourceNode *devDescNode,
    struct DeviceResourceIface *drsOps, struct UsbDeviceDescriptor *devDesc)
{
    uint16_t value;

    if (drsOps->GetUint8(devDescNode, DESC_LENGTH, &devDesc->bLength, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read length fail!", __func__);
        return HDF_FAILURE;
    }
    if (devDesc->bLength != sizeof(struct UsbDeviceDescriptor)) {
        HDF_LOGE("%s: dev desc length is not ringht!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(devDescNode, DESC_TYPE, &devDesc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_CLASS, &devDesc->bDeviceClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_SUBCLASS, &devDesc->bDeviceSubClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_PROTOCOL, &devDesc->bDeviceProtocol, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_MAXSIZE, &devDesc->bMaxPacketSize0, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_MANUFACTURER, &devDesc->iManufacturer, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_PRODUCT, &devDesc->iProduct, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_SERIALNUM, &devDesc->iSerialNumber, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(devDescNode, USBDEV_NUMCFG, &devDesc->bNumConfigurations, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    if (devDesc->bDescriptorType != USB_DDK_DT_DEVICE) {
        HDF_LOGE("%s: dev desc length is not ringht!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint16(devDescNode, USBDEV_BCD, &value, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read bcdUSB fail!", __func__);
        return HDF_FAILURE;
    }
    devDesc->bcdUSB = Le16ToCpu(value);
    if (drsOps->GetUint16(devDescNode, USBDEV_VENDOR, &value, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read idVendor fail!", __func__);
        return HDF_FAILURE;
    }
    devDesc->idVendor = Le16ToCpu(value);
    if (drsOps->GetUint16(devDescNode, USBDEV_IDPRODUCT, &value, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read idProduct fail!", __func__);
        return HDF_FAILURE;
    }
    devDesc->idProduct = Le16ToCpu(value);
    if (drsOps->GetUint16(devDescNode, USBDEV_BCDDEVICE, &value, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read bcdDevice fail!", __func__);
        return HDF_FAILURE;
    }
    devDesc->bcdDevice = Le16ToCpu(value);

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseUsbFnDevDesc(const struct DeviceResourceNode *node,
    struct UsbFnDeviceDesc *fnDevDesc)
{
    struct DeviceResourceIface *drsOps = NULL;
    const char *childNodeName = NULL;
    const struct DeviceResourceNode *devDescNode = NULL;
    if (node == NULL || fnDevDesc == NULL) {
        HDF_LOGE("%s: node or fnDevDesc is null!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops failure!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "usb_dev_desc", &childNodeName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: get usb_dev_desc node name failure!", __func__);
        return HDF_FAILURE;
    }
    devDescNode = drsOps->GetChildNode(node, childNodeName);
    if (devDescNode == NULL) {
        HDF_LOGE("%s: childDevDescNode is null!", __func__);
        return HDF_FAILURE;
    }
    fnDevDesc->deviceDesc =
        (struct UsbDeviceDescriptor *)UsbFnMemCalloc(sizeof(struct UsbDeviceDescriptor));
    if (fnDevDesc->deviceDesc == NULL) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        return HDF_FAILURE;
    }
    if (UsbFnCfgMgrParseDevDesc(devDescNode, drsOps, fnDevDesc->deviceDesc)) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        goto FAIL;
    }
    return HDF_SUCCESS;
FAIL:
    UsbFnMemFree(fnDevDesc->deviceDesc);
    return HDF_FAILURE;
}

static int32_t UsbFnCfgMgrParseString(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, struct UsbFnStrings *fnString)
{
    int32_t iCount;
    int32_t strCount;
    int32_t ret;
    const char *strNodeName = NULL;
    const struct DeviceResourceNode *strNode = NULL;

    if (node == NULL || fnString == NULL || drsOps == NULL) {
        return HDF_FAILURE;
    }
    if (drsOps->GetUint16(node, "language", &fnString->language, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    strCount = drsOps->GetElemNum(node, "stringList");
    if (strCount <= 0) {
        HDF_LOGE("%s: stringList not found!", __func__);
        return HDF_FAILURE;
    }
    fnString->strings = (struct UsbString *)UsbFnMemCalloc((strCount + 1) * sizeof(struct UsbString));
    if (fnString->strings == NULL) {
        HDF_LOGE("%s: fnString->strings is null!", __func__);
        goto FREE_USB_STRING;
    }
    fnString->strings[strCount].s = NULL;
    fnString->strings[strCount].id = 0xff;
    for (iCount = 0; iCount < strCount; iCount++) {
        ret = drsOps->GetStringArrayElem(node, "stringList", iCount, &strNodeName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read stringList fail!", __func__);
            goto FREE_USB_STRING;
        }
        strNode = drsOps->GetChildNode(node, strNodeName);
        if (strNode == NULL) {
            HDF_LOGE("%s: StrNode is null!", __func__);
            goto FREE_USB_STRING;
        }
        struct UsbString *usbStr = fnString->strings + iCount;
        if (drsOps->GetUint8(strNode, "id", &usbStr->id, 0) != HDF_SUCCESS) {
            goto FREE_USB_STRING;
        }
        if (drsOps->GetString(strNode, "str", &usbStr->s, 0) != HDF_SUCCESS) {
            goto FREE_USB_STRING;
        }
    }

    return HDF_SUCCESS;
FREE_USB_STRING:
    UsbFnMemFree(fnString->strings);
    return HDF_FAILURE;
}

static struct UsbFnStrings **UsbFnCfgMgrParseStrings(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps)
{
    int32_t strTabCount;
    int32_t count;
    int32_t ret;
    const char *stringNodeName = NULL;
    const struct DeviceResourceNode *stringNode = NULL;
    struct UsbFnStrings **fnStrings = NULL;

    if (node == NULL || drsOps == NULL) {
        return NULL;
    }
    strTabCount = drsOps->GetElemNum(node, "stringTabList");
    if (strTabCount <= 0) {
        return NULL;
    }
    fnStrings = (struct UsbFnStrings **)UsbFnMemCalloc((strTabCount + 1) * sizeof(struct UsbFnStrings *));
    if (fnStrings == NULL) {
        return NULL;
    }
    fnStrings[strTabCount] = NULL;
    for (count = 0; count < strTabCount; count++) {
        fnStrings[count] =
            (struct UsbFnStrings *)UsbFnMemCalloc(sizeof(struct UsbFnStrings));
        if (fnStrings[count] == NULL) {
            goto FREE_STRING;
        }
        ret = drsOps->GetStringArrayElem(node, "stringTabList", count, &stringNodeName, NULL);
        if (ret != HDF_SUCCESS) {
            goto FREE_STRING;
        }
        stringNode = drsOps->GetChildNode(node, stringNodeName);
        if (stringNode == NULL) {
            goto FREE_STRING;
        }
        if (UsbFnCfgMgrParseString(stringNode, drsOps, fnStrings[count]) != HDF_SUCCESS) {
            goto FREE_STRING;
        }
    }
    return fnStrings;
FREE_STRING:
    while ((--count) >= 0) {
        UsbFnMemFree(fnStrings[count]);
    }
    UsbFnMemFree(fnStrings);
    return NULL;
}

static int32_t UsbFnCfgMgrParseUsbFnDevStrings(const struct DeviceResourceNode *node,
    struct UsbFnDeviceDesc *fnDevDesc)
{
    struct DeviceResourceIface *drsOps = NULL;
    const char *fnDevStrNodeName = NULL;
    const struct DeviceResourceNode *fnDevStrNode = NULL;

    if (node == NULL || fnDevDesc == NULL) {
        HDF_LOGE("%s: node or fnDevDesc is null!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops failure!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "usb_dev_string", &fnDevStrNodeName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: get dev strings name failure!", __func__);
        return HDF_FAILURE;
    }
    fnDevStrNode = drsOps->GetChildNode(node, fnDevStrNodeName);
    if (fnDevStrNode == NULL) {
        HDF_LOGE("%s: childDevDescNode is null!", __func__);
        return HDF_FAILURE;
    }
    fnDevDesc->deviceStrings = UsbFnCfgMgrParseStrings(fnDevStrNode, drsOps);
    if (fnDevDesc->deviceStrings == NULL) {
        HDF_LOGE("%s: parse device string error!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseAccocInterfaceDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbInterfaceAssocDescriptor *desc = NULL;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbInterfaceAssocDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_FIRST, &desc->bFirstInterface, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_COUNT, &desc->bInterfaceCount, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, FUNCTION_CLASS, &desc->bFunctionClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, FUNCTION_SUBCLASS, &desc->bFunctionSubClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, FUNCTION_PROTOCOL, &desc->bFunctionProtocol, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, FUNCTION_INDEX, &desc->iFunction, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseInterfaceDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbInterfaceDescriptor *desc = NULL;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
    return HDF_FAILURE;
    }
    desc = (struct UsbInterfaceDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_NUMBER, &desc->bInterfaceNumber, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_ALTERNATE, &desc->bAlternateSetting, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_NENDPOINT, &desc->bNumEndpoints, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_CLASS, &desc->bInterfaceClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_SUBCLASS, &desc->bInterfaceSubClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_PROTOCOL, &desc->bInterfaceProtocol, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, INTERFACE_INTERFACE, &desc->iInterface, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseEndpointDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbEndpointDescriptor *desc = NULL;
    uint16_t value;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbEndpointDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, ENDPOINT_ADDRESS, &desc->bEndpointAddress, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, ENDPOINT_MATTR, &desc->bmAttributes, 0) != HDF_SUCCESS || \
        drsOps->GetUint16(node, ENDPOINT_MAXPACKSIZE_W, &value, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, ENDPOINT_INTERVAL, &desc->bInterval, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    desc->wMaxPacketSize = Le16ToCpu(value);
    if (USB_DDK_DT_ENDPOINT_AUDIO_SIZE == desc->bLength) {
        if (drsOps->GetUint8(node, ENDPOINT_REFRESH, &desc->bRefresh, 0) != HDF_SUCCESS || \
            drsOps->GetUint8(node, ENDPOINT_SYNCADDR, &desc->bSynchAddress, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: read fail!", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseStringDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbStringDescriptor *desc = NULL;
    uint16_t value;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbStringDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint16(node, STRING_DATA, &value, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    desc->wData[0] = Le16ToCpu(value);

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseSspIsocEndpointDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbSspIsocEpCompDescriptor *desc = NULL;
    uint16_t sValue;
    uint32_t iValue;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbSspIsocEpCompDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint16(node, SSP_ISOC_EPCOMP_WReseved, &sValue, 0) != HDF_SUCCESS || \
        drsOps->GetUint32(node, SSP_ISOC_EPCOMP_DWPERINTERVAL, &iValue, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    desc->wReseved = Le16ToCpu(sValue);
    desc->dwBytesPerInterval = Le32ToCpu(iValue);

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseSsEndpointDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbSsEpCompDescriptor *desc = NULL;
    uint16_t sValue;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbSsEpCompDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, SS_EP_COMP_MAXBURST, &desc->bMaxBurst, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, SS_EP_COMP_MTTRIBUTE, &desc->bmAttributes, 0) != HDF_SUCCESS || \
        drsOps->GetUint16(node, SS_EP_COMP_WPERINTERVAL, &sValue, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    desc->wBytesPerInterval = Le16ToCpu(sValue);

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseQualifierDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbQualifierDescriptor *desc = NULL;
    uint16_t sValue;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbQualifierDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, QUALIFIER_CLASS, &desc->bDeviceClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, QUALIFIER_SUBCLASS, &desc->bDeviceSubClass, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, QUALIFIER_PROTOCOL, &desc->bDeviceProtocol, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, QUALIFIER_MAXSIZE, &desc->bMaxPacketSize0, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, QUALIFIER_NUMCFG, &desc->bNumConfigurations, 0) != HDF_SUCCESS || \
        drsOps->GetUint16(node, QUALIFIER_BCD, &sValue, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    desc->bcdUSB = Le16ToCpu(sValue);

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseOtgDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff, uint8_t length)
{
    struct UsbOtg20Descriptor *desc2 = NULL;
    struct UsbOtgDescriptor *desc = NULL;
    uint16_t sValue;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbOtgDescriptor *)descBuff;

    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, OTG_MTTRIBUTE, &desc->bmAttributes, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    if (length == sizeof (struct UsbOtg20Descriptor)) {
        desc2 = (struct UsbOtg20Descriptor *)descBuff;
        if (drsOps->GetUint16(node, QUALIFIER_BCD, &sValue, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        desc2->bcdOTG = Le16ToCpu(sValue);
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseDebugDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbDebugDescriptor *desc = NULL;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbDebugDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DEBUG_IN, &desc->bDebugInEndpoint, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DEBUG_OUT, &desc->bDebugOutEndpoint, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseSecurityDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff)
{
    struct UsbSecurityDescriptor *desc = NULL;
    uint16_t sValue;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    desc = (struct UsbSecurityDescriptor *)descBuff;
    if (drsOps->GetUint8(node, DESC_LENGTH, &desc->bLength, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, DESC_TYPE, &desc->bDescriptorType, 0) != HDF_SUCCESS || \
        drsOps->GetUint8(node, SECURITY_ENCRYTYPE, &desc->bNumEncryptionTypes, 0) != HDF_SUCCESS || \
        drsOps->GetUint16(node, SECURITY_TOTALLENGTH, &sValue, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail!", __func__);
        return HDF_FAILURE;
    }
    desc->wTotalLength = Le16ToCpu(sValue);

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseOtherDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff, uint8_t length)
{
    if (node == NULL || drsOps == NULL || descBuff == NULL) {
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8Array(node, "desc_data", descBuff, length, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (descBuff[0] != length) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseDescriptor(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, uint8_t *descBuff, uint8_t descType, uint8_t length)
{
    int32_t ret;

    if (node == NULL || drsOps == NULL || descBuff == NULL) {
    return HDF_FAILURE;
    }
    switch (descType) {
        case USB_DDK_DT_INTERFACE:
            ret = UsbFnCfgMgrParseInterfaceDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_ENDPOINT:
            ret = UsbFnCfgMgrParseEndpointDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_STRING:
            ret = UsbFnCfgMgrParseStringDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_INTERFACE_ASSOCIATION:
            ret = UsbFnCfgMgrParseAccocInterfaceDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_SSP_ISOC_ENDPOINT_COMP:
            ret = UsbFnCfgMgrParseSspIsocEndpointDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_SS_ENDPOINT_COMP:
            ret = UsbFnCfgMgrParseSsEndpointDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_DEVICE_QUALIFIER:
            ret = UsbFnCfgMgrParseQualifierDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_OTG:
            ret = UsbFnCfgMgrParseOtgDesc(node, drsOps, descBuff, length);
            break;
        case USB_DDK_DT_DEBUG:
            ret = UsbFnCfgMgrParseDebugDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_SECURITY:
            ret = UsbFnCfgMgrParseSecurityDesc(node, drsOps, descBuff);
            break;
        case USB_DDK_DT_ENCRYPTION_TYPE:
            ret = UsbFnCfgMgrParseSecurityDesc(node, drsOps, descBuff);
            break;
        default:
            ret = UsbFnCfgMgrParseOtherDesc(node, drsOps, descBuff, length);
            break;
    }

    return ret;
}

static struct UsbDescriptorHeader *UsbFnCfgMgrParseDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps)
{
    uint8_t length;
    uint8_t descType;
    uint8_t *descBuff = NULL;
    int32_t ret;

    if (node == NULL || drsOps == NULL) {
        return NULL;
    }
    if (drsOps->GetUint8(node, DESC_LENGTH, &length, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read length fail!", __func__);
        return NULL;
    }
    if (drsOps->GetUint8(node, DESC_TYPE, &descType, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read type fail!", __func__);
        return NULL;
    }
    descBuff = (uint8_t *)UsbFnMemCalloc(length * sizeof(uint8_t));
    if (descBuff == NULL) {
        return NULL;
    }
    ret = UsbFnCfgMgrParseDescriptor(node, drsOps, descBuff, descType, length);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: length = 0x%x, descType = 0x%x Parse fail!",
            __func__, length, descType);
        UsbFnMemFree(descBuff);
        return NULL;
    }

    return (struct UsbDescriptorHeader *)descBuff;
}

static struct UsbDescriptorHeader **UsbFnCfgMgrParseFunDesc(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, const char *descName)
{
    int32_t descCount;
    int32_t iCount;
    const char *descNodeName = NULL;
    const struct DeviceResourceNode *descNode = NULL;
    struct UsbDescriptorHeader **descriptors = NULL;

    if (node == NULL || descName == NULL || drsOps == NULL) {
        return NULL;
    }
    descCount = drsOps->GetElemNum(node, descName);
    if (descCount <= 0) {
        return NULL;
    }
    descriptors = \
        (struct UsbDescriptorHeader **)UsbFnMemCalloc((descCount + 1) * sizeof(struct UsbDescriptorHeader *));
    if (descriptors == NULL) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        return NULL;
    }
    descriptors[descCount] = NULL;
    for (iCount = 0; iCount < descCount; iCount++) {
        if (drsOps->GetStringArrayElem(node, descName, iCount, &descNodeName, NULL) != HDF_SUCCESS) {
            goto FREE_DESC_HEAD;
        }
        descNode = drsOps->GetChildNode(node, descNodeName);
        if (descNode == NULL) {
            goto FREE_DESC_HEAD;
        }
        descriptors[iCount] = UsbFnCfgMgrParseDesc(descNode, drsOps);
        if (descriptors[iCount] == NULL) {
            goto FREE_DESC_HEAD;
        }
    }
    return descriptors;
FREE_DESC_HEAD:
    while ((--iCount) >= 0) {
        UsbFnMemFree(descriptors[iCount]);
    }
    UsbFnMemFree(descriptors);
    return NULL;
}

static int32_t UsbFnCfgMgrParseUsbFnFunction(const struct DeviceResourceNode *node,
    const struct DeviceResourceIface *drsOps, struct UsbFnFunction *fun)
{
    if (node == NULL || fun == NULL) {
        HDF_LOGE("%s: node or fnDevDesc or drsOps is null!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "funcName", &fun->funcName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: get function name failure!", __func__);
        return HDF_FAILURE;
    }
    fun->strings = UsbFnCfgMgrParseStrings(node, drsOps);
    if (fun->strings == NULL) {
        HDF_LOGE("%s: parse device string error!", __func__);
        return HDF_FAILURE;
    }
    fun->fsDescriptors = UsbFnCfgMgrParseFunDesc(node, drsOps, "fsDescList");
    if (fun->fsDescriptors == NULL) {
        HDF_LOGE("%s: parse FS_DESC error!", __func__);
    }
    fun->hsDescriptors = UsbFnCfgMgrParseFunDesc(node, drsOps, "hsDescList");
    if (fun->hsDescriptors == NULL) {
        HDF_LOGE("%s: parse HS_DESC error!", __func__);
    }
    fun->ssDescriptors = UsbFnCfgMgrParseFunDesc(node, drsOps, "ssDescList");
    if (fun->ssDescriptors == NULL) {
        HDF_LOGE("%s: parse SS_DESC error!", __func__);
    }
    fun->sspDescriptors = UsbFnCfgMgrParseFunDesc(node, drsOps, "sspDescList");
    if (fun->sspDescriptors == NULL) {
        HDF_LOGE("%s: parse SSP_DESC error!", __func__);
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseUsbFnFunctions(const struct DeviceResourceNode *node,
    struct DeviceResourceIface *drsOps, struct UsbFnConfiguration *fnConfig)
{
    int32_t iCount;
    int32_t funCount;
    int32_t ret;
    const char *funNodeName = NULL;
    const struct DeviceResourceNode *funNode = NULL;

    funCount = drsOps->GetElemNum(node, "functionList");
    if (funCount <= 0) {
        HDF_LOGE("%s: get functionList num failed!", __func__);
        return HDF_FAILURE;
    }
    fnConfig->functions = \
        (struct UsbFnFunction **)UsbFnMemCalloc((funCount + 1) * sizeof(struct UsbFnFunction *));
    if (fnConfig->functions == NULL) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        return HDF_FAILURE;
    }
    fnConfig->functions[funCount] = NULL;
    for (iCount = 0; iCount < funCount; iCount++) {
        fnConfig->functions[iCount] = \
            (struct UsbFnFunction *)UsbFnMemCalloc(sizeof(struct UsbFnFunction));
        if (fnConfig->functions[iCount] == NULL) {
            HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
            goto FREE_FUNTION;
        }
        ret = drsOps->GetStringArrayElem(node, "functionList", iCount, &funNodeName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read stringList fail!", __func__);
            goto FREE_FUNTION;
        }
        funNode = drsOps->GetChildNode(node, funNodeName);
        if (funNode == NULL) {
            HDF_LOGE("%s: funNode not found!", __func__);
            goto FREE_FUNTION;
        }
        if (UsbFnCfgMgrParseUsbFnFunction(funNode, drsOps, fnConfig->functions[iCount]) != HDF_SUCCESS) {
            HDF_LOGE("%s: parse function error!", __func__);
            goto FREE_FUNTION;
        }
    }

    return HDF_SUCCESS;
FREE_FUNTION:
    while ((--iCount) > 0) {
        UsbFnMemFree((void *)fnConfig->functions[iCount]);
    }
    UsbFnMemFree(fnConfig->functions);
    return HDF_FAILURE;
}

static int32_t UsbFnCfgMgrParseUsbFnConfiguration(const struct DeviceResourceNode *node,
    struct DeviceResourceIface *drsOps, struct UsbFnConfiguration *fnConfig)
{
    if (node == NULL || fnConfig == NULL || drsOps == NULL) {
        HDF_LOGE("%s: node or fnDevDesc or drsOps is null!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "configurationValue", &fnConfig->configurationValue, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read strCount fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "iConfiguration", &fnConfig->iConfiguration, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read strCount fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "attributes", &fnConfig->attributes, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read strCount fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint16(node, "maxPower", &fnConfig->maxPower, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read strCount fail!", __func__);
        return HDF_FAILURE;
    }
    if (UsbFnCfgMgrParseUsbFnFunctions(node, drsOps, fnConfig) != HDF_SUCCESS) {
        HDF_LOGE("%s: parse functions failure!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UsbFnCfgMgrParseUsbFnCfgLists(const struct DeviceResourceNode *configNode,
    struct DeviceResourceIface *drsOps, struct UsbFnDeviceDesc *fnDevDesc)
{
    int32_t configCount;
    int32_t count;
    int32_t ret;
    const char *childConfigNodeName = NULL;
    const struct DeviceResourceNode *childConfigNode = NULL;

    configCount = drsOps->GetElemNum(configNode, "configList");
    if (configCount <= 0) {
        HDF_LOGE("%s: get configList num failed!", __func__);
        return HDF_FAILURE;
    }
    fnDevDesc->configs =
        (struct UsbFnConfiguration **)UsbFnMemCalloc((configCount + 1) * sizeof(struct UsbFnConfiguration *));
    if (fnDevDesc->configs == NULL) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        return HDF_FAILURE;
    }
    fnDevDesc->configs[configCount] = NULL;
    for (count = 0; count < configCount; count++) {
        fnDevDesc->configs[count] = \
            (struct UsbFnConfiguration *)UsbFnMemCalloc(sizeof(struct UsbFnConfiguration));
        if (fnDevDesc->configs[count] == NULL) {
            HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
            goto FREE_CONFIG;
        }
        ret = drsOps->GetStringArrayElem(configNode, "configList", count, &childConfigNodeName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read stringList fail!", __func__);
            goto FREE_CONFIG;
        }
        childConfigNode = drsOps->GetChildNode(configNode, childConfigNodeName);
        if (childConfigNode == NULL) {
            HDF_LOGE("%s: StrNode is null!", __func__);
            goto FREE_CONFIG;
        }
        if (UsbFnCfgMgrParseUsbFnConfiguration(childConfigNode, drsOps,
            fnDevDesc->configs[count]) != HDF_SUCCESS) {
            HDF_LOGE("%s: parse config failure!", __func__);
            goto FREE_CONFIG;
        }
    }

    return HDF_SUCCESS;
FREE_CONFIG:
    while ((--count) >= 0) {
        UsbFnMemFree(fnDevDesc->deviceStrings[count]);
    }
    UsbFnMemFree(fnDevDesc->deviceStrings);
    return HDF_FAILURE;
}

static int32_t UsbFnCfgMgrParseUsbFnConfigurations(const struct DeviceResourceNode *node,
    struct UsbFnDeviceDesc *fnDevDesc)
{
    struct DeviceResourceIface *drsOps = NULL;
    const char *configNodeName = NULL;
    const struct DeviceResourceNode *configNode = NULL;

    if (node == NULL || fnDevDesc == NULL) {
        HDF_LOGE("%s: node or fnDevDesc is null!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops failure!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "usb_configuration", &configNodeName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: get config node name failure!", __func__);
        return HDF_FAILURE;
    }
    configNode = drsOps->GetChildNode(node, configNodeName);
    if (configNode == NULL) {
        HDF_LOGE("%s: configNode is null!", __func__);
        return HDF_FAILURE;
    }
    if (UsbFnCfgMgrParseUsbFnCfgLists(configNode, drsOps, fnDevDesc) != HDF_SUCCESS) {
        HDF_LOGE("%s: parse usb config lists failure!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct UsbFnDeviceDesc *UsbFnCfgMgrGetInstanceFromHCS(const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct UsbFnDeviceDesc *usbDevDesc = NULL;

    if (node == NULL) {
        HDF_LOGE("%s: node is null!", __func__);
        return NULL;
    }

    usbDevDesc =
        (struct UsbFnDeviceDesc *)UsbFnMemCalloc(sizeof(*usbDevDesc));
    if (usbDevDesc == NULL) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        return NULL;
    }
    ret = UsbFnCfgMgrParseUsbFnDevDesc(node, usbDevDesc);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: parse device descriptor failure!", __func__);
        return NULL;
    }
    ret = UsbFnCfgMgrParseUsbFnDevStrings(node, usbDevDesc);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: parse device string failure!", __func__);
        goto FAIL_DEV_STRING;
    }
    ret = UsbFnCfgMgrParseUsbFnConfigurations(node, usbDevDesc);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: parse config descriptor failure!", __func__);
        goto FAIL_DEV_STRING;
    }

    return usbDevDesc;

 FAIL_DEV_STRING:
    UsbFnMemFree(usbDevDesc->deviceDesc);
    UsbFnMemFree(usbDevDesc);
    return NULL;
}

void UsbFnCfgMgrFreeFnStrings(struct UsbFnStrings **fnStrings)
{
    int8_t iCount;

    for (iCount = 0; (fnStrings[iCount] != NULL); iCount++) {
        UsbFnMemFree(fnStrings[iCount]);
    }
    UsbFnMemFree(fnStrings);
}

void UsbFnCfgMgrFreeFuncDescHead(struct UsbDescriptorHeader **descHeads)
{
    int8_t iCount;

    for (iCount = 0; (descHeads[iCount] != NULL); iCount++) {
        UsbFnMemFree(descHeads[iCount]);
    }
    UsbFnMemFree(descHeads);
}

void UsbFnCfgMgrFreeFunctions(struct UsbFnFunction **fnFunctions)
{
    int8_t iCount;

    for (iCount = 0; (fnFunctions[iCount] != NULL); iCount++) {
        if (fnFunctions[iCount]->strings) {
            UsbFnCfgMgrFreeFnStrings(fnFunctions[iCount]->strings);
        }
        if (fnFunctions[iCount]->fsDescriptors) {
            UsbFnCfgMgrFreeFuncDescHead(fnFunctions[iCount]->fsDescriptors);
        }
        if (fnFunctions[iCount]->hsDescriptors) {
            UsbFnCfgMgrFreeFuncDescHead(fnFunctions[iCount]->hsDescriptors);
        }
        if (fnFunctions[iCount]->ssDescriptors) {
            UsbFnCfgMgrFreeFuncDescHead(fnFunctions[iCount]->ssDescriptors);
        }
        if (fnFunctions[iCount]->sspDescriptors) {
            UsbFnCfgMgrFreeFuncDescHead(fnFunctions[iCount]->sspDescriptors);
        }
        UsbFnMemFree(fnFunctions[iCount]);
    }
    UsbFnMemFree(fnFunctions);
}

void UsbFnCfgMgrFreeConfigs(struct UsbFnConfiguration **fnConfigs)
{
    int8_t iCount;

    for (iCount = 0; fnConfigs[iCount] != NULL; iCount++) {
        if (fnConfigs[iCount]->functions) {
            UsbFnCfgMgrFreeFunctions(fnConfigs[iCount]->functions);
        }
        UsbFnMemFree(fnConfigs[iCount]);
    }
    UsbFnMemFree(fnConfigs);
}

void UsbFnCfgMgrFreeUsbFnDeviceDesc(struct UsbFnDeviceDesc *fnDevDesc)
{
    if (fnDevDesc == NULL) {
        HDF_LOGE("%s: fnDevDesc null!", __func__);
        return;
    }
    if (fnDevDesc->deviceDesc) {
        UsbFnMemFree(fnDevDesc->deviceDesc);
    }
    if (fnDevDesc->deviceStrings) {
        UsbFnCfgMgrFreeFnStrings(fnDevDesc->deviceStrings);
    }
    if (fnDevDesc->configs) {
        UsbFnCfgMgrFreeConfigs(fnDevDesc->configs);
    }
    UsbFnMemFree(fnDevDesc);
}

static uint8_t IsPropRegisted(const struct UsbFnInterface *intf, const char *name)
{
    struct UsbFnCfgPropMgr *obj = NULL;
    struct UsbFnCfgPropMgr *temp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(obj, temp, &g_cfgEntry, \
        struct UsbFnCfgPropMgr, entry) {
        if (obj->intf && (obj->intf == intf) && strcmp(name, obj->name) == 0) {
            return 1;
        }
    }
    return 0;
}

static int32_t IsDevDescPropAndGetValue(const struct UsbFnInterface *intf, const char *name,
    uint16_t *value)
{
    struct UsbFnDeviceMgr *fnDevMgr = NULL;

    if (name == NULL || intf == NULL) {
        return 0;
    }
    fnDevMgr = (struct UsbFnDeviceMgr *)intf->object;
    if (fnDevMgr == NULL || fnDevMgr->des == NULL || fnDevMgr->des->deviceDesc == NULL) {
        HDF_LOGE("%s: fnDevMgr is null", __func__);
        return 0;
    }
    if (strcmp(USBDEV_CLASS, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bDeviceClass;
    } else if (strcmp(USBDEV_SUBCLASS, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bDeviceSubClass;
    } else if (strcmp(USBDEV_PROTOCOL, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bDeviceProtocol;
    } else if (strcmp(USBDEV_MAXSIZE, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bMaxPacketSize0;
    } else if (strcmp(USBDEV_MANUFACTURER, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->iManufacturer;
    } else if (strcmp(USBDEV_PRODUCT, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->iProduct;
    } else if (strcmp(USBDEV_SERIALNUM, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->iSerialNumber;
    } else if (strcmp(USBDEV_NUMCFG, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bNumConfigurations;
    } else if (strcmp(USBDEV_BCD, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bcdUSB;
    } else if (strcmp(USBDEV_VENDOR, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->idVendor;
    } else if (strcmp(USBDEV_IDPRODUCT, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->idProduct;
    } else if (strcmp(USBDEV_BCDDEVICE, name) == 0) {
        *value = (uint16_t)fnDevMgr->des->deviceDesc->bcdDevice;
    } else {
        HDF_LOGE("%s: other prop", __func__);
        return 0;
    }
    return 1;
}

static int32_t IsDevDescProp(const char *name)
{
    if (name == NULL) {
        return 0;
    }
    if ((strcmp(USBDEV_SUBCLASS, name) == 0) || (strcmp(DESC_TYPE, name) == 0) ||
        (strcmp(USBDEV_CLASS, name) == 0) || (strcmp(USBDEV_PROTOCOL, name) == 0) ||
        (strcmp(USBDEV_MAXSIZE, name) == 0) || (strcmp(USBDEV_MANUFACTURER, name) == 0) ||
        (strcmp(USBDEV_PRODUCT, name) == 0) || (strcmp(USBDEV_SERIALNUM, name) == 0) ||
        (strcmp(USBDEV_NUMCFG, name) == 0) || (strcmp(USBDEV_BCD, name) == 0) ||
        (strcmp(USBDEV_VENDOR, name) == 0) || (strcmp(USBDEV_IDPRODUCT, name) == 0) ||
        (strcmp(USBDEV_BCDDEVICE, name) == 0)) {
        return 1;
    }
    return 0;
}

static const char *UsbFnCfgGetPropValueFromPropList(const struct UsbFnDeviceMgr *fnDevMgr,
    const struct UsbFnInterface *intf, const struct DeviceResourceIface *drsOps,
    const struct DeviceResourceNode *propListNode, const char *name)
{
    int32_t propCount;
    int32_t count;
    int32_t ret;
    uint8_t configNum;
    uint8_t interfaceNum;
    const char *propNodeName = NULL;
    const char *propValue = NULL;
    const char *propName = NULL;
    const struct DeviceResourceNode *propNode = NULL;

    if (drsOps->GetUint8(propListNode, "configNum", &configNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read length fail!", __func__);
        return NULL;
    }
    if (drsOps->GetUint8(propListNode, "interfaceNum", &interfaceNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read length fail!", __func__);
        return NULL;
    }
    if ((intf->info.index != interfaceNum) || (intf->info.configIndex != configNum)) {
        HDF_LOGE("%s: prop List is not ringt!", __func__);
        return NULL;
    }
    propCount = drsOps->GetElemNum(propListNode, "propList");
    if (propCount <= 0) {
        return NULL;
    }
    for (count = 0; count < propCount; count++) {
        ret = drsOps->GetStringArrayElem(propListNode, "propList", count, &propNodeName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read stringList fail!", __func__);
            return NULL;
        }
        propNode = drsOps->GetChildNode(propListNode, propNodeName);
        if (propNode == NULL) {
            HDF_LOGE("%s: propNode is null!", __func__);
            return NULL;
        }
        if (drsOps->GetString(propNode, "name", &propName, 0) != HDF_SUCCESS) {
            return NULL;
        }
        if (strcmp(propName, name) == 0) {
            if (drsOps->GetString(propNode, "value", &propValue, 0) != HDF_SUCCESS) {
                return NULL;
            }
            return propValue;
        }
    }

    return NULL;
}

static const char *UsbFnCfgGetPropValueFromHcs(const struct UsbFnDeviceMgr *fnDevMgr,
    const struct UsbFnInterface *intf, const struct DeviceResourceIface *drsOps,
    const struct DeviceResourceNode *customNode, const char *name)
{
    int32_t propTabCount;
    int32_t count;
    int32_t ret;
    const struct DeviceResourceNode *propListNode = NULL;
    const char *propNodeName = NULL;
    const char *propValue = NULL;

    propTabCount = drsOps->GetElemNum(customNode, "propTable");
    if (propTabCount <= 0) {
        return NULL;
    }
    for (count = 0; count < propTabCount; count++) {
        ret = drsOps->GetStringArrayElem(customNode, "propTable", count, &propNodeName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read stringList fail!", __func__);
            return NULL;
        }
        propListNode = drsOps->GetChildNode(customNode, propNodeName);
        if (propListNode == NULL) {
            HDF_LOGE("%s: propNode is null!", __func__);
            return NULL;
        }
        propValue = UsbFnCfgGetPropValueFromPropList(fnDevMgr, intf, drsOps, propListNode, name);
        if (propValue) {
            return propValue;
        }
    }

    return NULL;
}

static const char *UsbFnCfgFindPropFromHcs(const struct UsbFnInterface *intf, const char *name)
{
    struct UsbFnDeviceMgr *fnDevMgr = NULL;
    const struct DeviceResourceNode *customNode = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    const char *propValue = NULL;

    if (name == NULL || intf == NULL) {
        return NULL;
    }
    fnDevMgr = (struct UsbFnDeviceMgr *)intf->object;
    if (fnDevMgr == NULL || fnDevMgr->node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return NULL;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops failure!", __func__);
        return NULL;
    }
    customNode = drsOps->GetChildNode(fnDevMgr->node, "custom_prop");
    if (customNode == NULL) {
        HDF_LOGE("%s: childDevDescNode is null!", __func__);
        return NULL;
    }
    propValue = UsbFnCfgGetPropValueFromHcs(fnDevMgr, intf, drsOps, customNode, name);

    return propValue;
}

static int32_t UsbFnCfgChangeUdcName(const struct UsbFnDeviceMgr *fnDevMgr,
    struct UsbFnAdapterOps *fnOps)
{
    int32_t ret;

    if (fnDevMgr == NULL || fnOps == NULL) {
        return HDF_FAILURE;
    }
    ret = fnOps->writeUDC(fnDevMgr->name, fnDevMgr->udcName, 0);
    if (ret) {
        return HDF_FAILURE;
    }
    ret = fnOps->writeUDC(fnDevMgr->name, fnDevMgr->udcName, 1);
    if (ret) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UsbFnCfgChangeStrings(const struct UsbFnDeviceMgr *fnDevMgr,
    struct UsbFnAdapterOps *fnOps, uint32_t index, const char *propName)
{
    int32_t i;
    int32_t ret;
    struct UsbFnStrings **strings = NULL;

    if (fnDevMgr == NULL || fnDevMgr->des == NULL || \
        fnDevMgr->des->deviceStrings == NULL) {
        return HDF_FAILURE;
    }
    strings = fnDevMgr->des->deviceStrings;
    for (i = 0; strings[i] != NULL; i++) {
        ret = fnOps->writeDesString(fnDevMgr->name, strings[i]->language, propName,
            strings[i]->strings[index].s);
        if (ret) {
        return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t UsbFnCfgChangeDevceDes(const struct UsbFnInterface *intf, const char *name,
    const char *value)
{
    int32_t ret;
    uint32_t data;
    struct UsbFnAdapterOps *fnOps = UsbFnAdapterGetOps();
    struct UsbFnDeviceMgr *fnDevMgr = NULL;

    if (name == NULL || intf == NULL || fnOps == NULL) {
        return HDF_FAILURE;
    }
    fnDevMgr = (struct UsbFnDeviceMgr *)intf->object;
    if (fnDevMgr == NULL || fnDevMgr->des == NULL || fnDevMgr->des->deviceDesc == NULL ||
        fnDevMgr->des->deviceStrings == NULL) {
        HDF_LOGE("%s: null", __func__);
        return HDF_FAILURE;
    }
    if (sscanf_s(value, "%x", &data) <= 0) {
        return HDF_FAILURE;
    }
    if (strcmp(name, USBDEV_MANUFACTURER) == 0 || strcmp(name, USBDEV_PRODUCT) == 0 ||
        strcmp(name, USBDEV_SERIALNUM) == 0) {
        ret = UsbFnCfgChangeStrings(fnDevMgr, fnOps, data, name);
        if (ret) {
            return HDF_ERR_INVALID_PARAM;
        }
    } else {
        ret = fnOps->writeProp(fnDevMgr->name, name, data);
        if (ret) {
            return HDF_ERR_INVALID_PARAM;
        }
    }
    if (UsbFnCfgChangeUdcName(fnDevMgr, fnOps) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct UsbFnCfgPropMgr *UsbfnCfgMgrFindPropMgr(const struct UsbFnInterface *intf, const char *name)
{
    struct UsbFnCfgPropMgr *obj = NULL;
    struct UsbFnCfgPropMgr *temp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(obj, temp, &g_cfgEntry, \
        struct UsbFnCfgPropMgr, entry) {
        if (obj->intf && (obj->intf == intf) && strcmp(name, obj->name) == 0) {
            return obj;
        }
    }
    return NULL;
}

int32_t UsbFnCfgMgrRegisterProp(const struct UsbFnInterface *intf,
    const struct UsbFnRegistInfo *registInfo)
{
    struct UsbFnCfgPropMgr *fnCfgPropMgr = NULL;
    uint8_t isDevProp;
    int32_t isRegist;
    int32_t ret;
    if (intf == NULL || registInfo == NULL || registInfo->name == NULL) {
        return HDF_FAILURE;
    }
    if (g_cfgEntry.next == 0) {
        DListHeadInit(&g_cfgEntry);
    }
    isDevProp = (uint8_t)IsDevDescProp(registInfo->name);
    isRegist = IsPropRegisted(intf, registInfo->name);
    if (isRegist) {
        if (isDevProp == 0) {
            return HDF_FAILURE;
        } else {
            fnCfgPropMgr = UsbfnCfgMgrFindPropMgr(intf, registInfo->name);
            if (fnCfgPropMgr == NULL) {
                HDF_LOGE("%s:%s not found", __func__, registInfo->name);
                return HDF_FAILURE;
            }
        }
    } else {
        fnCfgPropMgr = UsbFnMemCalloc(sizeof(struct UsbFnCfgPropMgr));
        if (fnCfgPropMgr == NULL) {
            HDF_LOGE("%s:%d UsbFnMemCalloc failure!", __func__, __LINE__);
            return HDF_FAILURE;
        }
    }

    fnCfgPropMgr->isDevProp = isDevProp;
    fnCfgPropMgr->intf = intf;
    ret = snprintf_s(fnCfgPropMgr->name, MAX_LEN, MAX_LEN - 1, "%s", registInfo->name);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_FAILURE;
    }
    ret = snprintf_s(fnCfgPropMgr->value, MAX_LEN, MAX_LEN - 1, "%s", registInfo->value);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_FAILURE;
    }
    fnCfgPropMgr->getPropCallback = registInfo->getProp;
    fnCfgPropMgr->setPropCallback = registInfo->setProp;
    if (isRegist == 0) {
        DListInsertTail(&fnCfgPropMgr->entry, &g_cfgEntry);
    }

    return HDF_SUCCESS;
}

void UsbFnCfgMgrUnRegisterAllProp(void)
{
    DListHeadInit(&g_cfgEntry);
    g_cfgEntry.next = 0;
}

int32_t UsbFnCfgMgrGetProp(const struct UsbFnInterface *intf, const char *name, char *value)
{
    struct UsbFnCfgPropMgr *fnCfgPropMgr = NULL;
    uint16_t val;
    int32_t ret;
    char tmp[MAX_LEN];
    const char *propValue = NULL;

    if (intf == NULL || name == NULL) {
        return HDF_FAILURE;
    }
    if (g_cfgEntry.next == 0) {
        DListHeadInit(&g_cfgEntry);
    }
    if (IsPropRegisted(intf, name) == 0) {
        if (IsDevDescPropAndGetValue(intf, name, &val)) {
            ret = snprintf_s(tmp, MAX_LEN, MAX_LEN - 1, "%x", val);
            if (ret < 0) {
                return HDF_FAILURE;
            }
            ret = memcpy_s(value, strlen(tmp), tmp, strlen(tmp));
            if (ret != 0) {
                return HDF_FAILURE;
            }
            return HDF_SUCCESS;
        } else {
            propValue = UsbFnCfgFindPropFromHcs(intf, name);
            if (propValue == NULL) {
                return HDF_FAILURE;
            }
            ret = memcpy_s(value, strlen(propValue), propValue, strlen(propValue));
            if (ret != EOK) {
                HDF_LOGE("%s: memcpy_s failed", __func__);
                return HDF_FAILURE;
            }
            return HDF_SUCCESS;
        }
    }
    fnCfgPropMgr = UsbfnCfgMgrFindPropMgr(intf, name);
    if (fnCfgPropMgr == NULL) {
        return HDF_FAILURE;
    }
    ret = memcpy_s(value, strlen(fnCfgPropMgr->value), fnCfgPropMgr->value, strlen(fnCfgPropMgr->value));
    if (ret != EOK) {
        return HDF_FAILURE;
    }
    if (fnCfgPropMgr->getPropCallback) {
        fnCfgPropMgr->getPropCallback(intf, name, fnCfgPropMgr->value);
    }
    return HDF_SUCCESS;
}

int32_t UsbFnCfgMgrSetProp(const struct UsbFnInterface *intf, const char *name, const char *value)
{
    struct UsbFnCfgPropMgr *fnCfgPropMgr = NULL;
    int32_t deviceProp;
    int32_t isRegist;
    int32_t ret;
    const char *propValue = NULL;
    struct UsbFnRegistInfo registInfo;
    if (intf == NULL || name == NULL) {
        return HDF_FAILURE;
    }
    if (g_cfgEntry.next == 0) {
        DListHeadInit(&g_cfgEntry);
    }
    registInfo.name = name;
    registInfo.value = value;
    registInfo.setProp = NULL;
    registInfo.getProp = NULL;
    deviceProp = IsDevDescProp(name);
    isRegist = IsPropRegisted(intf, name);
    if (isRegist == 0 && deviceProp == 0) {
        propValue = UsbFnCfgFindPropFromHcs(intf, name);
        if (propValue) {
            return UsbFnCfgMgrRegisterProp(intf, &registInfo);
        }
        return HDF_FAILURE;
    }
    if (isRegist) {
        fnCfgPropMgr = UsbfnCfgMgrFindPropMgr(intf, name);
        if (fnCfgPropMgr == NULL) {
            return HDF_FAILURE;
        }
        ret = snprintf_s(fnCfgPropMgr->value, MAX_LEN, MAX_LEN - 1, "%s", value);
        if (ret < 0) {
            return HDF_FAILURE;
        }
    }
    if (deviceProp) {
        if (isRegist == 0) {
            if (UsbFnCfgMgrRegisterProp(intf, &registInfo)) {
                return HDF_FAILURE;
            }
        }
        if (UsbFnCfgChangeDevceDes(intf, name, value)) {
            return HDF_FAILURE;
        }
    }
    if (isRegist && fnCfgPropMgr->setPropCallback) {
        fnCfgPropMgr->setPropCallback(intf, name, fnCfgPropMgr->value);
    }
    return HDF_SUCCESS;
}

