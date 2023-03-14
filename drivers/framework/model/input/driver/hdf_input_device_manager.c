/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_input_device_manager.h"
#include <securec.h>
#include "event_hub.h"
#include "hdf_base.h"
#include "hdf_device_object.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define NODE_MODE            0660
#define SERVICE_NAME_LEN     24
#define INPUT_DEV_EXIST      1
#define INPUT_DEV_NOT_EXIST  0
#define INPUTDEV_FIRST_ID    2
#define FILLER_FLAG          1
#define PLACEHOLDER_LENGTH   2
#define PLACEHOLDER_LIMIT    10

static InputManager *g_inputManager;

InputManager* GetInputManager(void)
{
    return g_inputManager;
}

static bool IsHidDevice(uint32_t devType)
{
    if ((devType > INDEV_TYPE_HID_BEGIN_POS) && (devType < INDEV_TYPE_UNKNOWN)) {
        return true;
#ifdef CONFIG_ARCH_ROCKCHIP
    } else if (devType == INDEV_TYPE_KEY) {
        return true;
#endif
    }
    return false;
}

static struct HdfDeviceObject *HidRegisterHdfDevice(InputDevice *inputDev)
{
    char svcName[SERVICE_NAME_LEN] = {0};
    const char *moduleName = "HDF_HID";
    struct HdfDeviceObject *hdfDev = NULL;

    int32_t len = (inputDev->devId < PLACEHOLDER_LIMIT) ? 1 : PLACEHOLDER_LENGTH;
    int32_t ret = snprintf_s(svcName, SERVICE_NAME_LEN, strlen("hdf_input_event") + len, "%s%u",
        "hdf_input_event", inputDev->devId);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return NULL;
    }

    hdfDev = HdfDeviceObjectAlloc(g_inputManager->hdfDevObj, moduleName);
    if (hdfDev == NULL) {
        HDF_LOGE("%s: failed to alloc device object", __func__);
        return NULL;
    }
    ret = HdfDeviceObjectRegister(hdfDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to regitst device %s", __func__, moduleName);
        HdfDeviceObjectRelease(hdfDev);
        return NULL;
    }
    ret = HdfDeviceObjectPublishService(hdfDev, svcName, SERVICE_POLICY_CAPACITY, 0664); // 0664:permission setting
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to regitst device %s", __func__, moduleName);
        HdfDeviceObjectRelease(hdfDev);
        return NULL;
    }
    inputDev->hdfDevObj = hdfDev;
    HDF_LOGI("%s: svcName is %s, devName = %s", __func__, svcName, inputDev->devName);
    return hdfDev;
}

static void HotPlugNotify(const InputDevice *inputDev, uint32_t status)
{
    HotPlugEvent event = {0};
    int32_t ret;

    if (inputDev->eventBuf == NULL) {
        HDF_LOGE("%s: event buffer is null", __func__);
        return;
    }

    event.devId = inputDev->devId;
    event.devType = inputDev->devType;
    event.status = status;

    if (!HdfSbufWriteBuffer(inputDev->eventBuf, &event, sizeof(HotPlugEvent))) {
        HDF_LOGE("%s: write buffer failed", __func__);
        HdfSbufFlush(inputDev->eventBuf);
        return;
    }
    ret = HdfDeviceSendEvent(g_inputManager->hdfDevObj, 0, inputDev->eventBuf);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: send event failed", __func__);
    }
    HdfSbufFlush(inputDev->eventBuf);
}

static int32_t CreateDeviceNode(InputDevice *inputDev)
{
    if (IsHidDevice(inputDev->devType)) {
        HDF_LOGI("%s: prepare to register hdf device", __func__);
        inputDev->hdfDevObj = HidRegisterHdfDevice(inputDev);
        if (inputDev->hdfDevObj == NULL) {
            return HDF_DEV_ERR_NO_DEVICE;
        }
        inputDev->hdfDevObj->priv = (void *)inputDev;
    }

    HDF_LOGI("%s: create node succ, devId is %d ", __func__, inputDev->devId);
    return HDF_SUCCESS;
}

static void DeleteDeviceNode(InputDevice *inputDev)
{
    if (IsHidDevice(inputDev->devType)) {
        HDF_LOGI("remove input device: hdf_input_event%u", inputDev->devId);
        HdfDeviceObjectRelease(inputDev->hdfDevObj);
    }

    HDF_LOGI("%s: delete node succ, devId is %d", __func__, inputDev->devId);
}

static void AddInputDevice(InputDevice *inputDev)
{
    InputDevice *tmpDev = NULL;
    if (g_inputManager->inputDevList == NULL) {
        g_inputManager->inputDevList = inputDev;
        (g_inputManager->inputDevList)->next = NULL;
    } else {
        tmpDev = g_inputManager->inputDevList;
        while (tmpDev != NULL) {
            if (tmpDev->next == NULL) {
                tmpDev->next = inputDev;
                inputDev->next = NULL;
                break;
            }
            tmpDev = tmpDev->next;
        }
    }
    g_inputManager->devCount++;
    HotPlugNotify(inputDev, ONLINE);
}

