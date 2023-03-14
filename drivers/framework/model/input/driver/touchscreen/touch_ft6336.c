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
#include "touch_ft6336.h"

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
    uint8_t regAddr;
    uint8_t regValue = 0;
    int32_t ret;
    InputI2cClient *i2cClient = &device->driver->i2cClient;

    regAddr = FTS_REG_POINT_RATE;
    ret = InputI2cWrite(i2cClient, &regAddr, 1);
    CHIP_CHECK_RETURN(ret);
    ret = InputI2cRead(i2cClient, &regAddr, 1, &regValue, 1);
    CHIP_CHECK_RETURN(ret);
    HDF_LOGI("%s: Report rate is %u * 10", __func__, regValue);

    regAddr = FTS_REG_FW_VER;
    ret = InputI2cWrite(i2cClient, &regAddr, 1);
    CHIP_CHECK_RETURN(ret);
    ret = InputI2cRead(i2cClient, &regAddr, 1, &regValue, 1);
    CHIP_CHECK_RETURN(ret);
    HDF_LOGI("%s: Firmware version = 0x%x", __func__, regValue);

    regAddr = FTS_REG_CHIP_ID;
    ret = InputI2cWrite(i2cClient, &regAddr, 1);
    CHIP_CHECK_RETURN(ret);
    ret = InputI2cRead(i2cClient, &regAddr, 1, &regValue, 1);
    CHIP_CHECK_RETURN(ret);
    HDF_LOGI("%s: Chip ID is %u", __func__, regValue);

    (void)ChipInit(device);
    (void)ChipResume(device);
    (void)ChipSuspend(device);
    return HDF_SUCCESS;
}

static int32_t ParsePointData(FrameData *frame, uint8_t *buf, uint8_t pointNum)
{
    int32_t i;
    for (i = 0; i < pointNum; i++) {
        frame->fingers[i].x = (unsigned int)((buf[FT_X_H_POS + FT_POINT_SIZE * i] & HALF_BYTE_MASK)
            << ONE_BYTE_OFFSET) | (unsigned int)buf[FT_X_L_POS + FT_POINT_SIZE * i];
        frame->fingers[i].y = (unsigned int)((buf[FT_Y_H_POS + FT_POINT_SIZE * i] & HALF_BYTE_MASK)
            << ONE_BYTE_OFFSET) | (unsigned int)buf[FT_Y_L_POS + FT_POINT_SIZE * i];
        frame->fingers[i].trackId = (int)(buf[FT_FINGER_POS + FT_POINT_SIZE * i]) >> HALF_BYTE_OFFSET;
        frame->fingers[i].status = (int)buf[FT_EVENT_POS + FT_POINT_SIZE * i] >> SIX_BIT_OFFSET;
        frame->fingers[i].valid = true;
        frame->definedEvent = frame->fingers[i].status;

        if ((frame->fingers[i].status == TOUCH_DOWN || frame->fingers[i].status == TOUCH_CONTACT) && (pointNum == 0)) {
            HDF_LOGE("%s: abnormal event data from driver chip", __func__);
            return HDF_FAILURE;
        }
    }
    if (pointNum == 0) {
        frame->definedEvent = TOUCH_UP;
    }
    return HDF_SUCCESS;
}

static int32_t ChipDataHandle(ChipDevice *device)
{
    uint8_t buf[POINT_BUFFER_LEN] = {0};
    uint8_t reg = 0x0;
    uint8_t pointNum;
    int32_t ret;
    InputI2cClient *i2cClient = &device->driver->i2cClient;
    FrameData *frame = &device->driver->frameData;

    ret = InputI2cRead(i2cClient, &reg, 1, buf, POINT_BUFFER_LEN);
    CHIP_CHECK_RETURN(ret);

    OsalMutexLock(&device->driver->mutex);
    (void)memset_s(frame, sizeof(FrameData), 0, sizeof(FrameData));
    pointNum = buf[FT_POINT_NUM_POS] & HALF_BYTE_MASK;
    if (pointNum > MAX_SUPPORT_POINT) {
        pointNum = MAX_SUPPORT_POINT;
    }

    frame->realPointNum = pointNum;
    ret = ParsePointData(frame, buf, pointNum);
    if (ret != HDF_SUCCESS) {
        OsalMutexUnlock(&device->driver->mutex);
        return ret;
    }
    OsalMutexUnlock(&device->driver->mutex);
    return HDF_SUCCESS;
}

static struct TouchChipOps g_ft6336ChipOps = {
    .Init = ChipInit,
    .Detect = ChipDetect,
    .Resume = ChipResume,
    .Suspend = ChipSuspend,
    .DataHandle = ChipDataHandle,
};

static void FreeChipConfig(TouchChipCfg *config)
{
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

static int32_t HdfFocalChipInit(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = NULL;
    ChipDevice *chipDev = NULL;
    HDF_LOGE("%s: enter", __func__);
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
    chipDev->ops = &g_ft6336ChipOps;
    chipDev->chipName = chipCfg->chipName;
    chipDev->vendorName = chipCfg->vendorName;

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

struct HdfDriverEntry g_touchFocalChipEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_TOUCH_FT6336",
    .Init = HdfFocalChipInit,
};

HDF_INIT(g_touchFocalChipEntry);