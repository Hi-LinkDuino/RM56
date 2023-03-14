/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "uart_if.h"

#define HDF_LOG_TAG uart_if_u_c
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

    return (void *)HdfIoServiceBind(name);
}

static void UartPutObjByPointer(const void *obj)
{
    if (obj == NULL) {
        return;
    }
    HdfIoServiceRecycle((struct HdfIoService *)obj);
};

DevHandle UartOpen(uint32_t port)
{
    int32_t ret;
    void *handle = NULL;

    handle = UartGetObjGetByBusNum(port);
    if (handle == NULL) {
        HDF_LOGE("%s: get handle error", __func__);
        return NULL;
    }

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        UartPutObjByPointer(handle);
        return NULL;
    }
    ret = service->dispatcher->Dispatch(&service->object, UART_IO_REQUEST, NULL, NULL);
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

    struct HdfIoService *service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        UartPutObjByPointer(handle);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, UART_IO_RELEASE, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartHostRelease error, ret %d", __func__, ret);
    }
    UartPutObjByPointer(handle);
}

static int32_t UartDispatch(DevHandle handle, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    if (service == NULL) {
        HDF_LOGE("%s: service is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: dispatcher is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = service->dispatcher->Dispatch(&service->object, cmd, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Dispatch failed: %d", __func__, ret);
    }
    return ret;
}

int32_t UartRead(DevHandle handle, uint8_t *buf, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t tmpLen;
    const void *tmpBuf = NULL;

    if (buf == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, len)) {
        HDF_LOGE("%s: write read size failed!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    reply = HdfSbufObtain(len + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply buf", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UartDispatch(handle, UART_IO_READ, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartDispatch failed: %d", __func__, ret);
        goto EXIT;
    }

    if (!HdfSbufReadBuffer(reply, &tmpBuf, &tmpLen)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    if (tmpLen > 0 && memcpy_s(buf, len, tmpBuf, tmpLen) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    ret = tmpLen;

EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t UartWrite(DevHandle handle, uint8_t *buf, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (buf == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain write buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteBuffer(data, buf, len)) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = UartDispatch(handle, UART_IO_WRITE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartDispatch failed: %d", __func__, ret);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t UartGetBaud(DevHandle handle, uint32_t *baudRate)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;

    if (baudRate == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UartDispatch(handle, UART_IO_GET_BAUD, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to write, ret %d", __func__, ret);
        HdfSbufRecycle(reply);
        return ret;
    }

    if (!HdfSbufReadUint32(reply, baudRate)) {
        HDF_LOGE("%s: read from reply failed", __func__);
        ret = HDF_ERR_IO;
    }

    HdfSbufRecycle(reply);
    return ret;
}

int32_t UartSetBaud(DevHandle handle, uint32_t baudRate)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, baudRate)) {
        HDF_LOGE("%s: sbuf write baud rate failed", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = UartDispatch(handle, UART_IO_SET_BAUD, data, NULL);
    HdfSbufRecycle(data);
    return ret;
}

int32_t UartGetAttribute(DevHandle handle, struct UartAttribute *attribute)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    uint32_t tmpLen;
    const void *tmpBuf = NULL;

    if (attribute == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UartDispatch(handle, UART_IO_GET_ATTRIBUTE, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartDispatch failed: %d", __func__, ret);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadBuffer(reply, &tmpBuf, &tmpLen)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (tmpLen != sizeof(*attribute)) {
        HDF_LOGE("%s: reply data len not match, exp:%zu, got:%u", __func__, sizeof(*attribute), tmpLen);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(attribute, sizeof(*attribute), tmpBuf, tmpLen) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfSbufRecycle(reply);
    return HDF_SUCCESS;
}

int32_t UartSetAttribute(DevHandle handle, struct UartAttribute *attribute)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    if (attribute == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain write buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteBuffer(data, (void *)attribute, sizeof(*attribute))) {
        HDF_LOGE("%s: sbuf write attribute failed", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = UartDispatch(handle, UART_IO_SET_ATTRIBUTE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UartDispatch failed: %d", __func__, ret);
    }
    HdfSbufRecycle(data);
    return ret;
}

int32_t UartSetTransMode(DevHandle handle, enum UartTransMode mode)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)mode)) {
        HDF_LOGE("%s: sbuf write mode failed", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = UartDispatch(handle, UART_IO_SET_TRANSMODE, data, NULL);
    HdfSbufRecycle(data);
    return ret;
}
