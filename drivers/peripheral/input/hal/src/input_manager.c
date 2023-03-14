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
#include "input_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <securec.h>
#include "hdf_io_service_if.h"
#include "input_common.h"

#define TOUCH_INDEX 1
#define PLACEHOLDER_LENGTH 2
#define PLACEHOLDER_LIMIT 10

static InputDevManager *g_devManager;
int32_t InstanceReporterHdi(InputReporter **hdi);
int32_t InstanceControllerHdi(InputController **hdi);
int32_t UpdateDevFullInfo(uint32_t devIndex);

InputDevManager *GetDevManager(void)
{
    return g_devManager;
}

static int32_t GetInputDevice(uint32_t devIndex, DeviceInfo **devInfo)
{
    int32_t ret;
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (devIndex >= MAX_INPUT_DEV_NUM || devInfo == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    ret = UpdateDevFullInfo(devIndex);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: update dev info failed", __func__);
        return ret;
    }

    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        *devInfo = &pos->payload;
        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGI("%s: device%u get dev info succ", __func__, devIndex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get device info", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetInputDeviceList(uint32_t *devNum, DeviceInfo **deviceList, uint32_t size)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    uint32_t tempSize = 0;
    DeviceInfo **tempList = NULL;

    if (devNum == NULL || deviceList == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    tempList = deviceList;
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (tempSize >= size) {
            *devNum = manager->attachedDevNum;
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: size is not enough, size = %u, devNum = %u", __func__,
                size, *devNum);
            return INPUT_FAILURE;
        }
        *tempList = &pos->payload;
        tempList++;
        tempSize++;
    }
    *devNum = manager->attachedDevNum;
    pthread_mutex_unlock(&manager->mutex);
    return INPUT_SUCCESS;
}

static int32_t CloseInputDevice(uint32_t devIndex)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        HdfIoServiceRecycle(pos->service);
        DListRemove(&pos->node);
        free(pos);
        manager->attachedDevNum--;
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t AddService(uint32_t index, const struct HdfIoService *service)
{
    InputDevManager *manager = NULL;
    DeviceInfoNode *device = NULL;

    GET_MANAGER_CHECK_RETURN(manager);
    device = (DeviceInfoNode *)malloc(sizeof(DeviceInfoNode));
    if (device == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }
    (void)memset_s(device, sizeof(DeviceInfoNode), 0, sizeof(DeviceInfoNode));

    device->payload.devIndex = index;
    device->service = (struct HdfIoService *)service;
    pthread_mutex_lock(&manager->mutex);
    DListInsertTail(&device->node, &manager->devList);
    manager->attachedDevNum++;
    pthread_mutex_unlock(&manager->mutex);
    return INPUT_SUCCESS;
}

static int32_t CheckIndex(uint32_t devIndex)
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
        if (pos->payload.devIndex == devIndex) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: the device%u has existed", __func__, devIndex);
            return INPUT_FAILURE;
        }
    }
    pthread_mutex_unlock(&manager->mutex);
    return INPUT_SUCCESS;
}

static int32_t OpenInputDevice(uint32_t devIndex)
{
    int32_t ret;
    int32_t len;
    struct HdfIoService *service = NULL;
    char serviceName[SERVICE_NAME_LEN] = {0};

    if (CheckIndex(devIndex) != INPUT_SUCCESS) {
        return INPUT_FAILURE;
    }

    len = (devIndex < PLACEHOLDER_LIMIT) ? 1 : PLACEHOLDER_LENGTH;
    ret = snprintf_s(serviceName, SERVICE_NAME_LEN, strlen("hdf_input_event") + len, "%s%u",
        "hdf_input_event", devIndex);
    if (ret == -1) {
        HDF_LOGE("%s: snprintf_s fail", __func__);
        return INPUT_FAILURE;
    }

    service = HdfIoServiceBind(serviceName);
    if (service == NULL) {
        HDF_LOGE("%s: fail to get io service: %s", __func__, serviceName);
        return INPUT_NULL_PTR;
    }

    if (AddService(devIndex, service) < 0) {
        HDF_LOGE("%s: add device%d failed", __func__, devIndex);
        HdfIoServiceRecycle(service);
        return INPUT_FAILURE;
    }

    HDF_LOGI("%s: open dev%u succ, service name = %s", __func__, devIndex, serviceName);
    return INPUT_SUCCESS;
}

static int32_t ScanInputDevice(DevDesc *staArr, uint32_t arrLen)
{
    InputDevManager *manager = NULL;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    char *data = {0};
    uint32_t count = 0;
    uint32_t replayDataSize = 0;
    int32_t ret;

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    if (manager->hostDev.service == NULL) {
        manager->hostDev.service = HdfIoServiceBind(DEV_MANAGER_SERVICE_NAME);
    }
    service = manager->hostDev.service;
    pthread_mutex_unlock(&manager->mutex);

    if (service == NULL) {
        HDF_LOGE("%s: HdfIoServiceBind failed", __func__);
        return INPUT_FAILURE;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: fail to obtain sbuf data", __func__);
        return INPUT_FAILURE;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: dispatch fail", __func__);
        HdfSbufRecycle(reply);
        return INPUT_FAILURE;
    }

    while (count < arrLen) {
        if (!HdfSbufReadBuffer(reply, (const void **)(&data), &replayDataSize) ||
            replayDataSize != sizeof(DevDesc)) {
            HDF_LOGE("%s: sbuf read failed", __func__);
            break;
        }
        if (memcpy_s(&staArr[count], sizeof(DevDesc), data, replayDataSize) != EOK) {
            HDF_LOGE("%s: memcpy failed, line: %d", __func__, __LINE__);
            HdfSbufRecycle(reply);
            return INPUT_FAILURE;
        }
        HDF_LOGI("%s: type = %d, id =%d", __func__, staArr[count].devType, staArr[count].devIndex);
        count++;
    }
    HdfSbufRecycle(reply);
    return INPUT_SUCCESS;
}

