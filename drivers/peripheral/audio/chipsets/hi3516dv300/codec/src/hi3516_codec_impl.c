/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_aiao_impl.h"
#include "hi3516_codec_impl.h"
#include "audio_control.h"
#include "audio_core.h"
#include "audio_driver_log.h"
#include "audio_platform_base.h"
#include "osal_io.h"
#include <asm/io.h>

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

static void *g_regAcodecBase = NULL; // CODEC Reg Base Addr
const int HALF_MINUTE = 30;
const int VOLUME_DB_MAX = 6;
const int VOLUME_DB_MIN = -121;

int32_t CodecHalSysInit(struct CodecData *codeData)
{
    if (codeData == NULL) {
        AUDIO_DEVICE_LOG_ERR("input invalid parameter.");
        return HDF_ERR_INVALID_PARAM;
    }
    // ACODEC REMAP
    if (g_regAcodecBase == NULL) {
        g_regAcodecBase = (void *)(uintptr_t)(codeData->virtualAddress);
        if (g_regAcodecBase == NULL) {
            AUDIO_DEVICE_LOG_ERR("regAcodecBase is null.");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

// Read contrl reg bits value
int32_t CodecRegBitsRead(struct AudioMixerControl *regAttr, uint32_t *regValue)
{
    if (g_regAcodecBase == NULL || regAttr == NULL ||
        regAttr->reg < 0 || regValue == NULL) {
        AUDIO_DEVICE_LOG_ERR("input invalid parameter.");
        return HDF_ERR_INVALID_PARAM;
    }
    regAttr->value = SysReadl((uintptr_t)g_regAcodecBase + regAttr->reg);
    *regValue = regAttr->value;
    regAttr->value = (*regValue >> regAttr->shift) & regAttr->mask;
    if (regAttr->value > regAttr->max || regAttr->value < regAttr->min) {
        AUDIO_DEVICE_LOG_DEBUG("invalid bitsValue=0x%x", regAttr->value);
        return HDF_FAILURE;
    }
    if (regAttr->invert) {
        regAttr->value = regAttr->max - regAttr->value;
    }
    return HDF_SUCCESS;
}

// Update contrl reg bits value
int32_t CodecRegBitsUpdate(struct AudioMixerControl regAttr)
{
    int32_t ret;
    uint32_t newValue, newMask, oldValue;
    if (g_regAcodecBase == NULL || regAttr.reg < 0) {
        AUDIO_DEVICE_LOG_ERR("input invalid parameter.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (regAttr.invert) {
        regAttr.value = regAttr.max - regAttr.value;
    }
    newValue = regAttr.value << regAttr.shift;
    newMask = regAttr.mask << regAttr.shift;
    ret = CodecRegBitsRead(&regAttr, &oldValue);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("CodecRegBitsRead failed, ret=%d.", ret);
        return HDF_FAILURE;
    }
    regAttr.value = (oldValue & ~newMask) | (newValue & newMask);
    SysWritel((uintptr_t)g_regAcodecBase + regAttr.reg, regAttr.value);
    return HDF_SUCCESS;
}

int32_t CodecRegDefaultInit(struct AudioRegCfgGroupNode **regCfgGroup)
{
    int32_t i;
    struct AudioAddrConfig *regAttr = NULL;
    if (g_regAcodecBase == NULL || regCfgGroup == NULL || regCfgGroup[AUDIO_INIT_GROUP] == NULL ||
        regCfgGroup[AUDIO_INIT_GROUP]->addrCfgItem == NULL || regCfgGroup[AUDIO_INIT_GROUP]->itemNum <= 0) {
        AUDIO_DEVICE_LOG_ERR("input invalid parameter.");
        return HDF_FAILURE;
    }
    regAttr = regCfgGroup[AUDIO_INIT_GROUP]->addrCfgItem;

    for (i = 0; i < regCfgGroup[AUDIO_INIT_GROUP]->itemNum; i++) {
        SysWritel((uintptr_t)g_regAcodecBase + regAttr[i].addr, regAttr[i].value);
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static unsigned int CodecGetI2sFs(const unsigned int rate)
{
    switch (rate) {
        case AUDIO_SAMPLE_RATE_8000:
        case AUDIO_SAMPLE_RATE_11025:
        case AUDIO_SAMPLE_RATE_12000:
            return ACODEC_I2S_FS_8000;
        case AUDIO_SAMPLE_RATE_16000:
        case AUDIO_SAMPLE_RATE_22050:
        case AUDIO_SAMPLE_RATE_24000:
            return ACODEC_I2S_FS_16000;
        case AUDIO_SAMPLE_RATE_32000:
        case AUDIO_SAMPLE_RATE_44100:
        case AUDIO_SAMPLE_RATE_48000:
            return ACODEC_I2S_FS_32000;
        case AUDIO_SAMPLE_RATE_64000:
        case AUDIO_SAMPLE_RATE_96000:
            return ACODEC_I2S_FS_64000;
        default:
            AUDIO_DEVICE_LOG_DEBUG("unsupport samplerate %d\n", rate);
            return ACODEC_I2S_FS_BUTT;
    }
}

static unsigned int CodecGetAdcModeSel(const unsigned int rate)
{
    switch (rate) {
        case AUDIO_SAMPLE_RATE_8000:
        case AUDIO_SAMPLE_RATE_16000:
        case AUDIO_SAMPLE_RATE_32000:
        case AUDIO_SAMPLE_RATE_64000:
            return ACODEC_ADC_MODESEL_4096;
        case AUDIO_SAMPLE_RATE_11025:
        case AUDIO_SAMPLE_RATE_12000:
        case AUDIO_SAMPLE_RATE_22050:
        case AUDIO_SAMPLE_RATE_24000:
        case AUDIO_SAMPLE_RATE_44100:
        case AUDIO_SAMPLE_RATE_48000:
        case AUDIO_SAMPLE_RATE_96000:
            return ACODEC_ADC_MODESEL_6144;
        default:
            AUDIO_DEVICE_LOG_DEBUG("unsupport samplerate %u.\n", rate);
            return ACODEC_I2S_FS_BUTT;
    }
}

static int32_t CodecGetI2s1DataWidth(unsigned int bitWidth, uint16_t *i2s1DataWidth)
{
    if (i2s1DataWidth == NULL) {
        AUDIO_DEVICE_LOG_ERR("input param is NULL");
        return HDF_FAILURE;
    }
    *i2s1DataWidth = AUDIO_CODEC_BIT_WIDTH_16;
    switch (bitWidth) {
        case BIT_WIDTH16:
            *i2s1DataWidth = AUDIO_CODEC_BIT_WIDTH_16;
            break;
        case BIT_WIDTH18:
            *i2s1DataWidth = AUDIO_CODEC_BIT_WIDTH_18;
            break;
        case BIT_WIDTH20:
            *i2s1DataWidth = AUDIO_CODEC_BIT_WIDTH_20;
            break;
        case BIT_WIDTH24:
            *i2s1DataWidth = AUDIO_CODEC_BIT_WIDTH_24;
            break;
        default:
            AUDIO_DEVICE_LOG_ERR("unsupport sample bit width %u.\n", bitWidth);
            return AUDIO_CODEC_BIT_WIDTH_BUTT;
    }
    return HDF_SUCCESS;
}

int32_t CodecDaiParamsUpdate(struct AudioRegCfgGroupNode **regCfgGroup,
    struct CodecDaiParamsVal codecDaiParamsVal)
{
    int32_t ret;
    const int itemNum = 3; // current only 3 items
    struct AudioMixerControl *regAttr = NULL;
    uint16_t codecBitWidth;

    ret = (regCfgGroup == NULL
        || regCfgGroup[AUDIO_DAI_PATAM_GROUP] == NULL
        || regCfgGroup[AUDIO_DAI_PATAM_GROUP]->regCfgItem == NULL
        || regCfgGroup[AUDIO_DAI_PATAM_GROUP]->itemNum < itemNum);
    if (ret) {
        AUDIO_DEVICE_LOG_ERR("input invalid parameter.");
        return HDF_FAILURE;
    }
    regAttr = regCfgGroup[AUDIO_DAI_PATAM_GROUP]->regCfgItem;

    regAttr[0].value = CodecGetI2sFs(codecDaiParamsVal.frequencyVal);
    ret = CodecRegBitsUpdate(regAttr[0]);               // i2s_frequency
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("set i2s_frequency failed.");
        return HDF_FAILURE;
    }
    regAttr[1].value = CodecGetAdcModeSel(codecDaiParamsVal.frequencyVal);
    ret = CodecRegBitsUpdate(regAttr[1]);           // adc_mode_sel
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("set adc_mode_sel failed.");
        return HDF_FAILURE;
    }

    ret = CodecGetI2s1DataWidth(codecDaiParamsVal.formatVal, &codecBitWidth);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("I2s1DataWidthSel failed.");
        return HDF_FAILURE;
    }
    regAttr[itemNum - 1].value = codecBitWidth;         // i2s_datawith
    ret = CodecRegBitsUpdate(regAttr[itemNum - 1]);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("set i2s_datawith failed.");
        return HDF_FAILURE;
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t CodecSetAdcTuneEnable(struct AudioRegCfgGroupNode **regCfgGroup)
{
    int32_t ret;
    struct AudioMixerControl *regAttr = NULL;
    const int itemNum = 1;
    ret = (regCfgGroup == NULL || regCfgGroup[AUDIO_DAI_STARTUP_PATAM_GROUP] == NULL
        || regCfgGroup[AUDIO_DAI_STARTUP_PATAM_GROUP]->regCfgItem == NULL
        || regCfgGroup[AUDIO_DAI_STARTUP_PATAM_GROUP]->itemNum < itemNum);
    if (ret) {
        AUDIO_DEVICE_LOG_ERR("input invalid parameter.");
        return HDF_FAILURE;
    }
    regAttr = regCfgGroup[AUDIO_DAI_STARTUP_PATAM_GROUP]->regCfgItem;
    ret = CodecRegBitsUpdate(regAttr[0]);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("set adc_tune_En09 failed.");
        return HDF_FAILURE;
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AudioCodecAiaoGetCtrlOps(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *elemValue)
{
    uint32_t curValue;
    uint32_t rcurValue;
    void *codecVir = NULL;
    struct AudioMixerControl *mixerCtrl = NULL;
    if (kcontrol == NULL || kcontrol->privateValue <= 0 || elemValue == NULL) {
        AUDIO_DEVICE_LOG_ERR("Audio input param is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    mixerCtrl = (struct AudioMixerControl *)((volatile uintptr_t)kcontrol->privateValue);
    if (mixerCtrl == NULL) {
        AUDIO_DEVICE_LOG_ERR("mixerCtrl is NULL.");
        return HDF_FAILURE;
    }
    codecVir = OsalIoRemap(AIAO_REG_BASE, AIAO_MAX_REG_SIZE);
    if (codecVir == NULL) {
        AUDIO_DEVICE_LOG_ERR("codecVir is NULL.");
        return HDF_FAILURE;
    }
    curValue = OSAL_READL((void *)((uintptr_t)codecVir + mixerCtrl->reg));
    rcurValue = OSAL_READL((void *)((uintptr_t)codecVir + mixerCtrl->rreg));
    OsalIoUnmap(codecVir);

    if (AudioGetCtrlOpsReg(elemValue, mixerCtrl, curValue) != HDF_SUCCESS ||
        AudioGetCtrlOpsRReg(elemValue, mixerCtrl, rcurValue) != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("Audio codec get kcontrol reg and rreg failed.");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void AudioUpdateCodecAiaoRegBits(const struct AudioMixerControl *mixerControl, uint32_t value)
{
    if (mixerControl == NULL) {
        AUDIO_DEVICE_LOG_ERR("param mixerControl is null.");
        return;
    }

    uint32_t curValue;
    void *codecVir = NULL;
    uint32_t mixerControlMask;

    value = value << mixerControl->shift;
    mixerControlMask = mixerControl->mask << mixerControl->shift;
    codecVir = OsalIoRemap(AIAO_REG_BASE, AIAO_MAX_REG_SIZE);
    if (codecVir == NULL) {
        AUDIO_DEVICE_LOG_ERR("codecVir is NULL.");
        return;
    }

    curValue = OSAL_READL((void *)((uintptr_t)codecVir + mixerControl->reg));
    curValue = (curValue & ~mixerControlMask) | (value & mixerControlMask);
    OSAL_WRITEL(curValue, (void *)((uintptr_t)codecVir + mixerControl->reg));
    OsalIoUnmap(codecVir);
}

int32_t AudioCodecAiaoSetCtrlOps(const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *elemValue)
{
    uint32_t value;
    uint32_t rvalue;
    bool updateRReg = false;
    struct AudioMixerControl *mixerCtrl = NULL;
    if (kcontrol == NULL || (kcontrol->privateValue <= 0) || elemValue == NULL) {
        AUDIO_DEVICE_LOG_ERR("Audio input param is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    mixerCtrl = (struct AudioMixerControl *)((volatile uintptr_t)kcontrol->privateValue);
    if (AudioSetCtrlOpsReg(kcontrol, elemValue, mixerCtrl, &value) != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AudioSetCtrlOpsReg is failed.");
        return HDF_ERR_INVALID_OBJECT;
    }
    AudioUpdateCodecAiaoRegBits(mixerCtrl, value);
    if (AudioSetCtrlOpsRReg(elemValue, mixerCtrl, &rvalue, &updateRReg) != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AudioSetCtrlOpsRReg is failed.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (updateRReg) {
        AudioUpdateCodecAiaoRegBits(mixerCtrl, rvalue);
    }

    return HDF_SUCCESS;
}
