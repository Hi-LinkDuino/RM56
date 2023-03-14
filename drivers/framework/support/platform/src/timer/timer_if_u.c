/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "timer_if.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG timer_if_u

#define TIMER_SERVICE_NAME "HDF_PLATFORM_TIMER_MANAGER"
static void *TimerManagerGetService(void)
{
    static void *manager = NULL;

    if (manager != NULL) {
        return manager;
    }
    manager = (void *)HdfIoServiceBind(TIMER_SERVICE_NAME);
    if (manager == NULL) {
        HDF_LOGE("%s: fail to get timer manager!", __func__);
    }
    return manager;
}

DevHandle HwTimerOpen(const uint32_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle;

    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return NULL;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return NULL;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HdfSbufRecycle(data);
        return NULL;
    }

    if (!HdfSbufWriteUint16(data, (uint16_t)number)) {
        HDF_LOGE("%s: write number fail!", __func__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return NULL;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_OPEN service process fail:%d", __func__, ret);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return NULL;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("%s: read reply fail!", __func__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return NULL;
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return (DevHandle)(uintptr_t)handle;
}

void HwTimerClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return;
    }

    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: write handle fail!", __func__);
        HdfSbufRecycle(data);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_CLOSE service process fail:%d", __func__, ret);
    }
    HdfSbufRecycle(data);
}

int32_t HwTimerStart(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: HdfSBufObtainDefaultSize fail!", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: write handle fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_START, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_START service process fail:%d", __func__, ret);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(data);

    return HDF_SUCCESS;
}

int32_t HwTimerStop(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: HdfSbufObtainDefaultSize fail!", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: write handle fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_STOP, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_STOP service process fail:%d", __func__, ret);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(data);

    return HDF_SUCCESS;
}

int32_t HwTimerSet(DevHandle handle, uint32_t useconds, TimerHandleCb cb)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *buf = NULL;
    struct TimerConfig cfg;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL
        || service->dispatcher->Dispatch == NULL || cb == NULL) {
        HDF_LOGE("%s:param is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (memset_s(&cfg, sizeof(cfg), 0, sizeof(cfg)) != EOK) {
        HDF_LOGE("%s:memset_s FAIL", __func__);
        return HDF_ERR_IO;
    }
    cfg.useconds = useconds;

    buf = HdfSbufObtainDefaultSize();
    if (buf == NULL) {
        HDF_LOGE("%s: failed to obtain buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteUint32(buf, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: sbuf write handle failed", __func__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufWriteBuffer(buf, &cfg, sizeof(cfg))) {
        HDF_LOGE("%s: sbuf write cfg failed", __func__);
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_SET, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_SET service process fail:%d", __func__, ret);
        HdfSbufRecycle(buf);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(buf);

    return HDF_SUCCESS;
}

int32_t HwTimerSetOnce(DevHandle handle, uint32_t useconds, TimerHandleCb cb)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *buf = NULL;
    struct TimerConfig cfg;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL
        || service->dispatcher->Dispatch == NULL || cb == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (memset_s(&cfg, sizeof(cfg), 0, sizeof(cfg)) != EOK) {
        HDF_LOGE("%s:memset_s FAIL", __func__);
        return HDF_ERR_IO;
    }
    cfg.useconds = useconds;

    buf = HdfSbufObtainDefaultSize();
    if (buf == NULL) {
        HDF_LOGE("%s: failed to obtain buf", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfSbufWriteUint32(buf, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: sbuf write handle failed", __func__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufWriteBuffer(buf, &cfg, sizeof(cfg))) {
        HDF_LOGE("%s: sbuf write cfg failed", __func__);
        HdfSbufRecycle(buf);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_SETONCE, buf, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_SETONCE service process fail:%d", __func__, ret);
        HdfSbufRecycle(buf);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(buf);

    return HDF_SUCCESS;
}

int32_t HwTimerGet(DevHandle handle, uint32_t *useconds, bool *isPeriod)
{
    int32_t ret = HDF_SUCCESS;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    const void *rBuf = NULL;
    struct HdfSBuf *reply = NULL;
    struct TimerConfig cfg;
    uint32_t rLen;

    if ((handle == NULL) || (useconds == NULL) || (isPeriod == NULL)) {
        HDF_LOGE("%s: param is invalid", __func__);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)TimerManagerGetService();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: HdfSBufObtainDefaultSize fail!", __func__);
        return HDF_FAILURE;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto __EXIT;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: write handle fail!", __func__);
        ret =  HDF_FAILURE;
        goto __EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, TIMER_IO_GET, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: TIMER_IO_GET service process fail:%d", __func__, ret);
        ret =  HDF_FAILURE;
        goto __EXIT;
    }

    if (!HdfSbufReadBuffer(reply, &rBuf, &rLen)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        ret = HDF_ERR_IO;
        goto __EXIT;
    }
    if (rLen != sizeof(struct TimerConfig)) {
        HDF_LOGE("%s: sbuf read buffer len error %u != %zu", __func__, rLen, sizeof(struct TimerConfig));
        ret = HDF_FAILURE;
        goto __EXIT;
    }
    if (memcpy_s(&cfg, sizeof(struct TimerConfig), rBuf, rLen) != EOK) {
        HDF_LOGE("%s: memcpy rBuf failed", __func__);
        ret = HDF_ERR_IO;
        goto __EXIT;
    }

    *useconds = cfg.useconds;
    *isPeriod = cfg.isPeriod;

__EXIT:
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return ret;
}
