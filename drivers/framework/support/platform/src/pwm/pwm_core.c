/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_core.h"
#include "hdf_log.h"
#include "securec.h"

#define HDF_LOG_TAG pwm_core

int32_t PwmDeviceGet(struct PwmDev *pwm)
{
    int32_t ret;

    if (pwm == NULL) {
        HDF_LOGE("%s pwm is null\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLock(&(pwm->lock));
    if (pwm->busy) {
        (void)OsalSpinUnlock(&(pwm->lock));
        HDF_LOGE("%s: pwm%u is busy", __func__, pwm->num);
        return HDF_ERR_DEVICE_BUSY;
    }
    if (pwm->method != NULL && pwm->method->open != NULL) {
        ret = pwm->method->open(pwm);
        if (ret != HDF_SUCCESS) {
            (void)OsalSpinUnlock(&(pwm->lock));
            HDF_LOGE("%s: open failed, ret %d", __func__, ret);
            return HDF_FAILURE;
        }
    }

    pwm->busy = true;
    (void)OsalSpinUnlock(&(pwm->lock));
    return HDF_SUCCESS;
}

int32_t PwmDevicePut(struct PwmDev *pwm)
{
    int32_t ret;

    if (pwm == NULL) {
        HDF_LOGE("%s pwm is null\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (pwm->method != NULL && pwm->method->close != NULL) {
        ret = pwm->method->close(pwm);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: close failed, ret %d", __func__, ret);
            return ret;
        }
    }
    (void)OsalSpinLock(&(pwm->lock));
    pwm->busy = false;
    (void)OsalSpinUnlock(&(pwm->lock));
    return HDF_SUCCESS;
}

int32_t PwmDeviceSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    int32_t ret;

    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s param is null\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (memcmp(config, &(pwm->cfg), sizeof(*config)) == 0) {
        HDF_LOGE("%s: do not need to set config", __func__);
        return HDF_SUCCESS;
    }

    if (pwm->method == NULL || pwm->method->setConfig == NULL) {
        HDF_LOGE("%s: setConfig is not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = pwm->method->setConfig(pwm, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed, ret %d", __func__, ret);
        return ret;
    }

    (void)OsalSpinLock(&(pwm->lock));
    pwm->cfg = *config;
    (void)OsalSpinUnlock(&(pwm->lock));
    return HDF_SUCCESS;
}

int32_t PwmDeviceGetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s param is null\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLock(&(pwm->lock));
    *config = pwm->cfg;
    (void)OsalSpinUnlock(&(pwm->lock));
    return HDF_SUCCESS;
}

int32_t PwmSetPriv(struct PwmDev *pwm, void *priv)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSpinLock(&(pwm->lock));
    pwm->priv = priv;
    (void)OsalSpinUnlock(&(pwm->lock));
    return HDF_SUCCESS;
}

void *PwmGetPriv(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: pwm is null", __func__);
        return NULL;
    }
    return pwm->priv;
}

static int32_t PwmUserSetConfig(struct PwmDev *pwm, struct HdfSBuf *data)
{
    size_t size;
    struct PwmConfig *config = NULL;

    if (data == NULL) {
        HDF_LOGE("%s: data null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadBuffer(data, (const void **)&config, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }

    if ((config == NULL) || (size != sizeof(struct PwmConfig))) {
        HDF_LOGE("%s: read buff error", __func__);
        return HDF_ERR_IO;
    }
    return PwmDeviceSetConfig(pwm, config);
}

static int32_t PwmUserGetConfig(struct PwmDev *pwm, struct HdfSBuf *reply)
{
    int32_t ret;
    struct PwmConfig config;

    if (reply == NULL) {
        HDF_LOGE("%s: reply null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = PwmDeviceGetConfig(pwm, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get config fail", __func__);
        return ret;
    }

    if (!HdfSbufWriteBuffer(reply, &config, sizeof(config))) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t PwmIoDispatch(struct HdfDeviceIoClient *client, int cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct PwmDev *pwm = NULL;

    if (client == NULL || client->device == NULL || client->device->service == NULL) {
        HDF_LOGE("%s: client info is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    pwm = (struct PwmDev *)client->device->service;
    switch (cmd) {
        case PWM_IO_GET:
            return PwmDeviceGet(pwm);
        case PWM_IO_PUT:
            return PwmDevicePut(pwm);
        case PWM_IO_SET_CONFIG:
            return PwmUserSetConfig(pwm, data);
        case PWM_IO_GET_CONFIG:
            return PwmUserGetConfig(pwm, reply);
        default:
            HDF_LOGE("%s: cmd %d not support", __func__, cmd);
            return HDF_ERR_NOT_SUPPORT;
    }
}

int32_t PwmDeviceAdd(struct HdfDeviceObject *obj, struct PwmDev *pwm)
{
    if (obj == NULL || pwm == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (pwm->method == NULL || pwm->method->setConfig == NULL) {
        HDF_LOGE("%s: setConfig is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (OsalSpinInit(&(pwm->lock)) != HDF_SUCCESS) {
        HDF_LOGE("%s: init spinlock fail", __func__);
        return HDF_FAILURE;
    }
    pwm->device = obj;
    obj->service = &(pwm->service);
    pwm->device->service->Dispatch = PwmIoDispatch;
    return HDF_SUCCESS;
}

void PwmDeviceRemove(struct HdfDeviceObject *obj, struct PwmDev *pwm)
{
    if (obj == NULL || pwm == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return;
    }
    (void)OsalSpinDestroy(&(pwm->lock));
    pwm->device = NULL;
    obj->service = NULL;
}
