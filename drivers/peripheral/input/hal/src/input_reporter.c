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
#include "input_reporter.h"
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <securec.h>
#include "hdf_io_service_if.h"
#include "hdf_syscall_adapter.h"
#include "osal_time.h"
#include "input_common.h"

#define POLL_WAIT_MS 100
#define MOUSE_DATA_LEN 4
#define REL_X_BYTE 1
#define REL_Y_BYTE 2
#define MAX_EVENT_PKG_NUM 100

InputDevManager *GetDevManager(void);

static int32_t EventListenerCallback(struct HdfDevEventlistener *listener, struct HdfIoService *service,
    uint32_t id, struct HdfSBuf *data)
{
    (void)listener;
    (void)id;
    int32_t count = 0;
    uint32_t len = 0;
    EventPackage *pkgs[MAX_EVENT_PKG_NUM] = {0};
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (service == NULL || data == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    manager = GetDevManager();
    if (manager == NULL) {
        HDF_LOGE("%s: get manager failed", __func__);
        return INPUT_NULL_PTR;
    }

    while (true) {
        if (count >= MAX_EVENT_PKG_NUM) {
            break;
        }

        if (!HdfSbufReadBuffer(data, (const void **)&pkgs[count], &len)) {
            HDF_LOGE("%s: sbuf read finished", __func__);
            break;
        }

        if (pkgs[count] == NULL) {
            break;
        }
        count++;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->service == service) {
            pos->eventCb->EventPkgCallback((const EventPackage **)pkgs, count, pos->payload.devIndex);
        }
    }
    return INPUT_SUCCESS;
}

static struct HdfDevEventlistener *EventListenerInstance(void)
{
    struct HdfDevEventlistener *listener = (struct HdfDevEventlistener *)malloc(sizeof(struct HdfDevEventlistener));
    if (listener == NULL) {
        HDF_LOGE("%s: instance listener failed", __func__);
        return NULL;
    }

    (void)memset_s(listener, sizeof(struct HdfDevEventlistener), 0, sizeof(struct HdfDevEventlistener));
    listener->onReceive = EventListenerCallback;
    return listener;
}

static int32_t RegisterReportCallback(uint32_t devIndex, InputEventCb *callback)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (callback == NULL) || (callback->EventPkgCallback == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        struct HdfDevEventlistener *listener = EventListenerInstance();
        if (listener == NULL) {
            pthread_mutex_unlock(&manager->mutex);
            return INPUT_FAILURE;
        }
        if (HdfDeviceRegisterEventListener(pos->service, listener) != INPUT_SUCCESS) {
            free(listener);
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: fail to register listener", __func__);
            return INPUT_FAILURE;
        }
        manager->evtCallbackNum++;
        pos->eventCb = callback;
        pos->listener = listener;
        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGI("%s: device%u register callback succ, evtCallbackNum = %d", __func__,
            devIndex, manager->evtCallbackNum);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't register callback", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t UnregisterReportCallback(uint32_t devIndex)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (devIndex >= MAX_INPUT_DEV_NUM) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (pos->eventCb != NULL) {
            if (HdfDeviceUnregisterEventListener(pos->service, pos->listener) != INPUT_SUCCESS) {
                pthread_mutex_unlock(&manager->mutex);
                HDF_LOGE("%s: fail to unregister listener", __func__);
                return INPUT_FAILURE;
            }
            free(pos->listener);
            pos->listener = NULL;
            pos->eventCb = NULL;
            manager->evtCallbackNum--;
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGI("%s: device%u unregister callback succ", __func__, devIndex);
            return INPUT_SUCCESS;
        } else {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: device%u does not register callback", __func__, devIndex);
            return INPUT_FAILURE;
        }
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't unregister callback", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t HotPlugEventListenerCallback(struct HdfDevEventlistener *listener,
    struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    (void)listener;
    (void)id;
    uint32_t len = 0;
    HotPlugEvent *event = NULL;
    InputDevManager *manager = NULL;
    bool ret = false;

    if (service == NULL || data == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);

    ret = HdfSbufReadBuffer(data, (const void **)&event, &len);
    if (!ret) {
        HDF_LOGE("%s: read sbuf failed", __func__);
        return INPUT_FAILURE;
    }
    manager->hostDev.hostCb->HotPlugCallback((const HotPlugEvent *)event);
    return INPUT_SUCCESS;
}

static struct HdfDevEventlistener *HotPlugEventListenerInstance(void)
{
    struct HdfDevEventlistener *listener = (struct HdfDevEventlistener *)malloc(sizeof(struct HdfDevEventlistener));
    if (listener == NULL) {
        HDF_LOGE("%s: instance listener failed", __func__);
        return NULL;
    }

    (void)memset_s(listener, sizeof(struct HdfDevEventlistener), 0, sizeof(struct HdfDevEventlistener));
    listener->onReceive = HotPlugEventListenerCallback;
    return listener;
}

static int32_t RegisterHotPlugCallback(InputHostCb *callback)
{
    InputDevManager *manager = NULL;

    if ((callback == NULL) || (callback->HotPlugCallback == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    if (manager->hostDev.service == NULL) {
        manager->hostDev.service = HdfIoServiceBind(DEV_MANAGER_SERVICE_NAME);
        if (manager->hostDev.service == NULL) {
            pthread_mutex_unlock(&manager->mutex);
            return INPUT_FAILURE;
        }
    }

    struct HdfDevEventlistener *listener = HotPlugEventListenerInstance();
    if (listener == NULL) {
        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGE("%s: fail to instance listener", __func__);
        return INPUT_FAILURE;
    }
    if (HdfDeviceRegisterEventListener(manager->hostDev.service, listener) != INPUT_SUCCESS) {
        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGE("%s: fail to register listener", __func__);
        free(listener);
        return INPUT_FAILURE;
    }
    manager->hostDev.hostCb = callback;
    manager->hostDev.listener = listener;
    pthread_mutex_unlock(&manager->mutex);
    return INPUT_SUCCESS;
}

static int32_t UnregisterHotPlugCallback(void)
{
    InputDevManager *manager = NULL;
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    if (manager->hostDev.listener != NULL) {
        free(manager->hostDev.listener);
        manager->hostDev.listener = NULL;
    }
    manager->hostDev.hostCb = NULL;
    pthread_mutex_unlock(&manager->mutex);
    return INPUT_SUCCESS;
}

int32_t InstanceReporterHdi(InputReporter **reporter)
{
    InputReporter *reporterHdi = (InputReporter *)malloc(sizeof(InputReporter));
    if (reporterHdi == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }

    (void)memset_s(reporterHdi, sizeof(InputReporter), 0, sizeof(InputReporter));

    reporterHdi->RegisterReportCallback = RegisterReportCallback;
    reporterHdi->UnregisterReportCallback = UnregisterReportCallback;
    reporterHdi->RegisterHotPlugCallback = RegisterHotPlugCallback;
    reporterHdi->UnregisterHotPlugCallback = UnregisterHotPlugCallback;
    *reporter = reporterHdi;

    return INPUT_SUCCESS;
}
