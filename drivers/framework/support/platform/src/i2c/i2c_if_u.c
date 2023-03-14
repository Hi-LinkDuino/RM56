/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i2c_if.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG i2c_if_u

#define I2C_SERVICE_NAME "HDF_PLATFORM_I2C_MANAGER"

static void *I2cManagerGetService(void)
{
    static void *manager = NULL;

    if (manager != NULL) {
        return manager;
    }
    manager = (void *)HdfIoServiceBind(I2C_SERVICE_NAME);
    if (manager == NULL) {
        HDF_LOGE("I2cManagerGetService: fail to get i2c manager!");
    }
    return manager;
}

DevHandle I2cOpen(int16_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle = 0;

    service = (struct HdfIoService *)I2cManagerGetService();
    if (service == NULL ||service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("I2cOpen: service is invalid!");
        return NULL;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("I2cOpen: obtain data fail!");
        return NULL;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("I2cOpen: obtain reply fail!");
        HdfSbufRecycle(data);
        return NULL;
    }

    if (!HdfSbufWriteUint16(data, (uint16_t)number)) {
        HDF_LOGE("I2cOpen: write number fail!");
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, I2C_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("I2cOpen: service call open fail:%d", ret);
        goto EXIT;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("I2cOpen: read handle fail!");
        goto EXIT;
    }
EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return (handle == 0) ? NULL : (DevHandle)(uintptr_t)handle;
}

void I2cClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    service = (struct HdfIoService *)I2cManagerGetService();
    if (service == NULL ||service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("I2cOpen: service is invalid!");
        return;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("I2cClose: write handle fail!");
        HdfSbufRecycle(data);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, I2C_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("I2cClose: close handle fail:%d", ret);
    }
    HdfSbufRecycle(data);
}

static int32_t I2cMsgWriteArray(DevHandle handle, struct HdfSBuf *data, struct I2cMsg *msgs, int16_t count)
{
    int16_t i;

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("I2cMsgWriteArray: write handle fail!");
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteBuffer(data, (uint8_t *)msgs, sizeof(*msgs) * count)) {
        HDF_LOGE("I2cMsgWriteArray: write msgs array fail!");
        return HDF_ERR_IO;
    }

    for (i = 0; i < count; i++) {
        if ((msgs[i].flags & I2C_FLAG_READ) != 0) {
            continue;
        }
        if (!HdfSbufWriteBuffer(data, (uint8_t *)msgs[i].buf, msgs[i].len)) {
            HDF_LOGE("I2cMsgWriteArray: write msg[%d] buf fail!", i);
            return HDF_ERR_IO;
        }
    }

    return HDF_SUCCESS;
}

static int32_t I2cMsgReadBack(struct HdfSBuf *data, struct I2cMsg *msg)
{
    uint32_t rLen;
    const void *rBuf = NULL;

    if ((msg->flags & I2C_FLAG_READ) == 0) {
        return HDF_SUCCESS; /* write msg no need to read back */
    }

    if (!HdfSbufReadBuffer(data, &rBuf, &rLen)) {
        HDF_LOGE("I2cMsgReadBack: read rBuf fail!");
        return HDF_ERR_IO;
    }
    if (msg->len != rLen) {
        HDF_LOGE("I2cMsgReadBack: err len:%u, rLen:%u", msg->len, rLen);
        if (rLen > msg->len) {
            rLen = msg->len;
        }
    }
    if (memcpy_s(msg->buf, msg->len, rBuf, rLen) != EOK) {
        HDF_LOGE("I2cMsgReadBack: memcpy rBuf fail!");
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static inline int32_t I2cMsgReadArray(struct HdfSBuf *reply, struct I2cMsg *msgs, int16_t count)
{
    int16_t i;
    int32_t ret;

    for (i = 0; i < count; i++) {
        ret = I2cMsgReadBack(reply, &msgs[i]);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t I2cServiceTransfer(DevHandle handle, struct I2cMsg *msgs, int16_t count)
{
    int16_t i;
    int32_t ret;
    uint32_t recvLen = 0;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("I2cServiceTransfer: failed to obtain data!");
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = I2cMsgWriteArray(handle, data, msgs, count);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("I2cServiceTransfer: failed to write msgs!");
        goto EXIT;
    }

    for (i = 0; i < count; i++) {
        recvLen += ((msgs[i].flags & I2C_FLAG_READ) == 0) ? 0 : (msgs[i].len + sizeof(uint64_t));
    }
    reply = (recvLen == 0) ? HdfSbufObtainDefaultSize() : HdfSbufObtain(recvLen);
    if (reply == NULL) {
        HDF_LOGE("I2cServiceTransfer: failed to obtain reply!");
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    service = (struct HdfIoService *)I2cManagerGetService();
    if (service == NULL ||service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        ret = HDF_ERR_NOT_SUPPORT;
        goto EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, I2C_IO_TRANSFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("I2cServiceTransfer: failed to send service call:%d", ret);
        goto EXIT;
    }

    ret = I2cMsgReadArray(reply, msgs, count);
    if (ret != HDF_SUCCESS) {
        goto EXIT;
    }

    ret = count;
EXIT:
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return ret;
}

int32_t I2cTransfer(DevHandle handle, struct I2cMsg *msgs, int16_t count)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (msgs == NULL || count <= 0) {
        HDF_LOGE("I2cTransfer: err params! msgs:%s, count:%d",
            (msgs == NULL) ? "0" : "x", count);
        return HDF_ERR_INVALID_PARAM;
    }

    return I2cServiceTransfer(handle, msgs, count);
}

