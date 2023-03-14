/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#include "hi3516_aiao_impl_test.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_types.h"
#include "hi3516_aiao_impl.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

const unsigned int CHANNELNUM = 2;
const unsigned int BITWIDTH = 16;
const unsigned int SAMPLERATE = 48000;

int32_t TestAiopRegCfg(void)
{
    int reg = 0x100;
    int offset = 0x08;
    uint32_t n = 0x01;
    int ret;

    HDF_LOGI("TestAiopRegCfg: enter");
    ret = AiopRegCfg(reg, offset, n);
    if (ret != 0x108) {
        HDF_LOGE("TestAiopRegCfg: AiopRegCfg fail ret = %d", ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("TestAiopRegCfg: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoHalSysInit(void)
{
    int ret;
    HDF_LOGI("TestAiaoHalSysInit: enter");
    ret = AiaoHalSysInit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAiaoHalSysInit: AiaoHalSysInit fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAiaoHalSysInit: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoClockReset(void)
{
    int ret;
    HDF_LOGI("TestAiaoClockReset: enter");
    ret = AiaoClockReset();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAiaoClockReset: AiaoClockReset fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAiaoClockReset: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoHalReadReg(void)
{
    uint32_t ret;
    unsigned int offset;
    const unsigned int offValue = 0x100;
    HDF_LOGI("TestAiaoHalReadReg: enter");
    offset = offValue;
    ret = AiaoHalReadReg(offset);
    if (ret == 0x0) {
        HDF_LOGE("TestAiaoHalReadReg: AiaoHalReadReg fail ret = %d", ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("TestAiaoHalReadReg: success");
    return HDF_SUCCESS;
}

int32_t TestAopHalSetBuffRptr(void)
{
    int ret;
    unsigned int chnId;
    unsigned int value;

    chnId = 0;
    value = 0;
    HDF_LOGI("TestAopHalSetBuffRptr: enter");
    ret = AopHalSetBuffRptr(chnId, value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAopHalSetBuffRptr: AopHalSetBuffRptr fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAopHalSetBuffRptr: success");
    return HDF_SUCCESS;
}

int32_t TestAopHalSetBuffWptr(void)
{
    int ret;
    unsigned int chnId;
    unsigned int value;

    chnId = 0;
    value = 0;
    HDF_LOGI("TestAopHalSetBuffWptr: enter");
    ret = AopHalSetBuffWptr(chnId, value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAopHalSetBuffRptr: AopHalSetBuffWptr fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAopHalSetBuffWptr: success");
    return HDF_SUCCESS;
}

int32_t TestAipHalSetBuffWptr(void)
{
    int ret;
    unsigned int chnId;
    unsigned int value;

    chnId = 0;
    value = 0;
    HDF_LOGI("TestAipHalSetBuffWptr: enter");
    ret = AipHalSetBuffWptr(chnId, value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAopHalSetBuffRptr: AopHalSetBuffWptr fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAipHalSetBuffWptr: success");
    return HDF_SUCCESS;
}

int32_t TestAipHalSetBuffRptr(void)
{
    int ret;
    unsigned int chnId;
    unsigned int value;

    chnId = 0;
    value = 0;
    HDF_LOGI("TestAipHalSetBuffRptr: enter");
    ret = AipHalSetBuffRptr(chnId, value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAipHalSetBuffRptr: AipHalSetBuffRptr fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAipHalSetBuffRptrs: success");
    return HDF_SUCCESS;
}

int32_t TestAopHalSetTxStart(void)
{
    unsigned int chnId;
    HDF_LOGI("TestAopHalSetTxStart: enter");

    chnId = 1; // 1 is dma channel
    if (AopHalSetTxStart(chnId, HI_TRUE) != HDF_SUCCESS) {
        HDF_LOGE("TestAopHalSetTxStart: AopHalSetTxStart fail.");
    }

    HDF_LOGI("TestAopHalSetTxStart: success");
    return HDF_SUCCESS;
}

int32_t TestAipHalSetRxStart(void)
{
    int ret;
    unsigned int chnId;
    bool en = HI_TRUE;

    chnId = 0;
    HDF_LOGI("TestAipHalSetRxStart: enter");
    ret = AipHalSetRxStart(chnId, en);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAipHalSetRxStart: AipHalSetRxStart fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAipHalSetRxStart: success");
    return HDF_SUCCESS;
}

int32_t TestAopHalDevEnable(void)
{
    int ret;
    unsigned int chnId;

    chnId = 0;
    HDF_LOGI("TestAopHalDevEnable: enter");
    ret = AopHalDevEnable(chnId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAopHalDevEnable: AopHalDevEnable fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAopHalDevEnable: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoDeviceInit(void)
{
    int ret;
    int32_t chnId;

    HDF_LOGI("TestAiaoDeviceInit: enter");
    chnId = 0;
    ret = AiaoDeviceInit(chnId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAiaoDeviceInit: AiaoDeviceInit fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAiaoDeviceInit: success");
    return HDF_SUCCESS;
}

int32_t TestAudioAoInit(void)
{
    struct PlatformData *platformData = NULL;
    int ret;
    HDF_LOGI("TestAudioAoInit: enter");

    ret = AudioAoInit(platformData);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("TestAudioAoInit: AudioAoInit fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAudioAoInit: success");
    return HDF_SUCCESS;
}

int32_t TestAudioAiInit(void)
{
    struct PlatformData *platformData = NULL;
    int ret;
    HDF_LOGI("TestAudioAiInit: enter");

    ret = AudioAiInit(platformData);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("TestAudioAiInit: AudioAiInit fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAudioAiInit: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoSysPinMux(void)
{
    int ret;

    HDF_LOGI("TestAiaoSysPinMux: enter");
    ret = AiaoSysPinMux();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAiaoSysPinMux: AiaoSysPinMux fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAiaoSysPinMux: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoGetMclk(void)
{
    int ret;
    unsigned int rate = SAMPLERATE;
    uint32_t *mclkSel = NULL;

    HDF_LOGI("TestAiaoGetMclk: enter");
    ret = AiaoGetMclk(rate, mclkSel);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("TestAiaoGetMclk: AiaoGetMclk fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestAiaoGetMclk: success");
    return HDF_SUCCESS;
}

int32_t TestAiaoSetSysCtlRegValue(void)
{
    uint32_t mclkSel = 0x0011A71E;
    uint32_t bitWidth = BITWIDTH;
    uint32_t rate = SAMPLERATE;
    uint32_t clkRegVal;
    int ret;

    HDF_LOGI("TestAiaoSetSysCtlRegValue: enter");
    ret = AiaoSetSysCtlRegValue(mclkSel, bitWidth, rate, &clkRegVal);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestAiaoSetSysCtlRegValue: AiaoSetSysCtlRegValue fail ret = %d", ret);
    }

    HDF_LOGI("TestAiaoSetSysCtlRegValue: success");
    return HDF_SUCCESS;
}