static int32_t CheckInputDevice(InputDevice *inputDev)
{
    InputDevice *tmpDev = NULL;
    if (g_inputManager->inputDevList == NULL) {
        return HDF_SUCCESS;
    } else {
        tmpDev = g_inputManager->inputDevList;
        while (tmpDev != NULL) {
            if (tmpDev->devId == inputDev->devId) {
                HDF_LOGE("%s: device%d registered", __func__, inputDev->devId);
                return INPUT_DEV_EXIST;
            }
            tmpDev = tmpDev->next;
        }
    }
    return INPUT_DEV_NOT_EXIST;
}

static int32_t DeleteInputDevice(InputDevice *inputDev)
{
    if (g_inputManager->inputDevList == NULL) {
        return HDF_FAILURE;
    } else {
        if ((g_inputManager->inputDevList)->devId == inputDev->devId) {
            g_inputManager->inputDevList = g_inputManager->inputDevList->next;
            goto EXIT;
        }

        InputDevice *preNode = g_inputManager->inputDevList;
        InputDevice *tmpDev = preNode->next;
        while (tmpDev != NULL) {
            if (tmpDev->devId == inputDev->devId) {
                preNode->next = tmpDev->next;
                goto EXIT;
            }
            preNode = tmpDev;
            tmpDev = tmpDev->next;
        }
        HDF_LOGE("%s: device%d not exist", __func__, inputDev->devId);
        return HDF_FAILURE;
    }

EXIT:
    g_inputManager->devCount--;
    HotPlugNotify(inputDev, OFFLINE);
    return HDF_SUCCESS;
}

#define DEFAULT_TOUCH_BUF_PKG_NUM      50
#define DEFAULT_KEY_BUF_PKG_NUM        10
#define DEFAULT_MOUSE_BUF_PKG_NUM      30
#define DEFAULT_KEYBOARD_BUF_PKG_NUM   20
#define DEFAULT_CROWN_BUF_PKG_NUM      20
#define DEFAULT_ENCODER_BUF_PKG_NUM    20
#define DEFAULT_ROCKER_BUF_PKG_NUM     40
#define DEFAULT_TRACKBALL_BUF_PKG_NUM  30

