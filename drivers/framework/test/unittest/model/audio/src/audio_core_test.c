/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_core_test.h"
#include "audio_core.h"
#include "audio_parse.h"
#include "devsvc_manager_clnt.h"

#define HDF_LOG_TAG audio_core_test

static struct AudioMixerControl g_audioTestReg = {
    .reg = 0x2004,  /* [0] output volume */
    .rreg = 0x2004, /* register value */
    .shift = 0x8,   /* reg offset */
    .rshift = 0x8,  /* right reg offset */
    .min = 0x28,    /* min value */
    .max = 0x7F,    /* max value */
    .mask = 0x7F,   /* mask value */
    .invert = 0,    /* invert */
};

static int32_t AudioDeviceReadRegMock(unsigned long virtualAddress, uint32_t reg, uint32_t *value)
{
    (void)virtualAddress;
    (void)reg;
    (void)value;
    return HDF_SUCCESS;
}

static int32_t AudioDeviceWriteRegMock(unsigned long virtualAddress, uint32_t reg, uint32_t value)
{
    (void)virtualAddress;
    (void)reg;
    (void)value;
    return HDF_SUCCESS;
}

int32_t AudioDeviceReadRegTest(void)
{
    int value;
    HDF_LOGI("%s: enter", __func__);
    return AudioDeviceReadRegMock(0, 0, &value);
}

int32_t AudioDeviceWriteRegTest(void)
{
    HDF_LOGI("%s: enter", __func__);
    return AudioDeviceWriteRegMock(0, 0, 0);
}

int32_t AudioSocRegisterPlatformTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct PlatformData platformData;
    HDF_LOGI("%s: enter", __func__);

    if (AudioSocRegisterPlatform(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioSocRegisterPlatform fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    (void)memset_s(&platformData, sizeof(struct PlatformData), 0, sizeof(struct PlatformData));
    platformData.drvPlatformName = "dma_service_0";
    if (AudioSocRegisterPlatform(device, &platformData) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioSocRegisterPlatform fail", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
int32_t AudioSocRegisterDaiTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct DaiData data;
    HDF_LOGI("%s: enter", __func__);

    if (AudioSocRegisterDai(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioSocRegisterDai fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    (void)memset_s(&data, sizeof(struct DaiData), 0, sizeof(struct DaiData));
    data.drvDaiName = "dai_service";
    if (AudioSocRegisterDai(device, &data) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioSocRegisterDai fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioRegisterDspTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct DaiData daiData;
    struct DspData dspData;
    HDF_LOGI("%s: enter", __func__);

    if (AudioRegisterDsp(NULL, NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioSocRegisterDsp fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    (void)memset_s(&daiData, sizeof(struct DspData), 0, sizeof(struct DspData));
    (void)memset_s(&daiData, sizeof(struct DaiData), 0, sizeof(struct DaiData));
    dspData.drvDspName = "dsp_service_0",
    daiData.drvDaiName = "dai_service";
    if (AudioRegisterDsp(device, &dspData, &daiData) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioSocRegisterDsp fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioRegisterCodecTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct DaiData daiData;
    struct CodecData codecData;
    HDF_LOGI("%s: enter", __func__);

    if (AudioRegisterCodec(NULL, NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioRegisterCodec fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    (void)memset_s(&codecData, sizeof(struct CodecData), 0, sizeof(struct CodecData));
    (void)memset_s(&daiData, sizeof(struct DaiData), 0, sizeof(struct DaiData));
    codecData.drvCodecName = "codec_dai";
    daiData.drvDaiName = "dai_service";
    if (AudioRegisterCodec(device, &codecData, &daiData) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioRegisterCodec fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioRegisterAccessoryTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct DaiData daiData;
    struct AccessoryData accessoryData;
    HDF_LOGI("%s: enter", __func__);

    if (AudioRegisterAccessory(NULL, NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioRegisterAccessory fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    (void)memset_s(&accessoryData, sizeof(struct AccessoryData), 0, sizeof(struct AccessoryData));
    (void)memset_s(&daiData, sizeof(struct DaiData), 0, sizeof(struct DaiData));
    accessoryData.drvAccessoryName = "accessory_dai";
    daiData.drvDaiName = "dai_service";
    if (AudioRegisterAccessory(device, &accessoryData, &daiData) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioRegisterAccessory fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioBindDaiLinkTest(void)
{
    HDF_LOGI("%s: enter", __func__);

    if (AudioBindDaiLink(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioBindDaiLink fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioUpdateCodecRegBitsTest(void)
{
    struct CodecData codecData;
    struct CodecDevice codec;
    HDF_LOGI("%s: enter", __func__);

    if (AudioUpdateCodecRegBits(NULL, 0, 0, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioUpdateCodecRegBits fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    (void)memset_s(&codecData, sizeof(struct CodecData), 0, sizeof(struct CodecData));
    (void)memset_s(&codec, sizeof(struct CodecDevice), 0, sizeof(struct CodecDevice));
    codecData.Read = AudioDeviceReadRegMock;
    codecData.Write = AudioDeviceWriteRegMock,
    codec.devCodecName = "codec_service_0";
    OsalMutexInit(&codec.mutex);
    codec.devData = &codecData;
    if (AudioUpdateCodecRegBits(&codec, g_audioTestReg.reg, g_audioTestReg.mask,
        g_audioTestReg.shift, g_audioTestReg.min + 1) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioUpdateCodecRegBits fail", __func__, __LINE__);
        OsalMutexDestroy(&codec.mutex);
        return HDF_FAILURE;
    }

    OsalMutexDestroy(&codec.mutex);
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioUpdateAccessoryRegBitsTest(void)
{
    struct AccessoryDevice accessory;
    HDF_LOGI("%s: enter", __func__);

    if (AudioUpdateAccessoryRegBits(NULL, 0, 0, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioUpdateAccessoryRegBits fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    (void)memset_s(&accessory, sizeof(struct AccessoryDevice), 0, sizeof(struct AccessoryDevice));
    accessory.devAccessoryName = "codec_service_1";
    OsalMutexInit(&accessory.mutex);
    if (AudioUpdateAccessoryRegBits(&accessory, g_audioTestReg.reg, g_audioTestReg.mask,
        g_audioTestReg.shift, g_audioTestReg.min + 1) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioUpdateAccessoryRegBits fail", __func__, __LINE__);
        OsalMutexDestroy(&accessory.mutex);
    }

    OsalMutexDestroy(&accessory.mutex);
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioUpdateDaiRegBitsTest(void)
{
    struct DaiDevice dai;
    HDF_LOGI("%s: enter", __func__);

    if (AudioUpdateDaiRegBits(NULL, 0, 0, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioUpdateDaiRegBits fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    (void)memset_s(&dai, sizeof(struct DaiDevice), 0, sizeof(struct DaiDevice));
    dai.devDaiName = "dai_service";
    if (AudioUpdateDaiRegBits(&dai, g_audioTestReg.reg, g_audioTestReg.mask,
        g_audioTestReg.shift, g_audioTestReg.min + 1) != HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioUpdateAccessoryRegBits fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioKcontrolGetCpuDaiTest(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    HDF_LOGI("%s: enter", __func__);

    if (AudioKcontrolGetCpuDai(kcontrol) == NULL) {
        HDF_LOGE("%s_[%d] AudioKcontrolGetCpuDai fail!", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioKcontrolGetCodecTest(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    HDF_LOGI("%s: enter", __func__);

    if (AudioKcontrolGetCodec(kcontrol) == NULL) {
        HDF_LOGE("%s_[%d] AudioKcontrolGetCodec fail!", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioKcontrolGetAccessoryTest(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    HDF_LOGI("%s: enter", __func__);

    if (AudioKcontrolGetAccessory(kcontrol) == NULL) {
        HDF_LOGE("%s_[%d] AudioKcontrolGetAccessory fail!", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAddControlsTest(void)
{
    HDF_LOGI("%s: enter", __func__);

    if (AudioAddControls(NULL, NULL, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAddControls fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAddControlTest(void)
{
    struct AudioCard *audioCard = NULL;
    struct AudioKcontrol control;
    HDF_LOGI("%s: enter", __func__);

    audioCard = GetCardInstance("hdf_audio_codec_dev0");
    (void)memset_s(&control, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    if (AudioAddControl(audioCard, &control) == NULL) {
        HDF_LOGE("%s_[%d] AudioAddControl fail!", __func__, __LINE__);
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioGetCtrlOpsRRegTest(void)
{
    struct AudioCtrlElemValue elemValue;
    struct AudioMixerControl mixerCtrl;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    (void)memset_s(&mixerCtrl, sizeof(struct AudioMixerControl), 0, sizeof(struct AudioMixerControl));
    AudioGetCtrlOpsRReg(&elemValue, &mixerCtrl, 0);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioGetCtrlOpsRegTest(void)
{
    struct AudioCtrlElemValue elemValue;
    struct AudioMixerControl mixerCtrl;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    (void)memset_s(&mixerCtrl, sizeof(struct AudioMixerControl), 0, sizeof(struct AudioMixerControl));
    AudioGetCtrlOpsReg(&elemValue, &mixerCtrl, 0);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioSetCtrlOpsRegTest(void)
{
    uint32_t value;
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    struct AudioMixerControl mixerCtrl;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    (void)memset_s(&mixerCtrl, sizeof(struct AudioMixerControl), 0, sizeof(struct AudioMixerControl));
    AudioSetCtrlOpsReg(&kcontrol, &elemValue, &mixerCtrl, &value);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioSetCtrlOpsRRegTest(void)
{
    uint32_t value;
    bool updateRReg;
    struct AudioCtrlElemValue elemValue;
    struct AudioMixerControl mixerCtrl;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    (void)memset_s(&mixerCtrl, sizeof(struct AudioMixerControl), 0, sizeof(struct AudioMixerControl));
    AudioSetCtrlOpsRReg(&elemValue, &mixerCtrl, &value, &updateRReg);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioDaiReadRegTest(void)
{
    uint32_t val;
    struct DaiDevice dai;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&dai, sizeof(struct DaiDevice), 0, sizeof(struct DaiDevice));
    if (AudioDaiReadReg(&dai, 0, &val) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioDaiReadReg fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioDaiWriteRegTest(void)
{
    struct DaiDevice dai;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&dai, sizeof(struct DaiDevice), 0, sizeof(struct DaiDevice));
    if (AudioDaiWriteReg(&dai, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioDaiWriteReg fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCodecReadRegTest(void)
{
    uint32_t val;
    struct CodecDevice codec;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&codec, sizeof(struct CodecDevice), 0, sizeof(struct CodecDevice));
    if (AudioCodecReadReg(&codec, 0, &val) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCodecReadReg fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCodecWriteRegTest(void)
{
    struct CodecDevice codec;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&codec, sizeof(struct CodecDevice), 0, sizeof(struct CodecDevice));
    if (AudioCodecWriteReg(&codec, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCodecWriteReg fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAccessoryReadRegTest(void)
{
    uint32_t val;
    struct AccessoryDevice accessory;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&accessory, sizeof(struct AccessoryDevice), 0, sizeof(struct AccessoryDevice));
    if (AudioAccessoryReadReg(&accessory, 0, &val) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAccessoryReadReg fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAccessoryWriteRegTest(void)
{
    struct AccessoryDevice accessory;
    HDF_LOGI("%s: enter", __func__);

    (void)memset_s(&accessory, sizeof(struct AccessoryDevice), 0, sizeof(struct AccessoryDevice));
    if (AudioAccessoryWriteReg(&accessory, 0, 0) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAccessoryWriteReg fail", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioInfoCtrlOpsTest(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemInfo elemInfo;
    HDF_LOGI("%s: enter", __func__);

    if (AudioInfoCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioInfoCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemInfo, sizeof(struct AudioCtrlElemInfo), 0, sizeof(struct AudioCtrlElemInfo));
    if (AudioInfoCtrlOps(&kcontrol, &elemInfo) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioInfoCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCodecGetCtrlOpsTest(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    HDF_LOGI("%s: enter", __func__);

    if (AudioCodecGetCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCodecGetCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (AudioCodecGetCtrlOps(&kcontrol, &elemValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCodecGetCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCodecSetCtrlOpsTest(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    HDF_LOGI("%s: enter", __func__);

    if (AudioCodecSetCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCodecSetCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (AudioCodecSetCtrlOps(&kcontrol, &elemValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCodecSetCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAccessoryGetCtrlOpsTest(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    HDF_LOGI("%s: enter", __func__);

    if (AudioAccessoryGetCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAccessoryGetCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (AudioAccessoryGetCtrlOps(&kcontrol, &elemValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAccessoryGetCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioAccessorySetCtrlOpsTset(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    HDF_LOGI("%s: enter", __func__);

    if (AudioAccessorySetCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAccessorySetCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (AudioAccessorySetCtrlOps(&kcontrol, &elemValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioAccessorySetCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCpuDaiSetCtrlOpsTest(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    HDF_LOGI("%s: enter", __func__);

    if (AudioCpuDaiSetCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCpuDaiSetCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (AudioCpuDaiSetCtrlOps(&kcontrol, &elemValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCpuDaiSetCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t AudioCpuDaiGetCtrlOpsTest(void)
{
    struct AudioKcontrol kcontrol;
    struct AudioCtrlElemValue elemValue;
    HDF_LOGI("%s: enter", __func__);

    if (AudioCpuDaiGetCtrlOps(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCpuDaiGetCtrlOps fail", __func__, __LINE__);
        return HDF_FAILURE;
    }
    (void)memset_s(&kcontrol, sizeof(struct AudioKcontrol), 0, sizeof(struct AudioKcontrol));
    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (AudioCpuDaiGetCtrlOps(&kcontrol, &elemValue) == HDF_SUCCESS) {
        HDF_LOGE("%s_[%d] AudioCpuDaiGetCtrlOps fail", __func__, __LINE__);
    }

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
