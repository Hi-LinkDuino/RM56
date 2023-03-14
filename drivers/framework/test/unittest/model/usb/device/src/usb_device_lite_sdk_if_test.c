/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "usb_device_lite_cdcacm_test.h"

#define HDF_LOG_TAG usb_device_sdk_test

struct AcmDevice *g_acmDevice = NULL;
extern struct UsbFnDeviceDesc g_acmFnDevice;

struct AcmDevice *UsbGetAcmDevice(void)
{
    return g_acmDevice;
}

int32_t UsbFnDviceTestCreate(void)
{
    dprintf("%s: start\n", __func__);
    remove_usb_device();
    g_acmDevice = SetUpAcmDevice();
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: UsbFnDviceTestCreate fail", __func__);
        return HDF_FAILURE;
    }
    dprintf("%s: success\n", __func__);
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCreate002(void)
{
    struct UsbFnDevice *fnDev = NULL;
    struct UsbFnDescriptorData descData;

    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = NULL;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    if (fnDev != NULL) {
        HDF_LOGE("%s: UsbFnDviceTestCreate success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCreate003(void)
{
    struct UsbFnDevice *fnDev = NULL;
    struct UsbFnDescriptorData descData;

    descData.type = USBFN_DESC_DATA_TYPE_PROP;
    descData.property = NULL;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    if (fnDev != NULL) {
        HDF_LOGE("%s: UsbFnDviceTestCreate success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCreate004(void)
{
    struct UsbFnDevice *fnDev = NULL;
    struct UsbFnDescriptorData descData;

    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_0", &descData);
    if (fnDev != NULL) {
        HDF_LOGE("%s: UsbFnDviceTestCreate success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCreate005(void)
{
    struct UsbFnDevice *fnDev = NULL;
    struct UsbFnDescriptorData descData;

    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice("100e0000.hidwc3_1", &descData);
    if (fnDev != NULL) {
        HDF_LOGE("%s: UsbFnDviceTestCreate success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCreate006(void)
{
    struct UsbFnDevice *fnDev = NULL;
    struct UsbFnDescriptorData descData;
    const char *udcName = NULL;

    descData.type = USBFN_DESC_DATA_TYPE_DESC;
    descData.descriptor = &g_acmFnDevice;
    fnDev = (struct UsbFnDevice *)UsbFnCreateDevice(udcName, &descData);
    if (fnDev != NULL) {
        HDF_LOGE("%s: UsbFnDviceTestCreate success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStatus(void)
{
    int32_t ret;
    UsbFnDeviceState devState;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetDeviceState(g_acmDevice->fnDev, &devState);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get status error", __func__);
        return HDF_FAILURE;
    }
    if (!(devState >= USBFN_STATE_BIND && devState <= USBFN_STATE_RESUME)) {
        HDF_LOGE("%s: device status error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStatus002(void)
{
    int32_t ret;
    UsbFnDeviceState devState;
    ret = UsbFnGetDeviceState(NULL, &devState);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get status success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStatus003(void)
{
    int32_t ret;
    int32_t count;
    UsbFnDeviceState devState;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    for (count = 0; count < TEST_TIMES; count++) {
        ret = UsbFnGetDeviceState(g_acmDevice->fnDev, &devState);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: get status error", __func__);
            return HDF_FAILURE;
        }
        if (!(devState >= USBFN_STATE_BIND && devState <= USBFN_STATE_RESUME)) {
            HDF_LOGE("%s: device status error", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStatus004(void)
{
    int32_t ret;
    ret = UsbFnGetDeviceState(NULL, NULL);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get status success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStatus005(void)
{
    int32_t ret;
    UsbFnDeviceState *devState = NULL;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetDeviceState(g_acmDevice->fnDev, devState);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get status success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetDevice(void)
{
    const struct UsbFnDevice *device = NULL;
    const char *udcName = "100e0000.hidwc3_0";
    device = UsbFnGetDevice(udcName);
    if (device == NULL) {
        HDF_LOGE("%s: get device fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetDevice002(void)
{
    const struct UsbFnDevice *device = NULL;
    const char *udcName = "100e0000.hidwc3_1";
    device = UsbFnGetDevice(udcName);
    if (device != NULL) {
        HDF_LOGE("%s: get device success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetDevice003(void)
{
    const struct UsbFnDevice *device = NULL;
    const char *udcName = NULL;
    device = UsbFnGetDevice(udcName);
    if (device != NULL) {
        HDF_LOGE("%s: get device success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetDevice004(void)
{
    const struct UsbFnDevice *device = NULL;
    const char *udcName = "100e0000.hidwc3_0 ";
    device = UsbFnGetDevice(udcName);
    if (device != NULL) {
        HDF_LOGE("%s: get device success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetDevice005(void)
{
    const struct UsbFnDevice *device = NULL;
    const char *udcName = "100e0000.hidwc3_0\0100e0000.hidwc3_0";
    device = UsbFnGetDevice(udcName);
    if (device != NULL) {
        HDF_LOGE("%s: get device success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetDevice006(void)
{
    const struct UsbFnDevice *device = NULL;
    const char *udcName = " 100e0000.hidwc3_0";
    device = UsbFnGetDevice(udcName);
    if (device != NULL) {
        HDF_LOGE("%s: get device success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetInterface(void)
{
    struct UsbFnInterface *fnInterface = NULL;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0);
    if (fnInterface == NULL) {
        HDF_LOGE("%s: get interface fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetInterface002(void)
{
    struct UsbFnInterface *fnInterface = NULL;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 1);
    if (fnInterface == NULL) {
        HDF_LOGE("%s: get interface fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetInterface003(void)
{
    struct UsbFnInterface *fnInterface = NULL;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        dprintf("%s, %d\n", __func__, __LINE__);
        return HDF_FAILURE;
    }
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0xA);
    if (fnInterface != NULL) {
        HDF_LOGE("%s: get interface success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetInterface004(void)
{
    struct UsbFnInterface *fnInterface = NULL;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, 0x20);
    if (fnInterface != NULL) {
        HDF_LOGE("%s: get interface success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetInterface005(void)
{
    struct UsbFnInterface *fnInterface = NULL;
    struct UsbFnDevice *fnDevice = NULL;
    fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(fnDevice, 0);
    if (fnInterface != NULL) {
        HDF_LOGE("%s: get interface success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetInterface006(void)
{
    struct UsbFnInterface *fnInterface = NULL;
    int32_t idCount;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is invail", __func__);
        return HDF_FAILURE;
    }
    for (idCount = 0; idCount < 0x2; idCount++) {
        fnInterface = (struct UsbFnInterface *)UsbFnGetInterface(g_acmDevice->fnDev, idCount);
        if (fnInterface == NULL) {
            HDF_LOGE("%s: get interface fail", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetPipeInfo(void)
{
    int32_t ret;
    struct UsbFnPipeInfo info;

    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 0, &info);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get pipe info error", __func__);
        return HDF_FAILURE;
    }
    if (info.id != 0) {
        HDF_LOGE("%s: get pipe id error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetPipeInfo002(void)
{
    int32_t ret;
    struct UsbFnPipeInfo info;

    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 1, &info);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get pipe info error", __func__);
        return HDF_FAILURE;
    }
    if (info.id != 1) {
        HDF_LOGE("%s: get pipe id error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetPipeInfo003(void)
{
    int32_t ret;
    struct UsbFnPipeInfo info;

    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 0xF, &info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get pipe info success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetPipeInfo004(void)
{
    int32_t ret;
    struct UsbFnPipeInfo *info = NULL;

    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->dataIface.fn, 0, info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get pipe info success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetPipeInfo005(void)
{
    int32_t ret;
    struct UsbFnPipeInfo info;
    struct UsbFnInterface *fn = NULL;

    ret = UsbFnGetInterfacePipeInfo(fn, 0, &info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get pipe info success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetPipeInfo006(void)
{
    int32_t ret;
    struct UsbFnPipeInfo info;

    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: dataIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfacePipeInfo(g_acmDevice->ctrlIface.fn, 0, &info);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get pipe info error", __func__);
        return HDF_FAILURE;
    }
    if (info.id != 0) {
        HDF_LOGE("%s: get pipe id error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t PropCallBack(const struct UsbFnInterface *intf, const char *name,
    const char *value)
{
    if (intf == NULL || name == NULL || value == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp(void)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Regist Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp002(void)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Regist Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp003(void)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    info.name = NULL;
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Regist Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp004(void)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    info.name = "name_test4";
    info.value = "test_value";
    info.getProp  = NULL;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, &info);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Regist Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp005(void)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    struct UsbFnInterface *fn = NULL;

    info.name = "name_test5";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(fn, &info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Regist Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp006(void)
{
    int32_t ret;
    struct UsbFnRegistInfo *info = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnRegistInterfaceProp(g_acmDevice->ctrlIface.fn, info);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Regist Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRegistProp007(void)
{
    int32_t ret;
    struct UsbFnRegistInfo info;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    info.name = "name_test";
    info.value = "test_value";
    info.getProp  = PropCallBack;
    info.setProp  = PropCallBack;
    ret = UsbFnRegistInterfaceProp(g_acmDevice->dataIface.fn, &info);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Regist Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "name_test", buff);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Get Prop error", __func__);
        return HDF_FAILURE;
    }
    if (strcmp(buffer, "test_value")) {
        HDF_LOGE("%s: Get Prop value error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp002(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "unknown", buff);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Get Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp003(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "idProduct", buff);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Get Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp004(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    struct UsbFnInterface *fn = NULL;

    ret = UsbFnGetInterfaceProp(fn, "idProduct", buff);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Get Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp005(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "idVendor", buff);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Get Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp006(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->ctrlIface.fn, "name_test", NULL);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Get Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp007(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->dataIface.fn, "name_test", buff);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Get Prop error", __func__);
        return HDF_FAILURE;
    }
    if (strcmp(buffer, "test_value")) {
        HDF_LOGE("%s: Get Prop value error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetProp008(void)
{
    int32_t ret;
    char buffer[BUFFER_LEN] = {0};
    char *buff = buffer;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetInterfaceProp(g_acmDevice->dataIface.fn, "idVendor", buff);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Get Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "name_test", "hello");
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Set Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp002(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "unknown", "hello");
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Set Prop success!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp003(void)
{
    int32_t ret;
    struct UsbFnInterface *fn = NULL;

    ret = UsbFnSetInterfaceProp(fn, "name_test", "hellotest");
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Set Prop success!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp004(void)
{
    int32_t ret;
    const char *propName = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, propName, "hellotest");
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Set Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp005(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "idVendor", "0x625");
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Set Prop error!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp006(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(g_acmDevice->ctrlIface.fn, "bLength", "0x14");
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: Set Prop success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestSetProp007(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(g_acmDevice->dataIface.fn, "name_test", "hello");
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: Set Prop error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->dataIface.handle,
        g_acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest002(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle,
        sizeof(struct UsbCdcLineCoding) + sizeof(struct UsbCdcLineCoding));
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest003(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, 0);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest004(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, 0x801);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest005(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    UsbFnInterfaceHandle handle = NULL;

    if (g_acmDevice == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(handle,
        g_acmDevice->notifyPipe.maxPacketSize);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest006(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, 0x800);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest007(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->dataIface.handle, 0);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocCtrlRequest008(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    UsbFnInterfaceHandle handle = NULL;

    req = UsbFnAllocCtrlRequest(handle, 0);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        sizeof(struct UsbCdcNotification));
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest002(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle,
        g_acmDevice->notifyPipe.id, 0);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest003(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle,
        g_acmDevice->notifyPipe.id, 0x800);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest004(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    UsbFnInterfaceHandle handle = NULL;

    if (g_acmDevice == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(handle, g_acmDevice->notifyPipe.id, 0x800);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest005(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, REQUEST_ALLOC_PIPE, REQUEST_ALLOC_LENGTH);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest006(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle,
        g_acmDevice->notifyPipe.id, 0x801);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest007(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle,
        g_acmDevice->dataOutPipe.id, 0);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest008(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    uint32_t length;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    length = g_acmDevice->dataOutPipe.maxPacketSize;
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, REQUEST_ALLOC_PIPE, length);
    if (req != NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestAllocRequest009(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle,
        g_acmDevice->dataOutPipe.id, g_acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req success!!", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestFreeRequest(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        g_acmDevice->notifyPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestFreeRequest002(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestFreeRequest003(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestFreeRequest004(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle,
        g_acmDevice->notifyPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestFreeRequest005(void)
{
    int32_t ret;
    int32_t count;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    for (count = 0; count < TEST_TIMES; count++) {
        req = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle,
            g_acmDevice->notifyPipe.maxPacketSize);
        if (req == NULL) {
            HDF_LOGE("%s: alloc req fail", __func__);
            return HDF_FAILURE;
        }
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestFreeRequest006(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    ret = UsbFnFreeRequest(req);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: free Request success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetRequestStatus(void)
{
    int32_t ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    notifyReq = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        sizeof(struct UsbCdcNotification));
    if (notifyReq == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get status error", __func__);
        return HDF_FAILURE;
    }
    if (!(status >= USB_REQUEST_COMPLETED && status <= USB_REQUEST_OVERFLOW)) {
        HDF_LOGE("%s: device status error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(notifyReq);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetRequestStatus002(void)
{
    int32_t ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = NULL;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    notifyReq = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle,
        sizeof(struct UsbCdcNotification));
    if (notifyReq == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get status error", __func__);
        dprintf("%s: get status error", __func__);
        return HDF_FAILURE;
    }
    if (!(status >= USB_REQUEST_COMPLETED && status <= USB_REQUEST_OVERFLOW)) {
        HDF_LOGE("%s: device status error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(notifyReq);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetRequestStatus003(void)
{
    int32_t ret;
    struct UsbFnRequest *req = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    if (req == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetRequestStatus(req, NULL);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get status success!!", __func__);
        ret = UsbFnFreeRequest(req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: free Request error", __func__);
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetRequestStatus004(void)
{
    int32_t ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = NULL;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    notifyReq = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    if (notifyReq == NULL) {
        HDF_LOGE("%s: alloc req fail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSubmitRequestAsync(notifyReq);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get status error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: get status error", __func__);
        return HDF_FAILURE;
    }
    if (!(status >= USB_REQUEST_COMPLETED && status <= USB_REQUEST_OVERFLOW)) {
        HDF_LOGE("%s: device status error", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnFreeRequest(notifyReq);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: free Request error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetRequestStatus005(void)
{
    int32_t ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = NULL;
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get status success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestGetRequestStatus006(void)
{
    int32_t ret;
    UsbRequestStatus *status = NULL;
    struct UsbFnRequest *notifyReq = NULL;
    ret = UsbFnGetRequestStatus(notifyReq, status);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: get status success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStopReceEvent(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: stop receive event error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStopReceEvent002(void)
{
    int32_t ret;
    struct UsbFnInterface *fn = NULL;
    ret = UsbFnStopRecvInterfaceEvent(fn);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: stop receive event success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStopReceEvent003(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: stop receive event success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStopReceEvent004(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->dataIface.fn);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: stop receive event success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void EventCallBack(struct UsbFnEvent *event)
{
    (void)event;
}

int32_t UsbFnDviceTestStartReceEvent(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff,
        NULL, g_acmDevice);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: start receive event successs!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStartReceEvent002(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0x0,
        EventCallBack, g_acmDevice);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: start receive event successs!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStartReceEvent003(void)
{
    int32_t ret;
    struct UsbFnInterface *fn = NULL;

    if (g_acmDevice == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStartRecvInterfaceEvent(fn, 0xff,
        EventCallBack, g_acmDevice);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: start receive event successs!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStartReceEvent004(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff,
        EventCallBack, g_acmDevice);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: start receive event error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStartReceEvent005(void)
{
    int32_t ret;
    int32_t count;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    for (count = 0; count < TEST_TIMES; count++) {
        ret = UsbFnStopRecvInterfaceEvent(g_acmDevice->ctrlIface.fn);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: stop receive event error", __func__);
            return HDF_FAILURE;
        }
        ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff,
            EventCallBack, g_acmDevice);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: stop receive event error", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStartReceEvent006(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->ctrlIface.fn, 0xff,
        EventCallBack, g_acmDevice);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: start receive event success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestStartReceEvent007(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnStartRecvInterfaceEvent(g_acmDevice->dataIface.fn, 0xff,
        EventCallBack, g_acmDevice);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: start receive event success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestOpenInterface(void)
{
    UsbFnInterfaceHandle handle;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    if (g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    handle = UsbFnOpenInterface(g_acmDevice->ctrlIface.fn);
    if (handle != NULL) {
        HDF_LOGE("%s: open interface success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestOpenInterface002(void)
{
    UsbFnInterfaceHandle handle;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    if (g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    handle = UsbFnOpenInterface(g_acmDevice->dataIface.fn);
    if (handle != NULL) {
        HDF_LOGE("%s: open interface success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestOpenInterface003(void)
{
    int32_t ret;
    UsbFnInterfaceHandle handle;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.fn == NULL) {
        HDF_LOGE("%s: ctrlIface.fn is invail", __func__);
        return HDF_FAILURE;
    }
    if (g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCloseInterface(g_acmDevice->ctrlIface.handle);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: close interface failed", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->ctrlIface.handle = NULL;
    handle = UsbFnOpenInterface(g_acmDevice->ctrlIface.fn);
    if (handle ==NULL) {
        HDF_LOGE("%s: open interface failed", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->ctrlIface.handle = handle;
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestOpenInterface004(void)
{
    int32_t ret;
    UsbFnInterfaceHandle handle;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.fn == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    if (g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCloseInterface(g_acmDevice->dataIface.handle);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: close interface failed", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->dataIface.handle = NULL;
    handle = UsbFnOpenInterface(g_acmDevice->dataIface.fn);
    if (handle == NULL) {
        HDF_LOGE("%s: open interface failed", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->dataIface.handle = handle;
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestOpenInterface005(void)
{
    UsbFnInterfaceHandle handle;
    struct UsbFnInterface *fn = NULL;

    handle = UsbFnOpenInterface(fn);
    if (handle != NULL) {
        HDF_LOGE("%s: open interface success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCloseInterface(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->ctrlIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCloseInterface(g_acmDevice->ctrlIface.handle);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: close interface error", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->ctrlIface.handle = NULL;

    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCloseInterface002(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->dataIface.handle == NULL) {
        HDF_LOGE("%s: ctrlIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCloseInterface(g_acmDevice->dataIface.handle);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: close interface error", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->dataIface.handle = NULL;

    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCloseInterface003(void)
{
    int32_t ret;

    if (g_acmDevice == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCloseInterface(g_acmDevice->ctrlIface.handle);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: close interface success!!", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->ctrlIface.handle = NULL;
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestCloseInterface004(void)
{
    int32_t ret;

    if (g_acmDevice == NULL) {
        HDF_LOGE("%s: dataIface.handle is invail", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnCloseInterface(g_acmDevice->dataIface.handle);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: close interface success!!", __func__);
        return HDF_FAILURE;
    }
    g_acmDevice->dataIface.handle = NULL;
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRemove(void)
{
    int32_t ret;
    struct UsbFnDevice *fnDev = NULL;

    ret = UsbFnRemoveDevice(fnDev);
    if (HDF_SUCCESS == ret) {
        HDF_LOGE("%s: UsbFnRemoveDevice success!!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbFnDviceTestRemove002(void)
{
    int32_t ret;
    if (g_acmDevice == NULL || g_acmDevice->fnDev == NULL) {
        HDF_LOGE("%s: fndev is null", __func__);
        return HDF_FAILURE;
    }

    ReleaseAcmDevice(g_acmDevice);
    ret = UsbFnRemoveDevice(g_acmDevice->fnDev);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: UsbFnRemoveDevice fail, ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    OsalMemFree(g_acmDevice);

    return HDF_SUCCESS;
}

