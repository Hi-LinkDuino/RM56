/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_if.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "uart_core.h"

#define HDF_LOG_TAG uart_if_c
#define UART_HOST_NAME_LEN 32

static void *UartGetObjGetByBusNum(uint32_t num)
{
    int ret;
    char name[UART_HOST_NAME_LEN + 1] = {0};

    ret = snprintf_s(name, UART_HOST_NAME_LEN + 1, UART_HOST_NAME_LEN, "HDF_PLATFORM_UART_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return NULL;
    }

    return (void *)DevSvcManagerClntGetService(name);
}

static void UartPutObjByPointer(const void *obj)
{
    if (obj == NULL) {
        return;
    }
}

DevHandle UartOpen(uint32_t port)
{
    int32_t ret;
    void *handle = NULL;

    handle = UartGetObjGetByBusNum(port);
    if (handle == NULL) {
        HDF_LOGE("%s: get handle error", __func__);
        return NULL;
    }
    ret = UartHostRequest((struct UartHost *)handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartHostRequest error, ret %d", __func__, ret);
        UartPutObjByPointer(handle);
        return NULL;
    }
    return (DevHandle)handle;
}

void UartClose(DevHandle handle)
{
    int32_t ret;
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return;
    }
    ret = UartHostRelease((struct UartHost *)handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartHostRelease error, ret %d", __func__, ret);
    }
    UartPutObjByPointer(handle);
}

int32_t UartRead(DevHandle handle, uint8_t *data, uint32_t size)
{
    return UartHostRead((struct UartHost *)handle, data, size);
}

int32_t UartWrite(DevHandle handle, uint8_t *data, uint32_t size)
{
    return UartHostWrite((struct UartHost *)handle, data, size);
}

int32_t UartGetBaud(DevHandle handle, uint32_t *baudRate)
{
    return UartHostGetBaud((struct UartHost *)handle, baudRate);
}

int32_t UartSetBaud(DevHandle handle, uint32_t baudRate)
{
    return UartHostSetBaud((struct UartHost *)handle, baudRate);
}

int32_t UartGetAttribute(DevHandle handle, struct UartAttribute *attribute)
{
    return UartHostGetAttribute((struct UartHost *)handle, attribute);
}

int32_t UartSetAttribute(DevHandle handle, struct UartAttribute *attribute)
{
    return UartHostSetAttribute((struct UartHost *)handle, attribute);
}

int32_t UartSetTransMode(DevHandle handle, enum UartTransMode mode)
{
    return UartHostSetTransMode((struct UartHost *)handle, mode);
}
