/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "light_controller.h"
#include <fcntl.h>
#include <stdio.h>
#include "hdf_base.h"
#include "hdf_dlist.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "light_if.h"
#include "light_type.h"
#include "osal_mem.h"

#define HDF_LOG_TAG           hdf_light_dal
#define LIGHT_SERVICE_NAME    "hdf_light"

static struct LightDevice *GetLightDevicePriv(void)
{
    static struct LightDevice lightDeviceData = {
        .initState = false,
        .lightNum = 0,
        .ioService = NULL,
        .lightInfoEntry = NULL,
    };

    return &lightDeviceData;
}

static int32_t SendLightMsg(uint32_t cmd, struct HdfSBuf *msg, struct HdfSBuf *reply)
{
    struct LightDevice *priv = GetLightDevicePriv();

    if (priv->ioService == NULL || priv->ioService->dispatcher == NULL ||
        priv->ioService->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: para invalid", __func__);
        return HDF_FAILURE;
    }

    int32_t ret = priv->ioService->dispatcher->Dispatch(&priv->ioService->object, cmd, msg, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: Light dispatch failed", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t ReadLightInfo(struct HdfSBuf *reply, struct LightDevice *priv)
{
    uint32_t len;
    struct LightInfo *pos = NULL;
    struct LightInfo *buf = NULL;

    if (!HdfSbufReadUint32(reply, &priv->lightNum)) {
        HDF_LOGE("%s: sbuf read lightNum failed", __func__);
        return HDF_FAILURE;
    }

    if (priv->lightInfoEntry != NULL) {
        OsalMemFree(priv->lightInfoEntry);
        priv->lightInfoEntry = NULL;
    }

    priv->lightInfoEntry = (struct LightInfo *)OsalMemCalloc(sizeof(*priv->lightInfoEntry) * priv->lightNum);
    if (priv->lightInfoEntry == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return HDF_FAILURE;
    }

    pos = priv->lightInfoEntry;

    for (uint32_t i = 0; i < priv->lightNum; ++i) {
        if (!HdfSbufReadBuffer(reply, (const void **)&buf, &len)) {
            return HDF_FAILURE;
        }

        pos->lightType = buf->lightType;
        pos->reserved = buf->reserved;
        pos++;
    }

    return HDF_SUCCESS;
}

static int32_t GetLightInfo(struct LightInfo **lightInfo, uint32_t *count)
{
    if ((lightInfo == NULL) || (count == NULL)) {
        HDF_LOGE("%s:line:%{public}d pointer is null and return ret", __func__, __LINE__);
        return HDF_FAILURE;
    }

    struct LightDevice *priv = GetLightDevicePriv();

    if (priv->lightNum > 0) {
        *count = priv->lightNum;
        *lightInfo = priv->lightInfoEntry;
        return HDF_SUCCESS;
    }

    (void)OsalMutexLock(&priv->mutex);
    struct HdfSBuf *reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: get sbuf failed", __func__);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    int32_t ret = SendLightMsg(LIGHT_IO_CMD_GET_INFO_LIST, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: Light get light type failed, ret[%{public}d]", __func__, ret);
        HdfSbufRecycle(reply);
        (void)OsalMutexUnlock(&priv->mutex);
        return ret;
    }

    if (ReadLightInfo(reply, priv) != HDF_SUCCESS) {
        HdfSbufRecycle(reply);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    HdfSbufRecycle(reply);
    (void)OsalMutexUnlock(&priv->mutex);

    *count = priv->lightNum;
    *lightInfo = priv->lightInfoEntry;

    return HDF_SUCCESS;
}

static int32_t ValidityJudgment(uint32_t type, struct LightEffect *effect)
{
    if (type >= LIGHT_TYPE_BUTT) {
        HDF_LOGE("%{public}s: type not supported", __func__);
        return LIGHT_NOT_SUPPORT;
    }

    if (effect->flashEffect.flashMode > LIGHT_FLASH_TIMED) {
        HDF_LOGE("%{public}s: flashMode not supported", __func__);
        return LIGHT_NOT_FLASH;
    }

    if ((effect->flashEffect.flashMode == LIGHT_FLASH_TIMED) && (effect->flashEffect.onTime == 0 ||
        effect->flashEffect.offTime == 0)) {
        HDF_LOGE("%{public}s: flashMode not supported", __func__);
        return LIGHT_NOT_FLASH;
    }

    return LIGHT_SUCCESS;
}

static int32_t OnLight(uint32_t type, struct LightEffect *effect)
{
    int32_t ret;

    if (effect == NULL) {
        HDF_LOGE("%{public}s: effect is NULL", __func__);
        return HDF_FAILURE;
    }

    ret = ValidityJudgment(type, effect);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: effect is false", __func__);
        return ret;
    }

    struct LightDevice *priv = GetLightDevicePriv();
    (void)OsalMutexLock(&priv->mutex);

    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain sBuf size", __func__);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, type)) {
        HDF_LOGE("%{public}s: Light write id failed", __func__);
        HdfSbufRecycle(msg);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, LIGHT_OPS_IO_CMD_ENABLE)) {
        HDF_LOGE("%{public}s: Light write enable failed", __func__);
        HdfSbufRecycle(msg);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteBuffer(msg, effect, sizeof(*effect))) {
        HDF_LOGE("%{public}s: Light write enable failed", __func__);
        HdfSbufRecycle(msg);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    ret = SendLightMsg(LIGHT_IO_CMD_OPS, msg, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: Light enable failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);
    (void)OsalMutexUnlock(&priv->mutex);

    return ret;
}