static int32_t InstanceManagerHdi(InputManager **manager)
{
    InputManager *managerHdi = (InputManager *)malloc(sizeof(InputManager));
    if (managerHdi == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }

    (void)memset_s(managerHdi, sizeof(InputManager), 0, sizeof(InputManager));

    managerHdi->ScanInputDevice = ScanInputDevice;
    managerHdi->OpenInputDevice = OpenInputDevice;
    managerHdi->CloseInputDevice = CloseInputDevice;
    managerHdi->GetInputDevice = GetInputDevice;
    managerHdi->GetInputDeviceList = GetInputDeviceList;
    *manager = managerHdi;
    return INPUT_SUCCESS;
}

static int32_t InitDevManager(void)
{
    InputDevManager *manager = (InputDevManager *)malloc(sizeof(InputDevManager));
    if (manager == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }

    (void)memset_s(manager, sizeof(InputDevManager), 0, sizeof(InputDevManager));
    DListHeadInit(&manager->devList);
    pthread_mutex_init(&manager->mutex, NULL);
    manager->attachedDevNum = 0;
    manager->evtCallbackNum = 0;
    g_devManager = manager;
    return INPUT_SUCCESS;
}

static void FreeInputHdi(IInputInterface *hdi)
{
    if (hdi->iInputManager != NULL) {
        free(hdi->iInputManager);
        hdi->iInputManager = NULL;
    }

    if (hdi->iInputController != NULL) {
        free(hdi->iInputController);
        hdi->iInputController = NULL;
    }

    if (hdi->iInputReporter != NULL) {
        free(hdi->iInputReporter);
        hdi->iInputReporter = NULL;
    }
    free(hdi);
}

static IInputInterface *InstanceInputHdi(void)
{
    int32_t ret;
    IInputInterface *hdi = (IInputInterface *)malloc(sizeof(IInputInterface));
    if (hdi == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return NULL;
    }
    (void)memset_s(hdi, sizeof(IInputInterface), 0, sizeof(IInputInterface));

    ret = InstanceManagerHdi(&hdi->iInputManager);
    if (ret != INPUT_SUCCESS) {
        FreeInputHdi(hdi);
        return NULL;
    }

    ret = InstanceControllerHdi(&hdi->iInputController);
    if (ret != INPUT_SUCCESS) {
        FreeInputHdi(hdi);
        return NULL;
    }

    ret = InstanceReporterHdi(&hdi->iInputReporter);
    if (ret != INPUT_SUCCESS) {
        FreeInputHdi(hdi);
        return NULL;
    }
    return hdi;
}

int32_t GetInputInterface(IInputInterface **inputInterface)
{
    int32_t ret;
    IInputInterface *inputHdi = NULL;

    if (inputInterface == NULL) {
        HDF_LOGE("%s: parameter is null", __func__);
        return INPUT_INVALID_PARAM;
    }

    inputHdi = InstanceInputHdi();
    if (inputHdi == NULL) {
        HDF_LOGE("%s: failed to instance hdi", __func__);
        return INPUT_NULL_PTR;
    }

    ret = InitDevManager();
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: failed to initialize manager", __func__);
        FreeInputHdi(inputHdi);
        return INPUT_FAILURE;
    }

    *inputInterface = inputHdi;
    HDF_LOGI("%s: exit succ", __func__);
    return INPUT_SUCCESS;
}

static void FreeDevManager(InputDevManager *manager)
{
    (void)HdfDeviceUnregisterEventListener(manager->hostDev.service, manager->hostDev.listener);
    if (manager->hostDev.listener != NULL) {
        free(manager->hostDev.listener);
        manager->hostDev.listener = NULL;
        manager->hostDev.hostCb = NULL;
    }
    (void)HdfIoServiceRecycle(manager->hostDev.service);
    pthread_mutex_unlock(&manager->mutex);
    pthread_mutex_destroy(&manager->mutex);
    free(manager);
    g_devManager = NULL;
}

void ReleaseInputInterface(IInputInterface *inputInterface)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (inputInterface == NULL) {
        return;
    }
    FreeInputHdi(inputInterface);
    inputInterface = NULL;

    if (g_devManager == NULL) {
        return;
    }
    manager = g_devManager;
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        (void)HdfDeviceUnregisterEventListener(pos->service, pos->listener);
        if (pos->listener != NULL) {
            free(pos->listener);
            pos->listener = NULL;
            pos->eventCb = NULL;
        }
        (void)HdfIoServiceRecycle(pos->service);
        DListRemove(&pos->node);
        free(pos);
    }
    FreeDevManager(manager);
}