/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_hid_adapter.h"
#include <securec.h>
#include "event_hub.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_timer.h"

#ifdef CONFIG_DFX_ZEROHUNG
#include <dfx/hung_wp_screen.h>
#endif

#define TIMER_INTERVAL 500
#define REPEAT_VALUE   2
#define MEMCPY_CHECK_RETURN(ret) do { \
    if ((ret) != 0) { \
        HDF_LOGE("%s: memcpy failed, line %d", __func__, __LINE__); \
        return; \
    } \
} while (0)

InputDevice *cachedHid[MAX_INPUT_DEV_NUM];
HidInfo *g_cachedInfo[MAX_INPUT_DEV_NUM];

uint32_t g_kbdcode = 0;
OsalTimer g_timer;

static bool HaveHidCache(void)
{
    if (cachedHid[0] == NULL) {
        return false;
    }
    return true;
}

static void LoadCachedHid(void)
{
    int32_t i = 0;
    int32_t ret;
    if (!HaveHidCache()) {
        HDF_LOGI("%s: exit", __func__);
        return;
    }
    while (i < MAX_INPUT_DEV_NUM && cachedHid[i] != NULL) {
        ret = RegisterInputDevice(cachedHid[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: add %s failed", __func__, cachedHid[i]->devName);
        }
        cachedHid[i] = NULL;
        i++;
    }
}

static int cachedPosId(void)
{
    int32_t id = 0;
    while (id < MAX_INPUT_DEV_NUM) {
        if (g_cachedInfo[id] == NULL) {
            return id;
        }
        id++;
    }
    return HDF_FAILURE;
}

void SendInfoToHdf(HidInfo *info)
{
    int ret;
    int32_t id = cachedPosId();
    if (id == HDF_FAILURE) {
        HDF_LOGE("%s: cached hid info failed", __func__);
        return;
    }
    g_cachedInfo[id] = (HidInfo *)OsalMemAlloc(sizeof(HidInfo));
    if (g_cachedInfo[id] == NULL) {
        HDF_LOGE("%s: malloc failed", __func__);
        return;
    }
    ret = memcpy_s(g_cachedInfo[id], sizeof(HidInfo), info, sizeof(HidInfo));
    if (ret != 0) {
        HDF_LOGE("%s: memcpy failed", __func__);
        OsalMemFree(g_cachedInfo[id]);
        g_cachedInfo[id] = NULL;
        return;
    }
}

static void FreeCachedInfo(void)
{
    int32_t id = 0;
    while (id < MAX_INPUT_DEV_NUM) {
        if (g_cachedInfo[id] != NULL) {
            OsalMemFree(g_cachedInfo[id]);
            g_cachedInfo[id] = NULL;
        }
        id++;
    }
}

static int32_t SetInputDevAbsAttr(InputDevice *inputDev, const HidInfo *info)
{
    int32_t ret;
    int i;
    for (i = 0; i < BITS_TO_LONG(ABS_CNT); i++) {
        if (inputDev->abilitySet.absCode[i] != 0) {
            ret = memcpy_s(inputDev->attrSet.axisInfo, sizeof(AbsAttr) * ABS_CNT,
                           info->axisInfo, sizeof(AbsAttr) * ABS_CNT);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static void GetInfoFromCache(InputDevice *inputDev, const HidInfo *info)
{
    uint32_t len;
    int32_t ret;

    len = sizeof(unsigned long);
    ret = memcpy_s(inputDev->abilitySet.devProp, len * BITS_TO_LONG(INPUT_PROP_CNT),
        info->devProp, len * BITS_TO_LONG(INPUT_PROP_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.eventType, len * BITS_TO_LONG(EV_CNT),
        info->eventType, len * BITS_TO_LONG(EV_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.absCode, len * BITS_TO_LONG(ABS_CNT),
        info->absCode, len * BITS_TO_LONG(ABS_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.relCode, len * BITS_TO_LONG(REL_CNT),
        info->relCode, len * BITS_TO_LONG(REL_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.keyCode, len * BITS_TO_LONG(KEY_CNT),
        info->keyCode, len * BITS_TO_LONG(KEY_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.ledCode, len * BITS_TO_LONG(LED_CNT),
        info->ledCode, len * BITS_TO_LONG(LED_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.miscCode, len * BITS_TO_LONG(MSC_CNT),
        info->miscCode, len * BITS_TO_LONG(MSC_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.soundCode, len * BITS_TO_LONG(SND_CNT),
        info->soundCode, len * BITS_TO_LONG(SND_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.forceCode, len * BITS_TO_LONG(FF_CNT),
        info->forceCode, len * BITS_TO_LONG(FF_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = memcpy_s(inputDev->abilitySet.switchCode, len * BITS_TO_LONG(SW_CNT),
        info->switchCode, len * BITS_TO_LONG(SW_CNT));
    MEMCPY_CHECK_RETURN(ret);
    ret = SetInputDevAbsAttr(inputDev, info);
    MEMCPY_CHECK_RETURN(ret);
    inputDev->attrSet.id.busType = info->bustype;
    inputDev->attrSet.id.vendor = info->vendor;
    inputDev->attrSet.id.product = info->product;
    inputDev->attrSet.id.version = info->version;
}

static void SetInputDevAbility(InputDevice *inputDev)
{
    HidInfo *info = NULL;
    int32_t id = 0;

    while (id < MAX_INPUT_DEV_NUM) {
        if (g_cachedInfo[id] != NULL && !strcmp(inputDev->devName, g_cachedInfo[id]->devName)) {
            info = g_cachedInfo[id];
            break;
        }
        id++;
    }
    if (id == MAX_INPUT_DEV_NUM || info == NULL) {
        HDF_LOGE("%s: match cached info failed", __func__);
        return;
    }

    GetInfoFromCache(inputDev, info);
    FreeCachedInfo();
}

static InputDevice* HidConstructInputDev(HidInfo *info)
{
    InputDevice *inputDev = (InputDevice *)OsalMemAlloc(sizeof(InputDevice));
    if (inputDev == NULL) {
        HDF_LOGE("%s: instance input device failed", __func__);
        return NULL;
    }
    (void)memset_s(inputDev, sizeof(InputDevice), 0, sizeof(InputDevice));

    inputDev->devType = info->devType;
    inputDev->devName = info->devName;
    SetInputDevAbility(inputDev);

    return inputDev;
}

static void DoRegisterInputDev(InputDevice* inputDev)
{
    int32_t ret;
    ret = RegisterInputDevice(inputDev);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(inputDev);
        return;
    }
}

static void CacheHid(InputDevice* inputDev)
{
    int32_t i = 0;
    while ((i < MAX_INPUT_DEV_NUM) && (cachedHid[i] != NULL)) {
        i++;
    }
    if (i < MAX_INPUT_DEV_NUM) {
        cachedHid[i] = inputDev;
        return;
    } else {
        HDF_LOGE("%s: cached hid device failed", __func__);
    }
}

static bool InputDriverLoaded(void)
{
    InputManager* g_inputManager = GetInputManager();
    if ((g_inputManager != NULL) && (g_inputManager->initialized != false)) {
        return true;
    }
    return false;
}

void* HidRegisterHdfInputDev(HidInfo *info)
{
    InputDevice* inputDev = HidConstructInputDev(info);
    if (inputDev == NULL) {
        HDF_LOGE("%s: hid construct input Dev failed", __func__);
        return NULL;
    }

    if (InputDriverLoaded()) {
        DoRegisterInputDev(inputDev);
    } else {
        CacheHid(inputDev);
    }
    return inputDev;
}

void HidUnregisterHdfInputDev(const void *inputDev)
{
    if (inputDev == NULL) {
        HDF_LOGE("%s: inputDev is null", __func__);
        return;
    }
    UnregisterInputDevice((InputDevice *)inputDev);
}

static void TimerFunc(uintptr_t arg)
{
    InputDevice *device = (InputDevice *)arg;
    PushOnePackage(device, EV_KEY, g_kbdcode, REPEAT_VALUE);
    PushOnePackage(device, 0, 0, SYN_CONFIG);
}

static void RepateEvent(const InputDevice *device)
{
    int32_t ret;
    static int32_t flag = 0;
    if (flag == 1) {
        (void)OsalTimerDelete(&g_timer);
    }

    ret = OsalTimerCreate(&g_timer, TIMER_INTERVAL, TimerFunc, (uintptr_t)device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: create timer failed, ret = %d", __func__, ret);
        return;
    }
    ret = OsalTimerStartLoop(&g_timer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: start timer failed, ret = %d", __func__, ret);
        return;
    }
    flag = 1;
}

void HidReportEvent(const void *inputDev, uint32_t type, uint32_t code, int32_t value)
{
#ifdef CONFIG_DFX_ZEROHUNG
    if (type == EV_KEY && code == KEY_POWER)
        hung_wp_screen_powerkey_ncb(value);
#endif
    InputDevice *device = (InputDevice *)inputDev;
    PushOnePackage(device, type, code, value);
    if (type == EV_KEY && KEY_RESERVED < code && code < KEY_MAX && value == 0 && code == g_kbdcode) {
        OsalTimerDelete(&g_timer);
        g_kbdcode = 0;
    }
    if (type == EV_KEY && KEY_RESERVED < code && code < KEY_MAX && value == 1 &&
        device->devType == INDEV_TYPE_KEYBOARD) {
        g_kbdcode = code;
        RepateEvent(device);
    }
}

static int32_t HdfHIDDriverInit(struct HdfDeviceObject *device)
{
    (void)device;
    static bool cachedHidRegistered = false;
    if (!cachedHidRegistered) {
        cachedHidRegistered = true;
        LoadCachedHid();
    }
    return HDF_SUCCESS;
}

static int32_t HidGetDevType(InputDevice *inputDev, struct HdfSBuf *reply)
{
    uint32_t devType = inputDev->devType;
    HDF_LOGI("%s: enter, devType is %u", __func__, devType);
    bool ret = HdfSbufWriteUint32(reply, devType);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HidGetDeviceStrInfo(InputDevice *inputDev, int32_t cmd, struct HdfSBuf *reply)
{
    const char *info = NULL;
    if (inputDev == NULL) {
        HDF_LOGE("%s: parameter invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmd) {
        case GET_CHIP_NAME:
            info = "null";
            break;
        case GET_VENDOR_NAME:
            info = "null";
            break;
        case GET_CHIP_INFO:
            info = "null";
            break;
        default:
            info = NULL;
            break;
    }

    bool ret = HdfSbufWriteString(reply, info);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: cmd is %d, the info is %s", __func__, cmd, info);
    return HDF_SUCCESS;
}

static int32_t HidGetDeviceAttr(InputDevice *inputDev, struct HdfSBuf *reply)
{
    int32_t ret;
    if (inputDev == NULL) {
        return HDF_FAILURE;
    }

    ret = strncpy_s(inputDev->attrSet.devName, DEV_NAME_LEN, inputDev->devName, strlen(inputDev->devName));
    if (ret != 0) {
        HDF_LOGE("%s: copy name from inputDev failed, ret = %d", __func__, ret);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteBuffer(reply, &inputDev->attrSet, sizeof(DevAttr))) {
        HDF_LOGE("%s: sbuf write dev attr failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t HidGetDeviceAbility(InputDevice *inputDev, struct HdfSBuf *reply)
{
    if (inputDev == NULL) {
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteBuffer(reply, &inputDev->abilitySet, sizeof(DevAbility))) {
        HDF_LOGE("%s: sbuf write dev ability failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t HdfHIDDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)cmd;
    int32_t ret;
    InputDevice *inputDev = NULL;
    if (client == NULL || data == NULL || reply == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    inputDev = (InputDevice *)client->device->priv;
    if (inputDev == NULL) {
        HDF_LOGE("%s: inputDev is null", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: cmd = %d", __func__, cmd);
    switch (cmd) {
        case GET_DEV_TYPE:
            ret = HidGetDevType(inputDev, reply);
            break;
        case GET_CHIP_NAME:
        case GET_VENDOR_NAME:
        case GET_CHIP_INFO:
            ret = HidGetDeviceStrInfo(inputDev, cmd, reply);
            break;
        case GET_DEV_ATTR:
            ret = HidGetDeviceAttr(inputDev, reply);
            break;
        case GET_DEV_ABILITY:
            ret = HidGetDeviceAbility(inputDev, reply);
            break;
        default:
            ret = HDF_SUCCESS;
            HDF_LOGE("%s: cmd unknown, cmd = 0x%x", __func__, cmd);
            break;
    }
    return ret;
}

static int32_t HdfHIDDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    static struct IDeviceIoService hidService = {
        .Dispatch = HdfHIDDispatch,
    };
    device->service = &hidService;
    return HDF_SUCCESS;
}

static void HdfHIDDriverRelease(struct HdfDeviceObject *device)
{
    FreeCachedInfo();
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
}

struct HdfDriverEntry g_hdfHIDEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_HID",
    .Bind = HdfHIDDriverBind,
    .Init = HdfHIDDriverInit,
    .Release = HdfHIDDriverRelease,
};

HDF_INIT(g_hdfHIDEntry);
