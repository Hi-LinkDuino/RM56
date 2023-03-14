/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_bl.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal.h"

#define OFFSET_TWO_BYTE       16
#define MAX_BL_NAME_LEN       32
#define MAX_BL_DEV            32
#define NAME_BUFFER_LEN       1220   // 32 * 38
#define MAX_DEST_STRING_LEN   38  // MAX_BL_NAME_LEN(32) + 6

enum BrightnessType {
    MIN_BRIGHTNESS,
    CURRENT_BRIGHTNESS,
    DEFAULT_BRIGHTNESS,
    MAX_BRIGHTNESS,
};

typedef int32_t (*BlCmdHandle)(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData);

struct BacklightDev {
    char name[MAX_BL_NAME_LEN];
    struct BacklightProperties props;
    struct BacklightOps *ops;
    struct OsalMutex mutex;
    bool isRegister;
    void *priv;
};

struct BlDevManager {
    struct BacklightDev *blDev[MAX_BL_DEV];
    uint32_t devNum;
};

static struct BlDevManager g_blDevManager;

static struct BlDevManager *GetBlDevManager(void)
{
    return &g_blDevManager;
}

static struct BacklightDev *BlDevInstance(const char *devName,
    struct BacklightProperties *props, struct BacklightOps *ops)
{
    int32_t ret;
    struct BacklightDev *blDev = NULL;

    blDev = (struct BacklightDev *)OsalMemCalloc(sizeof(struct BacklightDev));
    if (blDev == NULL) {
        HDF_LOGE("%s blDev malloc fail", __func__);
        return NULL;
    }
    blDev->ops = ops;
    ret = memcpy_s(blDev->name, MAX_BL_NAME_LEN - 1, devName, strlen(devName) + 1);
    if (ret != 0) {
        HDF_LOGE("%s blDev->name %s", __func__, blDev->name);
        goto FAIL;
    }
    ret = memcpy_s(&blDev->props, sizeof(struct BacklightProperties),
        props, sizeof(struct BacklightProperties));
    if (ret != 0) {
        HDF_LOGE("%s props memcpy fail", __func__);
        goto FAIL;
    }
    return blDev;
FAIL:
    OsalMemFree(blDev);
    return NULL;
}

struct BacklightDev *RegisterBlDev(const char *name, struct BacklightProperties *props,
    struct BacklightOps *ops, void *priv)
{
    int32_t devNum;
    struct BacklightDev *blDev = NULL;
    struct BlDevManager *blDevManager = NULL;

    if ((name == NULL) || (ops == NULL) || (props == NULL)) {
        HDF_LOGE("%s: name , ops or props is null", __func__);
        return NULL;
    }
    blDevManager = GetBlDevManager();
    devNum = blDevManager->devNum;
    if (devNum >= MAX_BL_DEV) {
        HDF_LOGE("%s: number of backlight device registrations exceeded", __func__);
        return NULL;
    }
    for (int32_t i = 0; i < devNum; i++) {
        if (strcmp(name, blDevManager->blDev[i]->name) == 0) {
            HDF_LOGE("%s: backlight name should be unique", __func__);
            return NULL;
        }
    }
    blDev = BlDevInstance(name, props, ops);
    if (blDev == NULL) {
        HDF_LOGE("%s: BlDevInstance fail", __func__);
        return NULL;
    }
    blDev->priv = priv;
    blDevManager->blDev[devNum] = blDev;
    blDevManager->devNum++;
    OsalMutexInit(&blDev->mutex);
    HDF_LOGE("%s: success", __func__);
    return blDev;
}

void *ToBlDevPriv(struct BacklightDev *blDev)
{
    if (blDev == NULL) {
        HDF_LOGE("%s blDev is null", __func__);
        return NULL;
    }
    return blDev->priv;
}

struct BacklightDev *GetBacklightDev(const char *name)
{
    struct BlDevManager *blDevManager = NULL;

    if (name == NULL) {
        HDF_LOGE("%s name is null", __func__);
        return NULL;
    }
    blDevManager = GetBlDevManager();
    for (int32_t i = 0; i < blDevManager->devNum; i++) {
        if (strcmp(name, blDevManager->blDev[i]->name) == 0) {
            return blDevManager->blDev[i];
        }
    }
    return NULL;
}

