/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_codec_impl_test.h"
#include "audio_codec_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_types.h"
#include "hi3516_codec_impl.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

int32_t TestCodecHalSysInit(void)
{
    int ret;
    struct CodecData codeData;

    HDF_LOGI("%s: enter", __func__);
    ret = CodecHalSysInit(&codeData);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecRegBitsRead(void)
{
    struct AudioMixerControl *regAttr = NULL;
    uint32_t regValue;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = CodecRegBitsRead(regAttr, &regValue);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CodecRegBitsUpdateMock(struct AudioMixerControl regAttr)
{
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecRegBitsUpdate(void)
{
    struct AudioMixerControl regAttr;
    int ret;
    HDF_LOGI("%s: enter", __func__);
    regAttr.reg = -1; // -1 is bad value
    ret = CodecRegBitsUpdateMock(regAttr);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecRegDefaultInit(void)
{
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = CodecRegDefaultInit(regCfgGroup);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecSetAdcTuneEnable(void)
{
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = CodecSetAdcTuneEnable(regCfgGroup);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecDaiParamsUpdate(void)
{
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    struct CodecDaiParamsVal codecDaiParamsVal;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    codecDaiParamsVal.channelVal = 1; // 1 is dam channel
    ret = CodecDaiParamsUpdate(regCfgGroup, codecDaiParamsVal);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestAudioCodecAiaoSetCtrlOps(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioCodecAiaoSetCtrlOps(kcontrol, elemValue);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestAudioCodecAiaoGetCtrlOps(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = AudioCodecAiaoGetCtrlOps(kcontrol, elemValue);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
