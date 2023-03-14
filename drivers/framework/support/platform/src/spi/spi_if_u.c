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
#include "spi_if.h"

#define HDF_LOG_TAG spi_if_u
#define HOST_NAME_LEN 32

struct SpiObject {
    struct SpiCntlr *cntlr;
    uint32_t csNum;
};

static struct HdfIoService *SpiGetCntlrByBusNum(uint32_t num)
{
    int ret;
    char *name = NULL;
    struct HdfIoService *service = NULL;

    name = (char *)OsalMemCalloc(HOST_NAME_LEN + 1);
    if (name == NULL) {
        return NULL;
    }
    ret = snprintf_s(name, HOST_NAME_LEN + 1, HOST_NAME_LEN, "HDF_PLATFORM_SPI_%u", num);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        OsalMemFree(name);
        return NULL;
    }
    service = HdfIoServiceBind(name);
    OsalMemFree(name);

    return service;
}

static int32_t SpiMsgWriteArray(struct SpiObject *object, struct HdfSBuf *data, struct SpiMsg *msgs, uint32_t count)
{
    uint32_t i;

    if (!HdfSbufWriteUint32(data, object->csNum)) {
        HDF_LOGE("%s: write csNum failed!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteBuffer(data, (uint8_t *)msgs, sizeof(*msgs) * count)) {
        HDF_LOGE("%s: write msgs array failed!", __func__);
        return HDF_ERR_IO;
    }

    for (i = 0; i < count; i++) {
        if (msgs[i].wbuf == NULL) {
            continue;
        }

        if (!HdfSbufWriteBuffer(data, (uint8_t *)msgs[i].wbuf, msgs[i].len)) {
            HDF_LOGE("%s: write msg[%d] buf failed!", __func__, i);
            return HDF_ERR_IO;
        }
    }

    return HDF_SUCCESS;
}

static int32_t SpiMsgReadBack(struct HdfSBuf *data, struct SpiMsg *msg)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if (!HdfSbufReadBuffer(data, &rBuf, &rLen)) {
        HDF_LOGE("%s: read rBuf failed!", __func__);
        return HDF_ERR_IO;
    }
    if (msg->len != rLen) {
        HDF_LOGE("%s: err len:%u, rLen:%u", __func__, msg->len, rLen);
        if (rLen > msg->len) {
            rLen = msg->len;
        }
    }
    if (memcpy_s(msg->rbuf, msg->len, rBuf, rLen) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t SpiMsgReadArray(struct HdfSBuf *reply, struct SpiMsg *msgs, uint32_t count)
{
    uint32_t i;
    int32_t ret;

    for (i = 0; i < count; i++) {
        if (msgs[i].rbuf == NULL) {
            continue;
        }
        ret = SpiMsgReadBack(reply, &msgs[i]);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    return HDF_SUCCESS;
}

int32_t SpiTransfer(DevHandle handle, struct SpiMsg *msgs, uint32_t count)
{
    int32_t ret;
    uint32_t i;
    uint32_t len = 0;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    struct SpiObject *object = NULL;

    if (handle == NULL || msgs == NULL) {
        HDF_LOGE("%s: invalid handle or msgs", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    object = (struct SpiObject *)handle;

    for (i = 0; i < count; i++) {
        len += ((msgs[i].wbuf == NULL) ? 0 : msgs[i].len) + sizeof(uint64_t) + sizeof(*msgs) + sizeof(uint32_t);
    }
    data = HdfSbufObtain(len);
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    for (i = 0; i < count; i++) {
        len += (msgs[i].rbuf == NULL) ? 0 : (msgs[i].len + sizeof(uint64_t));
    }
    reply = (len == 0) ? HdfSbufObtainDefaultSize() : HdfSbufObtain(len);
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply!", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    ret = SpiMsgWriteArray(object, data, msgs, count);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to write msgs!", __func__);
        goto EXIT;
    }

    service = (struct HdfIoService *)object->cntlr;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret =  HDF_FAILURE;
        goto EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_TRANSFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to send service call:%d", __func__, ret);
        goto EXIT;
    }

    ret = SpiMsgReadArray(reply, msgs, count);
    if (ret != HDF_SUCCESS) {
        goto EXIT;
    }

    ret = HDF_SUCCESS;
EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t SpiRead(DevHandle handle, uint8_t *buf, uint32_t len)
{
    struct SpiMsg msg = {0};

    msg.wbuf = NULL;
    msg.rbuf = buf;
    msg.len = len;
    return SpiTransfer(handle, &msg, 1);
}

int32_t SpiWrite(DevHandle handle, uint8_t *buf, uint32_t len)
{
    struct SpiMsg msg = {0};

    msg.wbuf = buf;
    msg.rbuf = NULL;
    msg.len = len;
    return SpiTransfer(handle, &msg, 1);
}

int32_t SpiSetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL || cfg == NULL) {
        HDF_LOGE("%s: invalid handle", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    object = (struct SpiObject *)handle;
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, object->csNum)) {
        HDF_LOGE("%s: write csNum failed!", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteBuffer(data, cfg, sizeof(*cfg))) {
        HDF_LOGE("%s: write cfg failed!", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)object->cntlr;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_SET_CONFIG, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }
    HdfSbufRecycle(data);

    return HDF_SUCCESS;
}

int32_t SpiGetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    int32_t ret;
    uint32_t len;
    const void *rBuf = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct SpiObject *object = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL  || cfg == NULL) {
        HDF_LOGE("%s: invalid handle", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    object = (struct SpiObject *)handle;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteUint32(data, object->csNum)) {
        HDF_LOGE("%s: write csNum failed!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    service = (struct HdfIoService *)object->cntlr;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_GET_CONFIG, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        goto EXIT;
    }
    if (!HdfSbufReadBuffer(reply, &rBuf, &len) || rBuf == NULL) {
        HDF_LOGE("%s: read buffer failed", __func__);
        goto EXIT;
    }
    if (memcpy_s(cfg, sizeof(struct SpiCfg), rBuf, len) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }
EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

void SpiClose(DevHandle handle)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return;
    }
    object = (struct SpiObject *)handle;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        goto EXIT;
    }
    if (!HdfSbufWriteUint32(data, object->csNum)) {
        HDF_LOGE("%s: write csNum failed!", __func__);
        goto EXIT;
    }

    service = (struct HdfIoService *)object->cntlr;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        goto EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
    }
