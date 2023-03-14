/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "osal_mem.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hdf_touch.h"
#include "input_i2c_ops.h"
#include "touch_gt911.h"

#define MAX_POINT 5

static int32_t ChipInit(ChipDevice *device)
{
    return HDF_SUCCESS;
}

static int32_t ChipResume(ChipDevice *device)
{
    return HDF_SUCCESS;
}

static int32_t ChipSuspend(ChipDevice *device)
{
    return HDF_SUCCESS;
}

static int32_t ChipDetect(ChipDevice *device)
{
    int32_t ret;
    int32_t version;
    int32_t xSolution;
    int32_t ySolution;
    InputI2cClient *i2cClient = &device->driver->i2cClient;
    uint8_t buf[GT_CFG_INFO_LEN] = {0};
    uint8_t reg[GT_ADDR_LEN] = {0};
    reg[0] = (GT_CFG_INFO_ADDR >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    reg[1] = GT_CFG_INFO_ADDR & ONE_BYTE_MASK;

    ret = InputI2cRead(i2cClient, reg, GT_ADDR_LEN, buf, GT_CFG_INFO_LEN);
    if (ret < 0) {
        HDF_LOGE("%s: read chip version failed", __func__);
        return HDF_FAILURE;
    }

    version = (buf[GT_FW_VER_HIGH] << ONE_BYTE_OFFSET) | buf[GT_FW_VER_LOW];
    xSolution = (buf[GT_SOLU_X_HIGH] << ONE_BYTE_OFFSET) | buf[GT_SOLU_X_LOW];
    ySolution = (buf[GT_SOLU_Y_HIGH] << ONE_BYTE_OFFSET) | buf[GT_SOLU_Y_LOW];
#if defined(CONFIG_ARCH_ROCKCHIP)
    if (version != 0x100 || buf[GT_PROD_ID_1ST] != '5' || buf[GT_PROD_ID_2ND] != '6') {
        HDF_LOGE("%s: version or ID wrong", __func__);
        return HDF_FAILURE;
    }
#endif
    HDF_LOGI("%s: IC FW version is 0x%x", __func__, version);
    if (buf[GT_FW_VER_HIGH] == 0x0) {
        HDF_LOGI("Product ID : %c%c%c_%02x%02x, xSol = %d, ySol = %d", buf[GT_PROD_ID_1ST], buf[GT_PROD_ID_2ND],
            buf[GT_PROD_ID_3RD], buf[GT_FW_VER_HIGH], buf[GT_FW_VER_LOW], xSolution, ySolution);
    } else {
        HDF_LOGI("Product_ID: %c%c%c%c_%02x%02x, x_sol = %d, y_sol = %d", buf[GT_PROD_ID_1ST], buf[GT_PROD_ID_2ND],
            buf[GT_PROD_ID_3RD], buf[GT_PROD_ID_4TH], buf[GT_FW_VER_HIGH], buf[GT_FW_VER_LOW], xSolution, ySolution);
    }

    (void)ChipInit(device);
    (void)ChipResume(device);
    (void)ChipSuspend(device);
    return HDF_SUCCESS;
}

static int ChipCleanBuffer(InputI2cClient *i2cClient)
{
    int32_t ret;
    uint8_t writeBuf[GT_CLEAN_DATA_LEN];
    writeBuf[GT_REG_HIGH_POS] = (GT_BUF_STATE_ADDR >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    writeBuf[GT_REG_LOW_POS] = GT_BUF_STATE_ADDR & ONE_BYTE_MASK;
    writeBuf[GT_CLEAN_POS] = GT_CLEAN_FLAG;
    ret = InputI2cWrite(i2cClient, writeBuf, GT_CLEAN_DATA_LEN);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InputI2cWrite failed, ret = %d", __func__, ret);
    }
    return ret;
}

#define X_OFFSET    1

static void ParsePointData(ChipDevice *device, FrameData *frame, uint8_t *buf, uint8_t pointNum)
{
    int32_t chipVer = device->chipCfg->chipVersion;
    int32_t resX = device->driver->boardCfg->attr.resolutionX;
    int32_t resY = device->driver->boardCfg->attr.resolutionY;
    int32_t i;

    for (i = 0; i < pointNum; i++) {
        if (chipVer == 0) {         // chipversion  A:gt911_zsj5p5
            frame->fingers[i].trackId = buf[GT_POINT_SIZE * i + GT_TRACK_ID];
#if defined(CONFIG_ARCH_SPRD)
            frame->fingers[i].y = (resX - 1 - ((buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) <<
                                  ONE_BYTE_OFFSET))) * resY / resX;
            frame->fingers[i].x = ((buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) <<
                                  ONE_BYTE_OFFSET)) * resX / resY;
#elif defined(CONFIG_ARCH_ROCKCHIP)
            frame->fingers[i].x = resX - ((buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET));
            frame->fingers[i].y = resY - ((buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET));
#elif defined(LOSCFG_PLATFORM_STM32MP157)
            frame->fingers[i].x = (buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
            frame->fingers[i].y = (buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
#else
            frame->fingers[i].y = (buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
            frame->fingers[i].x = (buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
#endif
            if (frame->fingers[i].x == 0) {
                frame->fingers[i].x = X_OFFSET;
            }
        } else if (chipVer == 1) {  // chipversion B:gt911_zsj4p0
            frame->fingers[i].x = resX - 1 - ((buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET));
            frame->fingers[i].y = resY - 1 - ((buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET));
        } else {                    // chipversion C:gt911_tg7p0
            frame->fingers[i].x = resX - 1 - ((buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET));
            frame->fingers[i].y = resY - 1 - ((buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                                  ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET));
        }
        frame->fingers[i].valid = true;
    }
}

static int32_t ChipDataHandle(ChipDevice *device)
{
    int32_t ret;
    uint8_t touchStatus = 0;
    uint8_t pointNum;
    uint8_t buf[GT_POINT_SIZE * MAX_SUPPORT_POINT] = {0};
    InputI2cClient *i2cClient = &device->driver->i2cClient;
    uint8_t reg[GT_ADDR_LEN] = {0};
    FrameData *frame = &device->driver->frameData;

    reg[0] = (GT_BUF_STATE_ADDR >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    reg[1] = GT_BUF_STATE_ADDR & ONE_BYTE_MASK;
    ret = InputI2cRead(i2cClient, reg, GT_ADDR_LEN, &touchStatus, 1);
    if (ret < 0 || touchStatus == GT_EVENT_INVALID) {
        return HDF_FAILURE;
    }

    OsalMutexLock(&device->driver->mutex);
    (void)memset_s(frame, sizeof(FrameData), 0, sizeof(FrameData));
    if (touchStatus == GT_EVENT_UP) {
        frame->realPointNum = 0;
        frame->definedEvent = TOUCH_UP;
        goto EXIT;
    }

    reg[0] = (GT_X_LOW_BYTE_BASE >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    reg[1] = GT_X_LOW_BYTE_BASE & ONE_BYTE_MASK;
    pointNum = touchStatus & GT_FINGER_NUM_MASK;
    if (pointNum == 0 || pointNum > MAX_SUPPORT_POINT) {
        HDF_LOGE("%s: pointNum is invalid, %u", __func__, pointNum);
        (void)ChipCleanBuffer(i2cClient);
        OsalMutexUnlock(&device->driver->mutex);
        return HDF_FAILURE;
    }
    frame->realPointNum = pointNum;
    frame->definedEvent = TOUCH_DOWN;
    (void)InputI2cRead(i2cClient, reg, GT_ADDR_LEN, buf, GT_POINT_SIZE * pointNum);
    ParsePointData(device, frame, buf, pointNum);

EXIT:
    OsalMutexUnlock(&device->driver->mutex);
    if (ChipCleanBuffer(i2cClient) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UpdateFirmware(ChipDevice *device)
{
    int32_t ret;
    InputI2cClient *i2cClient = &device->driver->i2cClient;
#if defined(CONFIG_ARCH_ROCKCHIP)
    uint8_t buf[1] = {0};
    uint8_t reg[GT_ADDR_LEN] = {0};

    reg[0] = (GTP_REG_CONFIG_DATA >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    reg[1] = GTP_REG_CONFIG_DATA & ONE_BYTE_MASK;
    ret = InputI2cRead(i2cClient, reg, GT_ADDR_LEN, buf, 1);
    if (ret < 0) {
        HDF_LOGE("%s: read fw version failed", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: buf[0]=0x%x", __func__, buf[0]);
    if (buf[0] == firmWareParm[FIRMWARE_3RD]) {
        HDF_LOGI("%s: needn't update fw version", __func__);
        return HDF_SUCCESS;
    }
#endif
    ret = InputI2cWrite(i2cClient, firmWareParm, FIRMWARE_LEN);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: update firmware success\n", __func__);
    return HDF_SUCCESS;
}

static void SetAbility(ChipDevice *device)
{
    device->driver->inputDev->abilitySet.devProp[0] = SET_BIT(INPUT_PROP_DIRECT);
    device->driver->inputDev->abilitySet.eventType[0] = SET_BIT(EV_SYN) |
        SET_BIT(EV_KEY) | SET_BIT(EV_ABS);
    device->driver->inputDev->abilitySet.absCode[0] = SET_BIT(ABS_X) | SET_BIT(ABS_Y);
    device->driver->inputDev->abilitySet.absCode[1] = SET_BIT(ABS_MT_POSITION_X) |
        SET_BIT(ABS_MT_POSITION_Y) | SET_BIT(ABS_MT_TRACKING_ID);
    device->driver->inputDev->abilitySet.keyCode[KEY_CODE_4TH] = SET_BIT(KEY_UP) | SET_BIT(KEY_DOWN);
    device->driver->inputDev->attrSet.axisInfo[ABS_X].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_X].max = device->boardCfg->attr.resolutionX - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_X].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].max = device->boardCfg->attr.resolutionY - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_Y].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].max = device->boardCfg->attr.resolutionX - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_X].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].min = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].max = device->boardCfg->attr.resolutionY - 1;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_POSITION_Y].range = 0;
    device->driver->inputDev->attrSet.axisInfo[ABS_MT_TRACKING_ID].max = MAX_POINT;
}

static struct TouchChipOps g_gt911ChipOps = {
    .Init = ChipInit,
    .Detect = ChipDetect,
    .Resume = ChipResume,
    .Suspend = ChipSuspend,
    .DataHandle = ChipDataHandle,
    .UpdateFirmware = UpdateFirmware,
    .SetAbility = SetAbility,
};

static TouchChipCfg *ChipConfigInstance(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = (TouchChipCfg *)OsalMemAlloc(sizeof(TouchChipCfg));
    if (chipCfg == NULL) {
        HDF_LOGE("%s: instance chip config failed", __func__);
        return NULL;
    }
    (void)memset_s(chipCfg, sizeof(TouchChipCfg), 0, sizeof(TouchChipCfg));

    if (ParseTouchChipConfig(device->property, chipCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: parse chip config failed", __func__);
        OsalMemFree(chipCfg);
        chipCfg = NULL;
    }
    return chipCfg;
}

static ChipDevice *ChipDeviceInstance(void)
{
    ChipDevice *chipDev = (ChipDevice *)OsalMemAlloc(sizeof(ChipDevice));
    if (chipDev == NULL) {
        HDF_LOGE("%s: instance chip device failed", __func__);
        return NULL;
    }
    (void)memset_s(chipDev, sizeof(ChipDevice), 0, sizeof(ChipDevice));
    return chipDev;
}

static void FreeChipConfig(TouchChipCfg *config)
{
    if (config == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return;
    }
    if (config->pwrSeq.pwrOn.buf != NULL) {
        OsalMemFree(config->pwrSeq.pwrOn.buf);
    }

    if (config->pwrSeq.pwrOff.buf != NULL) {
        OsalMemFree(config->pwrSeq.pwrOff.buf);
    }

    if (config->pwrSeq.resume.buf != NULL) {
        OsalMemFree(config->pwrSeq.resume.buf);
    }

    if (config->pwrSeq.suspend.buf != NULL) {
        OsalMemFree(config->pwrSeq.suspend.buf);
    }

    OsalMemFree(config);
}

static int32_t HdfGoodixChipInit(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = NULL;
    ChipDevice *chipDev = NULL;

    HDF_LOGI("%s: enter", __func__);
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    chipCfg = ChipConfigInstance(device);
    if (chipCfg == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    chipDev = ChipDeviceInstance();
    if (chipDev == NULL) {
        goto EXIT;
    }

    chipDev->chipCfg = chipCfg;
    chipDev->ops = &g_gt911ChipOps;
    chipDev->chipName = chipCfg->chipName;
    chipDev->vendorName = chipCfg->vendorName;
    device->priv = (void *)chipDev;

    if (RegisterTouchChipDevice(chipDev) != HDF_SUCCESS) {
        goto EXIT1;
    }
    HDF_LOGI("%s: exit succ, chipName = %s", __func__, chipCfg->chipName);
    return HDF_SUCCESS;

EXIT1:
    OsalMemFree(chipDev);
EXIT:
    FreeChipConfig(chipCfg);
    return HDF_FAILURE;
}

static void HdfGoodixChipRelease(struct HdfDeviceObject *device)
{
    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return;
    }
    HDF_LOGI("%s: goodix chip is release", __func__);
}

struct HdfDriverEntry g_touchGoodixChipEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_TOUCH_GT911",
    .Init = HdfGoodixChipInit,
    .Release = HdfGoodixChipRelease,
};

HDF_INIT(g_touchGoodixChipEntry);