static int32_t OffLight(uint32_t type)
{
    if (type >= LIGHT_TYPE_BUTT) {
        HDF_LOGE("%{public}s: type not supported", __func__);
        return HDF_FAILURE;
    }

    struct LightDevice *priv = GetLightDevicePriv();
    (void)OsalMutexLock(&priv->mutex);

    struct HdfSBuf *msg = HdfSbufObtainDefaultSize();
    if (msg == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain sBuf", __func__);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, type)) {
        HDF_LOGE("%{public}s: Light write id failed", __func__);
        HdfSbufRecycle(msg);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(msg, LIGHT_OPS_IO_CMD_DISABLE)) {
        HDF_LOGE("%{public}s: Light write disable failed", __func__);
        HdfSbufRecycle(msg);
        (void)OsalMutexUnlock(&priv->mutex);
        return HDF_FAILURE;
    }

    int32_t ret = SendLightMsg(LIGHT_IO_CMD_OPS, msg, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: Light disable failed, ret[%{public}d]", __func__, ret);
    }
    HdfSbufRecycle(msg);
    (void)OsalMutexUnlock(&priv->mutex);

    return ret;
}

const struct LightInterface *NewLightInterfaceInstance(void)
{
    static struct LightInterface lightDevInstance;
    struct LightDevice *priv = GetLightDevicePriv();

    if (priv->initState) {
        return &lightDevInstance;
    }

    OsalMutexInit(&priv->mutex);
    lightDevInstance.GetLightInfo = GetLightInfo;
    lightDevInstance.TurnOnLight = OnLight;
    lightDevInstance.TurnOffLight = OffLight;

    priv->ioService = HdfIoServiceBind(LIGHT_SERVICE_NAME);
    if (priv->ioService == NULL) {
        HDF_LOGE("%s: get light ioService failed", __func__);
        OsalMutexDestroy(&priv->mutex);
        return NULL;
    }

    priv->initState = true;
    HDF_LOGI("get light devInstance success");

    return &lightDevInstance;
}

int32_t FreeLightInterfaceInstance(void)
{
    struct LightDevice *priv = GetLightDevicePriv();

    if (!priv->initState) {
        HDF_LOGI("%s: light instance had released", __func__);
        return HDF_SUCCESS;
    }

    priv->lightNum = 0;

    if (priv->ioService != NULL) {
        HdfIoServiceRecycle(priv->ioService);
    }

    if (priv->lightInfoEntry != NULL) {
        OsalMemFree(priv->lightInfoEntry);
        priv->lightInfoEntry = NULL;
    }

    OsalMutexDestroy(&priv->mutex);

    return HDF_SUCCESS;
}