int32_t UpdateBacklightState(struct BacklightDev *blDev, enum FbPowerStatus status)
{
    if (blDev == NULL) {
        HDF_LOGE("%s blDev is null", __func__);
        return HDF_FAILURE;
    }
    if ((status < FB_POWER_ON) || (status > FB_POWER_OFF)) {
        HDF_LOGE("%s the status is illegal", __func__);
        return HDF_FAILURE;
    }
    OsalMutexLock(&blDev->mutex);
    blDev->props.fbStatus = status;
    OsalMutexUnlock(&blDev->mutex);
    return HDF_SUCCESS;
}

int32_t UpdateBrightness(struct BacklightDev *blDev, uint32_t brightness)
{
    int32_t ret = HDF_FAILURE;

    if (blDev == NULL) {
        HDF_LOGE("%s blDev is null", __func__);
        return HDF_FAILURE;
    }
    if (brightness > blDev->props.maxBrightness) {
        brightness = blDev->props.maxBrightness;
    }
    if (brightness < blDev->props.minBrightness) {
        brightness = blDev->props.minBrightness;
    }
    OsalMutexLock(&blDev->mutex);
    if (brightness == blDev->props.brightness) {
        HDF_LOGI("%s brightness does not change", __func__);
        OsalMutexUnlock(&blDev->mutex);
        return HDF_SUCCESS;
    }
    if ((blDev->props.fbStatus == FB_POWER_STANDBY) ||
        (blDev->props.fbStatus == FB_POWER_SUSPEND) ||
        (blDev->props.fbStatus == FB_POWER_OFF)) {
        brightness = 0;
    }
    if (blDev->ops != NULL && blDev->ops->updateBrightness != NULL) {
        ret = blDev->ops->updateBrightness(blDev, brightness);
        if (ret == HDF_SUCCESS) {
            blDev->props.brightness = brightness;
        } else {
            HDF_LOGE("%s: fail", __func__);
        }
    }
    OsalMutexUnlock(&blDev->mutex);
    return ret;
}

static uint32_t GetBlDevBrightness(struct BacklightDev *blDev, enum BrightnessType type)
{
    uint32_t brightness;

    OsalMutexLock(&blDev->mutex);
    switch (type) {
        case MIN_BRIGHTNESS:
            brightness = blDev->props.minBrightness;
            break;
        case CURRENT_BRIGHTNESS:
            if ((blDev->props.fbStatus == FB_POWER_STANDBY) ||
                (blDev->props.fbStatus == FB_POWER_SUSPEND) ||
                (blDev->props.fbStatus == FB_POWER_OFF)) {
                blDev->props.brightness = 0;
            }
            if ((blDev->ops != NULL) && (blDev->ops->getBrightness != NULL)) {
                brightness = blDev->ops->getBrightness(blDev);
            } else {
                brightness = blDev->props.brightness;
            }
            break;
        case DEFAULT_BRIGHTNESS:
            brightness = blDev->props.defBrightness;
            break;
        case MAX_BRIGHTNESS:
            brightness = blDev->props.maxBrightness;
            break;
    }
    OsalMutexUnlock(&blDev->mutex);
    return brightness;
}

int32_t GetMinBrightness(struct BacklightDev *blDev, uint32_t *brightness)
{
    if (blDev == NULL || brightness == NULL) {
        HDF_LOGE("%s: blDev or brightness is null", __func__);
        return HDF_FAILURE;
    }
    *brightness = GetBlDevBrightness(blDev, MIN_BRIGHTNESS);
    return HDF_SUCCESS;
}

int32_t GetCurrBrightness(struct BacklightDev *blDev, uint32_t *brightness)
{
    if (blDev == NULL || brightness == NULL) {
        HDF_LOGE("%s: blDev or brightness is null", __func__);
        return HDF_FAILURE;
    }
    *brightness = GetBlDevBrightness(blDev, CURRENT_BRIGHTNESS);
    return HDF_SUCCESS;
}

int32_t GetDefBrightness(struct BacklightDev *blDev, uint32_t *brightness)
{
    if (blDev == NULL || brightness == NULL) {
        HDF_LOGE("%s: blDev or brightness is null", __func__);
        return HDF_FAILURE;
    }
    *brightness = GetBlDevBrightness(blDev, DEFAULT_BRIGHTNESS);
    return HDF_SUCCESS;
}

int32_t GetMaxBrightness(struct BacklightDev *blDev, uint32_t *brightness)
{
    if (blDev == NULL || brightness == NULL) {
        HDF_LOGE("%s: blDev or brightness is null", __func__);
        return HDF_FAILURE;
    }
    *brightness = GetBlDevBrightness(blDev, MAX_BRIGHTNESS);
    return HDF_SUCCESS;
}

