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
#include "input_common.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include <hdf_device_desc.h>
#include <stdlib.h>
#include <securec.h>

#define DEV_MANAGER_SERVICE_NAME "hdf_input_host"
#define SHELL_LEN                50
#define ONLINE                   0

static InputDevManager *g_hotPlugManager;
static InputHostCb hotPlug;

static void HotPlugCallback(const HotPlugEvent *msg)
{
    int32_t ret;
    char shell[SHELL_LEN];
    if (msg == NULL) {
        HDF_LOGE("msg is null");
        return;
    }
    if (msg->status == ONLINE) {
        ret = sprintf_s(shell, SHELL_LEN, "chown system:system /dev/hdf_input_event%d", msg->devIndex);
        if (ret < 0) {
            HDF_LOGE("%s: sprintf shell cmd failed", __func__);
            return;
        }
        system(shell);
    }
}

static int32_t HotPlugEventListenerCallback(struct HdfDevEventlistener *listener,
    struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    (void)listener;
    (void)id;
    uint32_t len = 0;
    HotPlugEvent *event = NULL;
    bool ret = false;

    if (service == NULL || data == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    ret = HdfSbufReadBuffer(data, (const void **)&event, &len);
    if (!ret) {
        HDF_LOGE("%s: read sbuf failed", __func__);
        return INPUT_FAILURE;
    }
    g_hotPlugManager->hostDev.hostCb->HotPlugCallback((const HotPlugEvent *)event);

    return INPUT_SUCCESS;
}

static struct HdfDevEventlistener *HotPlugEventListenerInstance(void)
{
    int32_t ret;
    struct HdfDevEventlistener *listener = (struct HdfDevEventlistener *)malloc(sizeof(struct HdfDevEventlistener));
    if (listener == NULL) {
        HDF_LOGE("%s: instance listener failed", __func__);
        return NULL;
    }

    ret = memset_s(listener, sizeof(struct HdfDevEventlistener), 0, sizeof(struct HdfDevEventlistener));
    if (ret != 0) {
        HDF_LOGE("%s: memset failed, ret = %d", __func__, ret);
        free(listener);
        listener = NULL;
        return NULL;
    }
    listener->onReceive = HotPlugEventListenerCallback;
    return listener;
}

static int32_t RegisterHotPlugCallback(InputHostCb *callback)
{
    int32_t ret;
    if ((callback == NULL) || (callback->HotPlugCallback == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    g_hotPlugManager = (InputDevManager *)malloc(sizeof(InputDevManager));
    if (g_hotPlugManager == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }

    ret = memset_s(g_hotPlugManager, sizeof(InputDevManager), 0, sizeof(InputDevManager));
    if (ret != 0) {
        HDF_LOGE("%s: memset failed, ret = %d", __func__, ret);
        goto EXIT;
    }

    g_hotPlugManager->hostDev.service = HdfIoServiceBind(DEV_MANAGER_SERVICE_NAME);
    if (g_hotPlugManager->hostDev.service == NULL) {
        HDF_LOGE("%s: bind service failed", __func__);
        goto EXIT;
    }

    struct HdfDevEventlistener *listener = HotPlugEventListenerInstance();
    if (listener == NULL) {
        HDF_LOGE("%s: fail to instance listener", __func__);
        goto EXIT;
    }
    if (HdfDeviceRegisterEventListener(g_hotPlugManager->hostDev.service, listener) != INPUT_SUCCESS) {
        HDF_LOGE("%s: fail to register listener", __func__);
        free(listener);
        listener = NULL;
        goto EXIT;
    }
    g_hotPlugManager->hostDev.hostCb = callback;
    g_hotPlugManager->hostDev.listener = listener;
    return INPUT_SUCCESS;
EXIT:
    free(g_hotPlugManager);
    g_hotPlugManager = NULL;
    return INPUT_FAILURE;
}


static void HdfHotPlugDriverRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    if (g_hotPlugManager == NULL) {
        return;
    }
    if (g_hotPlugManager->hostDev.listener != NULL) {
        free(g_hotPlugManager->hostDev.listener);
        g_hotPlugManager->hostDev.listener = NULL;
    }
    if (g_hotPlugManager != NULL) {
        free(g_hotPlugManager);
        g_hotPlugManager = NULL;
    }
}

static int HdfHotPlugDriverBind(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    return HDF_SUCCESS;
}

static int HdfHotPlugDriverInit(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    int32_t ret;

    hotPlug.HotPlugCallback = HotPlugCallback;
    ret = RegisterHotPlugCallback(&hotPlug);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: register hot plug callback failed, ret = %d", __func__, ret);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

struct HdfDriverEntry g_inputuserHostDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "libhdf_input_hotplug.z.so",
    .Bind = HdfHotPlugDriverBind,
    .Init = HdfHotPlugDriverInit,
    .Release = HdfHotPlugDriverRelease,
};

HDF_INIT(g_inputuserHostDriverEntry);