static int32_t AllocPackageBuffer(InputDevice *inputDev)
{
    uint16_t pkgNum;
    switch (inputDev->devType) {
        case INDEV_TYPE_TOUCH:
            pkgNum = DEFAULT_TOUCH_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_KEY:
            pkgNum = DEFAULT_KEY_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_MOUSE:
            pkgNum = DEFAULT_MOUSE_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_KEYBOARD:
            pkgNum = DEFAULT_KEYBOARD_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_CROWN:
            pkgNum = DEFAULT_CROWN_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_ENCODER:
            pkgNum = DEFAULT_ENCODER_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_ROCKER:
            pkgNum = DEFAULT_ROCKER_BUF_PKG_NUM;
            break;
        case INDEV_TYPE_TRACKBALL:
            pkgNum = DEFAULT_TRACKBALL_BUF_PKG_NUM;
            break;
        default:
            HDF_LOGE("%s: devType not exist", __func__);
            return HDF_FAILURE;
    }
    inputDev->pkgBuf = HdfSbufObtain(sizeof(EventPackage) * pkgNum);
    if (inputDev->pkgBuf == NULL) {
        HDF_LOGE("%s: malloc sbuf failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    inputDev->eventBuf = HdfSbufObtain(sizeof(HotPlugEvent));
    if (inputDev->eventBuf == NULL) {
        HDF_LOGE("%s: malloc sbuf failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    inputDev->pkgNum = pkgNum;
    return HDF_SUCCESS;
}

static int32_t AllocDeviceID(InputDevice *inputDev)
{
    InputDevice *tmpDev = g_inputManager->inputDevList;
    uint32_t idList[MAX_INPUT_DEV_NUM + 1];
    uint32_t id;
    int32_t ret;
    ret = memset_s(idList, (MAX_INPUT_DEV_NUM + 1) * sizeof(uint32_t), 0,
                   (MAX_INPUT_DEV_NUM + 1) * sizeof(uint32_t));
    if (ret != 0) {
        HDF_LOGE("%s: memset_s is failed", __func__);
        return HDF_FAILURE;
    }
    while (tmpDev != NULL) {
        if (idList[tmpDev->devId] == 0) {
            idList[tmpDev->devId] = FILLER_FLAG;
        }
        tmpDev = tmpDev->next;
    }

    if (inputDev->devType == INDEV_TYPE_TOUCH) {
        inputDev->devId = 1;
        return HDF_SUCCESS;
    }
    for (id = INPUTDEV_FIRST_ID; id < MAX_INPUT_DEV_NUM + 1; id++) {
        if (idList[id] == 0) {
            inputDev->devId = id;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: alloc device id failed", __func__);
    return HDF_FAILURE;
}

int32_t RegisterInputDevice(InputDevice *inputDev)
{
    int32_t ret;

    HDF_LOGI("%s: enter", __func__);
    if (inputDev == NULL) {
        HDF_LOGE("%s: inputdev is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if ((g_inputManager == NULL) || (g_inputManager->initialized == false)) {
        HDF_LOGE("%s: dev manager is null or initialized failed", __func__);
        return HDF_FAILURE;
    }

    OsalMutexLock(&g_inputManager->mutex);
    ret = AllocDeviceID(inputDev);
    if (ret != HDF_SUCCESS) {
        goto EXIT;
    }
    ret = CreateDeviceNode(inputDev);
    if (ret != HDF_SUCCESS) {
        goto EXIT1;
    }

    ret = AllocPackageBuffer(inputDev);
    if (ret != HDF_SUCCESS) {
        goto EXIT1;
    }

    AddInputDevice(inputDev);
    OsalMutexUnlock(&g_inputManager->mutex);
    HDF_LOGI("%s: exit succ, devCount is %d", __func__, g_inputManager->devCount);
    return HDF_SUCCESS;

EXIT1:
    DeleteDeviceNode(inputDev);
EXIT:
    OsalMutexUnlock(&g_inputManager->mutex);
    return ret;
}

void UnregisterInputDevice(InputDevice *inputDev)
{
    int32_t ret;
    HDF_LOGI("%s: enter", __func__);
    if (inputDev == NULL) {
        HDF_LOGE("%s: inputdev is null", __func__);
        return;
    }

    if ((g_inputManager == NULL) || (g_inputManager->initialized == false)) {
        HDF_LOGE("%s: dev manager is null or initialized failed", __func__);
        return;
    }

    OsalMutexLock(&g_inputManager->mutex);
    if (CheckInputDevice(inputDev) == INPUT_DEV_NOT_EXIST) {
        HDF_LOGE("%s: dev%d not exist", __func__, inputDev->devId);
        goto EXIT;
    }

    DeleteDeviceNode(inputDev);
    HdfSbufRecycle(inputDev->pkgBuf);
    inputDev->pkgBuf = NULL;
    ret = DeleteInputDevice(inputDev);
    if (ret != HDF_SUCCESS) {
        goto EXIT;
    }
    HdfSbufRecycle(inputDev->eventBuf);
    inputDev->eventBuf = NULL;
    OsalMemFree(inputDev);
    OsalMutexUnlock(&g_inputManager->mutex);
    HDF_LOGI("%s: exit succ, devCount is %d", __func__, g_inputManager->devCount);
    return;

EXIT:
    OsalMutexUnlock(&g_inputManager->mutex);
    return;
}

static uint32_t GetDeviceCount(void)
{
    HDF_LOGI("%s: devCount = %d", __func__, g_inputManager->devCount);
    return g_inputManager->devCount;
}

static int32_t ScanAllDev(struct HdfSBuf *reply)
{
    DevDesc sta;
    InputDevice *tmpDev = g_inputManager->inputDevList;
    while (tmpDev != NULL) {
        sta.devType = tmpDev->devType;
        sta.devId = tmpDev->devId;

        if (!HdfSbufWriteBuffer(reply, &sta, sizeof(DevDesc))) {
            HDF_LOGE("%s: HdfSbufWriteBuffer failed", __func__);
            return HDF_FAILURE;
        }
        tmpDev = tmpDev->next;
    }
    HdfSbufWriteBuffer(reply, NULL, 0); // end flag
    return HDF_SUCCESS;
}

static int32_t ScanDevice(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)cmd;
    int32_t ret;
    if ((client == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }
    ret = ScanAllDev(reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: scan all dev failed", __func__);
    }
    return ret;
}

static int32_t HdfInputManagerBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    static IInputManagerService managerService = {
        .getDeviceCount = GetDeviceCount,
        .ioService.Dispatch = ScanDevice,
    };

    device->service = &managerService.ioService;
    return HDF_SUCCESS;
}

static InputManager *InputManagerInstance(void)
{
    InputManager *manager = (InputManager *)OsalMemAlloc(sizeof(InputManager));
    if (manager == NULL) {
        HDF_LOGE("%s: instance input manager failed", __func__);
        return NULL;
    }
    (void)memset_s(manager, sizeof(InputManager), 0, sizeof(InputManager));
    return manager;
}

static int32_t HdfInputManagerInit(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: enter", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    g_inputManager = InputManagerInstance();
    if (g_inputManager == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (OsalMutexInit(&g_inputManager->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: mutex init failed", __func__);
        OsalMemFree(g_inputManager);
        g_inputManager = NULL;
        return HDF_FAILURE;
    }
    g_inputManager->initialized = true;
    g_inputManager->hdfDevObj = device;
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;
}

static void HdfInputManagerRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    if (g_inputManager != NULL) {
        OsalMutexDestroy(&g_inputManager->mutex);
        OsalMemFree(g_inputManager);
        g_inputManager = NULL;
    }
}

struct HdfDriverEntry g_hdfInputEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_INPUT_MANAGER",
    .Bind = HdfInputManagerBind,
    .Init = HdfInputManagerInit,
    .Release = HdfInputManagerRelease,
};

HDF_INIT(g_hdfInputEntry);