EXIT:
    HdfSbufRecycle(data);
    HdfIoServiceRecycle(service);
    OsalMemFree(object);
}

DevHandle SpiOpen(const struct SpiDevInfo *info)
{
    int32_t ret;
    struct SpiObject *object = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (info == NULL) {
        HDF_LOGE("%s: error, info is NULL", __func__);
        return NULL;
    }
    service = SpiGetCntlrByBusNum(info->busNum);
    if (service == NULL) {
        HDF_LOGE("%s: service is null", __func__);
        return NULL;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        HdfIoServiceRecycle(service);
        return NULL;
    }
    if (!HdfSbufWriteUint32(data, info->csNum)) {
        HDF_LOGE("%s: write csNum failed!", __func__);
        HdfSbufRecycle(data);
        HdfIoServiceRecycle(service);
        return NULL;
    }

    if (service == NULL ||service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        HdfIoServiceRecycle(service);
        return NULL;
    }
    ret = service->dispatcher->Dispatch(&service->object, SPI_IO_OPEN, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        HdfIoServiceRecycle(service);
        return NULL;
    }

    object = (struct SpiObject *)OsalMemCalloc(sizeof(*object));
    if (object == NULL) {
        HDF_LOGE("%s: object malloc failed", __func__);
        HdfSbufRecycle(data);
        HdfIoServiceRecycle(service);
        return NULL;
    }
    object->cntlr = (struct SpiCntlr *)service;
    object->csNum = info->csNum;
    HdfSbufRecycle(data);
    return (DevHandle)object;
}