static int32_t HdfGetBrightness(enum BrightnessType type,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t devId;
    uint32_t brightness;
    struct BlDevManager *blDevManager = NULL;
    struct BacklightDev *blDev = NULL;

    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reqData, &devId)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    blDevManager = GetBlDevManager();
    if (devId >= blDevManager->devNum) {
        HDF_LOGE("%s: devId is illegal", __func__);
        return HDF_FAILURE;
    }
    blDev = blDevManager->blDev[devId];
    brightness = GetBlDevBrightness(blDev, type);
    if (!HdfSbufWriteUint32(rspData, brightness)) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HdfGetMinBrightness(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    return HdfGetBrightness(MIN_BRIGHTNESS, reqData, rspData);
}

static int32_t HdfGetCurrBrightness(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    return HdfGetBrightness(CURRENT_BRIGHTNESS, reqData, rspData);
}

static int32_t HdfGetDefBrightness(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    return HdfGetBrightness(DEFAULT_BRIGHTNESS, reqData, rspData);
}

static int32_t HdfGetMaxBrightness(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    return HdfGetBrightness(MAX_BRIGHTNESS, reqData, rspData);
}

static int32_t HdfSetBrightness(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    struct BlDevManager *blDevManager = NULL;
    struct BacklightDev *blDev = NULL;

    (void)device;
    (void)rspData;
    if (reqData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t para = 0;
    if (!HdfSbufReadUint32(reqData, &para)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed", __func__);
        return HDF_FAILURE;
    }
    uint32_t devId = (para >> OFFSET_TWO_BYTE) & 0xffff;
    uint32_t level = para & 0xffff;
    blDevManager = GetBlDevManager();
    if (devId >= blDevManager->devNum) {
        HDF_LOGE("%s: devId is illegal", __func__);
        return HDF_FAILURE;
    }
    blDev = blDevManager->blDev[devId];
    return UpdateBrightness(blDev, level);
}

static int32_t HdfGetBlDevList(struct HdfDeviceObject *device,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)device;
    (void)reqData;
    int32_t ret;
    char *devName = NULL;
    char *tmp = NULL;
    char buffer[NAME_BUFFER_LEN] = {0};
    struct BlDevManager *blDevManager = NULL;

    blDevManager = GetBlDevManager();
    tmp = buffer;
    for (int32_t i = 0; i < blDevManager->devNum; i++) {
        devName = blDevManager->blDev[i]->name;
        if ((tmp + MAX_DEST_STRING_LEN) > &buffer[NAME_BUFFER_LEN]) {
            HDF_LOGE("%s: Memory out of bounds", __func__);
            break;
        }
        // strlen("%d : \n") = 6
        ret = snprintf_s(tmp, MAX_DEST_STRING_LEN, strlen(devName) + 6, "%d : %s\n", i, devName);
        if (ret < 0) {
            HDF_LOGE("%s: snprintf_s fail", __func__);
            return HDF_FAILURE;
        }
        tmp = tmp + strlen(tmp) + 1;
    }
    if (!HdfSbufWriteBuffer(rspData, buffer, strlen(buffer) + 1) != 0) {
        HDF_LOGE("%s: copy info failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static BlCmdHandle g_blDevCmdHandle[] = {
    HdfGetMinBrightness,
    HdfGetCurrBrightness,
    HdfGetDefBrightness,
    HdfGetMaxBrightness,
    HdfGetBlDevList,
    HdfSetBrightness,
};

static int32_t BacklightDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    BlCmdHandle blCmdHandle = NULL;

    if (client == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if ((cmd < 0) || (cmd >= sizeof(g_blDevCmdHandle) / sizeof(BlCmdHandle))) {
        HDF_LOGE("%s: cmd is illegal", __func__);
        return HDF_FAILURE;
    }
    blCmdHandle = g_blDevCmdHandle[cmd];
    return blCmdHandle(client->device, data, reply);
}

static int BacklightBind(struct HdfDeviceObject *dev)
{
    if (dev == NULL) {
        return HDF_FAILURE;
    }
    static struct IDeviceIoService blService = {
        .Dispatch = BacklightDispatch,
    };
    dev->service = &blService;
    return HDF_SUCCESS;
}

static int32_t BacklightInit(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        HDF_LOGE("%s: object is null!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s success", __func__);
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_blDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_BL",
    .Init = BacklightInit,
    .Bind = BacklightBind,
};

HDF_INIT(g_blDevEntry);
